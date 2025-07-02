#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <iostream>

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef websocketpp::lib::shared_ptr<boost::asio::ssl::context> context_ptr;

class WebSocketClient
{
public:
    explicit WebSocketClient(const std::string &hostname)
        : m_hostname(hostname), m_uri("wss://" + hostname)
    {

        m_client.init_asio();

        m_client.set_access_channels(websocketpp::log::alevel::all);
        m_client.clear_access_channels(websocketpp::log::alevel::frame_payload);
        m_client.set_error_channels(websocketpp::log::elevel::all);

        m_client.set_open_handler(
            websocketpp::lib::bind(&WebSocketClient::on_open, this, websocketpp::lib::placeholders::_1));
        m_client.set_message_handler(
            websocketpp::lib::bind(&WebSocketClient::on_message, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
        m_client.set_fail_handler(
            websocketpp::lib::bind(&WebSocketClient::on_fail, this, websocketpp::lib::placeholders::_1));
        m_client.set_close_handler(
            websocketpp::lib::bind(&WebSocketClient::on_close, this, websocketpp::lib::placeholders::_1));
        m_client.set_tls_init_handler(
            websocketpp::lib::bind(&WebSocketClient::on_tls_init, this, websocketpp::lib::placeholders::_1));
    }

    void run()
    {
        websocketpp::lib::error_code ec;
        client::connection_ptr con = m_client.get_connection(m_uri, ec);
        if (ec)
        {
            std::cout << "Connection error: " << ec.message() << std::endl;
            return;
        }

        m_client.connect(con);
        m_client.run();
    }

private:
    client m_client;
    std::string m_hostname;
    std::string m_uri;

    void on_open(websocketpp::connection_hdl hdl)
    {
        std::cout << "WebSocket connection opened!" << std::endl;

        websocketpp::lib::error_code ec;
        client::connection_ptr con = m_client.get_con_from_hdl(hdl, ec);
        if (ec)
        {
            std::cout << "Error getting connection: " << ec.message() << std::endl;
            return;
        }

        std::string payload = R"({
            "jsonrpc":"2.0",
            "id":1,
            "method":"eth_subscribe",
            "params":["logs", {}]
        })";

        m_client.send(con, payload, websocketpp::frame::opcode::text);
    }

    void on_message(websocketpp::connection_hdl, client::message_ptr msg)
    {
        std::cout << "Received message: " << msg->get_payload() << std::endl;
    }

    void on_fail(websocketpp::connection_hdl)
    {
        std::cout << "WebSocket connection failed!" << std::endl;
    }

    void on_close(websocketpp::connection_hdl)
    {
        std::cout << "WebSocket connection closed!" << std::endl;
    }

    context_ptr on_tls_init(websocketpp::connection_hdl)
    {
        context_ptr ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(
            boost::asio::ssl::context::sslv23);

        try
        {
            ctx->set_options(boost::asio::ssl::context::default_workarounds |
                             boost::asio::ssl::context::no_sslv2 |
                             boost::asio::ssl::context::no_sslv3 |
                             boost::asio::ssl::context::single_dh_use);
        }
        catch (std::exception &e)
        {
            std::cout << "TLS init error: " << e.what() << std::endl;
        }

        return ctx;
    }
};

int main()
{
    WebSocketClient client("ethereum-rpc.publicnode.com");
    client.run();
    return 0;
}
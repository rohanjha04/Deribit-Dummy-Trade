#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <fstream>

std::ofstream log_file("output.txt"); // Log file to store response messages

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
//Define the client_id and client_secret for authentication. Test network credentials are used here, ideally should not be pushed to a public repository but eh, it's a test network.
std::string client_id = "OJMM9Ih_";
std::string client_secret = "XtDofirmP_zUNuqTjGrktCJgD80aEjzefh5s5BMJU2s";

class WebSocketClient {
private:
    client ws_client;
    websocketpp::connection_hdl ws_hdl;
    std::string uri;
    std::atomic<bool> is_connected; // Thread-safe flag to check connection status
public:
    WebSocketClient(const std::string &uri) : uri(uri), is_connected(false) { // Constructor
        ws_client.init_asio();
        ws_client.set_tls_init_handler([this](websocketpp::connection_hdl) {
            return std::make_shared<websocketpp::lib::asio::ssl::context>(websocketpp::lib::asio::ssl::context::tlsv12);
        });
        ws_client.set_message_handler([this](websocketpp::connection_hdl hdl, client::message_ptr msg) {
            on_message(hdl, msg);
        });
        ws_client.set_open_handler([this](websocketpp::connection_hdl hdl) {
            ws_hdl = hdl;
            is_connected = true; // Set connection flag
            std::cout << "Connection established" << std::endl;
        });
        ws_client.set_fail_handler([this](websocketpp::connection_hdl hdl) {
            std::cerr << "Connection failed" << std::endl;
        });
        ws_client.set_close_handler([this](websocketpp::connection_hdl hdl) {
            is_connected = false; // Reset connection flag
            std::cout << "Connection closed" << std::endl;
        });
        //mute all logs
        ws_client.clear_access_channels(websocketpp::log::alevel::all);
    }

    void connect() {
        websocketpp::lib::error_code ec;
        client::connection_ptr con = ws_client.get_connection(uri, ec);
        if (ec) {
            std::cerr << "Connection failed: " << ec.message() << std::endl;
            return;
        }
        ws_client.connect(con);
        // Run WebSocket client in a separate thread to avoid blocking
        ws_client.run();
    }

    // void send_json(const rapidjson::Document &json_data) { // Send JSON data if required
    void send_json(const std::string &json_data) {
        if (!is_connected) {
            std::cerr << "Cannot send message, not connected." << std::endl;
            while(!is_connected){
                std::this_thread::yield();// Yield CPU while waiting for connection
            }
        }

        websocketpp::lib::error_code ec;
        //Uncomment the following lines if you want to use rapidjson for sending data too. Slow but more flexible.
        // Convert RapidJSON Document to string
        // rapidjson::StringBuffer buffer;
        // rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        // json_data.Accept(writer);
        // std::string msg = buffer.GetString();

        //ws_client.send(ws_hdl, msg, websocketpp::frame::opcode::text, ec);
        ws_client.send(ws_hdl, json_data, websocketpp::frame::opcode::text, ec);

        if (ec) {
            std::cerr << "Send failed: " << ec.message() << std::endl;
        } //else {
            //std::cout << "Sent message: " << json_data << std::endl;
            // std::cout << "Sent message: " << std::endl;
        //}
    }

    void authenticate(){
        ////If required to use json for sending data, a working example is shown below
        // rapidjson::Document json_data;
        // json_data.SetObject();
        // rapidjson::Document::AllocatorType& allocator = json_data.GetAllocator();

        // // Add top-level keys
        // json_data.AddMember("jsonrpc", "2.0", allocator);
        // json_data.AddMember("id", 9929, allocator);
        // json_data.AddMember("method", "public/auth", allocator);
        //  // Create and populate the "params" object
        // rapidjson::Value params(rapidjson::kObjectType);
        // params.AddMember("grant_type", "client_credentials", allocator);
        // params.AddMember("client_id", client_id, allocator);
        // params.AddMember("client_secret", client_secret, allocator);

        // // Add the "params" object to the main JSON object
        // json_data.AddMember("params", params, allocator);
        
        std::string json_data = "{\"jsonrpc\":\"2.0\",\"id\":9929,\"method\":\"public/auth\",\"params\":{\"grant_type\":\"client_credentials\",\"client_id\":\"" + client_id + "\",\"client_secret\":\"" + client_secret + "\"}}";
        //send the json data
        send_json(json_data);
    }

    void place_order_buy(const std::string &inst_name, const std::string &mkt_type, const std::string &mkt_label, const std::string &amt, const std::string &extra_params = "") {
        
        ////If required to use json for sending data, a working example is shown below
        // Initialize JSON document and allocator
        // rapidjson::Document json_data;
        // json_data.SetObject();
        // auto& allocator = json_data.GetAllocator();

        // // Add top-level keys directly
        // json_data.AddMember("jsonrpc", "2.0", allocator);
        // json_data.AddMember("id", 9930, allocator);
        // json_data.AddMember("method", "private/buy", allocator);

        // // Add the "params" object with inlined member setup
        // rapidjson::Value params(rapidjson::kObjectType);
        // params.AddMember("instrument_name", rapidjson::Value(inst_name.c_str(), allocator), allocator);
        // params.AddMember("amount", amt, allocator);
        // params.AddMember("type", rapidjson::Value(mkt_type.c_str(), allocator), allocator);
        // params.AddMember("label", rapidjson::Value(mkt_label.c_str(), allocator), allocator);

        // // Add "params" to the main JSON object
        // json_data.AddMember("params", params, allocator);
        

        //direct string formatting
        std::string json_data = "{\"jsonrpc\":\"2.0\",\"id\":9930,\"method\":\"private/buy\",\"params\":{\"instrument_name\":\"" + inst_name + "\",\"amount\":" + amt + ",\"type\":\"" + mkt_type + "\",\"label\":\"" + mkt_label + "\" " + extra_params + "}}";

        // Send the JSON data
        send_json(json_data);
    }

    void place_order_sell(const std::string &inst_name, const std::string &type, const std::string &trigger, const std::string &amt, const std::string &price, const std::string &trigger_price, const std::string &extra_params = "") {
        std::string json_data = "{\"jsonrpc\":\"2.0\",\"id\":9931,\"method\":\"private/sell\",\"params\":{\"instrument_name\":\"" + inst_name + "\",\"amount\":" + amt + ",\"type\":\"" + type + "\",\"price\":" + price + ",\"trigger_price\":" + trigger_price + ",\"trigger\":\"" + trigger + "\" " + extra_params + "}}";
        send_json(json_data);
    }

    void cancel_order(const std::string &order_id) {
        std::string json_data = "{\"jsonrpc\":\"2.0\",\"id\":9932,\"method\":\"private/cancel\",\"params\":{\"order_id\":\"" + order_id + "\"}}";
        send_json(json_data);
    }

    void modify_order(const std::string &order_id, const std::string &amt, const std::string &price, const std::string &extra_params = "") {
        std::string json_data = "{\"jsonrpc\":\"2.0\",\"id\":9933,\"method\":\"private/edit\",\"params\":{\"order_id\":\"" + order_id + "\",\"amount\":" + amt + ",\"price\":" + price  + extra_params + "}}";
        send_json(json_data);
    }

    void get_orderbook(const std::string &inst_name, const std::string &depth) {
        std::string json_data = "{\"jsonrpc\":\"2.0\",\"id\":9934,\"method\":\"public/get_order_book\",\"params\":{\"instrument_name\":\"" + inst_name + "\",\"depth\":" + depth + "}}";
        send_json(json_data);
    }

    void view_instruments(const std::string &currency, const std::string &extra_params = "") {
        std::string json_data = "{\"jsonrpc\":\"2.0\",\"id\":9935,\"method\":\"public/get_instruments\",\"params\":{\"currency\":\"" + currency + "\" " + extra_params + "}}";
        send_json(json_data);
    }

    void get_position(const std::string &inst_name) {
        std::string json_data = "{\"jsonrpc\":\"2.0\",\"id\":9936,\"method\":\"private/get_position\",\"params\":{\"instrument_name\":\"" + inst_name + "\"}}";
        send_json(json_data);
    }

    void subscribe_symbol(const std::string &channels) {
        std::string json_data = "{\"jsonrpc\":\"2.0\",\"id\":9937,\"method\":\"private/subscribe\",\"params\":{\"channels\":[" + channels + "]}}";
        send_json(json_data);
    }

    void close() {
        ws_client.close(ws_hdl, websocketpp::close::status::normal, "Closing connection");
    }
    /*
    id: 9929: Authenticate
    id: 9930: Place Buy Order
    id: 9931: Place Sell Order
    id: 9932: Cancel Order
    id: 9933: Modify Order
    id: 9934: Get Orderbook
    id: 9935: View Instruments
    id: 9936: Get Position
    id: 9937: Subscribe Symbol
    These ids can be used to identify the response messages and process them accordingly if required. Currently, the response messages are logged to a file named output.txt.
    */

    void show_menu() {
    std::cout << "\nSelect an action:\n"
              << "1. Place Buy Order\n"
              << "2. Place Sell Order\n"
              << "3. Cancel Order\n"
              << "4. Modify Order\n"
              << "5. Get Orderbook\n"
              << "6. Get Position\n"
              << "7. View Instruments\n"
              << "8. Subscribe Symbol\n"
              << "9. Exit\n"
              << "Enter choice: ";
    }

    void trade(){
        authenticate();
        while(true){
            show_menu();
            int choice;
            std::cin >> choice;
            std::cin.ignore();
            switch(choice){
                case 1:{
                    std::string inst_name, mkt_type, mkt_label, amt, extra_params;
                    std::cout << "Enter instrument name: ";
                    std::getline(std::cin, inst_name);
                    std::cout << "Enter market type: ";
                    std::getline(std::cin, mkt_type);
                    std::cout << "Enter market label: ";
                    std::getline(std::cin, mkt_label);
                    std::cout << "Enter amount: ";
                    std::getline(std::cin, amt);
                    std::cout << "Enter extra parameters as comma-separated key-value pairs (optional): "; // example "time_in_force":"good_til_cancelled", "reduce_only":true
                    std::getline(std::cin, extra_params);
                    if(!extra_params.empty()) extra_params = "," + extra_params;
                    place_order_buy(inst_name, mkt_type, mkt_label, amt, extra_params);
                    break;
                }
                case 2:{
                    std::string inst_name, type, trigger, amt, price, trigger_price, extra_params;
                    std::cout << "Enter instrument name: ";
                    std::getline(std::cin, inst_name);
                    std::cout << "Enter type: ";
                    std::getline(std::cin, type);
                    std::cout << "Enter trigger: ";
                    std::getline(std::cin, trigger);
                    std::cout << "Enter amount: ";
                    std::getline(std::cin, amt);
                    std::cout << "Enter price: ";
                    std::getline(std::cin, price);
                    std::cout << "Enter trigger price: ";
                    std::getline(std::cin, trigger_price);
                    std::cout << "Enter extra parameters as comma-separated key-value pairs (optional): ";
                    std::getline(std::cin, extra_params);
                    if(!extra_params.empty()) extra_params = "," + extra_params;
                    place_order_sell(inst_name, type, trigger, amt, price, trigger_price, extra_params);
                    break;
                }
                case 3:{
                    std::string order_id;
                    std::cout << "Enter order ID: ";
                    std::getline(std::cin, order_id);
                    cancel_order(order_id);
                    break;
                }
                case 4:{
                    std::string order_id, amt, price, extra_params;
                    std::cout << "Enter order ID: ";
                    std::getline(std::cin, order_id);
                    std::cout << "Enter amount: ";
                    std::getline(std::cin, amt);
                    std::cout << "Enter price: ";
                    std::getline(std::cin, price);
                    std::cout << "Enter extra parameters as comma-separated key-value pairs (optional): ";
                    std::getline(std::cin, extra_params);
                    if(!extra_params.empty()) extra_params = "," + extra_params;
                    modify_order(order_id, amt, price, extra_params);
                    break;
                }
                case 5:{
                    std::string inst_name, depth;
                    std::cout << "Enter instrument name: ";
                    std::getline(std::cin, inst_name);
                    std::cout << "Enter depth: ";
                    std::getline(std::cin, depth);
                    get_orderbook(inst_name, depth);
                    break;
                }
                case 6:{
                    std::string inst_name;
                    std::cout << "Enter instrument name: ";
                    std::getline(std::cin, inst_name);
                    get_position(inst_name);
                    break;
                }
                case 7:{
                    std::string currency, extra_params;
                    std::cout << "Enter currency: ";
                    std::getline(std::cin, currency);
                    std::cout << "Enter extra parameters as comma-separated key-value pairs (optional): ";
                    std::getline(std::cin, extra_params);
                    if(!extra_params.empty()) extra_params = "," + extra_params;
                    view_instruments(currency, extra_params);
                    break;
                }
                case 8:{
                    std::string channels;
                    std::cout << "Enter channels: ";
                    std::getline(std::cin, channels);
                    subscribe_symbol(channels);
                    break;
                }
                case 9:{
                    close();
                    return;
                }
                default:{
                    std::cout << "Invalid choice. Please try again." << std::endl;
                    break;
                }
            }

        }
        // Code to measure latency, last measured latency was 250 microseconds
        // //start time
        // auto start = std::chrono::high_resolution_clock::now();
        // place_order_buy(inst_name, mkt_type, mkt_label, amt);
        // //end time
        // auto end = std::chrono::high_resolution_clock::now();
        // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        // std::cout << "Latency: " << duration.count() << " microseconds" << std::endl;
    }


private:
    void on_message(websocketpp::connection_hdl hdl, client::message_ptr msg) {
        //parse the message into a JSON document
        rapidjson::Document json_data;
        json_data.Parse(msg->get_payload().c_str());
        if (json_data.HasParseError()) {
            std::cerr << "Parse error: " << json_data.GetParseError() << std::endl;
            return;
        }
        //print the message
        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        json_data.Accept(writer);
        log_file << buffer.GetString() << std::endl;
        log_file.flush();
        //cout any error if received
        if(json_data.HasMember("error")){
            std::cerr << "Error: " << json_data["error"]["message"].GetString() << std::endl;
        }
    }
};

int main() {
    std::string uri = "wss://test.deribit.com/ws/api/v2"; 
    WebSocketClient ws_client(uri);
    std::vector<std::thread> threads;
   //spawn a thread to connect and run the websocket client
    threads.emplace_back(&WebSocketClient::connect, &ws_client);
    //trade in another thread
    threads.emplace_back(&WebSocketClient::trade, &ws_client);
    //wait for the threads to finish
    for (auto &thread : threads) {
        thread.join();
    }
    return 0;
}

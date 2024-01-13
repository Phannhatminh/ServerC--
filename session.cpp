#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <boost/asio.hpp>

using namespace boost::asio;
using CallbackLoginFunction = int (*)();
using CallbackMessageFunction = int (*)(std::string, boost::asio::ip::tcp::socket&);

class SessionManager {
private:
    std::map<int, int> sessionIDToSocketIndex;
    CallbackMessageFunction message_callback;
    CallbackLoginFunction login_callback;
    SessionManager() {
        std::cout << "SessionManager created\n";
    }
    ~SessionManager() {
        std::cout << "SessionManager destroyed\n";
    }
    // Make sure that there is at most one instance of SessionManager
    SessionManager(const SessionManager&) = delete; // delete copy constructor
    SessionManager& operator=(const SessionManager&) = delete;
public:
    static SessionManager& getInstance() {
        static SessionManager instance;
        return instance;
    }
    void setMessageCallback(CallbackMessageFunction cb) {
        message_callback = cb;
    }
    void setLoginCallback(CallbackLoginFunction cb) {
        login_callback = cb;
    }
    bool checkSessionID() { //method for checking if sessionID exists in the map
        return true;
    }
    bool checkLoginRequest() { //method for checking if the message is a login request
        return true;
    }
    bool checkMessage() { //method for checking if the message is a normal message
        return true;
    }
    int sendTo(const std::string& message, boost::asio::ip::tcp::socket& socket) {
        try {
            boost::system::error_code ec;

            // Convert the string message to a buffer and send it using the socket
            boost::asio::write(socket, boost::asio::buffer(message), ec);

            if (ec) {
                std::cerr << "Failed to send message: " << ec.message() << std::endl;
                return -1;  // Return -1 to indicate failure
            } else {
                std::cout << "Message sent successfully!" << std::endl;
                return 0;  // Return 0 to indicate success
            }
        } catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
            return -1;  // Return -1 to indicate failure
        }
    }
    int receive() {
        return 0;
    }
    void addSession(int sessionID, int socketHandle) { // Modify the parameter type to int
        sessionIDToSocketIndex[sessionID] = socketHandle;
    }

    int listenForConnection() {
        // Create an io_context object
        io_context io_context;

        // Create an endpoint to accept connections on port 8080
        ip::tcp::endpoint endpoint(ip::tcp::v4(), 8080);

        // Create an acceptor to listen for incoming connections
        ip::tcp::acceptor acceptor(io_context, endpoint);

        while (true) {
            // Create a socket and accept a new connection
            ip::tcp::socket socket(io_context);
            acceptor.accept(socket);
            if (checkSessionID()) {
                char sharedBuffer[1024];
                size_t len = socket.read_some(boost::asio::buffer(sharedBuffer, sizeof(sharedBuffer)));
                std::string message(sharedBuffer, len);
                return message_callback(message, socket);
            }
            else if (checkLoginRequest()) { //handle login request, add a sessionID and socket and return sessionID
                addSession(sessionIDToSocketIndex.size(), socket.native_handle());
                return login_callback();
            }
            else { 
                return -1;
            }
        }
    }
};

//Write the code for echoing the message back to the client
//Test
//Then write the code for login request and session checking
//Test
//Then write the code for logout request
//Test
//Then write the code for automatic logout
//Test
//Then write the code for handling general messages
//Test
//Then write the code for defining messages structure
//Test


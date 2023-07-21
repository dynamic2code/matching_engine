#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // Create a TCP socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // Set up server address
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080); // Server port 8080
    if (inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr)) <= 0) {
        std::cerr << "Failed to set up server address." << std::endl;
        close(clientSocket);
        return 1;
    }

    // Connect to the server
    if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1) {
        std::cerr << "Failed to connect to the server." << std::endl;
        close(clientSocket);
        return 1;
    }

    std::cout << "Connected to the server." << std::endl;
    std::cout << "For operation with the server, use the following keys as shown." << std::endl;

    std::string orderId, orderType, orderPrice, orderQuantity;
    std::string userInput;

    while (true) {
        std::cout << "Enter option (1: New Order, 2: Modify Order, 3: View Order, 4: Delete Order, exit: Close connection): ";
        std::cin >> userInput;

        if (userInput == "exit")
            break;

        // Send user option to the server
        ssize_t bytesSent = send(clientSocket, userInput.c_str(), userInput.length(), 0);
        if (bytesSent != userInput.length()) {
            std::cerr << "Failed to send user option to the server." << std::endl;
            close(clientSocket);
            return 1;
        }

        // Receive response from the server
        char buffer[1024];
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            std::cerr << "Failed to receive response from the server." << std::endl;
            close(clientSocket);
            return 1;
        }

        std::string response(buffer, bytesRead);
        std::cout << "Received response: " << response << std::endl;

        // Handle user option
        switch (std::stoi(userInput)) {
            case 1: {
                // Ask for order details from the user
                std::cout << "Enter order ID: ";
                std::cin >> orderId;
                std::cout << "Enter order type (buy/sell): ";
                std::cin >> orderType;
                std::cout << "Enter order price: ";
                std::cin >> orderPrice;
                std::cout << "Enter order quantity: ";
                std::cin >> orderQuantity;

                // Construct the order data string
                std::string orderData = orderId + "|" + orderType + "|" + orderPrice + "|" + orderQuantity;

                // Send order data to the server
                bytesSent = send(clientSocket, orderData.c_str(), orderData.length(), 0);
                std::cout<< bytesSent;
                if (bytesSent != orderData.length()) {
                    std::cerr << "Failed to send order data to the server." << std::endl;
                    close(clientSocket);
                    return 1;
                }

                break;
            }
            case 2:
                std::cout << " case two";
                // Ask for order details from the user
                std::cout << "Enter order ID: ";
                std::cin >> orderId;
                std::cout << "Enter order type (buy/sell): ";
                std::cin >> orderType;
                std::cout << "Enter order price: ";
                std::cin >> orderPrice;
                std::cout << "Enter order quantity: ";
                std::cin >> orderQuantity;
                break;

            case 3:
                // Logic for viewing order from server
                // ...
                break;
            case 4:
                // Logic for deleting order in server
                // ...
                break;
            default:
                std::cout << "Invalid option. Please try again." << std::endl;
                break;
        }
    }

    // Close the client socket
    close(clientSocket);

    return 0;
}


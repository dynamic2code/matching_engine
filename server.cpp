#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>

class Order {
private:
    int id;
    std::string type;
    double price;
    int quantity;

public:
    // Constructor
    Order(int orderId, const std::string& orderType, double orderPrice, int orderQuantity)
            : id(orderId), type(orderType), price(orderPrice), quantity(orderQuantity)
    {
    }

    // Getter methods
    int getId() const {
        return id;
    }

    std::string getType() const {
        return type;
    }

    double getPrice() const {
        return price;
    }

    int getQuantity() const {
        return quantity;
    }

    // Setter methods
    void setPrice(double newPrice) {
        price = newPrice;
    }

    void setQuantity(int newQuantity) {
        quantity = newQuantity;
    }
};

class OrderBook {
private:
    std::vector<Order> buyOrders;
    std::vector<Order> sellOrders;

public:
    void addBuyOrder(const Order& order) {
        buyOrders.push_back(order);
    }

    void addSellOrder(const Order& order) {
        sellOrders.push_back(order);
    }
    void modifyOrder(int orderId, const std::string& orderType, double newPrice, int newQuantity) {
        std::vector<Order>* orders;

        if (orderType == "buy") {
            orders = &buyOrders;
        } else if (orderType == "sell") {
            orders = &sellOrders;
        } else {
            std::cout << "Invalid order type." << std::endl;
            return;
        }

        auto it = std::find_if(orders->begin(), orders->end(), [orderId](const Order& order) {
            return order.getId() == orderId;
        });

        if (it != orders->end()) {
            it->setPrice(newPrice);
            it->setQuantity(newQuantity);
            std::cout << "Order ID: " << it->getId() << " modified successfully." << std::endl;
        } else {
            std::cout << "Order ID: " << orderId << " not found." << std::endl;
        }
    }
    std::string getOrderBookSummary() const {
        std::string summary = "Order Book Summary:\n";

        // Add buy orders to the summary
        summary += "Buy Orders:\n";
        for (const auto& buyOrder : buyOrders) {
            summary += "Order ID: " + std::to_string(buyOrder.getId()) + "\n";
            summary += "Order Type: " + buyOrder.getType() + "\n";
            summary += "Order Price: " + std::to_string(buyOrder.getPrice()) + "\n";
            summary += "Order Quantity: " + std::to_string(buyOrder.getQuantity()) + "\n";
            summary += "-----------------------\n";
        }

        // Add sell orders to the summary
        summary += "Sell Orders:\n";
        for (const auto& sellOrder : sellOrders) {
            summary += "Order ID: " + std::to_string(sellOrder.getId()) + "\n";
            summary += "Order Type: " + sellOrder.getType() + "\n";
            summary += "Order Price: " + std::to_string(sellOrder.getPrice()) + "\n";
            summary += "Order Quantity: " + std::to_string(sellOrder.getQuantity()) + "\n";
            summary += "-----------------------\n";
        }

        return summary;
    }


    void deleteOrder(int orderId, const std::string& orderType) {
        std::vector<Order>* orders;

        if (orderType == "buy") {
            orders = &buyOrders;
        } else if (orderType == "sell") {
            orders = &sellOrders;
        } else {
            std::cout << "Invalid order type." << std::endl;
            return;
        }

        auto it = std::find_if(orders->begin(), orders->end(), [orderId](const Order& order) {
            return order.getId() == orderId;
        });

        if (it != orders->end()) {
            orders->erase(it);
            std::cout << "Order ID: " << orderId << " deleted successfully." << std::endl;
        } else {
            std::cout << "Order ID: " << orderId << " not found." << std::endl;
        }
    }

    void viewOrder(int orderId, const std::string& orderType) {
        std::vector<Order>* orders;

        if (orderType == "buy") {
            orders = &buyOrders;
        } else if (orderType == "sell") {
            orders = &sellOrders;
        } else {
            std::cout << "Invalid order type." << std::endl;
            return;
        }

        auto it = std::find_if(orders->begin(), orders->end(), [orderId](const Order& order) {
            return order.getId() == orderId;
        });

        if (it != orders->end()) {
            const Order& order = *it;
            std::cout << "Order ID: " << order.getId() << std::endl;
            std::cout << "Order Type: " << order.getType() << std::endl;
            std::cout << "Order Price: " << order.getPrice() << std::endl;
            std::cout << "Order Quantity: " << order.getQuantity() << std::endl;
        } else {
            std::cout << "Order ID: " << orderId << " not found." << std::endl;
        }
    }

    void matchOrders() {
        for (const auto& sellOrder : sellOrders) {
            for (auto it = buyOrders.begin(); it != buyOrders.end(); ++it) {
                const auto& buyOrder = *it;
                if (sellOrder.getPrice() == buyOrder.getPrice() && sellOrder.getQuantity() == buyOrder.getQuantity()) {
                    std::cout << "Matched Sell Order ID: " << sellOrder.getId() << " with Buy Order ID: " << buyOrder.getId() << std::endl;
                    buyOrders.erase(it);
                    break;
                }
            }
        }
    }
};

int main() {
    OrderBook orderBook;

    // Create a TCP socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // Set up server address
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080); // Server listens on port 8080

    // Bind the socket to the server address
    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1) {
        std::cerr << "Failed to bind socket to address." << std::endl;
        close(serverSocket);
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, SOMAXCONN) == -1) {
        std::cerr << "Failed to listen for connections." << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Server is running and listening for connections..." << std::endl;

    while (true) {
        // Accept a client connection
        sockaddr_in clientAddress{};
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressLength);
        if (clientSocket == -1) {
            std::cerr << "Failed to accept client connection." << std::endl;
            continue;
        }

        std::cout << "Client connected: " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << std::endl;

        // Receive user option from the client
        char buffer[1024];
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            std::cerr << "Failed to receive user option from client." << std::endl;
            close(clientSocket);
            continue;
        }

        std::string userOption(buffer, bytesRead);
        std::cout << "Received user option: " << userOption << std::endl;

        // Process user option
        std::string response;

        switch (std::stoi(userOption)) {
            case 1:
                response = "Option 1 selected. Enter new order details.";
                break;
            case 2:
                response = "Option 2 selected. Enter order ID and new details to modify.";
                break;
            case 3:
                response = "Option 3 selected. Order book: " + orderBook.getOrderBookSummary();
                break;
            case 4:
                response = "Option 4 selected. Enter order ID to delete.";
                
                break;
            default:
                response = "Invalid option. Please try again.";
                break;
        }

        // Send response to the client
        ssize_t bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
        if (bytesSent != response.length()) {
            std::cerr << "Failed to send response to client." << std::endl;
        }

        // Close the client socket
        close(clientSocket);
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}


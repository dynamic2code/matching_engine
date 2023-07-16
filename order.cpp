#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>

class Order {
private:
    int id;
    std::string type;
    double price;
    int quantity;
    std::time_t timestamp;

public:
    // Constructor
    Order(int orderId, const std::string& orderType, double orderPrice, int orderQuantity)
            : id(orderId), type(orderType), price(orderPrice), quantity(orderQuantity), timestamp(std::time(nullptr))
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

    std::time_t getTimestamp() const {
        return timestamp;
    }
};

class OrderBook {
private:
    std::vector<Order> orders;

public:
    void addOrder(const Order& order) {
        orders.push_back(order);
    }

    void printOrders() {
        for (const auto& order : orders) {
            std::cout << "Order ID: " << order.getId() << std::endl;
            std::cout << "Order Type: " << order.getType() << std::endl;
            std::cout << "Order Price: " << order.getPrice() << std::endl;
            std::cout << "Order Quantity: " << order.getQuantity() << std::endl;

            // Format the timestamp
            std::time_t timestamp = order.getTimestamp();
            std::tm* timeinfo = std::gmtime(&timestamp);
            char buffer[80];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

            std::cout << "Timestamp: " << buffer << std::endl;
            std::cout << std::endl;
        }
    }
};

int main() {
    OrderBook orderBook;

    // Create and add orders to the order book
    Order order1(1, "buy", 10.99, 5);
    orderBook.addOrder(order1);

    Order order2(2, "sell", 15.49, 8);
    orderBook.addOrder(order2);

    // Print the orders with timestamps
    orderBook.printOrders();

    return 0;
}

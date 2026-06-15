#include <iostream>
#include <vector>
#include <string>

//Plan is to design an order matching engine


//We need to manage orders/commands
//There will be buy orders
//There will be sell orders


//Then we need a central interface/engine for handling this
//We need to keep track of the orders in the order they, possibly using a queue
//We need to keep track of current stock price
//
// Each order will contain, in the most basic sense:
// 1. Side (buy/sell)
// 2. Quantity
// 3. Price (for limit orders)
//
// So we need to take input by the user which gives us these 3 details
// How do we designate the total pool of available shares to buy?
// Handle order cancellation: locate the order via hash map; remove from its queue; remove the price level if the queue is empty
// We want to log these events and output them to the user?

enum class OrderType{
    Buy,
    Sell
};

struct Order{
    OrderType orderType;
    int quantity;
    int price;
};

int main(){
    Order testOrder;

    std::vector<Order> buyOrders;
    buyOrders.reserve(100000);  // allocate once upfront

    std::vector<Order> sellOrders;
    sellOrders.reserve(100000);  // allocate once upfront

    std::string inputString;
    while (true)
    {
        std::cout << "Enter order type: \n" << std::endl;
        std::cin >> inputString;

        if (inputString == "q") {
            break;
        }
        if (inputString == "buy") {
            testOrder.orderType = OrderType::Buy;
        }
        else if (inputString == "sell") {
            testOrder.orderType = OrderType::Sell;
        }
        else {
            std::cout << "Invalid order type. Enter again. \n";
            continue;
        }

        std::cout << "Enter stock quantity: \n" << std::endl;
        std::cin >> testOrder.quantity;

        std::cout << "Enter stock price in pence/cents/ticks: \n" << std::endl;
        std::cin >> testOrder.price;

        switch (testOrder.orderType){
            case OrderType::Buy:
            {
                //Let's do order matching now
                //For a Buy order we don't want to buy anything above the user given price
                //For a Sell order we don't want to sell anything below the user given price

                while(testOrder.quantity!= 0)
                {
                    if(sellOrders.empty())
                    {
                        buyOrders.push_back(testOrder);
                        break;
                    }

                    auto lowestPriceIt = sellOrders.begin();

                    for(auto it = sellOrders.begin(); it != sellOrders.end(); ++it) //The loop will find the lowest price in the sellOrders queue
                    {
                        if(it->price < lowestPriceIt->price)
                        {
                            lowestPriceIt = it;
                        }
                    }

                    if(testOrder.price >= lowestPriceIt->price)
                    {
                        if(lowestPriceIt->quantity <= testOrder.quantity) //Now we check to see if there are enough stocks in the lowest priced sell order to complete the order in one go
                        {
                            std::cout << "Buy order executes at price: " << lowestPriceIt->price << " for quantity: " << lowestPriceIt->quantity << std::endl;
                            testOrder.quantity = testOrder.quantity - lowestPriceIt->quantity; //this gives us how many shares from our order we still have remaining to buy
                            sellOrders.erase(lowestPriceIt); //since we have bought all the orders being sold at the lowestPrice, we can remove this vector element
                        }
                        else if(lowestPriceIt->quantity > testOrder.quantity)
                        {
                            std::cout << "Buy order executes at price: " << lowestPriceIt->price << " for quantity: " << testOrder.quantity << std::endl;
                            lowestPriceIt->quantity = lowestPriceIt->quantity - testOrder.quantity; //there are plenty of stocks remaining after buying at lowestPrice, so update quantity of the price point in the vector
                            testOrder.quantity = 0;
                            break;
                        }
                    }
                    else
                    {
                        buyOrders.push_back(testOrder);
                        std::cout << "Buy order at price: " << testOrder.price << " for quantity: " << testOrder.quantity << "set to pending" << std::endl; //resting order
                        break;
                    }
                }
                break;
            }
            case OrderType::Sell:
                sellOrders.push_back(testOrder);
                std::cout << "Sell order at price: " << testOrder.price << " for quantity: " << testOrder.quantity << " set to pending" << std::endl; //resting order
                break;
        }
        std::cout << "Buy orders: " << buyOrders.size() << "\n";
        std::cout << "Sell orders: " << sellOrders.size() << "\n";
    }

    return 0;
}


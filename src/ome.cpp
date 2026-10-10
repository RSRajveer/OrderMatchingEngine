#include "inc/ome.h"

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


void OrderMatchingEngine::orderMatchingLogic(Order* user_order)
{
    if(user_order->orderType == OrderType::Buy)
    {
        while(user_order->quantity!= 0)
        {
            if(sellOrders.empty())
            {
                buyOrders[user_order->price].push(*user_order);
                std::cout << "No shares available to buy. Order pending." << std::endl;
                break;
            }

            if(user_order->price >= sellOrders.begin()->first)
            {
                Order& lowest = sellOrders.begin()->second.front();

                if(lowest.quantity <= user_order->quantity)
                {
                    //sellOrders.erase(lowest.price);
                    user_order->quantity = user_order->quantity - lowest.quantity;
                    sellOrders.begin()->second.pop();
                    if(sellOrders.begin()->second.empty())
                    {
                        sellOrders.erase(sellOrders.begin());
                    }
                }
                else
                {
                    lowest.quantity = lowest.quantity - user_order->quantity;
                    user_order->quantity = 0;
                    std::cout << "Buy order executed." << std::endl;
                }
            }
            else
            {
                buyOrders[user_order->price].push(*user_order);
                std::cout << "No shares available to buy. Order pending." << std::endl;
                break;
            }

        }
        std::cout << "Buy orders: " << buyOrders.size() << "\n";
        std::cout << "Sell orders: " << sellOrders.size() << "\n";
    }
    else if(user_order->orderType == OrderType::Sell)
    {
        while(user_order->quantity!= 0)
        {
            if(buyOrders.empty())
            {
                sellOrders[user_order->price].push(*user_order);
                std::cout << "No shares available to sell. Sell order pending." << std::endl;
                break;
            }

            if(user_order->price <= buyOrders.rbegin()->first)
            {
                Order& highest = buyOrders.rbegin()->second.front();

                if(highest.quantity > user_order->quantity)
                {
                    highest.quantity = highest.quantity - user_order->quantity;
                    user_order->quantity = 0;
                    std::cout << "Sell order executed." << std::endl;
                }
                else
                {
                    user_order->quantity = user_order->quantity - highest.quantity;
                    highest.quantity = 0;
                    buyOrders.rbegin()->second.pop();
                    if(buyOrders.rbegin()->second.empty())
                    {
                        int bestBid = buyOrders.rbegin()->first;
                        buyOrders.erase(bestBid);
                    }
                }
            }
            else
            {
                sellOrders[user_order->price].push(*user_order);
                std::cout << "Sell order can't be executed. Order pending." << std::endl;
                break;
            }
        }
    }
    return;
}


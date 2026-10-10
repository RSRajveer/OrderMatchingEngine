#include "inc/ome.h"

int main()
{
    std::cout << "ORDER MATCHING ENGINE PROCESS STARTED" << std::endl;

    int order_type = 0;
    int user_price = 0;
    int user_quantity = 0;

    while(true)
    {
        std::cout << "Enter the order type. 1 to Buy and 2 for sell:" << std::endl;
        std::cin >> order_type;

        if(order_type == 0)
        {
            break;
        }

        std::cout << "Enter the order price:" << std::endl;
        std::cin >> user_price;

        std::cout << "Enter the order quantity:";
        std::cin >> user_quantity;

        Order user_order;
        if(order_type == 1)
        {
            user_order.orderType = OrderType::Buy;
        }
        else if(order_type == 2)
        {
            user_order.orderType = OrderType::Sell;
        }

        user_order.price = user_price;
        user_order.quantity = user_quantity;

        OrderMatchingEngine ome_process;
        ome_process.orderMatchingLogic(&user_order);
    }

    return 0;
}
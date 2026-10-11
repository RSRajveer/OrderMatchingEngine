#include <iostream>
#include <string>
#include <map>
#include <stdlib.h>
#include <queue>

enum class OrderType : uint8_t
{
    Buy,
    Sell
};

struct Order{
    OrderType orderType;
    int quantity;
    int price;
};

class OrderMatchingEngine
{
    public:
        OrderMatchingEngine() = default;

        void orderMatchingLogic(Order* user_order);

        [[nodiscard]] size_t  buyLevels()  const { return buyOrders.size(); }
        [[nodiscard]] size_t  sellLevels() const { return sellOrders.size(); }

    private:
        std::map<int, std::queue<Order>> buyOrders;

        std::map<int, std::queue<Order>> sellOrders;
};
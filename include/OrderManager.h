#ifndef TICKETSYSTEM_ORDERMANAGER_H
#define TICKETSYSTEM_ORDERMANAGER_H

#include <iostream>
#include "BPlusTree.hpp"
#include "Vector.h"
#include "MemoryRiver.h"
#include "Order.h"
#include "TrainManager.h"

class OrderManager {
private:
    BPlusTree<OrderKey, int, 50> order_index;
    MemoryRiver<Order, 3> order_data;
    BPlusTree<PendingOrderKey, PendingOrder, 50> pending_index;
    int order_cnt;



public:
    OrderManager();
    ~OrderManager();
    int buy_ticket(const std::string& u, const std::string& i, const std::string& d, const std::string& n,
        const std::string& f,const std::string& t, bool q, int timestamp, TrainManager& train_manager);
    int query_order(const std::string& u, sjtu::vector<Order>& res);
    int refund_ticket(const std::string& u, int n, TrainManager& train_manager);
    void process_pending(const std::string& trainID, const Date& date, TrainManager& train_manager);
    void clean();

};



#endif //TICKETSYSTEM_ORDERMANAGER_H
#include "../include/OrderManager.h"
#include <cstring>

OrderManager::OrderManager() : order_index("order_index"), order_data("order_data"), pending_index("pending_index") {
    order_data.initialise();
    order_data.get_info(order_cnt, 1);
}

OrderManager::~OrderManager() {
    order_data.write_info(order_cnt, 1);
    order_data.close();
}

int OrderManager::buy_ticket(const std::string &u, const std::string &i, const std::string &d, const std::string &n, const std::string &f, const std::string &t, bool q, int timestamp, TrainManager &train_manager) {
    int num = std::stoi(n);
    Train train;
    Date start_date;
    int from_idx = -1;
    int to_idx = -1;
    //先独立查一次站点索引，使得候补也能拿到from/to_idx
    Train tmp;
    if (!train_manager.get_train(i, tmp)) {
        return -1;
    }
    if (!tmp.released) {
        return -1;
    }
    from_idx = tmp.find_station(f.c_str());
    to_idx = tmp.find_station(t.c_str());
    if (from_idx == -1 || to_idx == -1 || from_idx >= to_idx) {
        return -1;
    }
    Date user_date;
    user_date.parse(d);
    int price = train_manager.try_buy(i, user_date, from_idx, to_idx, num, train, start_date);
    if (price == -1) {
        return -1;
    }
    ++order_cnt;
    if (price > 0) {
        Order order;
        order.orderID = order_cnt;
        order.timestamp = timestamp;
        order.status = OrderStatus::SUCCESS;
        std::strncpy(order.username, u.c_str(), 20);
        std::strncpy(order.trainID, i.c_str(), 20);
        order.start_date = start_date;
        order.user_date = user_date;
        std::strncpy(order.from_station, f.c_str(), 30);
        std::strncpy(order.to_station, t.c_str(), 30);
        order.from_idx = from_idx;
        order.to_idx = to_idx;
        order.leave_time = train.get_leave_time(from_idx, start_date);
        order.arrive_time = train.get_arrive_time(to_idx, start_date);
        order.price = train.get_price(from_idx, to_idx);
        order.num = num;
        int order_pos = order_data.write(order);
        OrderKey okey(u, order_cnt);
        order_index.insert(okey,order_pos);
        return order.price * num;
    }
    else {
        //余票不足
        if (!q) {
            --order_cnt;
            return -1;
        }
        Order order;
        order.orderID = order_cnt;
        order.timestamp = timestamp;
        order.status = OrderStatus::PENDING;
        std::strncpy(order.username, u.c_str(), 20);
        std::strncpy(order.trainID, i.c_str(), 20);
        order.start_date = start_date;
        order.user_date = user_date;
        std::strncpy(order.from_station, f.c_str(), 30);
        std::strncpy(order.to_station, i.c_str(), 30);
        order.from_idx = from_idx;
        order.to_idx = to_idx;
        order.leave_time = train.get_leave_time(from_idx, start_date);
        order.arrive_time = train.get_arrive_time(to_idx, start_date);
        order.price = train.get_price(from_idx, to_idx);
        order.num = num;
        int order_pos = order_data.write(order);
        OrderKey okey(u, order_cnt);
        order_index.insert(okey,order_pos);
        PendingOrderKey pkey(i, start_date, order_cnt);
        PendingOrder po;
        po.order_pos = order_pos;
        po.orderID = order_cnt;
        po.from_idx = from_idx;
        po.to_idx = to_idx;
        po.num = num;
        pending_index.insert(pkey,po);
        return -2;
    }
}

int OrderManager::query_order(const std::string &u, sjtu::vector<Order> &res) {
    OrderKey low(u, 0);
    OrderKey high(u, 2147483647);;
    sjtu::vector<int> positions;
    order_index.find_range(low, high, positions);
    std::cout << positions.size() << "\n";
    for (int i = positions.size() - 1; i >= 0; --i) {
        Order order;
        order_data.read(order, positions[i]);
        order.print();
        if (i > 0) {
            std::cout << "\n";
        }
    }
    return 0;
}

int OrderManager::refund_ticket(const std::string &u, int n, TrainManager &train_manager) {
    OrderKey low(u, 0);
    OrderKey high(u, 2147483647);;
    sjtu::vector<int> positions;
    order_index.find_range(low, high, positions);
    int total_orders = positions.size();
    if (n <= 0 || n > total_orders) {
        return -1;
    }
    int target_pos = positions[total_orders - n];
    Order order;
    order_data.read(order,target_pos);
    if (order.status == OrderStatus::REFUNDED) {
        return -1;
    }
    if (order.status == OrderStatus::SUCCESS) {
        train_manager.refund_seats(order.trainID, order.start_date, order.from_idx, order.to_idx, order.num);
        order.status = OrderStatus::REFUNDED;
        order_data.update(order, target_pos);
        process_pending(order.trainID, order.start_date, train_manager);
    }
    else if (order.status == OrderStatus::PENDING) {
        PendingOrderKey pkey(order.trainID, order.start_date, order.orderID);
        pending_index.remove(pkey);
        order.status = OrderStatus::REFUNDED;
        order_data.update(order, target_pos);
    }
    return 0;
}

void OrderManager::clean() {
    order_index.clear();
    pending_index.clear();
    order_cnt = 0;
    order_data.close();
    order_data.initialise("order_data");
}

void OrderManager::process_pending(const std::string &trainID, const Date &date, TrainManager &train_manager) {
    PendingOrderKey low(trainID, date, 0);
    PendingOrderKey high(trainID, date, 2147483647);
    sjtu::vector<PendingOrder> pendings;
    pending_index.find_range(low, high, pendings);
    if (pendings.empty()) {
        return;
    }
    TicketLeft tl;
    int ticket_pos;
    if (!train_manager.get_ticket_left(trainID, date, tl, ticket_pos)) {
        return;
    }
    bool updated = false;
    for (int i = 0; i < pendings.size(); ++i) {
        const PendingOrder& po = pendings[i];
        int left = tl.min_tickets(po.from_idx, po.to_idx);
        if (left >= po.num) {
            tl.update(po.from_idx, po.to_idx, -po.num);
            updated = true;
            Order order;
            order_data.read(order, po.order_pos);
            order.status = OrderStatus::SUCCESS;
            order_data.update(order, po.order_pos);
            PendingOrderKey pkey(trainID, date, po.orderID);
            pending_index.remove(pkey);
        }
    }
    if (updated) {
        train_manager.update_ticket_left(ticket_pos, tl);
    }
}







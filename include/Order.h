#ifndef TICKETSYSTEM_ORDER_H
#define TICKETSYSTEM_ORDER_H

#include <iostream>
#include "DateTime.h"

enum OrderStatus {
    SUCCESS,
    PENDING,
    REFUNDED
};

class Order {
public:
    int orderID = 0;  //订单号
    int timestamp = 0; //下单时间
    OrderStatus status;
    char username[21]{};
    char trainID[21]{};
    Date start_date; //始发站时间
    Date user_date; //上车站时间
    char from_station[31]{};
    char to_station[31]{};
    int from_idx = 0;
    int to_idx = 0;
    DateTime leave_time;
    DateTime arrive_time;
    int price = 0;
    int num = 0;

    Order() = default;
};

struct OrderKey {
    char username[21];
    int orderID = 0;

    OrderKey();
    OrderKey(const std::string& str, int id);

    bool operator<(const OrderKey& other) const;
    bool operator==(const OrderKey& other) const;
};

//候补订单
struct PendingOrder {
    int order_pos = -1;
    int orderID = 0;
    int from_idx = 0;
    int to_idx = 0;
    int num = 0;

    PendingOrder() = default;
};

struct PendingOrderKey {
    char trainID[21]{};
    Date date;
    int orderID = 0;

    PendingOrderKey() = default;
    PendingOrderKey(const std::string& id, const Date& d, int orderid);
    bool operator<(const PendingOrderKey& other) const;
    bool operator==(const PendingOrderKey& other) const;

};

#endif
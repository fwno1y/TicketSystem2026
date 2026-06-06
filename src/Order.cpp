#include "../include/Order.h"
#include <cstring>

void Order::print() const {
    if (status == SUCCESS) {
        std::cout << "[success] ";
    }
    else if (status == PENDING) {
        std::cout << "[pending] ";
    }
    else {
        std::cout << "[refunded] ";
    }
    std::cout << trainID << " " << from_station << " " << leave_time.to_string() << " -> " << to_station << " " << arrive_time.to_string() << " " << price << " " << num;
}
OrderKey::OrderKey() {
    memset(username, 0, sizeof(username));
    orderID = 0;
}

OrderKey::OrderKey(const std::string &str, int id) {
    memset(username, 0, sizeof(username));
    std::strcpy(username, str.c_str());
    orderID = id;
}

bool OrderKey::operator<(const OrderKey &other) const {
    int cmp = std::strcmp(username,other.username);
    if (cmp != 0) {
        return cmp < 0;
    }
    return orderID < other.orderID;
}

bool OrderKey::operator==(const OrderKey &other) const {
    return std::strcmp(username, other.username) == 0 && orderID == other.orderID;
}

bool OrderKey::operator<=(const OrderKey &other) const {
    return *this < other || *this == other;
}

bool OrderKey::operator>(const OrderKey &other) const {
    return !(*this <= other);
}

bool OrderKey::operator>=(const OrderKey &other) const {
    return !(*this < other);
}

bool PendingOrder::operator<(const PendingOrder &other) const {
    if (orderID != other.orderID) {
        return orderID < other.orderID;
    }
    return order_pos < other.order_pos;
}

bool PendingOrder::operator==(const PendingOrder &other) const {
    return order_pos == other.order_pos && orderID == other.orderID && from_idx == other.from_idx && to_idx == other.to_idx &&  num == other.num;
}

PendingOrderKey::PendingOrderKey() {
    memset(trainID, 0, sizeof(trainID));
    orderID = 0;
}

PendingOrderKey::PendingOrderKey(const std::string& id, const Date &d, int orderid) : date(d), orderID(orderid) {
    std::strcpy(trainID, id.c_str());
}

bool PendingOrderKey::operator<(const PendingOrderKey &other) const {
    int cmp = std::strcmp(trainID, other.trainID);
    if (cmp != 0) {
        return cmp < 0;
    }
    if (!(date == other.date)) {
        return date < other.date;
    }
    return orderID < other.orderID;
}

bool PendingOrderKey::operator==(const PendingOrderKey &other) const {
    return std::strcmp(trainID, other.trainID) == 0 && date == other.date && orderID == other.orderID;
}

bool PendingOrderKey::operator<=(const PendingOrderKey &other) const {
    return *this == other || *this < other;
}

bool PendingOrderKey::operator>(const PendingOrderKey &other) const {
    return !(*this <= other);
}

bool PendingOrderKey::operator>=(const PendingOrderKey &other) const {
    return !(*this < other);
}


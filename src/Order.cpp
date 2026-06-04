#include "../include/Order.h"
#include<cstring>

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




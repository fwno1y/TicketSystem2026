#include "../include/User.h"
#include <iostream>
#include <cstring>
User::User(const std::string &u, const std::string &p, const std::string &n, const std::string &m, int g) {
    std::strcpy(username, u.c_str());
    std::strcpy(password, p.c_str());
    std::strcpy(name, n.c_str());
    std::strcpy(mailAddr, m.c_str());
    privilege = g;
}

User::~User() = default;

UserKey::UserKey() {
    memset(username, 0, sizeof(username));
}

UserKey::UserKey(const std::string &str) {
    memset(username, 0, sizeof(username));
    std::strcpy(username, str.c_str());
}

bool UserKey::operator<(const UserKey &other) const {
    return std::strcmp(username, other.username) < 0;
}

bool UserKey::operator==(const UserKey &other) const {
    return std::strcmp(username, other.username) == 0;
}





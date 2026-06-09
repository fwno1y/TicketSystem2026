#include "../include/User.h"
#include <iostream>
#include <cstring>
User::User(const std::string &u, const std::string &p, const std::string &n, const std::string &m, int g) {
    memset(username, 0, sizeof(username));
    memset(password, 0, sizeof(password));
    memset(name, 0, sizeof(name));
    memset(mailAddr, 0, sizeof(mailAddr));
    std::strncpy(username, u.c_str(), sizeof(username) - 1);
    username[sizeof(username) - 1] = '\0';
    std::strncpy(password, p.c_str(), sizeof(password) - 1);
    password[sizeof(password) - 1] = '\0';
    std::strncpy(name, n.c_str(), sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';
    std::strncpy(mailAddr, m.c_str(), sizeof(mailAddr) - 1);
    mailAddr[sizeof(mailAddr) - 1] = '\0';
    privilege = g;
}

User::~User() = default;

UserKey::UserKey() {
    memset(username, 0, sizeof(username));
}

UserKey::UserKey(const std::string &str) {
    memset(username, 0, sizeof(username));
    std::strncpy(username, str.c_str(), sizeof(username) - 1);
    username[sizeof(username) - 1] = '\0';
}

bool UserKey::operator<(const UserKey &other) const {
    return std::strcmp(username, other.username) < 0;
}

bool UserKey::operator==(const UserKey &other) const {
    return std::strcmp(username, other.username) == 0;
}





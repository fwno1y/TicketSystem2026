#include "../include/UserManager.h"
#include <cstring>

UserManager::UserManager() : user_index("user_index"), user_data("user_data") {
    user_data.initialise();
}

UserManager::~UserManager() {
    user_data.close();
}

bool UserManager::is_logged(const std::string &u) const {
    return logged_users.count(u) > 0;
}

int UserManager::add_user(const std::string &c, const std::string &u, const std::string &p, const std::string &n, const std::string &m, const std::string &g) {
    UserKey key(u);
    int pos = -1;
    if (user_index.empty()) {
        User first(u,p,n,m,10);
        pos = user_data.write(first);
        user_index.insert(key,pos);
        return 0;
    }
    if (user_index.find(key,pos)) {
        return -1;
    }
    if (!is_logged(c)) {
        return -1;
    }
    auto it = logged_users.find(c);
    if (it == logged_users.end()) {
        return -1;
    }
    int cur_p = it->second;
    int new_p = std::stoi(g);
    if (cur_p <= new_p) {
        return -1;
    }
    User new_user(u,p,n,m,new_p);
    pos = user_data.write(new_user);
    user_index.insert(key,pos);
    return 0;
}

int UserManager::login(const std::string &u, const std::string &p) {
    if (is_logged(u)) {
        return -1;
    }
    UserKey key(u);
    int pos;
    if (!user_index.find(key,pos)) {
        return -1;
    }
    User user;
    user_data.read(user,pos);
    if (std::strcmp(user.password, p.c_str()) != 0) {
        return -1;
    }
    logged_users[u] = user.privilege;
    return 0;
}

int UserManager::logout(const std::string &u) {
    if (!is_logged(u)) {
        return -1;
    }
    logged_users.erase(logged_users.find(u));
    return 0;
}

int UserManager::query_profile(const std::string &c, const std::string &u) {
    if (!is_logged(c)) {
        return -1;
    }
    UserKey key(u);
    int pos;
    if (!user_index.find(key,pos)) {
        return -1;
    }
    User user;
    user_data.read(user, pos);
    auto it = logged_users.find(c);
    if (it == logged_users.end()) {
        return -1;
    }
    int cur_p = it->second;
    if (c != u && cur_p <= user.privilege) {
        return -1;
    }
    std::cout << user.username << ' ' << user.name << ' ' << user.mailAddr << ' ' << user.privilege << '\n';
    return 0;
}

int UserManager::modify_profile(const std::string &c, const std::string &u, const std::string &p, const std::string &n, const std::string &m, const std::string &g) {
    if (!is_logged(c)) {
        return -1;
    }
    UserKey key(u);
    int pos;
    if (!user_index.find(key,pos)) {
        return -1;
    }
    User user;
    user_data.read(user,pos);
    auto it = logged_users.find(c);
    if (it == logged_users.end()) {
        return -1;
    }
    int cur_p = it->second;
    if (c != u && cur_p <= user.privilege) {
        return -1;
    }
    if (!g.empty()) {
        int new_p = std::stoi(g);
        if (new_p >= cur_p) {
            return -1;
        }
        user.privilege = new_p;
        if (is_logged(u)) {
            logged_users[u] = new_p;
        }
    }
    if (!p.empty()) {
        std::strncpy(user.password, p.c_str(), 30);
        user.password[30] = '\0';
    }
    if (!n.empty()) {
        std::strncpy(user.name, n.c_str(), 15);
        user.name[15] = '\0';
    }
    if (!m.empty()) {
        std::strncpy(user.mailAddr, m.c_str(), 30);
        user.mailAddr[30] = '\0';
    }
    user_data.update(user,pos);
    std::cout << user.username << ' ' << user.name << ' ' << user.mailAddr << ' ' << user.privilege << '\n';
    return 0;
}

void UserManager::clean() {
    user_index.clear();
    logged_users.clear();
    user_data.close();
    std::ofstream tmp("user_data", std::ios::binary | std::ios::trunc);
    tmp.close();
    user_data.initialise();
}








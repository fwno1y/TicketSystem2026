#ifndef TICKETSYSTEM_USERMANAGER_H
#define TICKETSYSTEM_USERMANAGER_H

#include <iostream>
#include "BPlusTree.hpp"
#include "Map.h"
#include "MemoryRiver.h"
#include "User.h"

class UserManager {
private:
    BPlusTree<UserKey, int> user_index;
    MemoryRiver<User> user_data;
    sjtu::map<std::string, int> logged_users;
    sjtu::map<std::string, User> user_cache;


public:
    UserManager();
    ~UserManager();
    int add_user(const std::string& c, const std::string& u, const std::string& p,const std::string& n, const std::string& m, const std::string& g);
    int login(const std::string& u, const std::string& p);
    int logout(const std::string& u);
    int query_profile(const std::string& c, const std::string& u);
    int modify_profile(const std::string& c, const std::string& u, const std::string& p, const std::string& n, const std::string& m, const std::string& g);
    void clean();

    bool is_logged(const std::string& u) const;
};

#endif //TICKETSYSTEM_USERMANAGER_H
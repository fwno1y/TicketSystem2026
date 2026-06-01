#ifndef TICKETSYSTEM_USER_H
#define TICKETSYSTEM_USER_H

#include <iostream>

class User {
public:
    char username[21]{};
    char password[31]{};
    char name[16]{};
    char mailAddr[31]{};
    int privilege = 0;


    User() = default;
    User(const std::string& u, const std::string& p, const std::string& n, const std::string& m, int c);
    ~User();

};

struct UserKey {
    char username[21];

    UserKey();
    UserKey(const std::string& str);

    bool operator<(const UserKey& other) const;
    bool operator==(const UserKey& other) const;
};
#endif //TICKETSYSTEM_USER_H
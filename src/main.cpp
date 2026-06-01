#include <iostream>
#include "../include/BPlusTree.hpp"
#include "../include/Map.h"
#include "../include/User.h"
#include "../include/UserManager.h"
#include "../include/Parser.h"
#include "../include/Vector.h"
#include "../include/Train.h"
#include "../include/TrainManager.h"
#include "../include/Order.h"
#include "../include/OrderManager.h"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    UserManager user_manager;
    TrainManager train_manager;
    OrderManager order_manager;
    std::string cmd;
    while (std::getline(std::cin, cmd)) {
        Command command();

    }
    return 0;
}
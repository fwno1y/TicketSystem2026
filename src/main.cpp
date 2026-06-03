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
    CommandParser parser;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            continue;
        }
        Command cmd;
        if (!parser.parse(line, cmd)) {
            continue;
        }
        std::cout << "[" << cmd.timestamp << "]";
        if (cmd.cmd == "add_user") {
            std::cout << user_manager.add_user(cmd.get('c'), cmd.get('u'),cmd.get('p'), cmd.get('n'), cmd.get('m'), cmd.get('g')) << '\n';
        }
        else if (cmd.cmd == "login") {
            std::cout << user_manager.login(cmd.get('u'), cmd.get('p')) << '\n';
        }
        else if (cmd.cmd == "logout") {
            std::cout << user_manager.logout(cmd.get('u')) << '\n';
        }
        else if (cmd.cmd == "query_profile") {
            user_manager.query_profile(cmd.get('c'), cmd.get('u'));
        }
        else if (cmd.cmd == "modify_profile") {
            user_manager.modify_profile(cmd.get('c'), cmd.get('u'), cmd.get('p'), cmd.get('n'), cmd.get('m'), cmd.get('g'));
        }
        else if (cmd.cmd == "add_train") {
            std::cout << train_manager.add_train(cmd.get('i'), cmd.get('n'), cmd.get('m'), cmd.get('s'),cmd.get('p'), cmd.get('x'), cmd.get('t'), cmd.get('o'),cmd.get('d'), cmd.get('y')) << '\n';
        }
        else if (cmd.cmd == "delete_train") {
            std::cout << train_manager.delete_train(cmd.get('i')) << '\n';
        }
        else if (cmd.cmd == "release_train") {
            std::cout << train_manager.release_train(cmd.get('i')) << '\n';
        }
        else if (cmd.cmd == "query_train") {
            train_manager.query_train(cmd.get('i'), cmd.get('d'));
        }
        else if (cmd.cmd == "query_ticket") {

        }
        else if (cmd.cmd == "query_transfer") {

        }
        else if (cmd.cmd == "buy_ticket") {

        }
        else if (cmd.cmd == "query_order") {

        }
        else if (cmd.cmd == "refund_ticket") {

        }
        else if (cmd.cmd == "clean") {
            user_manager.clean();
            train_manager.clean();
            order_manager.clean();
            std::cout << 0 << '\n';
        }
        else if (cmd.cmd == "exit") {
            std::cout << "bye" << '\n';
            break;
        }
    }
    return 0;
}
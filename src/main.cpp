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
            bool p_time = false;
            if (cmd.get('p') == "time") {
                p_time = true;
            }
            sjtu::vector<Ticket> res;
            std::cout << train_manager.query_ticket(cmd.get('s'),cmd.get('t'), cmd.get('d'), p_time, res);
            for (int i = 0; i < res.size(); ++i) {
                const Ticket& ticket = res[i];
                std::cout << ticket.trainID << " " << ticket.from_station << " " << ticket.leave_time.to_string() << " -> " << ticket.to_station << " " << ticket.arrive_time.to_string() << " " << ticket.price << " " << ticket.seat << '\n';
            }
        }
        else if (cmd.cmd == "query_transfer") {
            bool p_time = false;
            if (cmd.get('p') == "time") {
                p_time = true;
            }
            Ticket t1, t2;
            bool found = train_manager.query_transfer(cmd.get('s'), cmd.get('t'), cmd.get('d'), p_time, t1, t2);
            if (!found) {
                std::cout << 0 << '\n';
            }
            else {
                std::cout << t1.trainID << " " << t1.from_station << " " << t1.leave_time.to_string() << " -> " << t1.to_station << " " << t1.arrive_time.to_string() << " " << t1.price << " " << t1.seat << '\n';
                std::cout << t2.trainID << " " << t2.from_station << " " << t2.leave_time.to_string() << " -> " << t2.to_station << " " << t2.arrive_time.to_string() << " " << t2.price << " " << t2.seat << '\n';
            }
        }
        else if (cmd.cmd == "buy_ticket") {
            if (!user_manager.is_logged(cmd.get('u'))) {
                std::cout << -1 << '\n';
            }
            else {
                bool q = false;
                if (cmd.has('q') && cmd.get('q') == "true") {
                    q = true;
                }
                int res = order_manager.buy_ticket(cmd.get('u'), cmd.get('i'), cmd.get('d'), cmd.get('n'), cmd.get('f'), cmd.get('t'), q, cmd.timestamp, train_manager);
                if (res == -2) {
                    std::cout << "queue" << '\n';
                }
                else {
                    std::cout << res << '\n';
                }
            }
        }
        else if (cmd.cmd == "query_order") {
            if (!user_manager.is_logged(cmd.get('u'))) {
                std::cout << -1 << '\n';
            }
            else {
                sjtu::vector<Order> res;
                order_manager.query_order(cmd.get('u'), res);
            }
        }
        else if (cmd.cmd == "refund_ticket") {
            if (!user_manager.is_logged(cmd.get('u'))) {
                std::cout << -1 << '\n';
            }
            else {
                int n = 1;
                if (cmd.has('n')) {
                    n = cmd.get_int('n');
                }
                std::cout << order_manager.refund_ticket(cmd.get('u'), n, train_manager) << '\n';
            }
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
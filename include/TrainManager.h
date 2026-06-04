#ifndef TICKETSYSTEM_TRAINMANAGER_H
#define TICKETSYSTEM_TRAINMANAGER_H

#include <iostream>
#include "BPlusTree.hpp"
#include "Vector.h"
#include "MemoryRiver.h"
#include "Train.h"



class TrainManager {
private:
    BPlusTree<TrainKey, int> train_index;
    BPlusTree<DayKey, int> train_date_index;
    BPlusTree<StationKey, Station> train_station_index;
    MemoryRiver<Train> train_data;
    MemoryRiver<TicketLeft> ticket_data;

public:
    TrainManager();
    ~TrainManager();
    int add_train(const std::string& i, const std::string& n, const std::string& m, const std::string& s, const std::string& p,
        const std::string& x, const std::string& t, const std::string& o, const std::string& d, const std::string& y);
    int delete_train(const std::string& i);
    int release_train(const std::string& i);
    int query_train(const std::string& i, const std::string& d, std::ostream& out);
    int query_ticket(const std::string& s, const std::string& t, const std::string& d, bool p, sjtu::vector<Ticket>& res);
    bool query_transfer(const std::string& s, const std::string& t, const std::string& d, bool p, Ticket& t1, Ticket& t2);

    void clean();
    //辅助函数
    bool get_train(const std::string& id, Train& train);
    // 检查并预扣票（用于购票流程）
    // 返回值：成功返回票价；座位不足返回 0；车次/日期/站点无效返回 -1
    int try_buy(const std::string& id, const Date& date,int from, int to, int num, Train& train_out, Date& start_date_out);
    // 退票：恢复座位数（refund 时调用）
    bool refund_seats(const std::string& id, const Date& start_date,int from, int to, int num);
    // 查询某车次某起始日的余票数据（供候补队列处理）
    bool get_ticket_left(const std::string& id, const Date& start,TicketLeft& tickets, int& ticket_pos);
    // 更新余票（候补订单成功时）
    void update_ticket_left(int ticket_pos, const TicketLeft& tickets);

};
#endif //TICKETSYSTEM_TRAINMANAGER_H
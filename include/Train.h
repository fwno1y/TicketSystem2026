#ifndef TICKETSYSTEM_TRAIN_H
#define TICKETSYSTEM_TRAIN_H

#include <iostream>
#include "DateTime.h"

const int MAX_STATION = 40;

class Train {
public:
    char trainID[21]{};
    int stationNum = 0;
    int seatNum = 0;
    char stations[MAX_STATION][31]{};
    int prices[MAX_STATION]{}; //前缀和存储
    Time startTime;
    int arriveTimes[MAX_STATION]{}; //每个站到达时间
    int leaveTimes[MAX_STATION]{}; //每个站出发时间
    Date saleDateStart;
    Date saleDateEnd;
    char type{};
    bool released = false;
    int ticket_pos = -1; //余票

    Train();
    Train(const std::string& i, const std::string& n, const std::string& m,
        const std::string& s, const std::string & p, const std::string& x,
        const std::string& t, const std::string& o, const std::string& d,
        const std::string& y);

    int find_station(const std::string& n) const;
    DateTime get_arrive_time(int idx, const Date& start_date) const;
    DateTime get_leave_time(int idx, const Date& start_date) const;
    int get_price(int from, int to) const;
};

struct TrainKey {
    char trainID[21];
    TrainKey();
    TrainKey(const std::string& str);
    bool operator<(const TrainKey& other) const;
    bool operator==(const TrainKey& other) const;
};

struct DayKey {
    char trainID[21];
    Date date;
    DayKey();
    DayKey(const std::string& str, const Date& d);
    bool operator<(const DayKey& other) const;
    bool operator==(const DayKey& other) const;
};

struct StationKey {
    char stationName[31];
    char trainID[21];
    StationKey();
    StationKey(const std::string& str, const std::string& id);
    bool operator<(const StationKey& other) const;
    bool operator==(const StationKey& other) const;
};

struct Station {
    char trainID[21]{};
    int station_idx = 0;
    Station() = default;
    Station(const std::string& id, int idx);
    bool operator<(const Station& other) const;
    bool operator==(const Station& other) const;
};

struct TicketLeft {
    int tickets[MAX_STATION]; // tickets[i] 表示第i站到第i + 1站的剩余票数
    TicketLeft() = default;
    TicketLeft(int seat, int station);

    int min_tickets(int from, int to) const;
    void update(int from, int to, int d);
};

struct Ticket {
    char trainID[21];
    char from_station[31];
    char to_station[31];
    DateTime leave_time;
    DateTime arrive_time;
    int price = 0;
    int seat = 0;
    int duration = 0;

    Ticket() = default;
    bool less_by_time(const Ticket& other) const;
    bool less_by_cost(const Ticket& other) const;
};
#endif //TICKETSYSTEM_TRAIN_H
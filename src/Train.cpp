#include "../include/Train.h"
#include "../include/Parser.h"
#include "cstring"

Train::Train() = default;

Train::Train(const std::string &i, const std::string &n, const std::string &m, const std::string &s, const std::string &p, const std::string &x, const std::string &t, const std::string &o, const std::string &d, const std::string &y) {
    std::strcpy(trainID, i.c_str());
    stationNum = std::stoi(n);
    seatNum = std::stoi(m);
    type = y[0];
    sjtu::vector<std::string> station_list;
    CommandParser::split(s, '|', station_list);
    for (int j = 0; j < stationNum; ++j) {
        std::strcpy(stations[j],station_list[j].c_str());
    }
    sjtu::vector<std::string> price_list;
    CommandParser::split(p,'|', price_list);
    prices[0] = 0;
    for (int j = 0; j < stationNum - 1; ++j) {
        prices[j + 1] = prices[j] + std::stoi(price_list[j]);
    }
    startTime.parse(x);
    sjtu::vector<std::string> travel_list;
    sjtu::vector<std::string> stopover_list;
    CommandParser::split(t, '|', travel_list);
    if (o != "_") {
        CommandParser::split(o, '|', stopover_list);
    }
    arriveTimes[0] = 0;
    leaveTimes[0] = 0;
    for (int j = 1; j < stationNum; ++j) {
        arriveTimes[j] = leaveTimes[j - 1] + std::stoi(travel_list[j - 1]);
        if (j < stationNum - 1) {
            leaveTimes[j] = arriveTimes[j] + std::stoi(stopover_list[j - 1]);
        }
        else {
            leaveTimes[j] = arriveTimes[j];
        }
    }
    sjtu::vector<std::string> date_list;
    CommandParser::split(d, '|', date_list);
    saleDateStart.parse(date_list[0]);
    saleDateEnd.parse(date_list[1]);
}

int Train::find_station(const char *n) const {
    for (int i = 0; i < stationNum; ++i) {
        if (std::strcmp(stations[i], n) == 0) {
            return i;
        }
    }
    return -1;
}

DateTime Train::get_arrive_time(int idx, const Date &start_date) const {
    DateTime start(start_date, startTime);
    return start.add_minutes(arriveTimes[idx]);
}

DateTime Train::get_leave_time(int idx, const Date &start_date) const {
    DateTime start(start_date, startTime);
    return start.add_minutes(leaveTimes[idx]);
}

int Train::get_price(int from, int to) const {
    return prices[to] - prices[from];
}

TrainKey::TrainKey() = default;

TrainKey::TrainKey(const std::string &str) {
    std::strcpy(trainID, str.c_str());
}

bool TrainKey::operator<(const TrainKey &other) const {
    return strcmp(trainID, other.trainID) < 0;
}

bool TrainKey::operator==(const TrainKey &other) const {
    return strcmp(trainID, other.trainID) == 0;
}

DayKey::DayKey() = default;

DayKey::DayKey(const std::string& str, const Date& d) : date(d) {
    std::strcpy(trainID, str.c_str());
}

DayKey::operator<(const DayKey &other) const {
    int cmp = std::strcmp(trainID, other.trainID);
    if (cmp != 0) {
        return cmp < 0;
    }
    return date < other.date;
}

DayKey::operator==(const DayKey &other) const {
    return std::strcmp(trainID,other.trainID) == 0 && date = other.date;
}

StationKey::StationKey() = default;

StationKey::StationKey(const std::string &str, const std::string &id) {
    std::strcpy(stationName, str.c_str());
    std::strcpy(trainID, id.c_str());
}

bool StationKey::operator<(const StationKey &other) const {
    int cmp = std::strcmp(stationName, other.stationName);
    if (cmp != 0) {
        return cmp < 0;
    }
    return std::strcpy(trainID, other.trainID) < 0;
}

bool StationKey::operator==(const StationKey &other) const {
    return strcmp(stationName, other.stationName) == 0 && strcmp(trainID, other.trainID) == 0;
}

TicketLeft::TicketLeft(int seat, int station) {
    for (int i = 0; i < station - 1; ++i) {
        tickets[i] = seat;
    }
}

int TicketLeft::min_tickets(int from, int to) const {
    int min = tickets[from];
    for (int i = from + 1; i < to; ++i) {
        if (tickets[i] < min) {
            min = tickets[i];
        }
    }
    return min;
}

void TicketLeft::update(int from, int to, int d) {
    for (int i = from; i < to; ++i) {
        tickets[i] += d;
    }
}

bool Ticket::less_by_time(const Ticket &other) const {
    if (duration != other.duration) {
        return duration < other.duration;
    }
    return std::strcmp(trainID, other.trainID) < 0;
}

bool Ticket::less_by_cost(const Ticket &other) const {
    if (price != other.price) {
        return price < other.price;
    }
    return std::strcmp(trainID, other.trainID) < 0;
}













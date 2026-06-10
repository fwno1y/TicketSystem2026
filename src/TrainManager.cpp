#include "../include/TrainManager.h"
#include"../include/Parser.h"
#include "cstring"
#include "../include/Map.h"

TrainManager::TrainManager() : train_index("train_index"), train_date_index("train_date_index"), train_station_index("train_station_index"), train_data("train_data"), ticket_data("ticket_data") {
    train_data.initialise();
    ticket_data.initialise();
}

TrainManager::~TrainManager() {
    train_data.close();
    ticket_data.close();
}

int TrainManager::add_train(const std::string &i, const std::string &n, const std::string &m, const std::string &s, const std::string &p, const std::string &x, const std::string &t, const std::string &o, const std::string &d, const std::string &y) {
    TrainKey key(i);
    int pos;
    if (train_index.find(key,pos)) {
        return -1;
    }
    Train train;
    memset(&train, 0, sizeof(train));
    std::strncpy(train.trainID, i.c_str(), 20);
    train.stationNum = std::stoi(n);
    train.seatNum = std::stoi(m);
    train.type = y[0];
    train.startTime.parse(x);
    CommandParser parser;
    sjtu::vector<std::string> stations_vec;
    parser.split(s, '|', stations_vec);
    for (int j = 0; j < stations_vec.size() && j < MAX_STATION; ++j) {
        std::strncpy(train.stations[j], stations_vec[j].c_str(), 30);
        train.stations[j][30] = '\0';
    }
    sjtu::vector<std::string> prices_vec;
    parser.split(p, '|', prices_vec);
    memset(train.prices, 0, sizeof(train.prices));
    for (int j = 0; j < prices_vec.size() && j < train.stationNum - 1; ++j) {
        train.prices[j + 1] = train.prices[j] + std::stoi(prices_vec[j]);
    }
    sjtu::vector<std::string> travel_vec;
    parser.split(t, '|', travel_vec);
    sjtu::vector<std::string> stopover_vec;
    if (o != "_") {
        parser.split(o, '|', stopover_vec);
    }
    train.arriveTimes[0] = 0;
    train.leaveTimes[0] = 0;
    for (int j = 1; j < train.stationNum; ++j) {
        train.arriveTimes[j] = train.leaveTimes[j - 1] + std::stoi(travel_vec[j - 1]);
        if (j < train.stationNum - 1) {
            train.leaveTimes[j] = train.arriveTimes[j] + std::stoi(stopover_vec[j - 1]);
        }
        else {
            train.leaveTimes[j] = train.arriveTimes[j];
        }
    }
    sjtu::vector<std::string> dates_vec;
    parser.split(d, '|', dates_vec);
    train.saleDateStart.parse(dates_vec[0]);
    train.saleDateEnd.parse(dates_vec[1]);
    train.released = false;
    train.ticket_pos = -1;
    pos = train_data.write(train);
    train_index.insert(key,pos);
    return 0;
}

int TrainManager::delete_train(const std::string &i) {
    TrainKey key(i);
    int pos;
    if (!train_index.find(key,pos)) {
        return -1;
    }
    Train train;
    train_data.read(train,pos);
    if (train.released) {
        return -1;
    }
    train_index.remove(key);
    return 0;
}

int TrainManager::release_train(const std::string &i) {
    TrainKey tkey(i);
    int pos;
    if (!train_index.find(tkey,pos)) {
        return -1;
    }
    Train train;
    train_data.read(train,pos);
    if (train.released) {
        return -1;
    }
    train.released = true;
    int start_day = train.saleDateStart.to_days();
    int end_day = train.saleDateEnd.to_days();
    bool first = true;
    for (int day = start_day; day <= end_day; ++day) {
        TicketLeft tl(train.seatNum, train.stationNum);
        int t_pos = ticket_data.write(tl);
        if (first) {
            train.ticket_pos = t_pos;
            first = false;
        }
        Date cur_d = Date::from_days(day);
        DayKey dkey(i, cur_d);
        train_date_index.insert(dkey,t_pos);
    }
    for (int j = 0; j < train.stationNum; ++j) {
        StationKey skey(train.stations[j], i);
        Station st(i, j);
        train_station_index.insert(skey, st);
    }
    train_data.update(train,pos);
    return 0;
}

int TrainManager::query_train(const std::string &i, const std::string &d) {
    TrainKey tkey(i);
    int pos;
    if (!train_index.find(tkey,pos)) {
        return -1;
    }
    Train train;
    train_data.read(train,pos);
    Date date;
    date.parse(d);
    if (date < train.saleDateStart || train.saleDateEnd < date) {
        return -1;
    }
    TicketLeft tl(train.seatNum,train.stationNum);
    if (train.released) {
        int t_pos;
        DayKey dkey(i,date);
        if (train_date_index.find(dkey,t_pos)) {
            ticket_data.read(tl, t_pos);
        }
    }
    std::cout << train.trainID << " " << train.type << "\n";
    for (int j = 0; j < train.stationNum; ++j) {
        std::cout << train.stations[j] << " ";
        if (j == 0) {
            std::cout << "xx-xx xx:xx";
        }
        else {
            std::cout << train.get_arrive_time(j,date).to_string();
        }
        std::cout << " -> ";
        if (j == train.stationNum - 1) {
            std::cout << "xx-xx xx:xx";
        }
        else {
            std::cout << train.get_leave_time(j, date).to_string();
        }
        std::cout << " " << train.prices[j] << " ";
        if (j == train.stationNum - 1) {
            std::cout << "x";
        }
        else {
            std::cout << tl.tickets[j];
        }
        std::cout << "\n";
    }
    return 0;
}

bool TrainManager::compare_ticket(const Ticket &a, const Ticket &b, bool p_time) {
    if (p_time) {
        return a.less_by_time(b);
    }
    return a.less_by_cost(b);
}

void TrainManager::sort_tickets(sjtu::vector<Ticket> &res, int low, int high, bool p_time) {
    if (low >= high) {
        return;
    }
    int i = low, j = high;
    Ticket pivot = res[low];
    while (i < j) {
        while (i < j && !compare_ticket(res[j], pivot, p_time)) {
            j--;
        }
        if (i < j) {
            res[i++] = res[j];
        }
        while (i < j && compare_ticket(res[i], pivot, p_time)) {
            i++;
        }
        if (i < j) {
            res[j--] = res[i];
        }
    }
    res[i] = pivot;
    sort_tickets(res, low, i - 1, p_time);
    sort_tickets(res, i + 1, high, p_time);
}

int TrainManager::query_ticket(const std::string &s, const std::string &t, const std::string &d, bool p, sjtu::vector<Ticket> &res) {
    Date user_date;
    user_date.parse(d);
    StationKey low_key(s,"");
    StationKey high_key(s, "\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f");
    sjtu::vector<Station> station_vec;
    train_station_index.find_range(low_key, high_key, station_vec);
    for (int k = 0; k < station_vec.size(); ++k) {
        const Station& st = station_vec[k];
        TrainKey tkey(st.trainID);
        int train_pos;
        if (!train_index.find(tkey, train_pos)) {
            continue;
        }
        Train train;
        train_data.read(train, train_pos);
        if (!train.released) {
            continue;
        }
        int from_idx = st.station_idx;
        int to_idx = train.find_station(t.c_str());
        if (to_idx == -1 || from_idx >= to_idx) {
            continue;
        }
        //计算始发日期
        int start_minutes = train.startTime.to_minutes();
        int day_offset = (start_minutes + train.leaveTimes[from_idx]) / 1440;
        int start_day = user_date.to_days() - day_offset;
        Date start_date = Date::from_days(start_day);
        if (start_date < train.saleDateStart || train.saleDateEnd < start_date) {
            continue;
        }
        DayKey dkey(train.trainID, start_date);
        int t_pos;
        if (!train_date_index.find(dkey, t_pos)) {
            continue;
        }
        TicketLeft tl;
        ticket_data.read(tl,t_pos);
        Ticket ticket;
        memset(&ticket, 0, sizeof(ticket));
        std::strncpy(ticket.trainID, train.trainID, 20);
        std::strncpy(ticket.from_station, s.c_str(), 30);
        std::strncpy(ticket.to_station, t.c_str(), 30);
        ticket.leave_time = train.get_leave_time(from_idx,start_date);
        ticket.arrive_time = train.get_arrive_time(to_idx,start_date);
        ticket.price = train.get_price(from_idx,to_idx);
        ticket.seat = tl.min_tickets(from_idx,to_idx);
        ticket.duration = ticket.arrive_time.diff(ticket.leave_time);
        res.push_back(ticket);
    }
    if (!res.empty()) {
        sort_tickets(res, 0, res.size() - 1, p);
    }
    return res.size();
}

bool TrainManager::query_transfer(const std::string &s, const std::string &t, const std::string &d, bool p, Ticket &t1, Ticket &t2) {
    Date user_date;
    user_date.parse(d);
    // 第一步：缓存所有从S出发的有效车次
    sjtu::map<std::string, sjtu::vector<int>> transfer_map;
    StationKey low_key_s(s, "");
    StationKey high_key_s(s, "\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f");
    sjtu::vector<Station> train1_vec;
    train_station_index.find_range(low_key_s, high_key_s, train1_vec);

    struct Train1Info {
        Train train;
        int s_idx;
        Date start_date;
        TicketLeft tl;
    };
    sjtu::vector<Train1Info> valid_train1;

    for (int i = 0; i < train1_vec.size(); ++i) {
        const Station& st1 = train1_vec[i];
        TrainKey tkey1(st1.trainID);
        int pos1;
        if (!train_index.find(tkey1, pos1)) continue;

        Train train1;
        train_data.read(train1, pos1);
        if (!train1.released) continue;

        int start_minutes_1 = train1.startTime.to_minutes();
        int day_offset_1 = (start_minutes_1 + train1.leaveTimes[st1.station_idx]) / 1440;
        Date start_date_1 = Date::from_days(user_date.to_days() - day_offset_1);

        if (start_date_1 < train1.saleDateStart || train1.saleDateEnd < start_date_1) continue;

        DayKey dkey1(train1.trainID, start_date_1);
        int tl_pos1;
        if (!train_date_index.find(dkey1, tl_pos1)) continue;

        TicketLeft tl1;
        ticket_data.read(tl1, tl_pos1);

        Train1Info info;
        info.train = train1;
        info.s_idx = st1.station_idx;
        info.start_date = start_date_1;
        info.tl = tl1;

        int idx = valid_train1.size();
        valid_train1.push_back(info);

        for (int j = st1.station_idx + 1; j < train1.stationNum; ++j) {
            if (std::strcmp(train1.stations[j], t.c_str()) != 0) {
                std::string transfer_name(train1.stations[j]);
                transfer_map[transfer_name].push_back(idx);
            }
        }
    }

    if (valid_train1.empty()) return false;

    // 第二步：缓存所有到T的车次
    StationKey low_key_t(t, "");
    StationKey high_key_t(t, "\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f");
    sjtu::vector<Station> train2_vec;
    train_station_index.find_range(low_key_t, high_key_t, train2_vec);

    struct Train2Info {
        Train train;
        int t_idx;
    };
    sjtu::vector<Train2Info> valid_train2;

    for (int k = 0; k < train2_vec.size(); ++k) {
        const Station& st2 = train2_vec[k];
        TrainKey tkey2(st2.trainID);
        int pos2;
        if (!train_index.find(tkey2, pos2)) continue;

        Train train2;
        train_data.read(train2, pos2);  // ← 只读一次！
        if (!train2.released) continue;

        Train2Info info2;
        info2.train = train2;
        info2.t_idx = st2.station_idx;
        valid_train2.push_back(info2);
    }

    bool found = false;
    Ticket best_t1, best_t2;

    // 第三步：遍历train2缓存
    for (int k = 0; k < valid_train2.size(); ++k) {
        const Train2Info& info2 = valid_train2[k];
        const Train& train2 = info2.train;
        int to2_idx = info2.t_idx;

        for (int ti = 0; ti < to2_idx; ++ti) {
            std::string transfer_name(train2.stations[ti]);
            auto it = transfer_map.find(transfer_name);
            if (it == transfer_map.end()) continue;

            const sjtu::vector<int>& train1_indices = it->second;

            for (int idx_i = 0; idx_i < train1_indices.size(); ++idx_i) {
                int i = train1_indices[idx_i];
                const Train1Info& info1 = valid_train1[i];

                if (std::strcmp(info1.train.trainID, train2.trainID) == 0) continue;

                int j = -1;
                for (int jj = info1.s_idx + 1; jj < info1.train.stationNum; ++jj) {
                    if (std::strcmp(info1.train.stations[jj], transfer_name.c_str()) == 0) {
                        j = jj;
                        break;
                    }
                }
                if (j == -1) continue;

                DateTime transfer_time = info1.train.get_arrive_time(j, info1.start_date);

                int start_minutes_2 = train2.startTime.to_minutes();
                int target_min = transfer_time.to_minutes() - start_minutes_2 - train2.leaveTimes[ti];
                int start_day_2 = target_min / 1440;
                if (target_min % 1440 > 0) start_day_2++;

                Date start_date_2 = Date::from_days(start_day_2);
                if (start_date_2 < train2.saleDateStart) start_date_2 = train2.saleDateStart;
                if (train2.saleDateEnd < start_date_2) continue;

                DayKey dkey2(train2.trainID, start_date_2);
                int tl_pos2;
                if (!train_date_index.find(dkey2, tl_pos2)) continue;

                TicketLeft tl2;
                ticket_data.read(tl2, tl_pos2);

                Ticket ti1, ti2;
                std::memset(&ti1, 0, sizeof(Ticket));
                std::memset(&ti2, 0, sizeof(Ticket));

                std::strncpy(ti1.trainID, info1.train.trainID, 20);
                std::strncpy(ti1.from_station, s.c_str(), 30);
                std::strncpy(ti1.to_station, transfer_name.c_str(), 30);
                ti1.leave_time = info1.train.get_leave_time(info1.s_idx, info1.start_date);
                ti1.arrive_time = transfer_time;
                ti1.price = info1.train.get_price(info1.s_idx, j);
                ti1.seat = info1.tl.min_tickets(info1.s_idx, j);
                ti1.duration = ti1.arrive_time.diff(ti1.leave_time);

                std::strncpy(ti2.trainID, train2.trainID, 20);
                std::strncpy(ti2.from_station, transfer_name.c_str(), 30);
                std::strncpy(ti2.to_station, t.c_str(), 30);
                ti2.leave_time = train2.get_leave_time(ti, start_date_2);
                ti2.arrive_time = train2.get_arrive_time(to2_idx, start_date_2);
                ti2.price = train2.get_price(ti, to2_idx);
                ti2.seat = tl2.min_tickets(ti, to2_idx);
                ti2.duration = ti2.arrive_time.diff(ti2.leave_time);

                bool better = false;
                if (!found) {
                    better = true;
                } else {
                    int total_time = ti2.arrive_time.diff(ti1.leave_time);
                    int best_total_time = best_t2.arrive_time.diff(best_t1.leave_time);
                    int total_cost = ti1.price + ti2.price;
                    int best_total_cost = best_t1.price + best_t2.price;

                    if (p) {
                        if (total_time < best_total_time) better = true;
                        else if (total_time == best_total_time) {
                            if (total_cost < best_total_cost) better = true;
                            else if (total_cost == best_total_cost) {
                                int cmp = std::strcmp(ti1.trainID, best_t1.trainID);
                                if (cmp < 0) better = true;
                                else if (cmp == 0 && std::strcmp(ti2.trainID, best_t2.trainID) < 0) better = true;
                            }
                        }
                    } else {
                        if (total_cost < best_total_cost) better = true;
                        else if (total_cost == best_total_cost) {
                            if (total_time < best_total_time) better = true;
                            else if (total_time == best_total_time) {
                                int cmp = std::strcmp(ti1.trainID, best_t1.trainID);
                                if (cmp < 0) better = true;
                                else if (cmp == 0 && std::strcmp(ti2.trainID, best_t2.trainID) < 0) better = true;
                            }
                        }
                    }
                }

                if (better) {
                    best_t1 = ti1;
                    best_t2 = ti2;
                    found = true;
                }
            }
        }
    }

    if (found) {
        t1 = best_t1;
        t2 = best_t2;
        return true;
    }
    return false;
}

void TrainManager::clean() {
    train_index.clear();
    train_date_index.clear();
    train_station_index.clear();
    train_data.close();
    ticket_data.close();
}

int TrainManager::try_buy(const std::string &id, const Date &date, int from, int to, int num, Train &train_out, Date &start_date_out) {
    TrainKey tkey(id);
    int pos;
    if (!train_index.find(tkey,pos)) {
        return -1;
    }
    Train train;
    train_data.read(train, pos);
    if (!train.released) {
        return -1;
    }
    if (from < 0 || to >= train.stationNum || from >= to) {
        return -1;
    }
    //购买数量不能大于总座位数
    if (num > train.seatNum) {
        return -1;
    }
    int start_minutes = train.startTime.to_minutes();
    int day_offset = (start_minutes + train.leaveTimes[from]) / 1440;
    int start_day = date.to_days() - day_offset;
    Date start_date = Date::from_days(start_day);

    if (start_date < train.saleDateStart || train.saleDateEnd < start_date) {
        return -1;
    }
    DayKey dkey(id, start_date);
    int ticket_pos;
    if (!train_date_index.find(dkey, ticket_pos)) {
        return -1;
    }
    TicketLeft tl;
    ticket_data.read(tl,ticket_pos);
    //fix : 先赋值，使得候补订单获取车次信息
    train_out = train;
    start_date_out = start_date;
    int left_tickets = tl.min_tickets(from, to);
    if (left_tickets < num) {
        return 0;
    }
    //扣票
    tl.update(from, to, -num);
    ticket_data.update(tl, ticket_pos);
    return train.get_price(from, to);
}

bool TrainManager::refund_seats(const std::string &id, const Date &start_date, int from, int to, int num) {
    DayKey dkey(id, start_date);
    int ticket_pos;
    if (!train_date_index.find(dkey, ticket_pos)) {
        return false;
    }
    TicketLeft tl;
    ticket_data.read(tl, ticket_pos);
    tl.update(from, to, num);
    ticket_data.update(tl, ticket_pos);
    return true;
}

bool TrainManager::get_ticket_left(const std::string &id, const Date &start, TicketLeft &tickets, int &ticket_pos) {
    DayKey dkey(id, start);
    if (!train_date_index.find(dkey, ticket_pos)) {
        return false;
    }
    ticket_data.read(tickets, ticket_pos);
    return true;
}

void TrainManager::update_ticket_left(int ticket_pos, const TicketLeft &tickets) {
    TicketLeft tl = tickets;
    ticket_data.update(tl,ticket_pos);
}

bool TrainManager::get_train(const std::string &id, Train &train) {
    TrainKey tkey(id);
    int pos;
    if (!train_index.find(tkey, pos)) {
        return false;
    }
    train_data.read(train, pos);
    return true;
}














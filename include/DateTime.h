#ifndef TICKETSYSTEM_DATE_H
#define TICKETSYSTEM_DATE_H

#include <cstring>
#include <iostream>

struct Date {
    int month = 0;
    int day = 0;

    Date() = default;
    Date(int m, int d);

    void parse(const std::string& str);
    std::string to_string() const;
    //从06-01开始的天数
    int to_days() const;
    static Date from_days(int days);
    bool operator<(const Date& other) const;
    bool operator==(const Date& other) const;
    bool operator<=(const Date& other) const;
};

struct Time {
    int hour = 0;
    int minute = 0;

    Time() = default;
    Time(int h, int m);

    void parse(const std::string& str);
    std::string to_string() const;
    int to_minutes() const;
    bool operator<(const Time& other) const;
    bool operator==(const Time& other) const;
};

struct DateTime {
    Date date;
    Time time;

    DateTime() = default;
    DateTime(const Date& d, const Time& t);

    int to_minutes() const;
    static DateTime to_datetime(int minutes);
    DateTime add_minutes(int minutes) const;
    int diff(const DateTime& other) const;

    bool operator<(const DateTime& other) const;
    bool operator==(const DateTime& other) const;
    bool operator<=(const DateTime& other) const;
    std::string to_string() const;
};

#endif //TICKETSYSTEM_DATE_H
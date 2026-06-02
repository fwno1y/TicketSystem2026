#include "../include/DateTime.h"
#include <iostream>
#include <iomanip>

static const int DAY_IN_MONTH[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
Date::Date(int m, int d) : month(m), day(d) {}

void Date::parse(const std::string& str) {
    size_t pos = str.find('-');
    if (pos != std::string::npos) {
        std::string month_ = str.substr(0,pos);
        std::string day_ = str.substr(pos + 1);
        month = 0;
        day = 0;
        for (char c : month_) {
            month = month * 10 + (c - '0');
        }
        for (char c : day_) {
            day = day * 10 + (c - '0');
        }
    }
}

std::string Date::to_string() const {
    std::ostringstream out;
    out << std::setfill('0') << std::setw(2) << month << '-' << std::setfill('0') << std::setw(2) << day;
    return out.str();
}

int Date::to_days() const {
    int days = 0;
    for (int m = 6; m < month; ++m) {
        days += DAY_IN_MONTH[m];
    }
    days += day - 1;
    return days;
}

Date Date::from_days(int days) {
    int m = 6, d = 1;
    d += days;
    while (m <= 12 && d > DAY_IN_MONTH[m]) {
        d -= DAY_IN_MONTH[m];
        m++;
    }
    return Date(m,d);
}

bool Date::operator<(const Date &other) const {
    if (month != other.month) {
        return month < other.month;
    }
    return day < other.day;
}

bool Date::operator==(const Date &other) const {
    return month == other.month && day == other.day;
}

bool Date::operator<=(const Date &other) const {
    return *this < other || *this == other;
}

bool Date::operator>(const Date &other) const {
    if (month != other.month) {
        return month > other.month;
    }
    return day > other.day;
}

bool Date::operator>=(const Date &other) const {
    return *this > other || *this == other;
}

bool Date::operator!=(const Date &other) const {
    return !(*this == other);
}

std::istream &operator>>(std::istream &is, Date &date) {
    std::string str;
    is >> str;
    date.parse(str);
    return is;
}

std::ostream &operator<<(std::ostream &os, const Date &date) {
    os << std::setfill('0') << std::setw(2) << date.month << '-' << std::setfill('0') << std::setw(2) << date.day;
    return os;
}

Time::Time(int h, int m) : hour(h), minute(m) {};

void Time::parse(const std::string &str) {
    size_t pos = str.find(':');
    if (pos != std::string::npos) {
        std::string hour_ = str.substr(0, pos);
        std::string minute_ = str.substr(pos + 1);
        hour = 0;
        minute = 0;
        for (char c : hour_) {
            hour = hour * 10 + (c - '0');
        }
        for (char c : minute_) {
            minute = minute * 10 + (c - '0');
        }
    }
}

std::string Time::to_string() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hour << ':' << std::setfill('0') << std::setw(2) << minute;
    return oss.str();
}

int Time::to_minutes() const {
    return hour * 60 + minute;
}

bool Time::operator<(const Time &other) const {
    return to_minutes() < other.to_minutes();
}

bool Time::operator==(const Time &other) const {
    return hour == other.hour && minute == other.minute;
}

std::istream &operator>>(std::istream &is, Time &time) {
    std::string str;
    is >> str;
    time.parse(str);
    return is;
}

std::ostream &operator<<(std::ostream &os, const Time &time) {
    os << std::setfill('0') << std::setw(2) << time.hour << ':' << std::setfill('0') << std::setw(2) << time.minute;
    return os;
}

DateTime::DateTime(const Date &d, const Time &t) : date(d), time(t) {}

int DateTime::to_minutes() const {
    return date.to_days() * 1440 + time.to_minutes();
}

DateTime DateTime::from_minutes(int minutes) {
    int days = minutes / 1440;
    int mins = minutes % 1440;
    if (mins < 0) {
        days--;
        mins += 1440;
    }
    return DateTime(Date::from_days(days), Time(mins / 60, mins % 60));
}

DateTime DateTime::add_minutes(int minutes) const {
    return from_minutes(to_minutes() + minutes);
}

int DateTime::diff(const DateTime &other) const {
    return to_minutes() - other.to_minutes();
}

bool DateTime::operator<(const DateTime &other) const {
    if (date != other.date) {
        return date < other.date;
    }
    return time < other.time;
}

bool DateTime::operator==(const DateTime &other) const {
    return date == other.date && time == other.time;
}

bool DateTime::operator<=(const DateTime &other) const {
    return *this < other || *this == other;
}

std::string DateTime::to_string() const {
    return date.to_string() + " " + time.to_string();
}

std::istream &operator>>(std::istream &is, DateTime &dt) {
    is >> dt.date >> dt.time;
    return is;
}

std::ostream &operator<<(std::ostream &os, const DateTime &dt) {
    os << dt.date << dt.time;
    return os;
}





















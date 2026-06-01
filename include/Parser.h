#ifndef TICKETSYSTEM_PARSER_H
#define TICKETSYSTEM_PARSER_H

#include <iostream>
#include "Vector.h"

struct Command {
    int timestamp;
    std::string cmd;

    bool has_key[26];
    std::string values[26];
    Command();
    bool has(char key) const;
    std::string get(char key) const;
    int get_int(char key) const;
};

class CommandParser {
public:
    bool parse(const std::string& line, Command& cmd);
    void split(const std::string& str, char d, sjtu::vector<std::string>& res);
};

#endif //TICKETSYSTEM_PARSER_H
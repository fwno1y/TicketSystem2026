#include "../include/Parser.h"
#include "../include/Vector.h"

Command::Command() : timestamp(0) {
    for (int i = 0; i < 26; ++i) {
        has_key[i] = 0;
    }
}

bool Command::has(char key) const {
    if (key < 'a' || key > 'z') {
        return false;
    }
    return has_key[key - 'a'];
}

std::string Command::get(char key) const {
    if (key < 'a' || key > 'z') {
        return "";
    }
    return values[key - 'a'];
}

int Command::get_int(char key) const {
    std::string val = get(key);
    if (val.empty()) {
        return 0;
    }
    return std::stoi(val);
}

bool CommandParser::parse(const std::string &line, Command &cmd) {
    cmd = Command();
    sjtu::vector<std::string> tokens;
    split(line, tokens);
    const std::string& str = tokens[0];
    if (str.size() < 3 || str[0] != '[' || str[str.size() - 1] != ']') {
        return false;
    }
    cmd.timestamp = 0;
    for (int i = 1; i < str.size(); ++i) {
        cmd.timestamp = cmd.timestamp * 10 + (str[i] - '0');
    }
    cmd.cmd = tokens[1];
    for (int i = 2; i < tokens.size() - 1; i += 2) {
        const std::string& key_str = tokens[i];
        if (key_str.size() != 2 || key_str[0] != '-') {
            continue;
        }
        char key = key_str[1];
        if (key < 'a' || key > 'z') {
            continue;
        }
        cmd.has_key[key - 'a'] = true;
        cmd.values[key - 'a'] = tokens[i + 1];
    }
    return true;
}

void CommandParser::split(const std::string &str, sjtu::vector<std::string> &res) {
    std::string cur;
    for (int i = 0; i < str.size(); ++i) {
        if (str[i] == ' ') {
            if (!cur.empty()) {
                res.push_back(cur);
                cur.clear();
            }
        }
        else {
            cur += str[i];
        }
    }
    if (!cur.empty()) {
        res.push_back(cur);
    }
}






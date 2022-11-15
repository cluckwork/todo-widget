#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <time.h>

// messy code but don't feel like refactoring. enjoy

enum flags {
    a,
    r,
    c,
    NA,
};

std::string parse_list_item(std::string inString) {
    auto out = std::stringstream{};
    for (int i = 0; i < inString.length(); i++) {
        if (inString[i] == '|') {
            break;
        } else {
            out << inString[i];
        }
    }
    return out.str();
}

std::string flip_completion_status(std::string item) {
    auto out = std::stringstream{};
    for (int i = 0; i < item.length(); i++) {
        if (item[i] == '|') {
            out << '|';
            if (item[i+1] == '0') {
                out << '1';
            } else {
                out << '0';
            }
            out << '|';
            i += 2;
        } else {
            out << item[i];
        }
    }
    return out.str();
}

std::string arg_to_string(std::string inString) {
    auto out = std::stringstream{};
    for (int i = 0; i < inString.length(); i++) {
        if (inString[i] == '_' || inString[i] == '|') {
            out << " ";
        } else {
            out << inString[i];
        }
    }
    return out.str();
}

void help_menu() {
    printf("events v1.1.0\n\n");
    printf("usage:\n");
    printf("-a [item] [YYYYMMDDHH]: add item\n");
    printf("-r [item]: remove item\n");
    printf("-c [item]: flip item's completion status\n");
    return;
}
//name flag item ts
//1    2    3    4
flags hash(std::string const& inString) {
    if (inString == "-a") return a;
    if (inString == "-r") return r;
    if (inString == "-c") return c;
    return NA;
}

time_t date_to_ts(std::string date) {
    // parses date and converts to unix timestamp
    int year,month,day,hour;
    struct tm t;
    year = atoi(date.substr(0,4).c_str());
    month = atoi(date.substr(4,2).c_str());
    day = atoi(date.substr(6,2).c_str());
    hour = atoi(date.substr(8,2).c_str());
    t.tm_isdst = - 1;
    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day;
    t.tm_hour = hour;
    return mktime(&t);
}

int main(int argc, char *argv[]) {
    const char* PATH = "/home/reimu/.config/conky/lua/events"; // path to data file to be parsed by conky
    
    const char* BUFFER_PATH = "/home/reimu/.config/conky/lua/events_buff"; // buffer for altering the file. this file shouldn't exist usually.
    if (argc < 3) {
        help_menu();
        return 0;
    }
    std::string item = arg_to_string(argv[2]);
    switch (hash(argv[1])) {
        case a:
            {
                if (argc < 4) {
                    help_menu();
                    return 0;
                }
                std::fstream f(PATH, std::ios::out | std::ios::app);
                if (!f.is_open()) {
                    printf("File not found");
                    return 0;
                }
                auto input = std::stringstream{};
                input << item << "|0|" << date_to_ts(argv[3]);
                f <<  input.str() << "\n";
                f.close();
                break;
            }
        case r:
            {
                if (argc < 3) {
                    help_menu();
                    return 0;
                }
                std::fstream f(PATH);
                std::ofstream buffer(BUFFER_PATH);
                if (!f.is_open()) {
                    printf("File not found");
                    return 0;
                }
                std::string line;
                while (std::getline(f,line)) {
                    if (parse_list_item(line) != item) {
                        buffer << line << "\n";
                    }
                }
                
                f.close();
                buffer.close();
                
                remove(PATH);
                rename(BUFFER_PATH,PATH);

                break;
            }
        case c:
            {
                if (argc <3) {
                    help_menu();
                    return 0;
                }
                std::fstream f(PATH);
                std::ofstream buffer(BUFFER_PATH);
                if (!f.is_open()) {
                    printf("File not found");
                    return 0;
                }
                std::string line;
                while (std::getline(f,line)) {
                    if (parse_list_item(line) != item) {
                        buffer << line << "\n";
                    } else {
                        buffer << flip_completion_status(line) << "\n";
                    }
                }

                f.close();
                buffer.close();
                
                remove(PATH);
                rename(BUFFER_PATH,PATH);
                break;
            }
        default:
            printf("Invalid flag\n");
            help_menu();
    };
    return 0;
}

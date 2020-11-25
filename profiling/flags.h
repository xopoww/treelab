#pragma once

// Small utility function for parsing command-line flag arguments

#include <iostream>
#include <string>
#include <sstream>
#include <regex>
#include <cmath>

unsigned int atoi(const std::string &s)
{
    unsigned int result = 0;
    for (int i = 0; i < s.size(); i++)
    {
        result += (s[s.size() - i - 1] - '0') * std::pow(10, i);
    }
    return result;
}

std::string parse_flag(
    int argc, const char * argv[], const char * name, const char * default_value
)
{
    std::stringstream r_s;
    r_s << "--" << name << "=[a-zA-Z_]+";
    std::regex re(r_s.str(), std::regex_constants::ECMAScript);

    for (int i = 1; i < argc; i++)
    {
        if (std::regex_match(argv[i], re))
        {
            return std::string(argv[i] + strlen(name) + 3);
        }
    }

    return std::string(default_value);
}

unsigned int parse_flag(
    int argc, const char * argv[], const char * name, unsigned int default_value
)
{
    std::stringstream r_s;
    r_s << "--" << name << "=[1-9][0-9]*";
    std::regex re(r_s.str(), std::regex_constants::ECMAScript);

    for (int i = 1; i < argc; i++)
    {
        if (std::regex_match(argv[i], re))
        {
            return atoi(std::string(argv[i] + strlen(name) + 3));
        }
    }

    return default_value;
}

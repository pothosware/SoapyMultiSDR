// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#pragma once
#include <SoapySDR/Config.hpp>
#include <vector>
#include <string>
#include <cctype>
#include <stdexcept>

//! transform an internal name to a indexed name in the format name[index]
static inline std::string toIndexedName(const std::string &name, const size_t index)
{
    return name + "[" + std::to_string(index) + "]";
}

//! is the input string in the indexed name format?
static inline bool isIndexedName(const std::string &inName)
{
    const size_t openBracketPos = inName.find_last_of("[");
    const size_t closeBracketPos = inName.find_last_of("]");
    if (openBracketPos == std::string::npos) return false;
    if (closeBracketPos == std::string::npos) return false;
    if (closeBracketPos < openBracketPos) return false;
    for (size_t i = openBracketPos+1; i < closeBracketPos; i++)
    {
        if (not std::isdigit(inName.at(i))) return false;
    }
    return true;
}

//! Split an indexed name into internal name and index
static inline std::string splitIndexedName(const std::string &inName, size_t &index)
{
    if (not isIndexedName(inName)) throw std::runtime_error("splitIndexedName("+inName+") not in name[index] format");
    const size_t openBracketPos = inName.find_last_of("[");
    const size_t closeBracketPos = inName.find_last_of("]");
    index = std::stoul(inName.substr(openBracketPos+1, closeBracketPos-openBracketPos-1));
    return inName.substr(0, openBracketPos);
}

//! Split a comma-separated string into its components
static inline std::vector<std::string> csvSplit(const std::string &in)
{
    std::vector<std::string> out;
    std::string tmp;

    //accumulate into tmp until comma is seen
    for (const auto &ch : in)
    {
        if (ch == ',')
        {
            out.push_back(tmp);
            tmp.clear();
        }
        else tmp += ch;
    }
    if (not tmp.empty()) out.push_back(tmp);

    //trim out leading and trailing space
    for (auto &s : out)
    {
        while (not s.empty() and std::isspace(s[0])) s = s.substr(1);
        while (not s.empty() and std::isspace(s[s.size()-1])) s = s.substr(0, s.size()-1);
    }

    return out;
}

//! Join multiple strings into comma separated format
static inline std::string csvJoin(const std::vector<std::string> &in)
{
    std::string out;
    for (const auto &s : in)
    {
        if (not out.empty()) out += ", ";
        out += s;
    }
    return out;
}

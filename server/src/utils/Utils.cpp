//
// Created by quent on 19/06/2019.
//

#include "Utils.h"

const vector<string> Utils::explode(const string &s, const char &c)
{
    string buff{""};
    vector<string> v;

    for(auto n:s)
    {
        if(n != c) buff+=n; else
        if(n == c && buff != "") { v.push_back(buff); buff = ""; }
    }
    if(buff != "") v.push_back(buff);

    return v;
}

std::string Utils::generateRandomString(const int len) {
    std::string string;

    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        string += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return string;
}

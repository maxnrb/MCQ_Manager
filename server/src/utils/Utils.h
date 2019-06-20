//
// Created by quent on 19/06/2019.
//

#ifndef PROJET_UTILS_H
#define PROJET_UTILS_H

#include <vector>
#include <string>

using std::vector;
using std::string;

class Utils{
public:
    static const vector<string> explode(const string& s, const char& c);
    static std::string generateRandomString(const int len);
};


#endif //PROJET_UTILS_H

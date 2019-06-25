//
// Created by quent on 19/06/2019.
//

#ifndef PROJET_UTILS_H
#define PROJET_UTILS_H

#include <vector>
#include <string>
#include <QImage>

using std::vector;
using std::string;

class Utils{
public:
    static const vector<string> explode(const string& s, const char& c);
    static string generateRandomString(const int len);
    static string getBase64ImgFromUrl(string url, int argc, char** argv);
    static QImage getImageFromUrl(string url, int argc, char** argv);
    /**
     * Return true if @s1 contains @s2
     * @param s1
     * @param s2
     * @return
     */
    static bool contains(string s1, string s2)
    {
        if (s1.find(s2) != std::string::npos) {
            return true;
        }
        return false;
    }
};


#endif //PROJET_UTILS_H

//
// Created by quent on 19/06/2019.
//

#include <QtWidgets/QApplication>
#include "Utils.h"
#include "../Qt/ImageDownloader.h"

/**
 * Split string into vector of strings according to @c separator
 * @param s
 * @param c
 * @return
 */
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

/**
 * Generate a random string of alphanumerics
 * @param len
 * @return
 */
string Utils::generateRandomString(const int len) {
    string string;

    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        string += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return string;
}

/**
 * Download an image from url provided and convert it to base64 string.
 * @param url
 * @param argc
 * @param argv
 * @return
 */
string Utils::getBase64ImgFromUrl(string url, int argc, char** argv)
{
    QApplication* app = new QApplication(argc, argv);
    ImageDownloader* downloader = new ImageDownloader(url);
    app->exec();
    string b64  = downloader->getBase64();
    delete app;
    return b64;
}

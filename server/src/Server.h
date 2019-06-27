//
// Created by quent on 18/06/2019.
//

#ifndef PROJET_HELLOHANDLER_H
#define PROJET_HELLOHANDLER_H

#include <pistache/endpoint.h>
#include "headers/CORS.h"
#include "utils/Utils.h"
#include "Database.h"
#include "headers/Authorization.h"
#include "utils/base64.h"
#include "Qt/ImageDownloader.h"
#include "Qt/QtTest.h"

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <QApplication>

using namespace Pistache;

using std::map;
using std::string;

class Server : public Http::Handler
{
private:
    Database *db;
    ImageDownloader* window;
    int argc;
    char** argv;
    string url_scans;
    bool console_output;
public:

HTTP_PROTOTYPE(Server)
    Server(int argc, char** argv);

    ~Server()
    {
        delete db;
    }

    map<string, string> parseParameters(string params);

    void onRequest(const Http::Request &request, Http::ResponseWriter response) override;
};

#endif //PROJET_HELLOHANDLER_H

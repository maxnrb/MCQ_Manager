//
// Created by quent on 18/06/2019.
//

#ifndef PROJET_DATABASE_H
#define PROJET_DATABASE_H

#include <iostream>
#include <soci/soci.h>
#include <soci/mysql/soci-mysql.h>
#include <vector>

#include "model/Group.h"

using std::string;
using std::vector;


class Database
{

private:
    string host;
    string dbName;
    string username;
    string password;

    soci::session* session;

public:
    Database(string host, string username, string password, string db = "mcq_manager"): host(host), dbName(db), username(username), password(password){};
    ~Database();

    void connect();
    bool addGroup(string name);
    vector<Group*> getGroups();
    bool addStudent(string name, string surname, int group_id);

    bool addUser(string login, string password, int privilege=0);
    bool checkUser(string login, string password);
    bool setToken(string login, string token);
    bool checkToken(string token);

};


#endif //PROJET_DATABASE_H

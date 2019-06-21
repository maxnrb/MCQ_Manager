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
#include "model/Student.h"

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
    bool deleteGroup(int id);
    bool addStudent(string name, string surname, int group_id);
    vector<Student*> getStudents(int group_id);
    bool deleteStudent(int id);

    bool addUser(string login, string password, int privilege=0);
    bool checkUser(string login, string password);
    bool addTest(string name, string group, string date, int user);
    bool deleteTest(int id);
    bool setToken(string login, string token);
    bool checkToken(string token);
    int getUserIdByToken(string token);

};


#endif //PROJET_DATABASE_H
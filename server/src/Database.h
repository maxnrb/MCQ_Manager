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
#include "model/Test.h"
#include "model/StudentAnswer.h"

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
    bool modifyGroup(int id, string name);
    bool deleteGroup(int id);

    bool addStudent(string name, string surname, int group_id);
    vector<Student*> getStudents(int group_id);
    Student* getStudentById(int student_id);
    bool modifyStudent(int id, string &first_name, string &last_name, int group);
    bool deleteStudent(int id);
    bool setParticipate(int student_id, int test_id);
    bool setCorrected(int student_id, int test_id);

    int addTest(string name, string group, string date, int user);
    vector<Test*> getTests(int group_id);
    vector<Test*> getTestsByStudent(int student_id);
    vector<Student*> getStudentsByTest(int test_id);
    Test* getTestById(int id);
    bool modifyTest(int id, string name, string date);
    bool deleteTest(int id);

    int addQuestion(int test, int scale);
    bool addAnswerToQuestion(int question, bool isGood);
    bool addStudentAnswer(int student, int answer, bool state);
    vector<StudentAnswer*> getStudentAnswersByQuestion(int question, int student);
    bool modifyStudentAnswer(int student, int answer, bool state);
    vector<Question*> getQuestionsByTest(int test);
    vector<Answer*> getAnswersByQuestion(int question);
    bool modifyAnswer(int id, bool good);
    Question* getQuestionById(int id);
    Answer* getAnswerById(int id);



    bool setToken(string login, string token);
    bool checkToken(string token);
    int getUserIdByToken(string token);
    bool addUser(string login, string password, int privilege=0);
    bool modifyUser(int id, string login);
    bool deleteUser(int id);
    bool checkUser(string login, string password);
    string getUsersSerialized();

};


#endif //PROJET_DATABASE_H

//
// Created by quent on 18/06/2019.
//

#include "Database.h"
#include "utils/sha1.h"
#include "utils/Utils.h"

Database::~Database()
{
    session->close();
    delete(session);
}

void Database::connect()
{
    try {
        soci::session* session = new soci::session("mysql://db="+this->dbName+" host="+this->host+" user="+this->username+" password='"+this->password+"'");
        soci::statement setEncoding = (session->prepare << "SET NAMES utf8");
        setEncoding.execute(true);

        this->session = session;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
bool Database::addGroup(string name)
{
    try
    {
        soci::statement query = session->prepare << "INSERT INTO groups (name) VALUES('" + name + "')";
        query.execute(false);
        return true;
    }catch (const std::exception &e){
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

vector<Group*> Database::getGroups()
{
    vector<Group*> groups;
    try
    {
        string pass_hash = SHA1::toSha1(password);
        soci::row group_row;

        soci::statement query = (session->prepare
                << "SELECT id,name FROM groups",
                soci::into(group_row));
        query.execute(true);
        if(query.got_data())
        {
            do
            {
                Group *gp = new Group(group_row.get<int>(0), group_row.get<std::string>(1));
                groups.push_back(gp);
            } while (query.fetch());
        }
    }catch (const std::exception &e){
        std::cout << "Error: " << e.what() << std::endl;
    }
    return groups;
}

bool Database::deleteGroup(int id)
{
    try
    {
        soci::statement query = (session->prepare
                << "DELETE FROM groups WHERE id='"+std::to_string(id)+"'");
        query.execute(false);
        return true;
    }catch (const std::exception &e){
        std::cout << "Error: " << e.what() << std::endl;
        false;
    }
}

bool Database::addStudent(string name, string surname, int group_id)
{
    try
    {
        soci::statement query = session->prepare << "INSERT INTO students (group_id, first_name, last_name) VALUES('" + std::to_string(group_id) + "', '"+ name +"', '"+ surname +"')";
        query.execute(false);
        return true;
    }catch (const std::exception &e){
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

vector<Student*> Database::getStudents(int group_id)
{
    vector<Student*> students;
    try
    {
        string pass_hash = SHA1::toSha1(password);
        soci::row group_row;

        soci::statement query = (session->prepare
                << "SELECT id,first_name,last_name,group_id FROM students WHERE group_id='"+std::to_string(group_id)+"'",
                soci::into(group_row));
        query.execute(true);
        if(query.got_data())
        {
            do
            {
                Student *st = new Student(group_row.get<int>(0), group_row.get<std::string>(1), group_row.get<string>(2), group_row.get<int>(3));
                students.push_back(st);
            } while (query.fetch());
        }
    }catch (const std::exception &e){
        std::cout << "Error: " << e.what() << std::endl;
    }
    return students;
}

bool Database::deleteStudent(int id)
{
    try
    {
        soci::statement query = (session->prepare
                << "DELETE FROM students WHERE id='"+std::to_string(id)+"'");
        query.execute(false);
        return true;
    }catch (const std::exception &e){
        std::cout << "Error: " << e.what() << std::endl;
        false;
    }
}

bool Database::addUser(string login, string password, int privilege)
{
    try
    {
        string pass_hash = SHA1::toSha1(password);
        soci::statement query = session->prepare << "INSERT INTO users (login, password, privilege) VALUES('" + login + "', '" + pass_hash + "', '" + std::to_string(privilege) + "')";
        query.execute(false);
        return true;
    }catch (const std::exception &e){
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}
bool Database::checkUser(string login, string password)
{
    try
    {
        string pass_hash = SHA1::toSha1(password);
        soci::row user;

        soci::statement query = (session->prepare
                << "SELECT * FROM users WHERE login='" + login + "' AND password='" + pass_hash + "'", soci::into(
                user));
        query.execute(true);
        if (query.got_data())
        {
            return true;
        }
        else
        {
            return false;
        }
    }catch (const std::exception &e){
        std::cout << "Error: " << e.what() << std::endl;
        return false;
    }
}

bool Database::addTest(string name, string group, string date, int user)
{
    try
    {
        string pass_hash = SHA1::toSha1(password);
        vector<string> dates = Utils::explode(date, '/');
        date = dates.at(2)+"-"+dates.at(0)+"-"+dates.at(1);
        soci::statement query = session->prepare << "INSERT INTO tests (name, date, group_id, user_id) VALUES('" + name + "', '"+date+"', '" + group + "', '"+std::to_string(user)+"')";
        query.execute(false);
        return true;
    }catch (const std::exception &e){
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

bool Database::deleteTest(int id)
{
    try
    {
        soci::statement query = (session->prepare
                << "DELETE FROM tests WHERE id='"+std::to_string(id)+"'");
        query.execute(false);
        return true;
    }catch (const std::exception &e){
        std::cout << "Error: " << e.what() << std::endl;
        false;
    }
}

bool Database::setToken(string login, string token)
{
    try
    {
        string pass_hash = SHA1::toSha1(password);

        soci::statement query = (session->prepare
                << "UPDATE users SET token='"+token+"' WHERE login='" + login + "'");
        query.execute(false);
        return true;
    }catch (const std::exception &e){
        std::cout << "Error: " << e.what() << std::endl;
        return false;
    }
}

bool Database::checkToken(string token)
{
    try
    {
        soci::row user;
        soci::statement query = (session->prepare
                << "SELECT * FROM users WHERE token='" + token + "'", soci::into(
                user));
        query.execute(true);
        if (query.got_data())
        {
            return true;
        }
        else
        {
            return false;
        }
    }catch (const std::exception &e){
        std::cout << "Error: " << e.what() << std::endl;
        return false;
    }
}

int Database::getUserIdByToken(string token)
{
    try
    {
        soci::row user;
        soci::statement query = (session->prepare
                << "SELECT id FROM users WHERE token='" + token + "'", soci::into(
                user));
        query.execute(true);
        if (query.got_data())
        {
            return user.get<int>(0);
        }
        else
        {
            return -1;
        }
    }catch (const std::exception &e){
        std::cout << "Error: " << e.what() << std::endl;
        return -1;
    }
}

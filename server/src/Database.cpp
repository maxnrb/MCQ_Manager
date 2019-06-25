//
// Created by quent on 18/06/2019.
//

#include "Database.h"
#include "utils/sha1.h"
#include "utils/Utils.h"
#include <string>

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


/************************************************************************************************\
 * Groups
\************************************************************************************************/
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

bool Database::modifyGroup(int id, string name)
{
    try
    {
        soci::statement query = (session->prepare
                << "UPDATE groups SET name='"+name+"' WHERE id='"+std::to_string(id)+"'");
        query.execute(false);
        return true;
    }catch (const std::exception &e){
        std::cout << "Error: " << e.what() << std::endl;
        return false;
    }
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
        return false;
    }
}


/************************************************************************************************\
 * Students
\************************************************************************************************/
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

Student* Database::getStudentById(int student_id)
{
    try
    {
        string pass_hash = SHA1::toSha1(password);
        soci::row group_row;

        soci::statement query = (session->prepare
                << "SELECT id,first_name,last_name,group_id FROM students WHERE id='"+std::to_string(student_id)+"'",
                soci::into(group_row));
        query.execute(true);
        if(query.got_data())
        {
            Student *student = new Student(group_row.get<int>(0), group_row.get<string>(1), group_row.get<string>(2), group_row.get<int>(3));
            return student;
        }
    }catch (const std::exception &e){
        std::cout << "Error: " << e.what() << std::endl;
        return nullptr;
    }
    return nullptr;
}

bool Database::modifyStudent(int id, string &first_name, string &last_name, int group)
{
    try
    {
        soci::statement query = (session->prepare
                << "UPDATE students SET first_name='"+first_name+"', last_name='"+last_name+"', group_id='"+std::to_string(group)+"' WHERE id='"+std::to_string(id)+"'");
        query.execute(false);
        return true;
    }catch (const std::exception &e){
        std::cout << "Error: " << e.what() << std::endl;
        return false;
    }
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
        return false;
    }
}

bool Database::setParticipate(int student_id, int test_id)
{
    try
    {
        soci::statement query = session->prepare << "INSERT INTO participate (student_id, test_id) VALUES('" + std::to_string(student_id) + "', '"+std::to_string(test_id)+"')";
        query.execute(false);
        return true;
    }catch (const std::exception &e){
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

bool Database::setCorrected(int student_id, int test_id)
{
    try
    {
        soci::statement query = session->prepare << "UPDATE participate SET corrected='1' WHERE student_id='"+std::to_string(student_id)+"' AND test_id='"+std::to_string(test_id)+"'";
        query.execute(false);
        return true;
    }catch (const std::exception &e){
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}


/************************************************************************************************\
 * Tests
\************************************************************************************************/
bool Database::addTest(string name, string group, string date, int user)
{
    try
    {
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

vector<Test*> Database::getTests(int group_id)
{
    vector<Test*> tests;
    try
    {
        soci::row group_row;

        soci::statement query = (session->prepare
                << "SELECT id,name,DATE_FORMAT(date, \"%m/%d/%Y\"),user_id FROM tests WHERE group_id='"+std::to_string(group_id)+"'",
                soci::into(group_row));
        query.execute(true);
        if(query.got_data())
        {
            do
            {
                Test *test = new Test(group_row.get<int>(0), group_row.get<string>(1), group_row.get<string>(2), group_row.get<int>(3));
                tests.push_back(test);
            } while (query.fetch());
        }
    }catch (const std::exception &e){
        std::cout << "Error: " << e.what() << std::endl;
    }
    return tests;
}

vector<Test*> Database::getTestsByStudent(int student_id)
{
    vector<Test*> tests;
    try
    {
        soci::row group_row;

        soci::statement query = (session->prepare
                << "SELECT test_id, corrected FROM participate WHERE student_id='"+std::to_string(student_id)+"'",
                soci::into(group_row));
        query.execute(true);
        if(query.got_data())
        {
            do
            {
                Test *test = getTestById(group_row.get<int>(0));
                if(group_row.get<int>(1) == 1)
                {
                    test->setCorrected(true);
                }
                tests.push_back(test);
            } while (query.fetch());
        }
    }catch (const std::exception &e){
        std::cout << "Error: " << e.what() << std::endl;
    }
    return tests;
}

vector<Student*> Database::getStudentsByTest(int test_id)
{
    vector<Student*> students;
    try
    {
        soci::row group_row;

        soci::statement query = (session->prepare
                << "SELECT student_id, corrected FROM participate WHERE test_id='"+std::to_string(test_id)+"'",
                soci::into(group_row));
        query.execute(true);
        if(query.got_data())
        {
            do
            {
                Student *student = getStudentById(group_row.get<int>(0));
                if(group_row.get<int>(1) == 1)
                {
                    student->setCorrected(true);
                }
                students.push_back(student);
            } while (query.fetch());
        }
    }catch (const std::exception &e){
        std::cout << "Error: " << e.what() << std::endl;
    }
    return students;
}

Test* Database::getTestById(int id)
{
    try
    {
        string pass_hash = SHA1::toSha1(password);
        soci::row group_row;

        soci::statement query = (session->prepare
                << "SELECT id,name,DATE_FORMAT(date, \"%m/%d/%Y\"),user_id FROM tests WHERE id='"+std::to_string(id)+"'",
                soci::into(group_row));
        query.execute(true);
        if(query.got_data())
        {
            Test *test = new Test(group_row.get<int>(0), group_row.get<string>(1), group_row.get<string>(2), group_row.get<int>(3));
            return test;
        }
    }catch (const std::exception &e){
        std::cout << "Error: " << e.what() << std::endl;
        return nullptr;
    }
    return nullptr;
} //Completer

bool Database::modifyTest(int id, string name, string date)
{
    try
    {
        vector<string> dates = Utils::explode(date, '/');
        date = dates.at(2)+"-"+dates.at(0)+"-"+dates.at(1);
        soci::statement query = (session->prepare
                << "UPDATE tests SET name='"+name+"', date='"+date+"' WHERE id='"+std::to_string(id)+"'");
        query.execute(false);
        return true;
    }catch (const std::exception &e){
        std::cout << "Error: " << e.what() << std::endl;
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
        return false;
    }
}

bool Database::addQuestion(int test, int scale)
{
    try
    {
        int number =  this->getQuestionsByTest(test).size()+1;

        soci::statement query = session->prepare << "INSERT INTO questions (question_number, scale, test_id) VALUES('"+std::to_string(number)+"', '"+std::to_string(scale)+"', '"+std::to_string(test)+"')";
        query.execute(false);
        return true;
    }catch (const std::exception &e){
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

bool Database::addAnswerToQuestion(int question, bool isGood)
{
    try
    {
        int number = this->getAnswersByQuestion(question).size()+1;
        soci::statement query = session->prepare << "INSERT INTO answer (answer_number, good_answer, question_id) VALUES('"+std::to_string(number)+"', '"+((isGood)?"1":"0")+"', '"+std::to_string(question)+"')";
        query.execute(false);
        return true;
    }catch (const std::exception &e){
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

bool Database::addStudentAnswer(int student, int answer, bool state)
{
    try
    {
        soci::statement query = session->prepare << "INSERT INTO students_answer (answer, student_id, answer_id) VALUES('"+std::to_string(state)+"', '"+std::to_string(student)+"', '"+std::to_string(answer)+"')";
        query.execute(false);
        return true;
    }catch (const std::exception &e){
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

vector<StudentAnswer*> Database::getStudentAnswersByQuestion(int question, int student)
{
    Question* q = getQuestionById(question);

    for(Answer* a : q->getAnswers())
    {
        vector<StudentAnswer*> student_answers;
        try
        {
            soci::row group_row;

            soci::statement query = (session->prepare
                    << "SELECT id,answer,student_id,answer_id FROM students_answer WHERE answer_id='"+std::to_string(a->getId())+"'",
                    soci::into(group_row));
            query.execute(true);
            if(query.got_data())
            {
                do
                {
                    StudentAnswer *sa = new StudentAnswer(group_row.get<int>(0), group_row.get<my_bool >(1), group_row.get<int>(2), group_row.get<int>(3));
                    student_answers.push_back(sa);
                } while (query.fetch());
            }
        }catch (const std::exception &e){
            std::cout << "Error: " << e.what() << std::endl;
        }
        delete q;
        return student_answers;
    }
    delete q;
}

bool Database::modifyStudentAnswer(int student, int answer, bool state)
{
    try
    {
        soci::statement query = (session->prepare
                << "UPDATE students_answer SET answer='"+std::to_string(state)+"' WHERE answer_id='"+std::to_string(answer)+"' AND student_id='"+std::to_string(student)+"'");
        query.execute(false);
        return true;
    }catch (const std::exception &e){
        std::cout << "Error: " << e.what() << std::endl;
        return false;
    }
}

vector<Question*> Database::getQuestionsByTest(int test)
{
    vector<Question*> questions;
    try
    {
        soci::row group_row;

        soci::statement query = (session->prepare
                << "SELECT id,question_number,test_id FROM questions WHERE test_id='"+std::to_string(test)+"'",
                soci::into(group_row));
        query.execute(true);
        if(query.got_data())
        {
            do
            {
                Question *question = new Question(group_row.get<int>(0), group_row.get<int>(1), group_row.get<int>(2));
                questions.push_back(question);

                soci::row answer_row;

                soci::statement queryA = (session->prepare
                        << "SELECT id,answer_number,good_answer FROM answer WHERE question_id='"+std::to_string(group_row.get<int>(0))+"'",
                        soci::into(answer_row));
                queryA.execute(true);

                if(queryA.got_data())
                {
                    do
                    {
                        Answer* answer = new Answer(answer_row.get<int>(0), answer_row.get<int>(1), answer_row.get<my_bool>(2));
                        question->addAnswer(answer);
                    }while (queryA.fetch());
                }

            } while (query.fetch());
        }
    }catch (const std::exception &e){
        std::cout << "Error: " << e.what() << std::endl;
    }
    return questions;
}

vector<Answer*> Database::getAnswersByQuestion(int question)
{
    vector<Answer*> answers;
    try
    {
        soci::row group_row;

        soci::statement query = (session->prepare
                << "SELECT id,answer_number,good_answer,question_id FROM answer WHERE question_id='"+std::to_string(question)+"'",
                soci::into(group_row));
        query.execute(true);
        if(query.got_data())
        {
            do
            {
                Answer *an = new Answer(group_row.get<int>(0), group_row.get<int>(1), group_row.get<int>(2));
                answers.push_back(an);
            } while (query.fetch());
        }
    }catch (const std::exception &e){
        std::cout << "Error: " << e.what() << std::endl;
    }
    return answers;
}

Question* Database::getQuestionById(int id)
{
    try
    {
        soci::row group_row;

        soci::statement query = (session->prepare
                << "SELECT id,question_number,test_id FROM questions WHERE id='"+std::to_string(id)+"'",
                soci::into(group_row));
        query.execute(true);
        if(query.got_data())
        {
            Question *question = new Question(group_row.get<int>(0), group_row.get<int>(1), group_row.get<int>(2));

            soci::row answer_row;

            soci::statement queryA = (session->prepare
                    << "SELECT id,answer_number,good_answer FROM answer WHERE question_id='"+std::to_string(group_row.get<int>(0))+"'",
                    soci::into(answer_row));
            queryA.execute(true);

            if(queryA.got_data())
            {
                do
                {
                    Answer* answer = new Answer(answer_row.get<int>(0), answer_row.get<int>(1), answer_row.get<int>(2));
                    question->addAnswer(answer);
                }while (queryA.fetch());
            }

            return question;
        }
        else
        {
            return nullptr;
        }
    }catch (const std::exception &e){
        std::cout << "Error: " << e.what() << std::endl;
    }
    return nullptr;
}

/************************************************************************************************\
 * Token and authentication
\************************************************************************************************/
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

bool Database::addUser(string login, string password, int privilege)
{
    try
    {
        string pass_hash = SHA1::toSha1(password);
        soci::statement query = session->prepare << "INSERT INTO users (login, password, superadmin) VALUES('" + login + "', '" + pass_hash + "', '" + std::to_string(privilege) + "')";
        query.execute(false);
        return true;
    }catch (const std::exception &e){
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

bool Database::modifyUser(int id, string login)
{
    try
    {
        soci::statement query = session->prepare << "UPDATE users SET login='" + login + "' WHERE id='"+std::to_string(id)+"'";
        query.execute(false);
        return true;
    }catch (const std::exception &e){
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

bool Database::deleteUser(int id)
{
    try
    {
        soci::statement query = session->prepare << "DELETE FROM users WHERE id='"+std::to_string(id)+"'";
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

string Database::getUsersSerialized()
{
    string json = "[";
    try
    {
        soci::row group_row;

        soci::statement query = (session->prepare
                << "SELECT id,login FROM users",
                soci::into(group_row));
        query.execute(true);
        if(query.got_data())
        {
            if(query.got_data())
            {
                do
                {
                    json += "{";
                    json += "\"id\": \"" + std::to_string(group_row.get<int>(0)) + "\",";
                    json += "\"login\": \"" + group_row.get<string>(1) + "\"";
                    json += "},";
                }while (query.fetch());
                json.pop_back();
            }
        }

        json += "]";
        return json;
    }catch (const std::exception &e){
        std::cout << "Error: " << e.what() << std::endl;
    }
    return nullptr;
}

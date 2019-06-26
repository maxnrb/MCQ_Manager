//
// Created by quent on 21/06/2019.
//

#ifndef SERVER_TEST_H
#define SERVER_TEST_H

#include <string>
#include <vector>
#include "Question.h"
#include "../utils/Utils.h"

using std::string;
using std::vector;

class Test
{
private:
    int id;
    string name;
    string date;
    int user_id;

    bool corrected= false;

    vector<Question*> questions;

public:
    Test(int id, const string &name, const string &date, int userId) : id(id),
                                                                                                            name(name),
                                                                                                            date(date),
                                                                                                            user_id(userId)
    {}

    ~Test()
    {
        for(Question* a : questions)
        {
            delete a;
        }
    }

    void setCorrected(bool corrected){this->corrected = corrected;}
    bool isCorrected(){return this->corrected;}

    int getId() const{return id;}
    void setId(int id){Test::id = id;}

    const string getName() const{return name;}
    void setName(const string name){Test::name = name;}

    const string getDate() const{return date;}
    void setDate(const string date){Test::date = date;}

    int getUserId() const{return user_id;}
    void setUserId(int userId){user_id = userId;}

    const vector<Question *> &getQuestions() const{return questions;}
    void setQuestions(const vector<Question *> &questions){Test::questions = questions;}
    void addQuestion(Question* question){questions.push_back(question);}

    string serialize()
    {
        string json = "{";
        json += "\"id\":\""+std::to_string(id)+"\",";
        json += "\"name\":\""+name+"\",";
        json += "\"date\":\""+date+"\",";
        json += "\"user_id\":\""+std::to_string(user_id)+"\",";
        json += "\"corrected\":\"";
        json += ((corrected)?"1":"0");
        json += "\",";
        json += "\"questions\":[";
        for(Question* question : questions)
        {
            json += question->serialize();
            json += ",";
        }
        if(questions.size() != 0) json.pop_back();
        json += "]";
        json += "}";
        return json;
    }
};


#endif //SERVER_TEST_H

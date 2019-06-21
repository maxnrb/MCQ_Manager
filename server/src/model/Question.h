//
// Created by quent on 21/06/2019.
//

#ifndef SERVER_QUESTION_H
#define SERVER_QUESTION_H

#include <vector>
#include "Answer.h"

using std::vector;

class Question
{
private:
    int id;
    int question_number;

    vector<Answer*> answers;

public:
    int getId() const{return id;}
    void setId(int id){Question::id = id;}

    int getQuestionNumber() const{return question_number;}
    void setQuestionNumber(int questionNumber){question_number = questionNumber;}

    const vector<Answer*> &getAnswers() const{return answers;}
    void setAnswers(const vector<Answer*> &answers){Question::answers = answers;}
    void addAnswer(Answer* answer){answers.push_back(answer);}

    string serialize()
    {
        string json = "{";
        json += "\"id\":\""+std::to_string(id)+"\",";
        json += "\"question_number\":\""+std::to_string(question_number)+"\",";
        json += "\"answers\":\"[";
        for(Answer* answer : answers)
        {
            json += answer->serialize();
            json += ",";
        }
        json.pop_back();
        json += "]";
        json += "}";
        return json;
    }
};


#endif //SERVER_QUESTION_H

//
// Created by quent on 21/06/2019.
//

#ifndef SERVER_ANSWER_H
#define SERVER_ANSWER_H

#include <string>

using std::string;

class Answer
{
private:
    int id;
    int answer_number;
    bool good_answer;

public:
    Answer(int id, int answerNumber, bool goodAnswer) : id(id), answer_number(answerNumber), good_answer(goodAnswer)
    {}

    int getId() const{return id;}
    void setId(int id){Answer::id = id;}

    int getAnswerNumber() const{return answer_number;}
    void setAnswerNumber(int answerNumber){answer_number = answerNumber;}

    bool isGoodAnswer() const{return good_answer;}
    void setGoodAnswer(bool goodAnswer){good_answer = goodAnswer;}

    string serialize()
    {
        string json = "{";
        json += "\"id\":\""+std::to_string(id)+"\",";
        json += "\"answer_number\":\""+std::to_string(answer_number)+"\",";
        json += "\"good_answer\":\""+std::to_string(good_answer)+"\"";
        json += "}";
        return json;
    }
};


#endif //SERVER_ANSWER_H

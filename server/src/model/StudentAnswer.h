//
// Created by quent on 24/06/2019.
//

#ifndef SERVER_STUDENTANSWER_H
#define SERVER_STUDENTANSWER_H


class StudentAnswer
{
private:
    int id;
    bool state;
    int student_id;
    int answer_id;

public:
    StudentAnswer(int id, bool state, int studentId, int answerId) : id(id), state(state), student_id(studentId), answer_id(answerId)
    {

    }

    int getId() const
    {
        return id;
    }

    void setId(int id)
    {
        StudentAnswer::id = id;
    }

    bool isState() const
    {
        return state;
    }

    void setState(bool state)
    {
        StudentAnswer::state = state;
    }

    int getStudentId() const
    {
        return student_id;
    }

    void setStudentId(int studentId)
    {
        student_id = studentId;
    }

    int getAnswerId() const
    {
        return answer_id;
    }

    void setAnswerId(int answerId)
    {
        answer_id = answerId;
    }
};


#endif //SERVER_STUDENTANSWER_H

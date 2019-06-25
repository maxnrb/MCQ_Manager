//
// Created by arthu on 24/06/2019.
//

#ifndef QCM2_QTTEST_H
#define QCM2_QTTEST_H

#include <QMainWindow>


class QtTest : public QObject
{
    Q_OBJECT


private:
    QImage image;

    QPointF*** coo;

    bool** state;

    int nbAnswer;
    int nbQuestions;

    bool error=false;
    int moyX=1;
    int moyY=1;
    int moyXRight=1;
    int moyYBot=1;
    //bool state[nbQuestions][nbReponses];

    QList<int>* lenghtX = new QList<int>;
    QList<int>* widthY = new QList<int>;
    QList<int>* change = new QList<int>;
    QList<int>* middle = new QList<int>;
    QList<int>* middleLeft = new QList<int>;
    QList<int>* middleRight = new QList<int>;
    QList<int>* middleTop = new QList<int>;
    QList<int>* middleBot = new QList<int>;

    void setMoyXRight();
    void setMoyYBot();

    void initChange(int value);
    void initChange2(int value);

    void initMiddle();


    void formattingData();
    void bigSquareLeft();
    void squareLeft();
    void squareRight();
    void squareTop();
    void squareBot();

    void horizontalLine();
    void checkBox();

public:
    int getNbAnswer() const;
    void setNbAnswer(int nbAnswer);
    int getNbQuestions() const;
    void setNbQuestions(int nbQuestions);

    void setMyPixmap(const QPixmap &myPixmap);
    void setImage();

    QtTest(QImage &image,QWidget* parent = nullptr);
    virtual ~QtTest() {};

    QPointF ***getCoo() const
    {
        return coo;
    }

    bool **getState() const
    {
        return state;
    }

};


#endif //QCM2_QTTEST_H

//
// Created by arthu on 24/06/2019.
//
#include <QHBoxLayout>
#include <QtWidgets/qlabel.h>
#include <QtCore/QDebug>
#include <QList>
#include <stdlib.h>
#include <QPainter>

#include "math.h"
#include "QtTest.h"

QtTest::QtTest(QImage &image,QWidget *parent)
{
    this->image = image;

    formattingData();
    bigSquareLeft();
    squareLeft();
    squareTop();
    squareRight();
    squareBot();
    horizontalLine();
    checkBox();
    circle();

    //QImage image("../rotation1.jpg");
}

void QtTest::formattingData(){
    QColor* actualPixelColor = new QColor;
    QColor* precXPixelColor = new QColor;
    QColor* precYPixelColor = new QColor;
    QColor* afterXPixelColor = new QColor;
    QColor* afterYPixelColor = new QColor;

    int lightnessact=0;
    int lightnessExX=0;
    int lightnessExY=0;
    int lightnessAfterX=0;
    int lightnessAfterY=0;

    long val = 0;
    long valPreX = 0;
    long valPreY = 0;
    long valAfterX = 0;
    long valAfterY = 0;

    int limen=200;
    int precision=5;

    for(int x=precision+1;x<2450;x++){
        for(int y=precision+1; y<3450; y++){

            val = image.pixel(x, y);
            valPreX = image.pixel(x-precision, y);
            valPreY = image.pixel(x, y-precision);
            valAfterX = image.pixel(x + precision, y);
            valAfterY = image.pixel(x, y + precision);

            actualPixelColor->setRgba(val);
            lightnessact = actualPixelColor->lightness();

            precXPixelColor->setRgba(valPreX);
            lightnessExX = precXPixelColor->lightness();

            precYPixelColor->setRgba(valPreY);
            lightnessExY = precYPixelColor->lightness();

            afterXPixelColor->setRgba(valAfterX);
            lightnessAfterX = afterXPixelColor->lightness();

            afterYPixelColor->setRgba(valAfterY);
            lightnessAfterY = afterYPixelColor->lightness();

            if(lightnessact < limen && lightnessExX < limen && lightnessExY < limen && lightnessAfterX < limen && lightnessAfterY < limen ){
                //image.setPixel(x,y,qRgb(0, 0, 255));
                lenghtX->push_back(x);
                widthY->push_back(y);
            }

        }
    }
}


void QtTest::bigSquareLeft(){
    int totalX=0;
    int totalY=0;
    int carre=350;

    int z=0;
    for (int i = 0; i < widthY->size(); ++i) {
        if(widthY->at(i) < carre && lenghtX->at(i) < carre){
            z++;
            totalX=totalX+lenghtX->at(i);
            totalY=totalY+widthY->at(i);
        }
    }
    if(z==0){
        error=true;
        //la feuille est trop mal placé il faut recommencer le scan
    }
    else{
        moyX = totalX/z;
        moyY = totalY/z;
    }
}

void QtTest::initChange(int value){
    change->clear();
    int last=0;
    for (int i = 0; i < widthY->size(); ++i) {
        if(lenghtX->at(i) > value-5 && lenghtX->at(i) < value+5 ){
            last=widthY->at(i);
            if (widthY->at(i) != widthY->at(i-1)+1 && widthY->at(i)!=widthY->at(i-2)){
                //qDebug()<< "longueur : " << lenghtX->at(i) << " / largeur : " << widthY->at(i) << value;
                change->push_back(widthY->at(i-1));
                change->push_back(widthY->at(i));
            }
        }
    }
    change->push_back(last);
    change->pop_front();
}
void QtTest::initChange2(int value){
    change->clear();
    int last=0;
    for (int i = 1; i < widthY->size(); ++i) {
        if(widthY->at(i) > value-5 && widthY->at(i) < value+5 ){
            last=lenghtX->at(i);
            if (lenghtX->at(i) != lenghtX->at(i-1)+1 ){
                change->push_back(lenghtX->at(i-1));
                change->push_back(lenghtX->at(i));
            }
        }
    }
    change->push_back(last);
    change->pop_front();
}
void QtTest::initMiddle() {
    middle->clear();
    int middleVar=0;
    for(int i = 0; i < change->size()-1; i=i+2){
        middleVar=(change->at(i)+change->at(i+1))/2;
        middle->push_back(middleVar);
        if(i==change->size()-3){
            i++;
            middleVar=(change->at(i)+change->at(i+1))/2;
            middle->push_back(middleVar);
        }
    }

    int f=0;
    for(int i = 0; i < middle->size()-1; i++) {
        if (middle->at(i) > middle->at(i+1)){
            f=i;
            while(middle->at(f) > middle->at(f+1) && f>0){
                middle->swap(f,f+1);
                f--;
            }
        }
    }
}

void QtTest::squareLeft() {
    middleLeft->clear();

    initChange(moyX);
    initMiddle();

    int totalmid=0;
    int nb=0;
    int goodMid=0;
    for(int i = 0; i < middle->size()-1; i++) {
        if(middle->at(i)+15 > middle->at(i+1)){
            totalmid=totalmid+middle->at(i);
            nb++;
        }
        else if(nb > 0 && totalmid > 0){
            goodMid = totalmid / nb ;
            middleLeft->push_back(goodMid);
            nb=0;
            totalmid=0;
        }
    }
    if(nb!=0) {
        goodMid = totalmid / nb ;
        middleLeft->push_back(goodMid);
        }
    else{
        error=true;
    }

}

void QtTest::squareTop() {
    initChange2(moyY);
    initMiddle();
    int totalmid=0;
    int nb=0;
    int goodMid=0;
    for(int i = 0; i < middle->size()-1; i++) {
        if(middle->at(i)+5 > middle->at(i+1)){
            totalmid=totalmid+middle->at(i);
            nb++;
        }
        else if(nb > 0 && totalmid > 0){
            goodMid = totalmid / nb ;
            middleTop->push_back(goodMid);
            nb=0;
            totalmid=0;
        }
    }
    if(nb!=0){
        goodMid = totalmid / nb ;
        middleTop->push_back(goodMid);
        }
    else{
        error=true;
    }
}

void QtTest::setMoyXRight(){
    int totalDroite=0;
    int nbDroite=0;

    for (int i = 0; i < widthY->size(); ++i) {
        if(lenghtX->at(i) > middleTop->at(middleTop->size()-1) + 30 ){
            if (widthY->at(i) != widthY->at(i-1)+1){
                totalDroite=totalDroite+lenghtX->at(i);
                nbDroite++;
            }
        }
    }

    if(nbDroite==0){
        error=true;
    }
    else{
        moyXRight=totalDroite/nbDroite;
    }
}

void QtTest::squareRight() {
    //obtenir le x moyen de droite
    setMoyXRight();
    initChange(moyXRight);
    initMiddle();

    int totalmid=0;
    int nb=0;
    int goodMid=0;
    for(int i = 0; i < middle->size()-1; i++) {
        if(middle->at(i)+5 > middle->at(i+1)){
            totalmid=totalmid+middle->at(i);
            nb++;
        }
        else if(nb > 0 && totalmid > 0){
            goodMid = totalmid / nb ;
            middleRight->push_back(goodMid);
            nb=0;
            totalmid=0;
        }
    }
    if(nb!=0) {
            goodMid = totalmid / nb;
            middleRight->push_back(goodMid);
        }
        else{
            error=true;
        }
}

void QtTest::setMoyYBot(){
    int totalBot=0;
    int nbBot=0;
    for (int i = 0; i < widthY->size(); ++i) {
        if(widthY->at(i) > middleLeft->at(middleLeft->size()-1) + 30 ){
            totalBot=totalBot+widthY->at(i);
            nbBot++;
        }
    }
    if(nbBot==0){
        error=true;
    }
    else{
        moyYBot=totalBot/nbBot;
    }
}
void QtTest::squareBot() {
    //obtenir le y moyen du bas
    setMoyYBot();
    initChange2(moyYBot);
    initMiddle();

    int totalmid=0;
    int nb=0;
    int goodMid;
    for(int i = 0; i < middle->size()-1; i++) {
        if(middle->at(i)+5 > middle->at(i+1)){
            totalmid=totalmid+middle->at(i);
            nb++;
        }
        else if(nb > 0 && totalmid > 0){
            goodMid = totalmid / nb ;
            middleBot->push_back(goodMid);
            nb=0;
            totalmid=0;
        }
    }
    if(nb!=0) {
            goodMid = totalmid / nb;
            middleBot->push_back(goodMid);
        }
        else{
            error=true;
        }
}

void QtTest::horizontalLine(){
//enlever la zone ou il y a le nom :
    int diffBase=middleLeft->at(2)-middleLeft->at(1);
    int diff=0;
    int marge = 50;
    int test=0;
//gauche
    int valPopLeft=1;
    for (int i=1; i<middleLeft->size();i++){
        diff=middleLeft->at(i+1)-middleLeft->at(i);
        if(test==0){
            if(diff > diffBase+marge){
                test = 1;
            }
        }
        else if(test==1){
            while(i>0) {
                i--;
                valPopLeft++;
            }
            break ;
        }
    }
//droite
    test=0;
    int valPopRight=0;
    diffBase=middleRight->at(1)-middleRight->at(1);
    for (int i=0; i<middleRight->size();i++){
        diff=middleRight->at(i+1)-middleRight->at(i);
        if(test==0){
            if(diff > diffBase+marge){
                test = 1;
            }
        }
        else if(test==1){
            while(i>0) {
                valPopRight++;
                i--;
            }
            break ;
        }
    }
//carré intersection

    QList<QLineF*>* qLine = new QList<QLineF*>;
    for(int i=valPopLeft-1;i<middleLeft->size();i++){
        qLine->push_back(new QLineF(moyX,middleLeft->at(i),moyXRight,middleLeft->at(i)));
    }


    QList<QLineF*>* qColonne = new QList<QLineF*>;
    for(int i=1;i<middleTop->size();i++){
        qColonne->push_back(new QLineF(middleTop->at(i),moyY,middleBot->at(i-1),moyYBot));
    }


    int nbReponses = (middleTop->size()-1)/2;
    int nbQuestion = (middleLeft->size()-valPopLeft+1)*2;

    setNbAnswer(nbReponses);
    setNbQuestions(nbQuestion);
    //QPointF* coo[nbQuestions][nbReponses];
    coo = (QPointF***) malloc(getNbQuestions()*sizeof(QPointF**));
    state = (bool**) malloc(getNbQuestions()*sizeof(bool**));
    for(int i = 0; i < getNbQuestions(); i++)
    {
        coo[i] = (QPointF**) malloc(getNbAnswer()*sizeof(QPointF*));
        state[i]= (bool*) malloc(getNbAnswer()*sizeof(bool));
    }

    for (int i=0; i < qLine->size(); i++) {
        for (int j=0; j < qColonne->size(); j++) {
            QPointF* a=new QPointF;
            qLine->at(i)->intersect(*qColonne->at(j),a);
            if(j<nbReponses){
                coo[i][j]=a;
            }
            else{
                coo[i+getNbQuestions()/2][j-getNbAnswer()]=a;
            }
        }
    }

    for(int i=0; i<getNbQuestions();i++){
        for(int j=0;j<getNbAnswer();j++){
            for (int m = 0; m < 40; m++) {
                for (int n = 0; n < 40; n++) {
                    //image.setPixel(coo[i][j]->x()-20+m,coo[i][j]->y()-20+n, qRgb(0, 0, 255));
                }
            }
        }
    }
}

int QtTest::getNbAnswer() const {
    return nbAnswer;
}
void QtTest::setNbAnswer(int nbAnswer) {
    QtTest::nbAnswer = nbAnswer;
}
int QtTest::getNbQuestions() const {
    return nbQuestions;
}
void QtTest::setNbQuestions(int nbQuestions) {
    QtTest::nbQuestions = nbQuestions;
}

void QtTest::checkBox(){
    int nbFois=0;
    int precision2=10;
    for(int i=0; i<getNbQuestions();i++){
        for(int j=0;j<getNbAnswer();j++){
            //qDebug() << i << j << coo[i][j]->x() << coo[i][j]->y();
            for (int s = 0; s < widthY->size(); s++) {
                if(lenghtX->at(s)>(coo[i][j]->x()-precision2) && lenghtX->at(s)<(coo[i][j]->x()+precision2)){
                    if(widthY->at(s)>coo[i][j]->y()-precision2 && widthY->at(s)<coo[i][j]->y()+precision2){
                        nbFois++;
                    }
                }
            }
            if(nbFois>10){
                state[i][j]=true;
            }
            else{
                state[i][j]=false;
            }
            nbFois=0;
        }
    }

    for(int i=0; i<getNbQuestions();i++) {
        for (int j = 0; j < getNbAnswer(); j++) {
            if(state[i][j]){
                qDebug() << i+1 << j+1 << state[i][j];
            }
        }
    }
}

void QtTest::circle() {
    QPainter painter(&image);
    painter.setPen( QPen(Qt::red, 8)); // personnaliser

    for(int i=0; i<getNbQuestions();i++) {
        for (int j = 0; j < getNbAnswer(); j++) {
            if(state[i][j]){
                painter.drawEllipse(*coo[i][j],25,30);
                qDebug() << i+1 << j+1 << state[i][j];
            }
        }
    }
}

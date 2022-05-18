#ifndef RIVER_H
#define RIVER_H
#include "glm_includes.h"
#include "turtle.h"
#include <QStack>
#include <QString>
#include <QChar>
#include <QHash>


class River{
  public:
    //how to setup rules for making F correlate to move forward and what not
    //MAP OR HASH FUNCTION and MAP character to a function
    QHash<QChar, QString> drawRules;
    QHash<QChar, void (River::*) (void)> charFunction;

    //how to setup branch rules

    //How to setup grammar expansion
    //MAP<QCHAR, QSTRING>


    //how to setup random branching?
    //Map character to a vector of characters or functions


    //how should i go about drawing the river now?
    //carving -- takes abs values of x - i, y - j, z - k
    //check if the distance is less than the radius (then this is part of the river)
    QString currAxiom;
    QStack<Turtle> turtleStack;
    Turtle turtle;

    River(QString axiom, int iterations, glm::vec2 pos, glm::vec2 orient, float len);

    void curveLeft();
    void curveRight();
    void moveForward();
    void loadPrevPos();
    void saveCurrPos();

};

#endif // RIVER_H

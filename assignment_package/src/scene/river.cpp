#include "river.h"

River::River(QString axiom, int interations, glm::vec2 pos, glm::vec2 orient, float len)
    : currAxiom(axiom), turtle(Turtle(pos, orient, len, 1)){

    charFunction.insert('F', &River::moveForward);
    charFunction.insert('+', &River::curveRight);
    charFunction.insert('-', &River::curveLeft);
    charFunction.insert('[', &River::saveCurrPos);
    charFunction.insert(']', &River::loadPrevPos);
    drawRules.insert('F', "[F][F-FF+FF][F+FF-F-F][F-F-F]F[F]F");
    drawRules.insert('+', "+");
    drawRules.insert('-', "-");
    drawRules.insert('[', "[");
    drawRules.insert(']', "]");

    QString expandedAxiom = axiom;
    QString currStringCopy = "";


    for(int i = 0; i < interations; i++){
        currStringCopy = expandedAxiom;
        expandedAxiom = "";
        //loop over axiom and get each of the correct functions
        for(int j = 0; j < axiom.length(); j++){
            //if our drawRules contains this character within our axiom
            //then we need to replace that character with the value within
            //the hashmap of the drawRules
            QChar currChar = currStringCopy[j];
            expandedAxiom += drawRules.value(currChar);
//            if(currChar == 'F'){
//                int randNum = glm::linearRand(0.f, 1.f);
//                if (randNum <= 0.6f){
//                    expandedAxiom += drawRules.value(currChar);
//                }
//            }else{
//                expandedAxiom += drawRules.value(currChar);
//            }
        }
    }

    //update axiom to be the expanded version now
    currAxiom = currStringCopy;
}

void River::curveLeft(){
    turtle.rotateL(glm::radians(30.0) * 180/3.14159265358979);
}

void River::curveRight(){
    turtle.rotateR(glm::radians(30.0) * 180/3.14159265358979);
}

void River::moveForward(){
//    int randNum = glm::linearRand(0.f, 1.f);
//    if(randNum <= 0.5){
//        turtle.rotateL(glm::radians(15.0) * 180/3.14159265358979);
//    }else{
//        turtle.rotateR(glm::radians(15.0) * 180/3.14159265358979);
//    }
    turtle.moveF();
}

void River::loadPrevPos(){
    turtle = turtleStack.pop();
}

void River::saveCurrPos(){
    turtleStack.push(turtle);
    turtle.depth = turtle.depth + 1 ;
}

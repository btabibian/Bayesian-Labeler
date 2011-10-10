#include "act_predict.h"
#include <QVector>
#include <QDebug>
act_predict::act_predict(QObject *parent) :
    QObject(parent)
{
    constructActions();
    active=true;
}
void act_predict::constructActions()
{
    eng.AddVariable("OPEN",4);
    eng.AddVariable("SAVE",4);
    eng.AddVariable("EDIT",4);
    eng.AddVariable("ADD",4);

}
void act_predict::enable()
{
    active=true;
}

void act_predict::disable()
{
    active=false;
}

void act_predict::updateVariable(ACTIONS init,ACTIONS dest)
{
    if (!active)
        return;
    if(dest<0)
        return;
    qDebug()<<"Going From State"<<init<<" To "<<dest;
    QVector<int> update(4,0);
    update[dest]++;
    switch(init)
    {
    case OPEN:
        eng.updateVariable("OPEN",update);
        break;
    case SAVE:
        eng.updateVariable("SAVE",update);
        break;
    case EDIT:
        eng.updateVariable("EDIT",update);
        break;
    case ADD:
        eng.updateVariable("ADD",update);
        break;
    }
    //Debugging
    qDebug()<<inferNextAction(dest);
}
QString act_predict::actionName(ACTIONS act)
{
    switch(act)
    {
    case OPEN:
        return "OPEN";
        break;
    case SAVE:
        return "SAVE";
        break;
    case EDIT:
        return "EDIT";
        break;
    case ADD:
        return "ADD";
        break;
    default:
        return "NULL";
    }
}

ACTIONS act_predict::inferNextAction(ACTIONS criteria)
{
    QVector<float> update(4,0);
    switch(criteria)
    {
    case OPEN:
        eng.getProbabilities("OPEN",update);
        break;
    case SAVE:
        eng.getProbabilities("SAVE",update);
        break;
    case EDIT:
        eng.getProbabilities("EDIT",update);
        break;
    case ADD:
        eng.getProbabilities("ADD",update);
        break;
    default:
        return START;
    }
    float highest=0;
    int index=0;
    for(int i=0;i<update.count();i++)
    {
        if (update[i]>highest)
        {
            highest=update[i];
            index=i;
        }
    }
    qDebug()<<"Highest prob for action "<<actionName(criteria)<<" is "<<actionName(ACTIONS(index))<<"With Probab."<<highest;
    return ACTIONS(index);
}

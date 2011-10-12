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
	//Basic intution about the priors for each action.
#ifdef PRIORS_ENABLED
    QVector<int> open=QVector<int>(4);
    open[0]=1;
    open[1]=1;
    open[2]=3;
    open[3]=5;

    QVector<int> save=QVector<int>(4);
    save[0]=4;
    save[1]=1;
    save[2]=2;
    save[3]=3;

    QVector<int> edit=QVector<int>(4);
    edit[0]=0;
    edit[1]=3;
    edit[2]=4;
    edit[3]=3;

    QVector<int> add=QVector<int>(4);
    add[0]=1;
    add[1]=3;
    add[2]=3;
    add[3]=3;
    eng.AddVariable("OPEN",4,&open);
    eng.AddVariable("SAVE",4,&save);
    eng.AddVariable("EDIT",4,&edit);
    eng.AddVariable("ADD",4,&edit);
#else
    eng.AddVariable("OPEN",4);
    eng.AddVariable("SAVE",4);
    eng.AddVariable("EDIT",4);
    eng.AddVariable("ADD",4);
#endif

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
        return "";
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
    if(highest<MINIMUM_PROB)
        return START;

    return ACTIONS(index);
}

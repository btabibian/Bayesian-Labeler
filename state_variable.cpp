#include "state_variable.h"
#include <QtAlgorithms>

state_variable::state_variable(int count,QString name,QObject *parent) :
    QObject(parent)
{
    this->name=name;
    this->count=count;
    states=QVector<int>(count,0);
    sum=0;
}

state_variable::state_variable(const QVector<int>& priors,int count,QString name,QObject *parent)
    :QObject(parent)
{
    this->name=name;
    this->count=count;
    sum=0;
    states=QVector<int>(count,0);
    qCopy(priors.constBegin(),priors.constEnd(),states.begin());
    QVector<int>::const_iterator iter=states.constBegin();
    int sum_l=0;
    for(;iter<states.end();iter++)
    {
        int c=*iter;
        sum_l+=c;
    }
    sum=sum_l;
}
int state_variable::getCount()
{
    return count;
}

void state_variable::update(const QVector<int>& changes)
{
    for(int i=0;i<changes.count();i++)
    {
        int c=changes[i];
        int d=states[i];
        states[i]=d+c;
        sum+=c;
    }
}

void state_variable::probabilities(QVector<float>& probability_vec)
{
    for(int i=0;i<states.count();i++)
    {
        int c=states[i];
        probability_vec[i]=c/((float)sum);
    }
}

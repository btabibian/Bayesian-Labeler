#include "infer_engine.h"


infer_engine::infer_engine(QObject *parent) :
    QObject(parent)
{

}

void infer_engine::AddVariable(QString name,int events)
{
    state_variable* var=new state_variable(events,name,this);
    vars.insert(name,var);
}

void infer_engine::updateVariable(QString name,const QVector<int>& updateVector)
{
    vars[name]->update(updateVector);
}

void infer_engine::getProbabilities(QString name,QVector<float>& probVector)
{
    vars[name]->probabilities(probVector);
}
void infer_engine::clean()
{
    qDeleteAll(vars);
    vars.clear();
}
infer_engine::~infer_engine()
{
    clean();
}

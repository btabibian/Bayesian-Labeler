#ifndef INFER_ENGINE_H
#define INFER_ENGINE_H

#include <QObject>
#include <QMap>
#include <state_variable.h>
class infer_engine : public QObject
{
    Q_OBJECT
private:
    QMap<QString,state_variable*> vars;
public:
    explicit infer_engine(QObject *parent = 0);
    void AddVariable(QString name ,int events,const QVector<int>* priors=0);
    void updateVariable(QString name,const QVector<int>& updateVector);
    void getProbabilities(QString name,QVector<float>& probVector);
    void clean();
    ~infer_engine();
signals:

public slots:

};

#endif // INFER_ENGINE_H

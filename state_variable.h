#ifndef STATE_VARIABLE_H
#define STATE_VARIABLE_H

#include <QObject>
#include <QVector>
class state_variable : public QObject
{
    Q_OBJECT
private:
    QString name; //Name of the event
    int sum; //Keeps sum of all incidents
    QVector<int> states; //this vector keeps number of each possible state.
    int count;
public:
    state_variable(int count,QString name,QObject *parent = 0);
    state_variable(const QVector<int>& priors,int count,QString name,QObject *parent = 0);
    void update(const QVector<int>& changes);
    void probabilities(QVector<float>& probability_vec);
    int getCount();


};

#endif // STATE_VARIABLE_H

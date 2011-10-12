#ifndef ACT_PREDICT_H
#define ACT_PREDICT_H

#include <QObject>
#include <infer_engine.h>
#define MINIMUM_PROB 0.3
#define PRIORS_ENABLED
enum ACTIONS { OPEN=0, SAVE=1, EDIT=2,ADD=3,START=-1};
class act_predict : public QObject
{
    Q_OBJECT

private:
    infer_engine eng;
    void constructActions();
    bool active;
public:

    explicit act_predict(QObject *parent = 0);
    void updateVariable(ACTIONS init,ACTIONS dest);
    ACTIONS inferNextAction(ACTIONS criteria);
    QString actionName(ACTIONS act);
    void enable();
    void disable();
signals:

public slots:

};

#endif // ACT_PREDICT_H

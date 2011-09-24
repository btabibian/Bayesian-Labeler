/*!
 *  \brief     implementing Label widget
 *  \details   This class is used to represent label of an image
 *  \author    Behzad Tabibian
 *  \version   0.1
 *  \date      Sep. 24, 2011
 *  \pre       This class has to be used with a QGraphicsView object.
 *  \copyright GNU Public License.
 */

#ifndef IMG_LABEL_H
#define IMG_LABEL_H

#include <QWidget>
#include <QGraphicsItem>
#define EPS 5
#define UNSELECTED_COLOR 255,0,0,100
#define SELECTED_COLOR 0,255,0,100
class img_label: public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
            explicit img_label(QGraphicsItem *parent=0);
    void addVertex(QPointF point);
    void setName(QString name);
    QString getName();
    void startEdit();
    void endEdit();
private:
    int x;
    int y;
    int width;
    int height;
    QString name;
    bool editMode;
    QVector<QPointF> points;
    void set_pos();

signals:

public slots:

protected:
    void paint(QPainter* painter,const QStyleOptionGraphicsItem* options,QWidget* widget);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    QRectF boundingRect() const;
};

#endif // IMG_LABEL_H

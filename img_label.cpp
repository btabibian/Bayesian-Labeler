/*! \author    Behzad Tabibian
 *  \date      Sep. 24, 2011
 */
#include "img_label.h"
#include <QPainter>
#include <iostream>
img_label::img_label(QGraphicsItem *parent) :
        QGraphicsItem(parent)
{
    x=EPS;
    y=EPS;
    width=EPS;
    height=EPS;

}
void img_label::paint(QPainter* painter,const QStyleOptionGraphicsItem* options,QWidget* widget)
{
    painter->setBrush(QBrush(Qt::red));
    QVector<QPointF>::Iterator iter= points.begin();
    QVector<QPointF> n_points;
    int i=0;
    for(;iter<points.end();iter++)
    {
        QPointF point=(*iter);
        QPointF n_point(point.x()-x,point.y()-y);
        n_points.push_back(n_point);
        painter->drawEllipse(n_point,5,5);
        std::cout<<i<<".  "<<n_point.x()<<","<<n_point.y()<<std::endl;
        i++;
    }
    painter->setBrush(QBrush(QColor(100,0,0,100)));
    painter->drawPolygon(QPolygonF(n_points),Qt::OddEvenFill);


}
void img_label::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);

}
QRectF img_label::boundingRect() const
{
    return QRect(0,0,width,height);
}
void img_label::addVertex(QPointF point)
{
    std::cout<<"Point to be added:"<<point.x()<<","<<point.y()<<std::endl;
    points.push_back(point);
    set_pos();
}
void img_label::setName(QString name)
{
    this->name=name;
    this->setToolTip(tr("Object Name:")+name);
}
QString img_label::getName()
{
    return name;
}

void img_label::set_pos()
{
    QVector<QPointF>::Iterator iter= points.begin();
    int min_x=INT_MAX,min_y=INT_MAX;
    int max_x=INT_MIN,max_y=INT_MIN;
    for(;iter<points.end();iter++)
    {
        QPointF point=(*iter);
        if (point.x()<min_x)
            min_x=point.x();
        if (point.y()<min_y)
            min_y=point.y();
        if (point.x()>max_x)
            max_x=point.x();
        if (point.y()>max_y)
            max_y=point.y();
    }
    x=min_x;
    y=min_y;
    width=max_x-min_x;
    height=max_y-min_y;
    this->setPos(x,y);
    this->prepareGeometryChange();

}

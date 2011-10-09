/*! \author    Behzad Tabibian
 *  \date      Sep. 24, 2011
 */
#include "img_label.h"
#include <QPainter>
#include <iostream>
img_label::img_label(QGraphicsItem *parent) :
        QGraphicsPolygonItem(parent)
{
    x=EPS;
    y=EPS;
    width=EPS;
    height=EPS;
    this->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable);
    this->editMode=true;

}
void img_label::startEdit()
{
    this->setFlags(0);
    editMode=true;
}
void img_label::endEdit()
{
    this->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable);
    editMode=false;
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
        painter->drawEllipse(n_point,2,2);

        i++;
    }
    if(!editMode)
    {
    if(!isSelected())
    {
    painter->setBrush(QBrush(QColor(UNSELECTED_COLOR)));

    }
    if(isSelected())
    {
    painter->setBrush(QBrush(QColor(SELECTED_COLOR)));

    }
    setPolygon(n_points);
    painter->drawConvexPolygon(QPolygonF(n_points));
    }


}
QVector<QPointF> img_label::getVertecies()
{
    return points;
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
    x=min_x-EPS;
    y=min_y-EPS;
    width=max_x-min_x+EPS*2;
    height=max_y-min_y+EPS*2;
    this->setPos(x,y);
    this->prepareGeometryChange();

}

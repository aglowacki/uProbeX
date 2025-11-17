
#ifndef GSTAR_CLIPPER_ITEM_H
#define GSTAR_CLIPPER_ITEM_H


#include <QGraphicsItem>
#include <QPainter>

namespace gstar
{

class ClipperItem : public QGraphicsItem
{
public:
    // Constructor takes the fixed size for the clipper
    ClipperItem(qreal width, qreal height, QGraphicsItem *parent = nullptr);

    virtual QRectF boundingRect() const override
    {
        return QRectF(0, 0, _width, _height);
    }
    
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override 
    {
        // Optional: Draw a border around the fixed-size area
        /*
        QPen pen(Qt::red);
        pen.setWidth(2);
        painter->setPen(pen);
        painter->drawRect(boundingRect());
        */
    }
    
    void updateSize(qreal width, qreal height);

private:
    qreal _width;
    qreal _height;
};

}

#endif
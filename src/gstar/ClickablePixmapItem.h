#include <QGraphicsPixmapItem>

class ClickablePixmapItem : public QGraphicsPixmapItem
{
public:
    // Constructor
    ClickablePixmapItem(const QPixmap &pixmap, QGraphicsItem *parent = nullptr)
        : QGraphicsPixmapItem(pixmap, parent)
    {
        // This flag ensures the item can receive mouse events even through transparent parts
        // and interacts well with the scene's event propagation system.
        setAcceptHoverEvents(true); 
    }

    const QPointF& last_local_intersection_point(){return _last_local_intersection_point;}

protected:
    // Override the mouse press event handler
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override
    {
        _last_local_intersection_point = event->pos();   
        //qDebug() << "Intersection occurred at local X:" << local_intersection_point.x()
        //         << "Y:" << local_intersection_point.y();
        QGraphicsPixmapItem::mousePressEvent(event);
    }

    QPointF _last_local_intersection_point;
};

#include "gstar/ClipperItem.h"

using namespace gstar;

// Constructor takes the fixed size for the clipper
ClipperItem::ClipperItem(qreal width, qreal height, QGraphicsItem *parent) : QGraphicsItem(parent), _width(width), _height(height)
{
    // Ignore transformations from the view/parent, maintaining fixed screen size
    //setFlag(QGraphicsItem::ItemIgnoresTransformations);
    // Ensure children are clipped to this item's shape
    setFlag(QGraphicsItem::ItemClipsChildrenToShape);
}


void ClipperItem::updateSize(qreal width, qreal height)
{
    _width = width;
    _height = height;
}
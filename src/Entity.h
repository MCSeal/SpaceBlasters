#pragma once
#include <QGraphicsSvgItem>
#include <QGraphicsScene>

// base for everything on screen. owns one svg item and cleans it up itself
class Entity {
public:
    Entity(QGraphicsScene &scene, const QString &file,
           qreal x, qreal y, qreal scale);
    virtual ~Entity();

    // does nothing by default, subclasses override it
    virtual void update() {}

    qreal x() const { return item->x(); }
    qreal y() const { return item->y(); }
    void setPos(qreal x, qreal y) { item->setPos(x, y); }
    void moveBy(qreal dx, qreal dy) { item->moveBy(dx, dy); }
    void setOpacity(qreal o) { item->setOpacity(o); }
    QRectF bounds() const { return item->sceneBoundingRect(); }

    // svgs are 320x320 with empty space around the art, so shrink the box
    // to roughly the visible part. numbers are guesses, tweak if hits feel off
    QRectF hitbox() const;

protected:
    // how much to trim off each side, set by the subclasses
    qreal hitX = 0;
    qreal hitY = 0;

private:
    QGraphicsScene &scene;
    QGraphicsSvgItem *item;
};
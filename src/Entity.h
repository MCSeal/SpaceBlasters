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

    // swap to a different svg, used for animation frames
    void setSprite(const QString &file);

    // box around the part of the svg that actually has art, not the whole
    // 320x320 square. worked out once when the svg is loaded
    QRectF hitbox() const { return item->mapRectToScene(art); }

private:
    QGraphicsScene &scene;
    QGraphicsSvgItem *item;
    // visible art in the svgs own coordinates
    QRectF art;
};
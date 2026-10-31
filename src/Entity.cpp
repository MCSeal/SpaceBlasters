#include "Entity.h"

// loads an svg, scales it, puts it on the scene. used for everything
Entity::Entity(QGraphicsScene &scene, const QString &file,
               qreal x, qreal y, qreal scale)
    : scene(scene), item(new QGraphicsSvgItem(file)) {
    item->setScale(scale);
    item->setPos(x, y);
    scene.addItem(item);
}

// take it off the scene first, then delete
Entity::~Entity() {
    scene.removeItem(item);
    delete item;
}

QRectF Entity::hitbox() const {
    QRectF r = item->sceneBoundingRect();
    return r.adjusted(r.width() * hitX, r.height() * hitY,
                      -r.width() * hitX, -r.height() * hitY);
}
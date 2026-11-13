#include "Entity.h"
#include <QImage>
#include <QPainter>
#include <QSvgRenderer>
#include <map>

// draws the svg once and finds the box around the pixels that have art.
// results are kept so shooting doesnt redraw the bullet svg every time
static QRectF findArt(const QString &file) {
    static std::map<QString, QRectF> found;
    auto it = found.find(file);
    if (it != found.end()) return it->second;

    QSvgRenderer renderer(file);
    QSize size = renderer.defaultSize();
    QImage img(size, QImage::Format_ARGB32);
    img.fill(Qt::transparent);
    QPainter p(&img);
    renderer.render(&p);
    p.end();

    QRect box;
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            // faint pixels are skipped so a guide grid left in the svg doesnt count
            if (qAlpha(img.pixel(x, y)) > 128) box |= QRect(x, y, 1, 1);
        }
    }
    QRectF art = box.isNull() ? QRectF(0, 0, size.width(), size.height()) : QRectF(box);
    found[file] = art;
    return art;
}

// loads an svg, scales it, puts it on the scene. used for everything
Entity::Entity(QGraphicsScene &scene, const QString &file,
               qreal x, qreal y, qreal scale)
    : scene(scene), item(new QGraphicsSvgItem(file)), art(findArt(file)) {
    item->setScale(scale);
    item->setPos(x, y);
    scene.addItem(item);
}

void Entity::setSprite(const QString &file) {
    item->renderer()->load(file);
    art = findArt(file);
    item->update();
}

// take it off the scene first, then delete
Entity::~Entity() {
    scene.removeItem(item);
    delete item;
}

#pragma once
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QRandomGenerator>
#include <vector>

// stars scrolling down behind everything. three sizes, the small dim ones are
// slower so it looks like some are further away
class Starfield {
public:
    explicit Starfield(QGraphicsScene &scene, int count = 70) : scene(scene) {
        auto *rng = QRandomGenerator::global();
        for (int i = 0; i < count; ++i) {
            int layer = i % 3;
            Star s;
            s.speed = 0.3 + layer * 0.3;
            s.item = scene.addRect(0, 0, 1 + layer, 1 + layer, Qt::NoPen,
                                   QColor(200, 220, 255, 90 + layer * 70));
            s.item->setPos(rng->bounded(800), rng->bounded(600));
            // under the planets, ships and text
            s.item->setZValue(-10);
            stars.push_back(s);
        }
    }

    ~Starfield() {
        for (auto &s : stars) {
            scene.removeItem(s.item);
            delete s.item;
        }
    }

    // a star that goes off the bottom comes back at the top somewhere new
    void update() {
        auto *rng = QRandomGenerator::global();
        for (auto &s : stars) {
            qreal y = s.item->y() + s.speed;
            if (y > 600) s.item->setPos(rng->bounded(800), -4);
            else s.item->setY(y);
        }
    }

private:
    struct Star {
        QGraphicsRectItem *item;
        qreal speed;
    };
    QGraphicsScene &scene;
    std::vector<Star> stars;
};

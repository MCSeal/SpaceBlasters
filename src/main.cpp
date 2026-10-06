#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSvgItem>

static QGraphicsSvgItem* addSprite(QGraphicsScene &scene, const QString &file,
                                   qreal x, qreal y, qreal scale) {
    auto *item = new QGraphicsSvgItem(file);
    item->setScale(scale);
    item->setPos(x, y);
    scene.addItem(item);
    return item;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QGraphicsScene scene(0, 0, 800, 600);
    scene.setBackgroundBrush(Qt::black);

    addSprite(scene, "assets/planet-template.svg", 650, 60, 0.34375);
    addSprite(scene, "assets/player-ship-template.svg", 360, 480, 0.25);
    addSprite(scene, "assets/bullet-template.svg", 395, 390, 0.3);

    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 5; ++col) {
            addSprite(scene, "assets/enemy-template.svg",
                      100 + col * 70, 120 + row * 90, 0.1875);
        }
    }

    QGraphicsView view(&scene);
    view.setWindowTitle("Space Blasters");
    view.setFixedSize(820, 640);
    view.show();

    return app.exec();
}
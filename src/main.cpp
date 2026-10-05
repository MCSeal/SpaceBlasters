#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSvgItem>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QGraphicsScene scene(0, 0, 800, 600);
    scene.setBackgroundBrush(Qt::black);

    auto *player = new QGraphicsSvgItem("assets/player-ship-template.svg");
    player->setPos(390, 500);
    scene.addItem(player);

    auto *planet = new QGraphicsSvgItem("assets/planet-template.svg");
    planet->setPos(650, 60);
    scene.addItem(planet);

    QGraphicsView view(&scene);
    view.setWindowTitle("Space Blasters");
    view.setFixedSize(820, 640);
    view.show();

    return app.exec();
}
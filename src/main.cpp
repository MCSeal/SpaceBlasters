#include <QApplication>
#include <QGraphicsScene>
#include "GameView.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // scene is the game world, view is the window looking at it
    QGraphicsScene scene(0, 0, 800, 600);
    scene.setBackgroundBrush(Qt::black);

    // score text in the top left
    auto *scoreText = scene.addSimpleText("SCORE: 0");
    scoreText->setFont(QFont("Consolas", 16));
    scoreText->setBrush(QColor("#5DCAA5"));
    scoreText->setPos(20, 10);

    GameView view(&scene, scoreText);
    view.setWindowTitle("Space Blasters");
    view.setFixedSize(820, 640);
    view.show();

    return app.exec();
}
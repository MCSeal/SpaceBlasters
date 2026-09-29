#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QGraphicsScene scene(0, 0, 800, 600);
    scene.setBackgroundBrush(Qt::black);
// okay!

    QGraphicsView view(&scene);
    view.setWindowTitle("Space Blasters");
    view.setFixedSize(820, 640);
    view.show();

    return app.exec();
}
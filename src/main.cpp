#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QGraphicsScene scene(0, 0, 800, 600);
    QGraphicsView view(&scene);
    view.setWindowTitle("SpaceBlasters");
    view.show();

    return app.exec();
}
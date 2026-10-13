#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSvgItem>
#include <QKeyEvent>
#include <QTimer>
#include <set>

// loads an svg, scales it, puts it on the scene. used for everything for now
static QGraphicsSvgItem* addSprite(QGraphicsScene &scene, const QString &file,
                                   qreal x, qreal y, qreal scale) {
    auto *item = new QGraphicsSvgItem(file);
    item->setScale(scale);
    item->setPos(x, y);
    scene.addItem(item);
    return item;
}

class GameView : public QGraphicsView {
public:
    //qtimer every 16ms which translates to 60fps
    GameView(QGraphicsScene *scene, QGraphicsSvgItem *player)
    
        : QGraphicsView(scene), player(player) {
        // tick() runs every time the timer goes off
        connect(&timer, &QTimer::timeout, this, [this] { tick(); });
        timer.start(16);
    }

    //keyboard input 
protected:
    // autorepeat check so holding a key doesnt spam events
    void keyPressEvent(QKeyEvent *e) override {
        if (!e->isAutoRepeat()) held.insert(e->key());
    }
    void keyReleaseEvent(QKeyEvent *e) override {
        if (!e->isAutoRepeat()) held.erase(e->key());
    }

//movement for player
private:
    void tick() {
        const qreal speed = 5;
        qreal x = player->x();
        // arrows or a/d, whichever
        if (held.count(Qt::Key_Left) || held.count(Qt::Key_A))  x -= speed;
        if (held.count(Qt::Key_Right) || held.count(Qt::Key_D)) x += speed;
        // keep the ship inside the window (800 wide minus ship width, roughly)
        if (x < 0) x = 0;
        if (x > 720) x = 720;
        player->setX(x);
    }

    QGraphicsSvgItem *player;
    QTimer timer;
    // keys currently held down, checked every tick
    std::set<int> held;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // scene is the game world, view is the window looking at it
    QGraphicsScene scene(0, 0, 800, 600);
    scene.setBackgroundBrush(Qt::black);

    // scales are just what looked right against the 320x320 svgs
    addSprite(scene, "assets/planet-template.svg", 650, 60, 0.34375);
    addSprite(scene, "assets/bullet-template.svg", 395, 390, 0.3);
    auto *player = addSprite(scene, "assets/player-ship-template.svg", 360, 480, 0.25);

    // 2 rows of 5 enemies, not moving yet
    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 5; ++col) {
            addSprite(scene, "assets/enemy-template.svg",
                      100 + col * 70, 120 + row * 90, 0.1875);
        }
    }

    GameView view(&scene, player);
    view.setWindowTitle("Space Blasters");
    view.setFixedSize(820, 640);
    view.show();

    return app.exec();
}
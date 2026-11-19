#pragma once
#include "Entity.h"

// short animation that plays once and then is done. the view deletes it
class Effect : public Entity {
public:
    // cx, cy is where the middle of the explosion goes.
    // pickup = same frames backwards in the given colour, for picking things up
    Effect(QGraphicsScene &scene, qreal cx, qreal cy,
           bool pickup = false, const QColor &colour = QColor("#1F6BFF"))
        : Entity(scene, path(0, pickup), cx - size / 2, cy - size / 2, scale),
          heal(pickup) {
        if (pickup) tint(colour);
    }

    // swaps to the next frame every few ticks
    void update() override {
        if (++ticks % ticksPerFrame != 0) return;
        if (++frame >= frameCount) {
            finished = true;
            return;
        }
        setSprite(path(frame, heal));
    }

    bool done() const { return finished; }

private:
    static constexpr int frameCount = 4;
    static constexpr int ticksPerFrame = 5;
    static constexpr qreal scale = 0.25;
    static constexpr qreal size = 320 * scale;
    static constexpr const char *frames[frameCount] = {
        "assets/explosion-1.svg", "assets/explosion-2.svg",
        "assets/explosion-3.svg", "assets/explosion-4.svg"};

    static const char *path(int i, bool heal) {
        return frames[heal ? frameCount - 1 - i : i];
    }

    bool heal;
    int frame = 0;
    int ticks = 0;
    bool finished = false;
};

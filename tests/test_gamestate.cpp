#include <QtTest>
#include "GameState.h"

// checks the game rules with no window, no timer and no graphics
class TestGameState : public QObject {
    Q_OBJECT
private slots:
    void startsWithThreeLives();
    void loseLifeEndsGameAtZero();
    void wrenchStopsAtFour();
    void scoreAdds();
    void captureNeedsHalfTheWave_data();
    void captureNeedsHalfTheWave();
    void thirdCaptureWins();
    void waveSizesGrow();
    void firstWaveIsPlain();
    void boltAddsTwoSpeed();
    void boltOnlyInFirstAndThirdWave();
    void enemiesSpeedUp();
    void enemyFireGetsFaster();
    void resetGoesBackToStart();
};

void TestGameState::startsWithThreeLives() {
    GameState g;
    QCOMPARE(g.lives(), 3);
    QCOMPARE(g.outcome(), GameState::Playing);
}

void TestGameState::loseLifeEndsGameAtZero() {
    GameState g;
    QVERIFY(!g.loseLife());
    QVERIFY(!g.loseLife());
    QCOMPARE(g.lives(), 1);
    QCOMPARE(g.outcome(), GameState::Playing);
    QVERIFY(g.loseLife());
    QCOMPARE(g.lives(), 0);
    QCOMPARE(g.outcome(), GameState::Lost);
    // losing again does not go negative
    g.loseLife();
    QCOMPARE(g.lives(), 0);
}

void TestGameState::wrenchStopsAtFour() {
    GameState g;
    QVERIFY(g.gainLife());
    QCOMPARE(g.lives(), 4);
    QVERIFY(!g.gainLife());
    QCOMPARE(g.lives(), 4);
    // and can drop back down and be fixed again
    g.loseLife();
    QVERIFY(g.gainLife());
    QCOMPARE(g.lives(), 4);
}

void TestGameState::scoreAdds() {
    GameState g;
    g.addScore(10);
    g.addScore(25);
    QCOMPARE(g.score(), 35);
}

void TestGameState::captureNeedsHalfTheWave_data() {
    QTest::addColumn<int>("size");
    QTest::addColumn<int>("kills");
    QTest::addColumn<bool>("cleared");
    QTest::newRow("none") << 10 << 0 << false;
    QTest::newRow("just under half") << 10 << 4 << false;
    QTest::newRow("exactly half") << 10 << 5 << true;
    QTest::newRow("all") << 10 << 10 << true;
    // odd sizes round up, 7 enemies needs 4
    QTest::newRow("odd under") << 7 << 3 << false;
    QTest::newRow("odd over") << 7 << 4 << true;
}

void TestGameState::captureNeedsHalfTheWave() {
    QFETCH(int, size);
    QFETCH(int, kills);
    QFETCH(bool, cleared);
    GameState g;
    g.startWave(size);
    for (int i = 0; i < kills; ++i) g.recordKill();
    QCOMPARE(g.waveCleared(), cleared);
}

void TestGameState::thirdCaptureWins() {
    GameState g;
    QVERIFY(!g.capturePlanet());
    QCOMPARE(g.wave(), 1);
    QVERIFY(!g.capturePlanet());
    QCOMPARE(g.outcome(), GameState::Playing);
    QVERIFY(g.capturePlanet());
    QCOMPARE(g.outcome(), GameState::Won);
}

void TestGameState::waveSizesGrow() {
    GameState g;
    QCOMPARE(g.rows() * g.cols(), 10);
    g.capturePlanet();
    QCOMPARE(g.rows() * g.cols(), 12);
    g.capturePlanet();
    QCOMPARE(g.rows() * g.cols(), 21);
}

void TestGameState::firstWaveIsPlain() {
    GameState g;
    QCOMPARE(g.holeChance(), 0);
    QCOMPARE(g.toughChance(), 0);
    g.capturePlanet();
    QVERIFY(g.holeChance() > 0);
    QVERIFY(g.toughChance() > 0);
}

void TestGameState::boltAddsTwoSpeed() {
    GameState g;
    QCOMPARE(g.playerSpeed(), 5.0);
    g.collectBolt();
    QCOMPARE(g.playerSpeed(), 7.0);
}

void TestGameState::boltOnlyInFirstAndThirdWave() {
    GameState g;
    QVERIFY(g.boltPending());
    g.boltHasDropped();
    // only once, even if the wave gets replayed
    QVERIFY(!g.boltPending());
    g.capturePlanet();
    QVERIFY(!g.boltPending());
    g.capturePlanet();
    QVERIFY(g.boltPending());
}

void TestGameState::enemiesSpeedUp() {
    GameState g;
    g.startWave(10);
    QCOMPARE(g.enemySpeed(10), 1.0);
    QVERIFY(g.enemySpeed(5) > g.enemySpeed(10));
    QVERIFY(g.enemySpeed(1) > g.enemySpeed(5));
    // next wave starts faster than the last one did
    g.capturePlanet();
    g.startWave(12);
    QVERIFY(qFuzzyCompare(g.enemySpeed(12), 1.3));
}

void TestGameState::enemyFireGetsFaster() {
    GameState g;
    int first = g.enemyFireDelay();
    g.capturePlanet();
    QVERIFY(g.enemyFireDelay() < first);
    QVERIFY(g.enemyFireDelay() >= 30);
}

void TestGameState::resetGoesBackToStart() {
    GameState g;
    g.addScore(100);
    g.loseLife();
    g.capturePlanet();
    g.collectBolt();
    g.boltHasDropped();
    g.reset();
    QCOMPARE(g.score(), 0);
    QCOMPARE(g.lives(), 3);
    QCOMPARE(g.wave(), 0);
    QCOMPARE(g.playerSpeed(), 5.0);
    QVERIFY(g.boltPending());
    QCOMPARE(g.outcome(), GameState::Playing);
}

QTEST_APPLESS_MAIN(TestGameState)
#include "test_gamestate.moc"

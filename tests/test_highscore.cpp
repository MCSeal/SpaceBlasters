#include <QtTest>
#include <QTemporaryDir>
#include "HighScore.h"

// uses a temp folder so the real scores.csv is never touched
class TestHighScore : public QObject {
    Q_OBJECT
private slots:
    void noFileMeansZero();
    void saveThenBest();
    void bestIsTheBiggestNotTheLast();
    void oldGamesStayInTheFile();
    void badLinesAreSkipped();
};

void TestHighScore::noFileMeansZero() {
    QTemporaryDir dir;
    QCOMPARE(HighScore::best(dir.filePath("none.csv")), 0);
}

void TestHighScore::saveThenBest() {
    QTemporaryDir dir;
    QString path = dir.filePath("scores.csv");
    HighScore::save(120, 2, "loss", path);
    QCOMPARE(HighScore::best(path), 120);
}

void TestHighScore::bestIsTheBiggestNotTheLast() {
    QTemporaryDir dir;
    QString path = dir.filePath("scores.csv");
    HighScore::save(300, 3, "win", path);
    HighScore::save(50, 0, "loss", path);
    HighScore::save(140, 1, "loss", path);
    QCOMPARE(HighScore::best(path), 300);
}

void TestHighScore::oldGamesStayInTheFile() {
    QTemporaryDir dir;
    QString path = dir.filePath("scores.csv");
    HighScore::save(10, 0, "loss", path);
    HighScore::save(20, 1, "win", path);
    QFile f(path);
    QVERIFY(f.open(QIODevice::ReadOnly | QIODevice::Text));
    QStringList lines = QString(f.readAll()).split('\n', Qt::SkipEmptyParts);
    QCOMPARE(lines.size(), 2);
    // time,score,planets,outcome
    QStringList parts = lines[1].split(',');
    QCOMPARE(parts.size(), 4);
    QCOMPARE(parts[1], QString("20"));
    QCOMPARE(parts[2], QString("1"));
    QCOMPARE(parts[3], QString("win"));
}

void TestHighScore::badLinesAreSkipped() {
    QTemporaryDir dir;
    QString path = dir.filePath("scores.csv");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Text));
    f.write("garbage\n\n2026-11-13T10:00:00,75,1,loss\nx,notanumber,0,loss\n");
    f.close();
    QCOMPARE(HighScore::best(path), 75);
}

QTEST_APPLESS_MAIN(TestHighScore)
#include "test_highscore.moc"

#pragma once
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QStringList>

// one line per finished game: time,score,planets,outcome
namespace HighScore {
    inline const QString file = "scores.csv";

    inline void save(int score, int planets, const QString &outcome) {
        QFile f(file);
        // append so old games stay in the file
        if (!f.open(QIODevice::Append | QIODevice::Text)) return;
        QTextStream out(&f);
        out << QDateTime::currentDateTime().toString(Qt::ISODate) << ","
            << score << "," << planets << "," << outcome << "\n";
    }

    // reads every line and keeps the biggest score. 0 if no file yet
    inline int best() {
        QFile f(file);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return 0;
        QTextStream in(&f);
        int best = 0;
        while (!in.atEnd()) {
            QStringList parts = in.readLine().split(',');
            if (parts.size() >= 2) best = qMax(best, parts[1].toInt());
        }
        return best;
    }
}
#ifndef ANALYSISDATA_H
#define ANALYSISDATA_H

#include <QObject>
#include <QDebug>
#include <QTime>

class AnalysisData : public QObject
{
Q_OBJECT
public:
    AnalysisData();
public slots:
    void cvtPlotData(const QByteArray buf);

signals:
    void hasPlotData(QVector<float> plotdata);

};

#endif // ANALYSISDATA_H

#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#include <QObject>
#include <QTime>
#include <QMap>
#include "mynetwork.h"
#include "serialport.h"

class Communicate: public QObject
{
    Q_OBJECT
public:
    Communicate(QObject * parent = nullptr);
    //~Communicate();

signals:
    void hasGetData(const QByteArray data);
    void hasPlotData(const QMap<QString, double> data);

public:
    TcpServer  * tcpServer;
    SerialPort * serial;

private:
    QTime m_time;

private slots:
    void analyzeData(const QByteArray data);

};

#endif // COMMUNICATE_H

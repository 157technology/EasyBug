#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QObject>

#include <QTimer>

#include <QDebug>

class SerialPort : public QSerialPort
{
    Q_OBJECT
public:
    SerialPort(QSerialPort * parent = nullptr);

public slots:
    void startBind(qint32 baud, QString port);
    void closePort();
    void sendData(QByteArray data);
    void CnvPlotData(const QByteArray data);

private slots:
    void loadData();
    void searchPort();

signals:
    void hasGetData(const QByteArray data);
    void hasNewPort(const QStringList portlist);
    void hasPlotData(const QVector<float> data);

public:
    QTimer m_timer; //通过定时器来实时扫描端口号

private:
    QStringList m_portList;
};

#endif // SERIALPORT_H

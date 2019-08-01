#include "serialport.h"
#include "QThread"

extern int is_serial_open;

SerialPort::SerialPort(QSerialPort * parent) : QSerialPort (parent)
{

    qDebug() << "Serial: " << QThread::currentThread();
    qDebug() << "Serial Timer(init): " << m_timer.thread();
    this->m_portList.clear();
    connect(&this->m_timer, &QTimer::timeout, this, &SerialPort::searchPort);
    this->m_timer.start(500);
    //qDebug() << "init serial ok";
}


void SerialPort::startBind(qint32 baud, QString port)
{
    this->setDataBits(QSerialPort::Data8);
    this->setParity(QSerialPort::NoParity);
    this->setStopBits(QSerialPort::OneStop);
    this->setFlowControl(QSerialPort::NoFlowControl);
    this->setBaudRate(baud);
    this->setPortName(port);

    this->open(QIODevice::ReadWrite);

    if ( this->isOpen() )
    {
        qDebug() << "Serial Bind: " << QThread::currentThread();
        this->m_timer.stop();
        disconnect(&this->m_timer, &QTimer::timeout, this, &SerialPort::searchPort);
        connect(this, &QIODevice::readyRead, this, &SerialPort::loadData);
        //return true;
        is_serial_open = 1;
    }
    else
    {
        qDebug() << "open port " << port << ".FALSE";
        //return false;
        is_serial_open = -1;
    }
}

void SerialPort::closePort()
{
    if ( this->isOpen() )
    {
        disconnect(this, &QIODevice::readyRead, this, &SerialPort::loadData);
        this->close();


        connect(&this->m_timer, &QTimer::timeout, this, &SerialPort::searchPort);
        this->m_timer.start(1000);
    }

    if ( this->isOpen() )
        is_serial_open = 1;
    else
        is_serial_open = -1;
}

void SerialPort::searchPort()
{
    QStringList strlist;

    qDebug() << "Serial Timer: " << QThread::currentThread();
    qDebug() << "Serial Timer-timer: " << m_timer.thread();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if ( serial.open(QIODevice::ReadWrite) )
        {
            strlist.append(serial.portName());
            serial.close();
        }
    }
    if ( strlist != this->m_portList )
    {
        this->m_portList = strlist;
        emit this->hasNewPort(strlist);
    }
}


void SerialPort::loadData()
{
    qDebug() << "Serial Rec: " << QThread::currentThread();
    emit hasGetData(this->readAll());
}

void SerialPort::sendData(QByteArray data)
{
    this->write(data);
}

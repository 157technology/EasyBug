#include "serialport.h"

SerialPort::SerialPort(QSerialPort * parent) : QSerialPort (parent)
{
    this->m_portList.clear();
    connect(&this->m_timer, &QTimer::timeout, this, &SerialPort::searchPort);
    this->m_timer.start(500);
    //qDebug() << "init serial ok";
}


bool SerialPort::startBind(qint32 baud, QString port)
{
    this->setDataBits(QSerialPort::Data8);
    this->setParity(QSerialPort::NoParity);
    this->setStopBits(QSerialPort::OneStop);
    this->setFlowControl(QSerialPort::NoFlowControl);
    this->setBaudRate(baud);
    this->setPortName(port);

    this->open(QIODevice::ReadOnly);

    if ( this->isOpen() )
    {
        this->m_timer.stop();
        disconnect(&this->m_timer, &QTimer::timeout, this, &SerialPort::searchPort);
        connect(this, &QIODevice::readyRead, this, &SerialPort::loadData);
        return true;
    }
    else
    {
        qDebug() << "open port " << port << ".FALSE";
        return false;
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
}

void SerialPort::searchPort()
{
    QStringList strlist;
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

static QByteArray serialBuf;
void SerialPort::loadData()
{
    emit hasGetData(this->readAll());
}

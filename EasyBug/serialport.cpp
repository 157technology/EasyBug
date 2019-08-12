#include "serialport.h"
#include "QThread"
#include "QTime"

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

        //return true;
        is_serial_open = 1;
        connect(this, &QIODevice::readyRead, this, &SerialPort::loadData);

        qDebug() << "Serial <<< " ;
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

void SerialPort::CnvPlotData(const QByteArray buf)
{
    static QByteArray mbuf;
    static QTime mtime;
    static bool first = true;
    QByteArray temp;
    QVector<float> plotdata;
    if ( first )
    {
        mtime.start();
        first = false;
    }

    mbuf += buf;
    if ( mtime.elapsed() >= 2 && mbuf.size()>=4 )
    {
        int begin = -1;
        int end = -1;

        temp = mbuf;

        begin = mbuf.indexOf("[[");
        end   = mbuf.indexOf("]]", begin + 2);
        plotdata.clear();
        while ( begin != -1 && end != -1 )
        {
            //qDebug() << mbuf;
            //qDebug() << "[" << begin << "," << end << "]";
            char * s;
            float * f;
            QByteArray temp = mbuf.left(end);temp = temp.remove(0, begin+2);
            //qDebug() << temp << temp.size();

            mbuf = mbuf.right(mbuf.size()-end-2);
            //qDebug() << ">>> " << mbuf;
            s = temp.data();
            f = (float*)s;

            qDebug() << s;
            qDebug() << f[0] << f[1] << f[2] ;
            plotdata.append(f[0]);
            plotdata.append(f[1]);

            emit hasPlotData(plotdata);

            begin = end = -1;
            begin = mbuf.indexOf("[[");
            end = mbuf.indexOf("]]", begin+2);

        }


        //qDebug() << ">>> " << begin << "--" << end;
        if ( begin == -1 )
        {
            //qDebug() << ">>> " << mbuf;
            if (mbuf != "")
            {
                qDebug() << "ERROR";
                qDebug() << ">>> " << temp;
                qDebug() << ">>> " << mbuf;
                if ( mbuf.size() >= 4 )
                    mbuf.clear();
            }
            //qDebug() << "no begin";
        }
        else
        {
            if ( end != -1 )
                qDebug() << "NULL" << end;
        }
    }
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
    //qDebug() << "Serial Rec: " << QThread::currentThread();
    static QTime mtime;
    static bool flag = true;
    static QByteArray buf;
/*
    buf += this->readAll();

    if ( flag )
    {
        flag = false;
        mtime.start();
        emit hasGetData(buf);
        buf.clear();
    }
    else
    {
        if ( mtime.elapsed() >= 10 )
        {
            emit hasGetData(buf);
            buf.clear();
            mtime.restart();
        }
    }
    */
    emit hasGetData(this->readAll());
}

void SerialPort::sendData(QByteArray data)
{
    this->write(data);
}

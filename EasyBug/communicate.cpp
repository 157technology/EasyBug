#include "communicate.h"


#include <QDebug>


Communicate::Communicate(QObject * parent) : QObject (parent)
{
    this->tcpServer = new TcpServer;

    this->serial    = new SerialPort;

    connect(this->serial, &SerialPort::hasGetData, this, &Communicate::analyzeData);

}


/**/
void Communicate::analyzeData(const QByteArray data)
{
    static bool first = true;
    static QByteArray buf;
    int poss, pose;

    if ( first )
    {
        this->m_time.start();
        first = false;
        buf += data;
    }
    else
    {
        int t = this->m_time.elapsed();
        this->m_time.restart();
        //qDebug() << "Time : " << t;
        if ( t > 3 )
        {
            /**/

            poss = buf.indexOf("<<");
            pose = buf.indexOf(">>", poss+2);
            while ( poss != -1 && pose != -1 )
            {
                //qDebug() << buf << "  --->" << poss << pose << buf.right(buf.size()-pose-2);

                /**/
                QByteArray temp = buf.left(pose);temp = temp.remove(0, poss+2);

                QByteArrayList list = temp.split(' ');
                QMap<QString, double> m_map;

                if ( poss != 0 )
                {
                    qDebug() << "<< not at head!" << poss << pose;
                    qDebug() << buf;
                }

                for ( auto it = list.begin(); it != list.end(); it ++ )
                {
                    //qDebug() << *it;

                    QByteArrayList m_list = it->split(':');

                    //qDebug() << m_list.at(0);
                    //qDebug() << m_list.at(1);
                    m_map.insert(QString::fromLocal8Bit(m_list.at(0)), m_list.at(1).toDouble());
                    if ( m_list.at(0).startsWith("adc") == false )
                    {
                        qDebug() << ">>> " << buf << poss << pose;
                        qDebug() << temp;
                    }
                }
                //qDebug() << m_map;
                emit hasPlotData(m_map);
                // chufa<--->
                //qDebug() << list;
                /**/

                buf = buf.right(buf.size()-pose-2);
                //qDebug() << buf;

                poss = buf.indexOf("<<");
                pose = buf.indexOf(">>", poss+2);
            }

            {
                if ( poss != -1 )
                    buf = buf.right(buf.size()-poss);
                else
                {
                //qDebug() << ">>> " << buf;
                ////////////////buf.clear();
                }
                // qDebug() << "Yes" << poss << pose;
                //qDebug() << ">>> " << buf;
            }
            /**/

            //buf.clear();
        }
        buf += data;
    }
}

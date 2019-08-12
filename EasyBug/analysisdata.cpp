#include "analysisdata.h"

AnalysisData::AnalysisData()
{

}

void AnalysisData::cvtPlotData(const QByteArray buf)
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

            //qDebug() << s;
            //qDebug() << f[0] << f[1] << f[2] ;
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

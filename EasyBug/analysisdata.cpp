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
            if ( temp.size() != 24 )
            {
                qDebug() << "ERROR:" << temp.size();
            }
            //qDebug() << s;
            //qDebug() << f[0] << f[1] << f[2] ;
            extern int g_plot_count;
            g_plot_count = temp.size() / 4;
            //qDebug() << g_plot_count;
            for ( int i = 0; i < g_plot_count; i ++ )
            {
                plotdata.append(f[i]);
            }
            emit hasPlotData(plotdata);
            plotdata.clear();
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

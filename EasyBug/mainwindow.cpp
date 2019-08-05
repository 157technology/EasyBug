#include "mainwindow.h"
#include "ui_mainwindow.h"


int is_serial_open;
int is_tcp_open;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    //m_communicate = new Communicate;

    //connect(m_communicate->tcpServer, &TcpServer::hasAlterLink, this, &MainWindow::alterLink); // when link change
    //connect(this, &MainWindow::stopServer, m_communicate->tcpServer, &TcpServer::stopServer);  // start server
    //connect(this, &MainWindow::startServer, m_communicate->tcpServer, &TcpServer::startServer);// stop server
    qDebug() << "MainWindow Thread: " << QThread::currentThread();

    connect(this, &MainWindow::splot, this, &MainWindow::plot);

    m_net_mode_list = new QStringList({"Tcp Server","Tcp Client","Udp Server","Tcp Client"});

    /*
    ui->CB_TCPMode->addItem("Tcp Server");
    ui->CB_TCPMode->addItem("Tcp Client");
    ui->CB_TCPMode->addItem("Udp Server");
    ui->CB_TCPMode->addItem("Tcp Client");
    */

    ui->CB_TCPMode->addItems(*m_net_mode_list);

    m_serial = new SerialPort;
    m_tcp_server = new TcpServer;
    m_tcp_client = new TcpSocket(0);

    m_tcp_client->abort();

    QThread * thread_serial = new QThread(m_serial);
    QThread * thread_tcp_server = new QThread(m_tcp_server);
    QThread * thread_tcp_client = new QThread(m_tcp_client);

    // move to a thread
    m_serial->moveToThread(thread_serial);
    m_serial->m_timer.moveToThread(thread_serial);
    thread_serial->start();

    m_tcp_server->moveToThread(thread_tcp_server);
    thread_tcp_server->start();

    m_tcp_client->moveToThread(thread_tcp_client);
    thread_tcp_client->start();



    /* about serial */
    connect(m_serial, &SerialPort::hasNewPort, this, &MainWindow::alterPorts);
    connect(this, &MainWindow::startSerial, m_serial, &SerialPort::startBind);
    connect(this, &MainWindow::stopSeial, m_serial, &SerialPort::closePort);


    /* about tcp server */


    //connect(this->m_communicate->serial, &SerialPort::hasNewPort, this, &MainWindow::alterPorts);

    //connect(this->m_communicate, &Communicate::hasPlotData, this, &MainWindow::exePlot);
    //connect(this->m_communicate->tcpServer, &TcpServer::hasGetData, this, &MainWindow::showData);
    //qDebug("connect ok");
    //connect(m_serial, &SerialPort::hasGetData, this, &MainWindow::serialData);



   // m_server->startServer(BROADCAST, 2222);                                    // init start server





    //m_udp = new UdpServer;
    //m_udp->startServer(1234);



    // set some basic customPlot config:
    mPlot = ui->plot;
    mPlot->yAxis->setTickLabels(true);
    mPlot->yAxis2->setTickLabels(true);
    connect(mPlot->yAxis2, SIGNAL(rangeChanged(QCPRange)), mPlot->yAxis, SLOT(setRange(QCPRange))); // left axis only mirrors inner right axis
    mPlot->yAxis2->setVisible(true);
    mPlot->yAxis2->setRange(-10, 10);
    mPlot->axisRect()->addAxis(QCPAxis::atRight);
    mPlot->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(40); // add some padding to have space for tags
    mPlot->axisRect()->axis(QCPAxis::atRight, 1)->setPadding(40); // add some padding to have space for tags
    mGraph1 = mPlot->addGraph(mPlot->xAxis, mPlot->axisRect()->axis(QCPAxis::atRight, 0));
    mGraph2 = mPlot->addGraph(mPlot->xAxis, mPlot->axisRect()->axis(QCPAxis::atRight, 1));

    connect(mPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), mPlot->axisRect()->axis(QCPAxis::atRight, 0), SLOT(setRange(QCPRange)));
    connect(mPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), mPlot->axisRect()->axis(QCPAxis::atRight, 1), SLOT(setRange(QCPRange)));
    //mPlot->axisRect()->axis(QCPAxis::atRight,0)->setRange(-3, 3);
    mPlot->axisRect()->axis(QCPAxis::atRight,1)->setRange(mPlot->yAxis2->range());

    mGraph1->setPen(QPen(QColor(250, 120, 0)));
    mGraph2->setPen(QPen(QColor(0, 180, 60)));

    mTag1 = new AxisTag(mGraph1->valueAxis());
    mTag1->setPen(mGraph1->pen());
    mTag2 = new AxisTag(mGraph2->valueAxis());
    mTag2->setPen(mGraph2->pen());

    ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom |  QCP::iSelectPlottables | QCP::iSelectAxes);
    ui->plot->axisRect()->setupFullAxesBox();
    ui->plot->rescaleAxes();


    //connect(&mDataTimer, SIGNAL(timeout()), this, SLOT(timerSlot()));
    //mDataTimer.start(40);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerSlot()
{
  // calculate and add a new data point to each graph:
  mGraph1->addData(mGraph1->dataCount(), qSin(mGraph1->dataCount()/50.0)+qSin(mGraph1->dataCount()/50.0/0.3843)*0.25);
  mGraph2->addData(mGraph2->dataCount(), qCos(mGraph2->dataCount()/50.0)+qSin(mGraph2->dataCount()/50.0/0.4364)*0.15);

  // make key axis range scroll with the data:
  mPlot->xAxis->rescale();
  //mGraph1->rescaleValueAxis(false, true);
  //mGraph2->rescaleValueAxis(false, true);
  mPlot->xAxis->setRange(mPlot->xAxis->range().upper, 1500, Qt::AlignRight);

  // update the vertical axis tag positions and texts to match the rightmost data point of the graphs:
  double graph1Value = mGraph1->dataMainValue(mGraph1->dataCount()-1);
  double graph2Value = mGraph2->dataMainValue(mGraph2->dataCount()-1);
  mTag1->updatePosition(graph1Value);
  mTag2->updatePosition(graph2Value);
  mTag1->setText(QString::number(graph1Value, 'f', 2));
  mTag2->setText(QString::number(graph2Value, 'f', 2));

  mPlot->replot();
}


void MainWindow::showPlotData(const QByteArray buf)
{
    static QByteArray mbuf;
    static QTime mtime;
    static bool first = true;
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

            emit splot(plotdata);

            begin = end = -1;
            begin = mbuf.indexOf("[[");
            end = mbuf.indexOf("]]", begin+2);

        }


        //qDebug() << ">>> " << begin << "--" << end;
        if ( begin == -1 )
        {
            qDebug() << ">>> " << mbuf;
            if (mbuf != "") qDebug() << "ERROR";
            mbuf.clear();
            //qDebug() << "no begin";
        }
        else
        {
            //qDebug() << "NULL";
        }
    }
}

void MainWindow::plot(QVector<float> datalist)
{
    double data[10];

    for ( int i = 0; i < datalist.size(); i ++ )
        data[i] = datalist.at(i);

    // calculate and add a new data point to each graph:
    mGraph1->addData(mGraph1->dataCount(), data[0]);//qSin(mGraph1->dataCount()/50.0)+qSin(mGraph1->dataCount()/50.0/0.3843)*0.25);
    mGraph2->addData(mGraph2->dataCount(), data[1]);//qCos(mGraph2->dataCount()/50.0)+qSin(mGraph2->dataCount()/50.0/0.4364)*0.15);

    // make key axis range scroll with the data:
    mPlot->xAxis->rescale();
    //mGraph1->rescaleValueAxis(false, true);
    //mGraph2->rescaleValueAxis(false, true);
    mPlot->xAxis->setRange(mPlot->xAxis->range().upper, 1500, Qt::AlignRight);

    // update the vertical axis tag positions and texts to match the rightmost data point of the graphs:
    double graph1Value = mGraph1->dataMainValue(mGraph1->dataCount()-1);
    double graph2Value = mGraph2->dataMainValue(mGraph2->dataCount()-1);
    mTag1->updatePosition(graph1Value);
    mTag2->updatePosition(graph2Value);
    mTag1->setText(QString::number(graph1Value, 'f', 2));
    mTag2->setText(QString::number(graph2Value, 'f', 2));

    mPlot->replot();
}

void MainWindow::showSerialData(const QByteArray buf)
{
    ui->TB_Serialshow->insertPlainText(QString::fromLocal8Bit(buf));
}

void MainWindow::alterPorts(QStringList ports)
{
    qDebug() << "ports change";
    ui->CB_Serial->clear();
    ui->CB_Serial->addItems(ports);

    ui->CB_Plot->clear();
    ui->CB_Plot->addItems(ports);

    //
    //m_communicate->serial->startBind(115200, ports.at(0));
}


void MainWindow::showTcpData(const QByteArray buf)
{
    ui->TB_TCPshow->insertPlainText(QString::fromLocal8Bit(buf));
}

void MainWindow::alterLink(QStringList iplist)
{
    qDebug() << ">>> main: " << iplist;
    ui->CB_TCPMode->setEnabled(true);
    ui->CB_TCPMode->clear();
    ui->CB_TCPMode->addItems(iplist);
}



/* TCP */
void MainWindow::on_PB_TCPopen_clicked()
{
    static bool flag = false;
    static int state;

    is_tcp_open = 0;

    if ( flag == false )
    {
        state = ui->CB_TCPMode->currentIndex();
        switch (state)
        {
        case 0:
            connect(this, &MainWindow::startTcpServer, m_tcp_server, &TcpServer::startServer);
            emit startTcpServer("0.0.0.0", 3333);
            while ( is_tcp_open == 0 )
            {
                this->thread()->msleep(5);
            }
            if ( is_tcp_open == 1 )
            {
                flag = true;
                connect(this, &MainWindow::stopTcpServer, m_tcp_server, &TcpServer::stopServer);
                connect(this, &MainWindow::sendTcpData, m_tcp_server, &TcpServer::sendData);
                connect(m_tcp_server, &TcpServer::hasGetData, this, &MainWindow::showTcpData);
                connect(m_tcp_server, &TcpServer::hasAlterLink, this, &MainWindow::alterLink);
            }
            else
            {
                qDebug() << "start tcp server faild";
            }
            break;
        case 1:
            connect(this, &MainWindow::startTcpServer, m_tcp_client, &TcpSocket::startClient);
            emit startTcpServer("169.254.138.220", 3333);
            while ( is_tcp_open == 0 )
            {
                this->thread()->msleep(5);
            }
            if ( is_tcp_open == 1 )
            {
                connect(this, &MainWindow::stopTcpServer, m_tcp_client, &TcpSocket::closeClient);
                connect(this, &MainWindow::sendTcpData, m_tcp_client, &TcpSocket::sendClienData);
                connect(m_tcp_client, &TcpSocket::hasGetData, this, &MainWindow::showTcpData);

                qDebug() << "connect success thread->" << QThread::currentThread();
                flag = true;
            }
            else
            {
                qDebug() << "connect failed thread->" << QThread::currentThread();
            }

            break;
        case 2:
            break;
        case 3:
            break;
        default:
            break;
        }

        if ( flag == true )
        {
            ui->CB_TCPMode->setEnabled(false);
            ui->PB_TCPsend->setEnabled(true);
            ui->PB_TCPopen->setText("stop");
        }
    }
    else
    {
        switch (state)
        {
        case 0:
            emit stopTcpServer();
            while ( is_tcp_open == 0 )
            {
                this->thread()->msleep(5);
            }
            if ( is_tcp_open == -1 )
            {
                flag = false;
                disconnect(this, &MainWindow::sendTcpData, m_tcp_server, &TcpServer::sendData);
                disconnect(this, &MainWindow::startTcpServer, m_tcp_server, &TcpServer::startServer);
                disconnect(this, &MainWindow::stopTcpServer, m_tcp_server, &TcpServer::stopServer);
                disconnect(m_tcp_server, &TcpServer::hasGetData, this, &MainWindow::showTcpData);
                disconnect(m_tcp_server, &TcpServer::hasAlterLink, this, &MainWindow::alterLink);
                qDebug() << "stop tcp server:thread->" << this->thread()->currentThread();
            }
            else
            {
                qDebug() << "stop tcp server faild";
            }

            break;

        case 1:
            emit stopTcpServer();
            while ( is_tcp_open == 0 )
            {
                this->thread()->msleep(5);
            }
            if ( is_tcp_open == -1 )
            {
                flag = false;
                disconnect(this, &MainWindow::startTcpServer, m_tcp_client, &TcpSocket::startClient);
                disconnect(this, &MainWindow::sendTcpData, m_tcp_client, &TcpSocket::sendClienData);
                disconnect(m_tcp_client, &TcpSocket::hasGetData, this, &MainWindow::showTcpData);

                qDebug() << "stop tcp client:thread->" << this->thread()->currentThread();
            }
            else
            {
                qDebug() << "stop tcp client faild";
            }

            break;

        case 2:
            break;

        case 3:
            break;

        default:

            break;
        }

        if ( flag == false )
        {
            ui->CB_TCPMode->setEnabled(true);
            ui->CB_TCPMode->clear();
            ui->CB_TCPMode->addItems(*m_net_mode_list);
            ui->CB_TCPMode->setCurrentIndex(state);
            ui->PB_TCPsend->setEnabled(true);
            ui->PB_TCPopen->setText("start");
        }
    }
}

void MainWindow::on_PB_ClearTCPshow_clicked()
{
    ui->TB_TCPshow->clear();
}

void MainWindow::on_PB_ClearTCPinput_clicked()
{
    ui->TE_TCPedit->clear();
}

void MainWindow::on_PB_TCPsend_clicked()
{
    //m_communicate->tcpServer->m_sendData(ui->TE_TCPedit->toPlainText().toLocal8Bit(), "0.0.0.0");
    emit sendTcpData(ui->TE_TCPedit->toPlainText().toLocal8Bit());
}


/* Serial */
void MainWindow::on_PB_Serialopen_clicked()
{
    static bool flag = false;
    is_serial_open = 0;
    if ( flag == false )
    {
        emit startSerial(1152000, ui->CB_Serial->currentText());
    }
    else
    {
        emit stopSeial();
    }

    while ( is_serial_open == 0 )
    {
        qDebug() << "Wait to open Serial";
        this->thread()->msleep(1);
    }



    if ( flag == false )
    {
        if ( is_serial_open == 1 )
        {
            qDebug() << "Serial Open Success";


            connect(this, &MainWindow::sendSerial, m_serial, &SerialPort::sendData);
            connect(m_serial, &SerialPort::hasGetData, this, &MainWindow::showSerialData);

            flag = true;
            ui->PB_Serialopen->setText("stop");

            ui->CB_Serial->setEnabled(false);
            ui->LE_Serialbaud->setEnabled(false);
            ui->PB_Seralsend->setEnabled(true);
        }
        else
        {
            qDebug() << "Serial Open False";
        }
    }
    else
    {
        if ( is_serial_open == -1 )
        {
            qDebug() << "Serial close Success";
            disconnect(this, &MainWindow::sendSerial, m_serial, &SerialPort::sendData);
            disconnect(m_serial, &SerialPort::hasGetData, this, &MainWindow::showSerialData);

            flag = false;
            ui->PB_Serialopen->setText("start");
            ui->CB_Serial->setEnabled(true);
            ui->LE_Serialbaud->setEnabled(true);
            ui->PB_Seralsend->setEnabled(false);
        }
        else
        {
            qDebug() << "Serial Open False";
        }
    }



}



void MainWindow::on_PB_ClearSerialshow_clicked()
{
    ui->TB_Serialshow->clear();
}

void MainWindow::on_PB_ClearSerialinput_clicked()
{
    ui->TE_Serialedit->clear();
}

void MainWindow::on_PB_Seralsend_clicked()
{
    emit sendSerial(ui->TE_Serialedit->toPlainText().toLocal8Bit());
}

void MainWindow::on_PB_PlotStart_clicked()
{
    static bool flag = false;
    if ( ui->PB_Serialopen->text() == "stop" )
    {
        QMessageBox::warning(NULL, "warning", "Already Connect");
    }
    else
    {
        if ( flag == false )
        {
            emit startSerial(115200, ui->CB_Plot->currentText());
            connect(this, &MainWindow::sendSerial, m_serial, &SerialPort::sendData);
            connect(m_serial, &SerialPort::hasGetData, this, &MainWindow::showPlotData);
            flag = true;
        }
        else
        {
            emit stopSeial();
            disconnect(this, &MainWindow::sendSerial, m_serial, &SerialPort::sendData);
            disconnect(m_serial, &SerialPort::hasGetData, this, &MainWindow::showPlotData);
            flag = false;
        }

    }
}

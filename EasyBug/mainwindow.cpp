#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    m_communicate = new Communicate;

    connect(m_communicate->tcpServer, &TcpServer::hasAlterLink, this, &MainWindow::alterLink); // when link change
    connect(this, &MainWindow::stopServer, m_communicate->tcpServer, &TcpServer::stopServer);  // start server
    connect(this, &MainWindow::startServer, m_communicate->tcpServer, &TcpServer::startServer);// stop server




    connect(this->m_communicate->serial, &SerialPort::hasNewPort, this, &MainWindow::alterPorts);

    connect(this->m_communicate, &Communicate::hasPlotData, this, &MainWindow::exePlot);
    connect(this->m_communicate->tcpServer, &TcpServer::hasGetData, this, &MainWindow::showData);
    qDebug("connect ok");
    //connect(m_serial, &SerialPort::hasGetData, this, &MainWindow::serialData);



   // m_server->startServer(BROADCAST, 2222);                                    // init start server

    QThread * thread = new QThread(m_server);                                  // move to a thread
    m_communicate->tcpServer->moveToThread(thread);
    thread->start();



    //m_udp = new UdpServer;
    //m_udp->startServer(1234);



    // set some basic customPlot config:
    ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom |  QCP::iSelectPlottables);
    ui->plot->axisRect()->setupFullAxesBox();
    ui->plot->rescaleAxes();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::alterPorts(QStringList ports)
{
    qDebug() << "ports change";
    ui->CB_Serial->clear();
    ui->CB_Serial->addItems(ports);

    //
    m_communicate->serial->startBind(115200, ports.at(0));
}

void MainWindow::exePlot(QMap<QString, double> data)
{
    static QMap<QString, int> m_data;
    static int index = 0;
    static double t = 0;
    for ( auto it = data.begin(); it != data.end(); it ++ )
    {
        if ( m_data.contains(it.key()) == false )
        {
            qDebug() << it.key();
            m_data.insert(it.key(), index ++);
            ui->plot->addGraph();
            QColor color(0+200/10.0*index,70*(1.6-index/8.0), 150, 150);
            ui->plot->graph()->setPen(QPen(color));
        }


        ui->plot->graph(m_data.value(it.key()))->addData(t, it.value());
    }
    t += 0.02;
    ui->plot->replot();
}

void MainWindow::showData(const QByteArray buf)
{
    ui->TB_TCPshow->append(QString::fromLocal8Bit(buf));
}

void MainWindow::alterLink(QStringList iplist)
{
    qDebug() << iplist;
}




void MainWindow::on_PB_TCPopen_clicked()
{
    emit startServer("0.0.0.0", 3333);
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
    m_communicate->tcpServer->m_sendData(ui->TE_TCPedit->toPlainText().toLocal8Bit(), "0.0.0.0");
}



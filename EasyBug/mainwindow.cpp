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

    m_serial = new SerialPort;
    m_tcp_server = new TcpServer;

    QThread * thread_serial = new QThread(m_serial);

    QThread * thread_tcp_server = new QThread(m_tcp_server);

    // move to a thread
    m_serial->moveToThread(thread_serial);
    m_serial->m_timer.moveToThread(thread_serial);
    thread_serial->start();

    m_tcp_server->moveToThread(thread_tcp_server);
    thread_tcp_server->start();



    /* about serial */
    connect(this, &MainWindow::startSerial, m_serial, &SerialPort::startBind);
    connect(this, &MainWindow::stopSeial, m_serial, &SerialPort::closePort);
    connect(this, &MainWindow::sendSerial, m_serial, &SerialPort::sendData);
    connect(m_serial, &SerialPort::hasNewPort, this, &MainWindow::alterPorts);
    connect(m_serial, &SerialPort::hasGetData, this, &MainWindow::showSerialData);


    /* about tcp server */
    connect(this, &MainWindow::startTcpServer, m_tcp_server, &TcpServer::startServer);
    connect(this, &MainWindow::stopTcpServer, m_tcp_server, &TcpServer::stopServer);
    connect(m_tcp_server, &TcpServer::hasGetData, this, &MainWindow::showTcpData);

    //connect(this->m_communicate->serial, &SerialPort::hasNewPort, this, &MainWindow::alterPorts);

    //connect(this->m_communicate, &Communicate::hasPlotData, this, &MainWindow::exePlot);
    //connect(this->m_communicate->tcpServer, &TcpServer::hasGetData, this, &MainWindow::showData);
    //qDebug("connect ok");
    //connect(m_serial, &SerialPort::hasGetData, this, &MainWindow::serialData);



   // m_server->startServer(BROADCAST, 2222);                                    // init start server





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


void MainWindow::showSerialData(const QByteArray buf)
{
    ui->TB_Serialshow->insertPlainText(QString::fromLocal8Bit(buf));
}

void MainWindow::alterPorts(QStringList ports)
{
    qDebug() << "ports change";
    ui->CB_Serial->clear();
    ui->CB_Serial->addItems(ports);

    //
    //m_communicate->serial->startBind(115200, ports.at(0));
}


void MainWindow::showTcpData(const QByteArray buf)
{
    ui->TB_TCPshow->insertPlainText(QString::fromLocal8Bit(buf));
}

void MainWindow::alterLink(QStringList iplist)
{
    qDebug() << iplist;
}



/* TCP */
void MainWindow::on_PB_TCPopen_clicked()
{
    emit startTcpServer("0.0.0.0", 3333);
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
}


/* Serial */
void MainWindow::on_PB_Serialopen_clicked()
{
    static bool flag = false;
    is_serial_open = 0;
    if ( flag == false )
    {
        emit startSerial(115200, ui->CB_Serial->currentText());
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

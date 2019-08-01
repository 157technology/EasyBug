#include "mainwindow.h"
#include "ui_mainwindow.h"



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

    QThread * thread = new QThread(m_serial);

    // move to a thread
    m_serial->moveToThread(thread);
    m_serial->m_timer.moveToThread(thread);
    thread->start();

    connect(this, &MainWindow::startSerial, m_serial, &SerialPort::startBind);
    connect(this, &MainWindow::stopSeial, m_serial, &SerialPort::closePort);
    connect(m_serial, &SerialPort::hasNewPort, this, &MainWindow::alterPorts);
    connect(m_serial, &SerialPort::hasGetData, this, &MainWindow::showSerialData);

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


void MainWindow::showData(const QByteArray buf)
{
    ui->TB_TCPshow->append(QString::fromLocal8Bit(buf));
}

void MainWindow::alterLink(QStringList iplist)
{
    qDebug() << iplist;
}



/* TCP */
void MainWindow::on_PB_TCPopen_clicked()
{
    //emit startServer("0.0.0.0", 3333);
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
    emit startSerial(115200, ui->CB_Serial->currentText());
}



void MainWindow::on_PB_ClearSerialshow_clicked()
{
    ui->TB_Serialshow->clear();
}

void MainWindow::on_PB_ClearSerialinput_clicked()
{
    ui->TE_Serialedit->clear();
}

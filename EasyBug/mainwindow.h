#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <communicate.h>

#include "qcustomplot.h"
#include "QTimer"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget * parent = nullptr);
    ~MainWindow();

public slots:
    void showData(const QByteArray buf);
    void showSerialData(const QByteArray buf);

private slots:
    void alterLink(QStringList iplist);
    void alterPorts(QStringList ports);


    void on_PB_TCPopen_clicked();
    void on_PB_ClearTCPshow_clicked();
    void on_PB_TCPsend_clicked();
    void on_PB_ClearTCPinput_clicked();

    void on_PB_Serialopen_clicked();
    void on_PB_ClearSerialinput_clicked();
    void on_PB_ClearSerialshow_clicked();

signals:
    void startSerial(qint32 baud, QString port);
    void stopSeial();

    void startServer(const QString ip, const int port);
    void stopServer();

private:
    Ui::MainWindow * ui;
    QTcpServer * m_server;
    SerialPort * m_serial;

public:
    Communicate * m_communicate;
};

#endif // MAINWINDOW_H

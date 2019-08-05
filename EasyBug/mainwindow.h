#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <communicate.h>

#include "axistag.h"
#include "mynetwork.h"
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
    void showTcpData(const QByteArray buf);
    void showSerialData(const QByteArray buf);
    void showPlotData(const QByteArray buf);

private slots:
    void alterLink(QStringList iplist);
    void alterPorts(QStringList ports);

    void plot(QVector<float> datalist);

    void on_PB_TCPopen_clicked();
    void on_PB_ClearTCPshow_clicked();
    void on_PB_TCPsend_clicked();
    void on_PB_ClearTCPinput_clicked();

    void on_PB_Serialopen_clicked();
    void on_PB_ClearSerialinput_clicked();
    void on_PB_ClearSerialshow_clicked();

    void on_PB_Seralsend_clicked();

signals:
    void splot(QVector<float> datalist);

    void startSerial(qint32 baud, QString port);
    void stopSeial();
    void sendSerial(const QByteArray buf);

    void startTcpServer(const QString ip, const int port);
    void stopTcpServer();
    void sendTcpData(const QByteArray buf);

private slots:
  void timerSlot();

  void on_PB_PlotStart_clicked();

private:
    Ui::MainWindow * ui;

    TcpSocket * m_tcp_client;
    TcpServer * m_tcp_server;
    SerialPort * m_serial;

    QStringList * m_net_mode_list;

    QCustomPlot *mPlot;
    QPointer<QCPGraph> mGraph1;
    QPointer<QCPGraph> mGraph2;
    AxisTag *mTag1;
    AxisTag *mTag2;
    QTimer mDataTimer;
public:
    Communicate * m_communicate;
};

#endif // MAINWINDOW_H

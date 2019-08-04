#ifndef MYNETWORK_H
#define MYNETWORK_H

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QAbstractSocket>
#include <QThread>
#include <QMap>
#include <QObject>

#include <QDebug>

#define BROADCAST   "0.0.0.0"

class TcpSocket : public QTcpSocket
{
    Q_OBJECT

public:
    TcpSocket(qintptr socketdesc, QTcpSocket * parent = nullptr);

public slots:
    void sendData(QByteArray data, QString ip = "0.0.0.0");
    void sendClienData(QByteArray buf);
    void closeSocket(QString ip);
    void startClient(const QString ip, const int port);
    void closeClient();

private slots:
    void recvData();
    void socketError(QAbstractSocket::SocketError sError);

signals:
    void hasGetData(QByteArray data);

private:
    QString m_remoteIp;
};



class TcpServer : public QTcpServer
{
    Q_OBJECT

public:
    TcpServer(QTcpServer * parent = nullptr);
    ~TcpServer();

protected:
    void incomingConnection(qintptr socketDesc);

public slots:
    void startServer(const QString ip = BROADCAST, const int port = 2222);
    void sendData(QByteArray buf);
    void stopServer();

signals:
    /* the two signals are for UI */
    void hasGetData(QByteArray data);
    void hasAlterLink(QStringList iplist);

    /* the two signals are for sockets */
    void m_sendData(QByteArray data, const QString ip);
    void m_deleteSock(const QString ip);

private slots:
    /* the two slots are for sockets */
    void getData(QByteArray data);
    void alterLink();

private:
    QMap<QString, TcpSocket *> m_map;

};



class UdpServer : public QUdpSocket
{
    Q_OBJECT

public:
    UdpServer(QUdpSocket * parent = nullptr);

public slots:
    void startServer(const int port = 1234);
    void stopServer();
    void sendData(const QByteArray & data, const QString ip = BROADCAST, const int port = 3333);

private slots:
    void recvData();

signals:
    void hasGetData(QByteArray data);
};


#endif // MYNETWORK_H

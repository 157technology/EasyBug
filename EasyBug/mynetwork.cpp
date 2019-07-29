#include "mynetwork.h"

/* about tcp socket */
/* init socket and connect socket io */
TcpSocket::TcpSocket(qintptr sock, QTcpSocket * parent) : QTcpSocket (parent)
{
    setSocketDescriptor(sock);
    m_remoteIp = peerAddress().toString().remove(QString("::ffff:"));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(this, &QIODevice::readyRead, this, &TcpSocket::recvData);
}

/* sock has got data, then pass it to server */
void TcpSocket::recvData()
{
    TcpSocket * psock = qobject_cast<TcpSocket *>(sender());
    //psock->m_remoteIp.toLatin1()
    emit hasGetData(psock->readAll());
}

/* if something wrong happends, then debug there */
void TcpSocket::socketError(QAbstractSocket::SocketError sError)
{
    TcpSocket * socket = qobject_cast<TcpSocket *>(sender());
    qDebug() << sError;
    qDebug() << socket->socketDescriptor() << socket->peerAddress().toString() << socket->peerPort() << socket->errorString();
}

/* check ip then send data */
void TcpSocket::sendData(QByteArray data, QString ip)
{
    if ( ip == BROADCAST || ip == m_remoteIp )   write(data);
}

/* active check ip then close socket */
void TcpSocket::closeSocket(QString ip)
{
    if ( ip == BROADCAST || ip == m_remoteIp )
    {
        emit this->disconnected();
        //close();
        //qDebug() << ip << "state: " << state();
    }
}









/* about tcp server */

TcpServer::TcpServer(QTcpServer * parent) : QTcpServer (parent)
{
    m_map.clear();
}

TcpServer::~TcpServer()
{

}

void TcpServer::startServer(const QString ip, const int port)
{
    qDebug() << ip << port;
    if ( ip == BROADCAST )  listen(QHostAddress::Any, quint16(port));
    else                    listen(QHostAddress(ip), quint16(port));
    qDebug() << "startServer :: " << isListening();
}

/* delete all socket then close server */
void TcpServer::stopServer()
{
    if ( m_map.isEmpty() )
    {
        close();
        qDebug() << "close YES";
    }
    else
    {
        m_deleteSock(BROADCAST);
    }
}

void TcpServer::getData(QByteArray data)
{
    qDebug() << QString(data);
    emit hasGetData(data);
}

void TcpServer::alterLink()
{
    TcpSocket * psock = qobject_cast<TcpSocket *>(sender());
    //qDebug() << "Private Slot:: alterLink remove Ip -->> " <<  m_map.key(psock);
    m_map.remove(m_map.key(psock));
    delete psock;
    emit hasAlterLink(m_map.keys());
}

void TcpServer::incomingConnection(qintptr socketDesc)
{
    TcpSocket * socket = new TcpSocket(socketDesc);
    QString remoteip = socket->peerAddress().toString().remove(QString("::ffff:")); // get remote client ip address , convert tp ip4


    if ( m_map.key(socket, "null") == "null" )
    {
        m_map.insert(remoteip, socket);
        emit hasAlterLink(m_map.keys());
    }

    qDebug() << "incomingConnection... Add a socket, remoteIp is: " << remoteip;

    connect(socket, &QAbstractSocket::disconnected, this, &TcpServer::alterLink);
    connect(socket, &TcpSocket::hasGetData, this, &TcpServer::getData);

    connect(this, &TcpServer::m_deleteSock, socket, &TcpSocket::closeSocket);
    connect(this, &TcpServer::m_sendData, socket, &TcpSocket::sendData);

    QThread * thread = new QThread(socket);
    connect(socket, &QAbstractSocket::disconnected, thread, &QThread::quit);
    socket->moveToThread(thread);
    thread->start();

    emit newConnection();
}




/* about udp it's better to use p2p */

UdpServer::UdpServer(QUdpSocket * parent) : QUdpSocket (parent)
{
    connect(this, &QUdpSocket::readyRead, this, &UdpServer::recvData);
}

void UdpServer::startServer(const int port)
{
    bind(quint16(port));
}

void UdpServer::stopServer()
{
    this->close();
}

void UdpServer::sendData(const QByteArray & data, const QString ip, const int port)
{
    qDebug() << this->writeDatagram(data, QHostAddress::Broadcast, quint16(port));
}


void UdpServer::recvData()
{
    QByteArray arr;
    while ( this->hasPendingDatagrams() )
    {
        arr.resize((pendingDatagramSize()));
        this->readDatagram(arr.data(), arr.size());
    }
    qDebug() << arr;
}

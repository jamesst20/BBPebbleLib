/*
 * BluetoothDevice.h
 *
 *  Created on: 2015-03-06
 *      Author: James
 */

#ifndef BLUETOOTHDEVICE_H_
#define BLUETOOTHDEVICE_H_

#include <QList>
#include <QObject>


#ifdef __QNXNTO__
#include <QtConnectivity/QBluetoothUuid>
#include <QtConnectivity/QBluetoothSocket>
#include <QtConnectivity/QBluetoothDeviceInfo>

using QtMobility::QBluetoothUuid;
using QtMobility::QBluetoothSocket;
using QtMobility::QBluetoothDeviceInfo;
#else
#include <QBluetoothUuid>
#include <QBluetoothSocket>
#include <QBluetoothDeviceInfo>
#endif


class BluetoothDevice: public QObject
{
    Q_OBJECT
public:
    BluetoothDevice(const QBluetoothDeviceInfo& device, QObject *parent = 0);
    virtual ~BluetoothDevice();

    QString getBluetoothAddress() const;
    QString getDeviceName() const;

    void abortConnection();
    void connectToDevice();
    void sendData(const QByteArray &bytes);

    bool isConnected() const;
    bool isConnecting() const;

signals:
    void connected();
    void disconnected();
    void aboutToClose();
    void bytesWritten (qint64 bytes);
    void dataReceived(QBluetoothSocket &m_socket);
    void socketError(const QBluetoothSocket::SocketError &error);
    void stateChanged(const QBluetoothSocket::SocketState &state);


private slots:
    void readyRead();
    void onError(QBluetoothSocket::SocketError error);
    void onStateChanged(QBluetoothSocket::SocketState state);

private:
    QBluetoothDeviceInfo m_device;
    QBluetoothSocket m_socket;
};

#endif /* BLUETOOTHDEVICE_H_ */

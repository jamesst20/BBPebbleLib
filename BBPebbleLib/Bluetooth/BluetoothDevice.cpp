/*
 * BluetoothDevice.cpp
 *
 *  Created on: 2015-03-06
 *      Author: James
 */

#include <QtConnectivity/QBluetoothAddress>

#include "BluetoothDevice.h"


BluetoothDevice::BluetoothDevice(const QBluetoothDeviceInfo& device, QObject *parent)
    : QObject(parent)
    , m_device(device)
{
    init();
}

BluetoothDevice::BluetoothDevice(const QString& mac, const QString& name, quint32 deviceClass, QObject *parent)
    :QObject(parent)
{
    m_device = QBluetoothDeviceInfo(QtMobility::QBluetoothAddress(mac), name, deviceClass);

    init();
}

BluetoothDevice::~BluetoothDevice()
{

}

void BluetoothDevice::init()
{
    connect(&m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(&m_socket, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(&m_socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(&m_socket, SIGNAL(bytesWritten(qint64)), this, SIGNAL(bytesWritten(qint64)));
    connect(&m_socket, SIGNAL(aboutToClose()), this, SIGNAL(aboutToClose()));
    connect(&m_socket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(onError(QBluetoothSocket::SocketError)));
    connect(&m_socket, SIGNAL(stateChanged(QBluetoothSocket::SocketState)), this, SLOT(onStateChanged(QBluetoothSocket::SocketState)));
}

QString BluetoothDevice::getBluetoothAddress() const
{
    return this->m_device.address().toString();
}

QString BluetoothDevice::getDeviceName() const
{
    return this->m_device.name();
}

quint32 BluetoothDevice::getDeviceType() const
{
    return this->m_device.majorDeviceClass();
}

void BluetoothDevice::abortConnection(){
    this->m_socket.abort();
}

void BluetoothDevice::sendData(const QByteArray &bytes){
    this->m_socket.write(bytes);
}

bool BluetoothDevice::isConnected() const{
    return this->m_socket.state() == QBluetoothSocket::ConnectedState;
}

bool BluetoothDevice::isConnecting() const{
    return this->m_socket.state() == QBluetoothSocket::ConnectingState;
}

void BluetoothDevice::connectToDevice(){
    if(!isConnected()){
        this->m_socket.abort();
        this->m_socket.connectToService(this->m_device.address(), QBluetoothUuid(QBluetoothUuid::SerialPort));
    }else{
        qDebug() << "Socket already initialized. Already connected ?";
    }
}

void BluetoothDevice::readyRead(){
    emit dataReceived(this->m_socket);
}

void BluetoothDevice::onError(QBluetoothSocket::SocketError error){
    emit socketError(error);
}

void BluetoothDevice::onStateChanged(QBluetoothSocket::SocketState state){
    emit stateChanged(state);
}

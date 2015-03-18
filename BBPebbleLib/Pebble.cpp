/*
 * Pebble.cpp
 *
 *  Created on: 2015-03-07
 *      Author: James
 */

#include "Pebble.h"

const quint8 Pebble::PEBBLE_CLIENT_VERSION = 2;

Pebble::Pebble(BluetoothDevice *bt_device)
{
    if (!bt_device->isConnected()) {
        throw std::logic_error("Pebble object can't be created with a disconnected bluetooth device.");
    }

    this->pebbleReady = false;
    this->payloadSize = 0;
    this->endPoint = 0;
    this->bt_device = bt_device;

    connect(bt_device, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
    connect(bt_device, SIGNAL(dataReceived(QBluetoothSocket&)), this, SLOT(onDataReceived(QBluetoothSocket&)));

    connect(this, SIGNAL(dataReadFinished(quint16, const QByteArray&)), this, SLOT(onDataReadFinished(quint16, const QByteArray&)));
}

Pebble::~Pebble()
{

}

void Pebble::onDisconnect()
{
    emit disconnected(this->bt_device);
}

void Pebble::onDataReceived(QBluetoothSocket &bt_socket)
{
    QDataStream in(&bt_socket);

    if (this->payloadSize == 0) {
        if (bt_socket.bytesAvailable() < (int)2*sizeof(quint16)) {
            return;
        }
        in >> this->payloadSize;
        in >> this->endPoint;
    }

    if(bt_socket.bytesAvailable() < this->payloadSize){
        return;
    }
    char buffer[this->payloadSize];
    in.readRawData(buffer, this->payloadSize);
    QByteArray payload(buffer);

    qDebug() << "Received data. Endpoint : " << endPoint << " Payload Size : " << payloadSize << " HEX : " << payload.toHex() << "STR : " << QString(payload);

    emit onDataReadFinished(endPoint, payload);

    this->payloadSize = 0;
    this->endPoint = 0;

    onDataReceived(bt_socket);
}

void Pebble::onDataReadFinished(quint16 endPoint, const QByteArray &payload){
    //PhoneVersion is received immediatly after connecting to Pebble. We must respond to it first.
    if(!pebbleReady && endPoint == Enums::Endpoint::PhoneVersion){
        QByteArray payloadToSend;
        QDataStream dataStream(&payloadToSend, QIODevice::WriteOnly);
        dataStream << (quint8)PEBBLE_CLIENT_VERSION << (quint8)0xFF << (quint8)0xFF << (quint8)0xFF << (quint8)0xFF;
        dataStream << (quint32) Enums::Session::GAMMA_RAY;
        dataStream << (quint32) (Enums::Client::SMS | Enums::Client::TELEPHONY | Enums::Client::ANDROID);

        sendDataToPebble(Enums::Endpoint::PhoneVersion, payloadToSend);
        //pebbleReady = true;
        qDebug() << "Phone version asked, just replied. But does it really work?";

    }
}

void Pebble::sendDataToPebble(quint16 endPoint, const QByteArray &payload) const {
    QByteArray finalData;
    QDataStream in(&finalData, QIODevice::WriteOnly);
    in << (quint16)payload.size();
    in << (quint16)endPoint;
    finalData.append(payload);
    in.setByteOrder(QDataStream::BigEndian);
    this->bt_device->sendData(finalData);
}

void Pebble::pingPebble(quint32 pingData) const {
    QByteArray bytes;
    QDataStream in(&bytes, QIODevice::WriteOnly);
    in << (quint8)0;
    in << (quint32)pingData;
    sendDataToPebble(Enums::Endpoint::Ping, bytes);
}

void Pebble::notifyEmail(const QString &sender, const QString &subject, const QString &body) const {
    QStringList strings;
    strings << sender << body << subject;
    sendNotification(Enums::Notifications::Email, strings);
}

void Pebble::notifySMS(const QString &sender, const QString &body) const {
    QStringList strings;
    strings << sender << body;
    sendNotification(Enums::Notifications::SMS, strings);
}

void Pebble::notifyFacebook(const QString &sender, const QString &body) const{
    QStringList strings;
    strings << sender << body;
    sendNotification(Enums::Notifications::Facebook, strings);
}

void Pebble::notifyTwitter(const QString &sender, const QString &body) const {
    QStringList strings;
    strings << sender << body;
    sendNotification(Enums::Notifications::Twitter, strings);
}

void Pebble::sendNotification(quint8 type, const QStringList &strings) const {
    QByteArray notificationArray;
    QDataStream in(&notificationArray, QIODevice::WriteOnly);

    in << (quint8)type;
    //Send Sender + Body
    for(int i = 0; i < strings.length() && i < 2; i++){
        in << (quint8)strings.at(i).size();
        in.writeRawData(strings.at(i).toStdString().c_str(), strings.at(i).size());
    }
    //Send Timestamp
    QString timestamp = QString::number(std::time(0));
    in << (quint8)timestamp.size();
    in.writeRawData(timestamp.toStdString().c_str(), timestamp.size());
    //Send more (i.e 'Subject' in case of an email) if needed.
    for(int i = 2; i < strings.length(); i++){
        in << (quint8)strings.at(i).size();
        in.writeRawData(strings.at(i).toStdString().c_str(), strings.at(i).size());
    }

    qDebug() << "Notification array : " << notificationArray.toHex();
    sendDataToPebble(Enums::Endpoint::Notification, notificationArray);
}

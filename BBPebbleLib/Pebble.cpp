/*
 * Pebble.cpp
 *
 *  Created on: 2015-03-07
 *      Author: James
 */

#include <QtEndian>
#include <QDateTime>

#include "math.h"

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
    static const int header_length = (int)2*sizeof(quint16);

    while (bt_socket.bytesAvailable() >= header_length) {
        // Take a look at the header, but do not remove it from the socket input buffer.
        // We will only remove it once we're sure the entire packet is in the buffer.
        uchar header[header_length];
        bt_socket.peek(reinterpret_cast<char*>(header), header_length);

        quint16 message_length = qFromBigEndian<quint16>(&header[0]);
        quint16 endpoint = qFromBigEndian<quint16>(&header[2]);

        // Sanity checks on the message_length
        if (message_length == 0) {
            qDebug() << "received empty message";
            bt_socket.read(header_length); // skip this header
            continue; // check if there are additional headers.
        } else if (message_length > 8 * 1024) {
            // Protocol does not allow messages more than 8K long, seemingly.
            qDebug() << "received message size too long: " << message_length;
            bt_socket.readAll(); // drop entire input buffer
            return;
        }

        // Now wait for the entire message
        if (bt_socket.bytesAvailable() < header_length + message_length) {
            qDebug() << "incomplete msg body in read buffer";
            return; // try again once more data comes in
        }

        // We can now safely remove the header from the input buffer,
        // as we know the entire message is in the input buffer.
        bt_socket.read(header_length);

        // Now read the rest of the message
        QByteArray data = bt_socket.read(message_length);

        qDebug() << "<<<< received data. Endpoint : " << endpoint << " Payload Size : " << data.size() << " HEX : " << data.toHex() << "STR : " << QString(data);

        emit onDataReadFinished(endpoint, data);

        onDataReceived(bt_socket);
    }
}

void Pebble::onDataReadFinished(quint16 endPoint, const QByteArray &payload) {
    Q_UNUSED(payload);

    //PhoneVersion is received immediatly after connecting to Pebble. We must respond to it first.
    if(!pebbleReady && endPoint == Enums::Endpoint::PhoneVersion) {
        unsigned int sessionCap = Enums::Session::GAMMA_RAY;
        unsigned int remoteCap = Enums::Client::TELEPHONY | Enums::Client::SMS | Enums::Client::ANDROID;
        QByteArray res;

        //Prefix
        res.append(0x01);
        res.append(0xff);
        res.append(0xff);
        res.append(0xff);
        res.append(0xff);

        //Session Capabilities
        res.append((char)((sessionCap >> 24) & 0xff));
        res.append((char)((sessionCap >> 16) & 0xff));
        res.append((char)((sessionCap >> 8) & 0xff));
        res.append((char)(sessionCap & 0xff));

        //Remote Capabilities
        res.append((char)((remoteCap >> 24) & 0xff));
        res.append((char)((remoteCap >> 16) & 0xff));
        res.append((char)((remoteCap >> 8) & 0xff));
        res.append((char)(remoteCap & 0xff));

        //Version Magic
        res.append((char)0x02);

        //Append Version
        res.append((char)0x02); //Major
        res.append((char)0x00); //Minor
        res.append((char)0x00); //Bugfix

        sendDataToPebble(Enums::Endpoint::PhoneVersion, res);
        pebbleReady = true;
    }
}

void Pebble::sendDataToPebble(quint16 endPoint, const QByteArray &payload) const {
    QByteArray finalData;
    QDataStream in(&finalData, QIODevice::WriteOnly);
    in << (quint16)payload.size();
    in << (quint16)endPoint;
    finalData.append(payload);
    in.setByteOrder(QDataStream::BigEndian);

    qDebug() << ">>>> sent data. Endpoint : " << endPoint << " Payload Size : " << payload.size() << " HEX : " << payload.toHex() << "STR : " << QString(payload);

    this->bt_device->sendData(finalData);
}

void Pebble::setTime() const
{
    QByteArray res;
    QDateTime UTC(QDateTime::currentDateTimeUtc());
    QDateTime local(UTC.toLocalTime());
    local.setTimeSpec(Qt::UTC);
    int offset = UTC.secsTo(local);
    uint val = (local.toMSecsSinceEpoch() + offset) / 1000;

    res.append(0x02); //SET_TIME_REQ
    res.append((char)((val >> 24) & 0xff));
    res.append((char)((val >> 16) & 0xff));
    res.append((char)((val >> 8) & 0xff));
    res.append((char)(val & 0xff));
    sendDataToPebble(Enums::Endpoint::Time, res);
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

void Pebble::sendNewNotification(const QString& sender, const QString& subject, const QString& data) const
{
    // default source
    int source = 1;

    int attributesCount = 0;
    QByteArray attributes;

    attributesCount++;
    QByteArray senderBytes = sender.left(64).toUtf8();
    attributes.append(0x01); // id = title
    attributes.append(senderBytes.length() & 0xFF); attributes.append(((senderBytes.length() >> 8) & 0xFF)); // length
    attributes.append(senderBytes); // content

    attributesCount++;
    QByteArray subjectBytes = (subject.isEmpty() ? data : subject).left(64).toUtf8();
    attributes.append(0x02); // id = subtitle
    attributes.append(subjectBytes.length() & 0xFF); attributes.append((subjectBytes.length() >> 8) & 0xFF); // length
    attributes.append(subjectBytes); //content

    if (!data.isEmpty()) {
        attributesCount++;
        QByteArray dataBytes = data.left(512).toUtf8();
        attributes.append(0x03); // id = body
        attributes.append(dataBytes.length() & 0xFF); attributes.append((dataBytes.length() >> 8) & 0xFF); // length
        attributes.append(dataBytes); // content
    }

    attributesCount++;
    attributes.append(0x04); // id = tinyicon
    attributes.append(0x04); attributes.append('\0'); // length
    attributes.append(source); attributes.append('\0'); attributes.append('\0'); attributes.append('\0'); // content


    QByteArray actions;
    actions.append('\0'); // action id
    actions.append(0x04); // type = dismiss
    actions.append(0x01); // attributes length = 1
    actions.append(0x01); // attribute id = title
    actions.append(0x07); actions.append('\0'); // attribute length
    actions.append("Dismiss"); // attribute content


    QByteArray itemId = QUuid::createUuid().toRfc4122();
    int time = QDateTime::currentMSecsSinceEpoch() / 1000;
    QByteArray item;
    item.append(itemId); // item id
    item.append(QUuid().toRfc4122()); // parent id
    item.append(time & 0xFF); item.append((time >> 8) & 0xFF); item.append((time >> 16) & 0xFF); item.append((time >> 24) & 0xFF); // timestamp
    item.append('\0'); item.append('\0'); // duration
    item.append(0x01); // type: notification
    item.append('\0'); item.append('\0'); // flags
    item.append(0x01); // layout

    int length = attributes.length() + actions.length();
    item.append(length & 0xFF); item.append((length >> 8) & 0xFF); // data length
    item.append(attributesCount); // attributes count
    item.append(0x01); // actions count
    item.append(attributes);
    item.append(actions);

    int token = (qrand() % ((int)pow(2, 16) - 2)) + 1;
    QByteArray blob;
    blob.append(0x01); // command = insert
    blob.append(token & 0xFF); blob.append((token >> 8) & 0xFF); // token
    blob.append(0x04); //database id = notification
    blob.append(itemId.length() & 0xFF); // key length
    blob.append(itemId); // key
    blob.append(item.length() & 0xFF); blob.append((item.length() >> 8) & 0xFF); // value length
    blob.append(item);

    sendDataToPebble(Enums::Endpoint::BlobDB, blob);
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

    sendDataToPebble(Enums::Endpoint::Notification, notificationArray);
}

void Pebble::notifyPhoneCallStart(bool incoming, const QString &number, const QString &name, quint32 cookie) const {
    QByteArray notifArray;

    QDataStream in(&notifArray, QIODevice::WriteOnly);

    in << Enums::PhoneControlCommands::INCOMING_CALL;
    in << cookie;
    in << (quint8)number.size();
    in.writeRawData(number.toStdString().c_str(), number.size());
    in << (quint8)name.size();
    in.writeRawData(name.toStdString().c_str(), name.size());

    sendDataToPebble(Enums::Endpoint::PhoneControl, notifArray);

    //HACK : Pebble isn't supporting OUTGOING_CALL. What can be done is sending call start immediatly after incoming call.
    if(!incoming){
        notifArray.clear();

        in.device()->seek(0);
        in << Enums::PhoneControlCommands::START;
        in << cookie;

        sendDataToPebble(Enums::Endpoint::PhoneControl, notifArray);
    }
}

void Pebble::notifyPhoneCallEnd(quint32 cookie) const {
    QByteArray notifArray;

    QDataStream in(&notifArray, QIODevice::WriteOnly);

    in << Enums::PhoneControlCommands::END;
    in << cookie;

    sendDataToPebble(Enums::Endpoint::PhoneControl, notifArray);
}

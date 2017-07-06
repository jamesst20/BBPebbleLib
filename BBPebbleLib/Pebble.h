/*
 * Pebble.h
 *
 *  Created on: 2015-03-07
 *      Author: James
 */

#ifndef PEBBLE_H_
#define PEBBLE_H_

#include "Enums.h"
#include "Bluetooth/BluetoothDevice.h"

#include <QObject>
#include <QStringList>

#include <stdexcept>
#include <ctime>

class Pebble: public QObject
{
    Q_OBJECT
public:
    Pebble(BluetoothDevice *bt_device);
    virtual ~Pebble();

    void sendDataToPebble(quint16 endPoint, const QByteArray &payload) const;

    void pingPebble(quint32 pingData = 0) const;
    void sendNewNotification(const QString& sender, const QString& subject, const QString& data) const;
    void notifyEmail(const QString &sender, const QString &subject, const QString &body) const;
    void notifySMS(const QString &sender, const QString &body) const;
    void notifyFacebook(const QString &sender, const QString &body) const;
    void notifyTwitter(const QString &sender, const QString &body) const;
    void notifyPhoneCallStart(bool incoming, const QString &number, const QString &name, quint32 cookie = 0) const;
    void notifyPhoneCallEnd(quint32 cookie = 0) const;

signals:
    void dataReadFinished(quint16 endPoint, const QByteArray &payload);
    void disconnected(BluetoothDevice *bt_device);

private slots:
    void onDisconnect();
    void onDataReceived(QBluetoothSocket &bt_socket);
    void onDataReadFinished(quint16 endPoint, const QByteArray &payload);


private:
    void sendNotification(quint8 type, const QStringList &strings) const;

    static const quint8 PEBBLE_CLIENT_VERSION;

    BluetoothDevice *bt_device;

    quint16 payloadSize;
    quint16 endPoint;

    bool pebbleReady;
};

#endif /* PEBBLE_H_ */

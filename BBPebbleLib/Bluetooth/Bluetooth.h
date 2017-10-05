/*
 * Bluetooth.h
 *
 *  Created on: 2015-03-06
 *      Author: James
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include "BluetoothDevice.h"
#ifdef __QNXNTO__
#include <QtConnectivity/QBluetoothAddress>
#include <QtConnectivity/QBluetoothUuid>
#include <QtConnectivity/QBluetoothDeviceDiscoveryAgent>
#include <QtConnectivity/QBluetoothDeviceInfo>
#include <QtConnectivity/QBluetoothSocket>

using QtMobility::QBluetoothDeviceInfo;
using QtMobility::QBluetoothDeviceDiscoveryAgent;
using QtMobility::QBluetoothAddress;
using QtMobility::QBluetoothUuid;
using QtMobility::QBluetoothSocket;
#else
#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QBluetoothSocket>
#endif

class Bluetooth: public QObject
{
Q_OBJECT

public:
    Bluetooth(QObject *parent = 0);
    virtual ~Bluetooth();

signals:
    void discovered(BluetoothDevice *bt_device);
    void scanFinished();
    void error(QString error);

public slots:
    void startDiscovery();
    void stopDiscovery();

private slots:
    void onDiscovered(const QBluetoothDeviceInfo& info);
    void onError();
    void discoveryFinished();
    void discoveryCanceled();

private:
    QBluetoothDeviceDiscoveryAgent* m_agent;
    volatile bool m_discoveryRunning;
};

#endif /* BLUETOOTH_H_ */

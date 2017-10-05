/*
 * Pebble.cpp
 *
 *  Created on: 2015-03-06
 *      Author: James
 */

#include "Bluetooth.h"

Bluetooth::Bluetooth(QObject *parent)
    : QObject(parent)
    , m_agent(new QBluetoothDeviceDiscoveryAgent(this))
    , m_discoveryRunning(false)
{

    bool bOk = connect(m_agent, SIGNAL(finished()), this, SLOT(discoveryFinished()), Qt::UniqueConnection);
    Q_ASSERT(bOk);
    bOk = connect(m_agent, SIGNAL(canceled()), this, SLOT(discoveryCanceled()), Qt::UniqueConnection);
    Q_ASSERT(bOk);
    bOk = connect(m_agent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)), this, SLOT(onDiscovered(const QBluetoothDeviceInfo&)));
    Q_ASSERT(bOk);
    bOk = connect(m_agent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)), this, SLOT(onError()));
    Q_ASSERT(bOk);
}

Bluetooth::~Bluetooth()
{

}

void Bluetooth::startDiscovery()
{
    m_discoveryRunning = true;
    m_agent->start();
}

void Bluetooth::onDiscovered(const QBluetoothDeviceInfo& info)
{
    emit discovered(new BluetoothDevice(info, this));
}

void Bluetooth::stopDiscovery()
{
    m_agent->stop();
}

void Bluetooth::onError()
{
    m_discoveryRunning = false;
    emit error(m_agent->errorString());
}

void Bluetooth::discoveryFinished()
{
    if(m_discoveryRunning)
    {
        m_discoveryRunning = false;
        emit scanFinished();
    }
}
void Bluetooth::discoveryCanceled()
{
    m_discoveryRunning = false;
    emit scanFinished();
}

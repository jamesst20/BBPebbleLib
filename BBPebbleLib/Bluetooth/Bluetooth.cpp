/*
 * Pebble.cpp
 *
 *  Created on: 2015-03-06
 *      Author: James
 */

#include "Bluetooth.h"

Bluetooth::Bluetooth(QObject *parent) : QObject(parent)
{

    m_agent = new QBluetoothDeviceDiscoveryAgent(this);

    connect(m_agent, SIGNAL(finished()), this, SIGNAL(scanFinished()));
    connect(m_agent, SIGNAL(canceled()), this, SIGNAL(scanFinished()));
    connect(m_agent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)), this, SLOT(onDiscovered(const QBluetoothDeviceInfo&)));
    connect(m_agent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)), this, SLOT(onError()));

}

Bluetooth::~Bluetooth()
{

}

void Bluetooth::startDiscovery()
{
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
    emit error(m_agent->errorString());
}

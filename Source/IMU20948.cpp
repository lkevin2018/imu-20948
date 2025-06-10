/*
------------------------------------------------------------------

This file is part of the Open Ephys GUI
Copyright (C) 2022 Open Ephys

------------------------------------------------------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "IMU20948.h"

#include "ProcessorPluginEditor.h"
#ifdef _WIN32
   // Skip Arduino I2C on Windows; sensor code is stubbed or use alternative API
#else
   #include <Wire.h>
   #include <SparkFun_ICM_20948_Arduino_Library.h>
#endif


IMU20948::IMU20948()
    : GenericProcessor("ICM-20948"), imuSensor()
{
    // Initialize channel layout for 9 IMU channels
    for (int i = 0; i < NUM_IMU_CHANNELS; ++i)
    {
        String chName;
        switch (i)
        {
            case 0: chName = "Accel X"; break;
            case 1: chName = "Accel Y"; break;
            case 2: chName = "Accel Z"; break;
            case 3: chName = "Gyro X"; break;
            case 4: chName = "Gyro Y"; break;
            case 5: chName = "Gyro Z"; break;
            case 6: chName = "Mag X"; break;
            case 7: chName = "Mag Y"; break;
            case 8: chName = "Mag Z"; break;
        }
        channelIndices[i] = addChannel(chName, RecordingChannel::IMU); // add IMU channel type
    }
}


IMU20948::~IMU20948()
{
}


AudioProcessorEditor* IMU20948::createEditor()
{
    editor = std::make_unique<ProcessorPluginEditor>(this);
    return editor.get();
}


void IMU20948::updateSettings()
{
    // nothing to update for IMU sensor
}


void IMU20948::process(AudioBuffer<float>& buffer)
{
    if (!imuInitialized)
        imuInitialized = initSensor();

    if (imuInitialized && readSensorData())
    {
        // write each axis data to channel buffer (single-sample at current timestamp)
        for (int i = 0; i < NUM_IMU_CHANNELS; ++i)
        {
            setChannelSample(channelIndices[i], imuData[i]);
        }
    }
    checkForEvents(true);
}


bool IMU20948::initSensor()
{
    Wire.begin();
    if (imuSensor.begin() != ICM_20948_Stat_Ok)
        return false;
    return true;
}


bool IMU20948::readSensorData()
{
    if (imuSensor.dataReady())
    {
        imuSensor.getAGMT();
        imuData[0] = imuSensor.accX();
        imuData[1] = imuSensor.accY();
        imuData[2] = imuSensor.accZ();
        imuData[3] = imuSensor.gyrX();
        imuData[4] = imuSensor.gyrY();
        imuData[5] = imuSensor.gyrZ();
        imuData[6] = imuSensor.magX();
        imuData[7] = imuSensor.magY();
        imuData[8] = imuSensor.magZ();
        return true;
    }
    return false;
}


void IMU20948::handleTTLEvent(TTLEventPtr event)
{

}


void IMU20948::handleSpike(SpikePtr spike)
{

}


void IMU20948::handleBroadcastMessage(String message)
{

}


void IMU20948::saveCustomParametersToXml(XmlElement* parentElement)
{

}


void IMU20948::loadCustomParametersFromXml(XmlElement* parentElement)
{

}

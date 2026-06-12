#pragma once
#include "../src/gate.h"

struct SensorRow {
    bool sensorLock         = false;
    bool sensorLights       = false;
    bool sensorPhotoEye     = false;
    bool sensorExternalRelay = false;
};

class GateTestHarness : public Gate {
public:
    void setSimTime(unsigned long t) { _simTime = t; }
    void setSensors(const SensorRow& row) { _sensors = row; }

protected:
    unsigned long _millis() override { return _simTime; }
    bool _readSensorLock()          override { return _sensors.sensorLock; }
    bool _readSensorLights()        override { return _sensors.sensorLights; }
    bool _readSensorPhotoEye()      override { return _sensors.sensorPhotoEye; }
    bool _readSensorExternalRelay() override { return _sensors.sensorExternalRelay; }

private:
    unsigned long _simTime = 0;
    SensorRow     _sensors;
};

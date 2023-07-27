/*
===============================================================================

Copyright (C) 2023 Bernardo Escalona. All Rights Reserved.

  This file is part of AES70_OCP1_StringGenerator, found at:
  https://github.com/escalonely/AES70_OCP1_StringGenerator

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

===============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Ocp1Message.h"


namespace AES70
{

enum ClassIdx
{
    OCA_ROOT = 1,
    OCA_WORKER,
    OCA_ACTUATOR,
    OCA_SWITCH,
    OCA_MUTE,
    OCA_GAIN,
    OCA_DELAY,
    OCA_BASIC_ACTUATOR,
    OCA_STRING_ACTUATOR,
    OCA_INT32_ACTUATOR,
    OCA_SENSOR,
    OCA_BASIC_SENSOR,
    OCA_BOOLEAN_SENSOR,
    OCA_INT32_SENSOR,
    OCA_FLOAT32_SENSOR,
    OCA_STRING_SENSOR,
    OCA_LEVEL_SENSOR,
    OCA_AUDIO_LEVEL_SENSOR,
    OCA_AGENT,
    OCA_MAX_CLASS_IDX,
};

static const std::map<int, juce::String> MapOfClassNames = {
    { OCA_ROOT,                 "OcaRoot" },
    { OCA_WORKER,               " +-OcaWorker" },
    { OCA_ACTUATOR,             " |  +-OcaActuator" },
    { OCA_SWITCH,               " |  |  +-OcaSwitch" },
    { OCA_MUTE,                 " |  |  +-OcaMute" },
    { OCA_GAIN,                 " |  |  +-OcaGain" },
    { OCA_DELAY,                " |  |  +-OcaDelay" },
    { OCA_BASIC_ACTUATOR,       " |  |  +-OcaBasicActuator" },
    { OCA_STRING_ACTUATOR,      " |  |     +-OcaStringActuator" },
    { OCA_INT32_ACTUATOR,       " |  |     +-OcaInt32Actuator" },
    { OCA_SENSOR,               " |  +-OcaSensor" },
    { OCA_BASIC_SENSOR,         " |     +-OcaBasicSensor" },
    { OCA_BOOLEAN_SENSOR,       " |     |  +-OcaBooleanSensor" },
    { OCA_INT32_SENSOR,         " |     |  +-OcaInt32Sensor" },
    { OCA_FLOAT32_SENSOR,       " |     |  +-OcaFloat32Sensor" },
    { OCA_STRING_SENSOR,        " |     |  +-OcaStringSensor" },
    { OCA_LEVEL_SENSOR,         " |     +-OcaLevelSensor" },
    { OCA_AUDIO_LEVEL_SENSOR,   " |        +-OcaAudioLevelSensor" },
    { OCA_AGENT,                " +-OcaAgent" }
};

struct Property
{
    int m_defLevel;
    int m_index;
    int m_type;
    juce::String m_name;
    int m_getMethodIdx;
    int m_setMethodIdx;
};

/**
 * OcaRoot
 */
struct OcaRoot
{
    static OcaRoot* Create(int classIdx);
 
    //virtual juce::String Name() const
    //{
    //    return "OcaRoot";
    //}

    virtual int DefLevel() const
    {
        return 1;
    }

    virtual std::vector<Property> GetProperties() const
    {
        std::vector<Property> ret;
        ret.push_back({
            OcaRoot::DefLevel(), 5 /* idx */, NanoOcp1::OCP1DATATYPE_STRING, "Role", 5 /* get */, 0 /* set */
            });
        return ret;
    }
};

/**
 * OcaWorker
 */
struct OcaWorker : public OcaRoot
{
    int DefLevel() const override
    {
        return OcaRoot::DefLevel() + 1;
    }

    std::vector<Property> GetProperties() const override
    {
        std::vector<Property> ret = OcaRoot::GetProperties();
        ret.push_back({
            OcaWorker::DefLevel(), 1 /* idx */, NanoOcp1::OCP1DATATYPE_BOOLEAN, "Enabled", 1 /* get */, 2 /* set */
            });
        return ret;
    }
};

/**
 * OcaActuator
 */
struct OcaActuator : public OcaWorker
{
    int DefLevel() const override
    {
        return OcaWorker::DefLevel() + 1;
    }

    std::vector<Property> GetProperties() const override
    {
        return OcaWorker::GetProperties();
    }
};

/**
 * OcaSwitch
 */
struct OcaSwitch : public OcaActuator
{
    int DefLevel() const override
    {
        return OcaActuator::DefLevel() + 1;
    }

    std::vector<Property> GetProperties() const override
    {
        std::vector<Property> ret = OcaActuator::GetProperties();
        ret.push_back({
            OcaSwitch::DefLevel(), 1 /* idx */, NanoOcp1::OCP1DATATYPE_UINT16, "Position", 1 /* get */, 2 /* set */
            });
        return ret;
    }
};

/**
 * OcaMute
 */
struct OcaMute : public OcaActuator
{
    int DefLevel() const override
    {
        return OcaActuator::DefLevel() + 1;
    }

    std::vector<Property> GetProperties() const override
    {
        std::vector<Property> ret = OcaActuator::GetProperties();
        ret.push_back({
            OcaMute::DefLevel(), 1 /* idx */, NanoOcp1::OCP1DATATYPE_UINT8, "Mute", 1 /* get */, 2 /* set */
            });
        return ret;
    }
};

/**
 * OcaGain
 */
struct OcaGain : public OcaActuator
{
    int DefLevel() const override
    {
        return OcaActuator::DefLevel() + 1;
    }

    std::vector<Property> GetProperties() const override
    {
        std::vector<Property> ret = OcaActuator::GetProperties();
        ret.push_back({
            OcaGain::DefLevel(), 1 /* idx */, NanoOcp1::OCP1DATATYPE_FLOAT32, "Gain", 1 /* get */, 2 /* set */
            });
        return ret;
    }
};

/**
 * OcaDelay
 */
struct OcaDelay : public OcaActuator
{
    int DefLevel() const override
    {
        return OcaActuator::DefLevel() + 1;
    }

    std::vector<Property> GetProperties() const override
    {
        std::vector<Property> ret = OcaActuator::GetProperties();
        ret.push_back({
            OcaDelay::DefLevel(), 1 /* idx */, NanoOcp1::OCP1DATATYPE_FLOAT32, "DelayTime", 1 /* get */, 2 /* set */
            });
        return ret;
    }
};

/**
 * OcaBasicActuator
 */
struct OcaBasicActuator : public OcaActuator
{
    int DefLevel() const override
    {
        return OcaActuator::DefLevel() + 1;
    }

    std::vector<Property> GetProperties() const override
    {
        return OcaActuator::GetProperties();
    }
};

/**
 * OcaStringActuator
 */
struct OcaStringActuator : public OcaBasicActuator
{
    int DefLevel() const override
    {
        return OcaBasicActuator::DefLevel() + 1;
    }

    std::vector<Property> GetProperties() const override
    {
        std::vector<Property> ret = OcaBasicActuator::GetProperties();
        ret.push_back({
            OcaStringActuator::DefLevel(), 1 /* idx */, NanoOcp1::OCP1DATATYPE_STRING, "Setting", 1 /* get */, 2 /* set */
            });
        return ret;
    }
};

/**
 * OcaInt32Actuator
 */
struct OcaInt32Actuator : public OcaBasicActuator
{
    int DefLevel() const override
    {
        return OcaBasicActuator::DefLevel() + 1;
    }

    std::vector<Property> GetProperties() const override
    {
        std::vector<Property> ret = OcaBasicActuator::GetProperties();
        ret.push_back({
            OcaInt32Actuator::DefLevel(), 1 /* idx */, NanoOcp1::OCP1DATATYPE_INT32, "Setting", 1 /* get */, 2 /* set */
            });
        return ret;
    }
};

/**
 * OcaSensor
 */
struct OcaSensor : public OcaWorker
{
    int DefLevel() const override
    {
        return OcaWorker::DefLevel() + 1;
    }

    std::vector<Property> GetProperties() const override
    {
        std::vector<Property> ret = OcaWorker::GetProperties();
        ret.push_back({
            OcaSensor::DefLevel(), 1 /* idx */, NanoOcp1::OCP1DATATYPE_UINT8, "ReadingState", 1 /* get */, 0 /* set */
            });
        return ret;
    }
};

/**
 * OcaBasicSensor
 */
struct OcaBasicSensor : public OcaSensor
{
    int DefLevel() const override
    {
        return OcaSensor::DefLevel() + 1;
    }

    std::vector<Property> GetProperties() const override
    {
        return OcaSensor::GetProperties();
    }
};

/**
 * OcaBooleanSensor
 */
struct OcaBooleanSensor : public OcaBasicSensor
{
    int DefLevel() const override
    {
        return OcaBasicSensor::DefLevel() + 1;
    }

    std::vector<Property> GetProperties() const override
    {
        std::vector<Property> ret = OcaBasicSensor::GetProperties();
        ret.push_back({
            OcaBooleanSensor::DefLevel(), 1 /* idx */, NanoOcp1::OCP1DATATYPE_BOOLEAN, "Reading", 1 /* get */, 0 /* set */
            });
        return ret;
    }
};

/**
 * OcaInt32Sensor
 */
struct OcaInt32Sensor : public OcaBasicSensor
{
    int DefLevel() const override
    {
        return OcaBasicSensor::DefLevel() + 1;
    }

    std::vector<Property> GetProperties() const override
    {
        std::vector<Property> ret = OcaBasicSensor::GetProperties();
        ret.push_back({
            OcaInt32Sensor::DefLevel(), 1 /* idx */, NanoOcp1::OCP1DATATYPE_INT32, "Reading", 1 /* get */, 0 /* set */
            });
        return ret;
    }
};

/**
 * OcaFloat32Sensor
 */
struct OcaFloat32Sensor : public OcaBasicSensor
{
    int DefLevel() const override
    {
        return OcaBasicSensor::DefLevel() + 1;
    }

    std::vector<Property> GetProperties() const override
    {
        std::vector<Property> ret = OcaBasicSensor::GetProperties();
        ret.push_back({
            OcaFloat32Sensor::DefLevel(), 1 /* idx */, NanoOcp1::OCP1DATATYPE_FLOAT32, "Reading", 1 /* get */, 0 /* set */
            });
        return ret;
    }
};

/**
 * OcaStringSensor
 */
struct OcaStringSensor : public OcaBasicSensor
{
    int DefLevel() const override
    {
        return OcaBasicSensor::DefLevel() + 1;
    }

    std::vector<Property> GetProperties() const override
    {
        std::vector<Property> ret = OcaBasicSensor::GetProperties();
        ret.push_back({
            OcaStringSensor::DefLevel(), 1 /* idx */, NanoOcp1::OCP1DATATYPE_STRING, "Reading", 1 /* get */, 0 /* set */
            });
        return ret;
    }
};

/**
 * OcaLevelSensor
 */
struct OcaLevelSensor : public OcaSensor
{
    int DefLevel() const override
    {
        return OcaSensor::DefLevel() + 1;
    }

    std::vector<Property> GetProperties() const override
    {
        std::vector<Property> ret = OcaSensor::GetProperties();
        ret.push_back({
            OcaLevelSensor::DefLevel(), 1 /* idx */, NanoOcp1::OCP1DATATYPE_FLOAT32, "Reading", 1 /* get */, 0 /* set */
            });
        return ret;
    }
};

/**
 * OcaLevelSensor
 */
struct OcaAudioLevelSensor : public OcaLevelSensor
{
    int DefLevel() const override
    {
        return OcaLevelSensor::DefLevel() + 1;
    }

    std::vector<Property> GetProperties() const override
    {
        return OcaLevelSensor::GetProperties();
    }
};

/**
 * OcaAgent
 */
struct OcaAgent : public OcaRoot
{
    int DefLevel() const override
    {
        return OcaRoot::DefLevel() + 1;
    }

    std::vector<Property> GetProperties() const override
    {
        std::vector<Property> ret = OcaRoot::GetProperties();
        ret.push_back({
            OcaAgent::DefLevel(), 1 /* idx */, NanoOcp1::OCP1DATATYPE_STRING, "Label", 1 /* get */, 2 /* set */
            });
        return ret;
    }
};


/**
 * Implementation of OcaRoot methods
 */

OcaRoot* OcaRoot::Create(int classIdx)
{
    OcaRoot* obj(nullptr);
    switch (classIdx)
    {
        case OCA_ROOT:
            obj = new OcaRoot;
            break;
        case OCA_WORKER:
            obj = new OcaWorker;
            break;
        case OCA_ACTUATOR:
            obj = new OcaActuator;
            break;
        case OCA_SWITCH:
            obj = new OcaSwitch;
            break;
        case OCA_MUTE:
            obj = new OcaMute;
            break;
        case OCA_GAIN:
            obj = new OcaGain;
            break;
        case OCA_DELAY:
            obj = new OcaDelay;
            break;
        case OCA_BASIC_ACTUATOR:
            obj = new OcaBasicActuator;
            break;
        case OCA_STRING_ACTUATOR:
            obj = new OcaStringActuator;
            break;
        case OCA_INT32_ACTUATOR:
            obj = new OcaInt32Actuator;
            break;
        case OCA_SENSOR:
            obj = new OcaSensor;
            break;
        case OCA_BASIC_SENSOR:
            obj = new OcaBasicSensor;
            break;
        case OCA_BOOLEAN_SENSOR:
            obj = new OcaBooleanSensor;
            break;
        case OCA_INT32_SENSOR:
            obj = new OcaInt32Sensor;
            break;
        case OCA_FLOAT32_SENSOR:
            obj = new OcaFloat32Sensor;
            break;
        case OCA_STRING_SENSOR:
            obj = new OcaStringSensor;
            break;
        case OCA_LEVEL_SENSOR:
            obj = new OcaLevelSensor;
            break;
        case OCA_AUDIO_LEVEL_SENSOR:
            obj = new OcaAudioLevelSensor;
            break;
        case OCA_AGENT:
            obj = new OcaAgent;
            break;
        default:
            break;
    }

    jassert(obj != nullptr); // Missing implementation!
    return obj;
}


}
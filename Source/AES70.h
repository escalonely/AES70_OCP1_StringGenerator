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

/**
 * Supported AES70 classes.
 * NOTE: The assigned index numbers are arbitrary, are not defined in AES70, 
 *       are in no way related to their ClassID as defined in AES70.
 */
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

/**
 * Decorated names for all supported AES70 classes.
 */
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

/**
 * AES70 Property
 */
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
    /**
     * Factory method to create an OcaRoot-related object.
     *
     * @param[in] classIdx  Index of AES70 class based on the ClassIdx enum.
     * @return  A pointer to an OcaRoot-related object. NOTE: Ownership of the object
     *          is responsibility of the caller of the method. 
     */
    static OcaRoot* Create(int classIdx);

    /**
     * Factory method to create a custom / proprietary / non-standard AES70 object.
     *
     * @param[in] TODO
     * @return  A pointer to an OcaRoot-related object. NOTE: Ownership of the object
     *          is responsibility of the caller of the method.
     */
    static OcaRoot* CreateCustom();

    /**
     * Definition level of the AES70 class, where OcaRoot is at level 1 and the level
     * increases with depth in the inheritance tree.
     * 
     * @return  Definition level of this class.
     */
    virtual int DefLevel() const;

    /**
     * Obtain the properties defined by AES70 for this class. This includes the properties
     * defined by all parent classes all the way up to OcaRoot.
     * 
     * @return  A list of all properties defined by this class.
     */
    virtual std::vector<Property> GetProperties() const;

    /**
     * Create a component which can be used to represent an AES70 class property on a GUI.
     * 
     * @param[in] prop  The class property, whose definition level, index, and data type define 
     *                  how to best represent and allow users to set this property on a GUI.
     * @param[in] onChangeFunction  Method which shall be called whenever the value on the created component changes.
     * @return  A pointer to a juce::Component to be used on the GUI. 
     *          NOTE: Ownership of the object is responsibility of the caller of the method. 
     */
    virtual juce::Component* CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction);

    /**
     * Read the user value of the given GUI component, and obtain the byte array used to represent 
     * this value in a AES70 OCP.1 message, based on the data type of the property.
     * 
     * @param[in] component The GUI component.
     * @param[in] prop      The class property, whose definition level, index, and data type define 
     *                      how the user value shall be interpreted for transmission.
     * @return  Byte array that can be used as parameter data for an OCP.1 message.
     */
    virtual std::vector<std::uint8_t> CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop);
};

/**
 * OcaWorker
 */
struct OcaWorker : public OcaRoot
{
    int DefLevel() const override;
    std::vector<Property> GetProperties() const override;
    juce::Component* CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction) override;
    std::vector<std::uint8_t> CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop) override;
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
    int DefLevel() const override;
    std::vector<Property> GetProperties() const override;
    juce::Component* CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction) override;
    std::vector<std::uint8_t> CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop) override;
};

/**
 * OcaMute
 */
struct OcaMute : public OcaActuator
{
    int DefLevel() const override;
    std::vector<Property> GetProperties() const override;
    juce::Component* CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction) override;
    std::vector<std::uint8_t> CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop) override;
};

/**
 * OcaGain
 */
struct OcaGain : public OcaActuator
{
    int DefLevel() const override;
    std::vector<Property> GetProperties() const override;
    juce::Component* CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction) override;
    std::vector<std::uint8_t> CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop) override;
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
 * OcaAudioLevelSensor
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
 * OcaCustomClass
 */
struct OcaCustomClass : public OcaRoot
{
    int DefLevel() const override;
    std::vector<Property> GetProperties() const override;
    juce::Component* CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction) override;
    std::vector<std::uint8_t> CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop) override;

    Property m_customProp; // User-defined property
};

}
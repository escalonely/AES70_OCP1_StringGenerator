/*
===============================================================================

 Copyright (C) 2023 Bernardo Escalona. All Rights Reserved.

  This file is part of AES70_OCP1_StringGenerator, found at:
  https://github.com/escalonely/AES70_OCP1_StringGenerator

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License version 3.0 as published
 by the Free Software Foundation.

 This library is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License
 along with this library; if not, write to the Free Software Foundation, Inc.,
 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

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
    OCA_BASIC_ACTUATOR,
    OCA_INT32_ACTUATOR,
    OCA_STRING_ACTUATOR,
    OCA_MUTE,
    OCA_SWITCH,
    OCA_GAIN,
    OCA_DELAY,
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
 * Class IDs and names for all supported AES70 classes.
 */
static const std::map<int, juce::String> MapOfClassNamesAndIds = {
    { OCA_ROOT,                 "1: OcaRoot" },
    { OCA_WORKER,               "1.1: OcaWorker" },
    { OCA_ACTUATOR,             "1.1.1: OcaActuator" },
    { OCA_BASIC_ACTUATOR,       "1.1.1.1: OcaBasicActuator" },
    { OCA_INT32_ACTUATOR,       "1.1.1.1.4: OcaInt32Actuator" },
    { OCA_STRING_ACTUATOR,      "1.1.1.1.12: OcaStringActuator" },
    { OCA_MUTE,                 "1.1.1.2: OcaMute" },
    { OCA_SWITCH,               "1.1.1.4: OcaSwitch" },
    { OCA_GAIN,                 "1.1.1.5: OcaGain" },
    { OCA_DELAY,                "1.1.1.7: OcaDelay" },
    { OCA_SENSOR,               "1.1.2: OcaSensor" },
    { OCA_BASIC_SENSOR,         "1.1.2.1: OcaBasicSensor" },
    { OCA_BOOLEAN_SENSOR,       "1.1.2.1.1: OcaBooleanSensor" },
    { OCA_INT32_SENSOR,         "1.1.2.1.4: OcaInt32Sensor" },
    { OCA_FLOAT32_SENSOR,       "1.1.2.1.10: OcaFloat32Sensor" },
    { OCA_STRING_SENSOR,        "1.1.2.1.12: OcaStringSensor" },
    { OCA_LEVEL_SENSOR,         "1.1.2.2: OcaLevelSensor" },
    { OCA_AUDIO_LEVEL_SENSOR,   "1.1.2.2.1: OcaAudioLevelSensor" },
    { OCA_AGENT,                "1.2: OcaAgent" }
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
     * @return  A pointer to an OcaRoot-related object. NOTE: Ownership of the object
     *          is responsibility of the caller of the method.
     */
    static OcaRoot* CreateCustom();

    /**
     * Helper method to obtain the AES70 class index which matches the class' name. 
     *
     * @param[in] className Name of AES70 class.
     * @return  Index of the corresponding AES70 class within the ClassIdx enum,
     *          or zero if no such class could be found.
     */
    static int GetClassIdxFromName(const juce::String& className);

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
    int DefLevel() const override;
    std::vector<Property> GetProperties() const override;
    juce::Component* CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction) override;
    std::vector<std::uint8_t> CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop) override;
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
};

/**
 * OcaStringActuator
 */
struct OcaStringActuator : public OcaBasicActuator
{
    int DefLevel() const override;
    std::vector<Property> GetProperties() const override;
    juce::Component* CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction) override;
    std::vector<std::uint8_t> CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop) override;
};

/**
 * OcaInt32Actuator
 */
struct OcaInt32Actuator : public OcaBasicActuator
{
    int DefLevel() const override;
    std::vector<Property> GetProperties() const override;
    juce::Component* CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction) override;
    std::vector<std::uint8_t> CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop) override;
};

/**
 * OcaSensor
 */
struct OcaSensor : public OcaWorker
{
    int DefLevel() const override;
    std::vector<Property> GetProperties() const override;
    juce::Component* CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction) override;
    std::vector<std::uint8_t> CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop) override;
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
};

/**
 * OcaBooleanSensor
 */
struct OcaBooleanSensor : public OcaBasicSensor
{
    int DefLevel() const override;
    std::vector<Property> GetProperties() const override;
    juce::Component* CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction) override;
    std::vector<std::uint8_t> CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop) override;
};

/**
 * OcaInt32Sensor
 */
struct OcaInt32Sensor : public OcaBasicSensor
{
    int DefLevel() const override;
    std::vector<Property> GetProperties() const override;
    juce::Component* CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction) override;
    std::vector<std::uint8_t> CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop) override;
};

/**
 * OcaFloat32Sensor
 */
struct OcaFloat32Sensor : public OcaBasicSensor
{
    int DefLevel() const override;
    std::vector<Property> GetProperties() const override;
    juce::Component* CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction) override;
    std::vector<std::uint8_t> CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop) override;
};

/**
 * OcaStringSensor
 */
struct OcaStringSensor : public OcaBasicSensor
{
    int DefLevel() const override;
    std::vector<Property> GetProperties() const override;
    juce::Component* CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction) override;
    std::vector<std::uint8_t> CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop) override;
};

/**
 * OcaLevelSensor
 */
struct OcaLevelSensor : public OcaSensor
{
    int DefLevel() const override;
    std::vector<Property> GetProperties() const override;
    juce::Component* CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction) override;
    std::vector<std::uint8_t> CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop) override;
};

/**
 * OcaAudioLevelSensor
 */
struct OcaAudioLevelSensor : public OcaLevelSensor
{
    int DefLevel() const override;
    std::vector<Property> GetProperties() const override;
    juce::Component* CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction) override;
    std::vector<std::uint8_t> CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop) override;
};

/**
 * OcaAgent
 */
struct OcaAgent : public OcaRoot
{
    int DefLevel() const override;
    std::vector<Property> GetProperties() const override;
    juce::Component* CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction) override;
    std::vector<std::uint8_t> CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop) override;
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
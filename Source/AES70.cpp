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

#include "AES70.h"

namespace AES70
{

//==============================================================================
// Class OcaRoot
//==============================================================================

int OcaRoot::DefLevel() const 
{
    return 1;
}

std::vector<Property> OcaRoot::GetProperties() const
{
    std::vector<Property> ret;
    ret.push_back({
        OcaRoot::DefLevel(), 5 /* idx */, NanoOcp1::OCP1DATATYPE_STRING, "Role", 5 /* get */, 0 /* set */
        });
    return ret;
}

juce::Component* OcaRoot::CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction)
{
    if ((prop.m_defLevel == OcaRoot::DefLevel()) &&
        (prop.m_index == 5))
    {
        auto pTextEditor = new juce::TextEditor("OcaRoot.Role");
        pTextEditor->setIndents(pTextEditor->getLeftIndent(), 0); // Hack for JUCE justification bug
        pTextEditor->setJustification(juce::Justification(juce::Justification::centredLeft));
        pTextEditor->setText("Some text");
        pTextEditor->onTextChange = [=]()
        {
            onChangeFunction();
        };

        return pTextEditor;
    }

    return nullptr;
}

std::vector<std::uint8_t> OcaRoot::CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop)
{
    if ((prop.m_defLevel == OcaRoot::DefLevel()) &&
        (prop.m_index == 5))
    {
        const juce::TextEditor* pTextEditor = static_cast<const juce::TextEditor*>(component);
        return NanoOcp1::DataFromString(pTextEditor->getText());
    }

    return std::vector<std::uint8_t>();
}


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

OcaRoot* OcaRoot::CreateCustom()
{
    return new OcaCustomClass();
}


//==============================================================================
// Class OcaWorker
//==============================================================================

int OcaWorker::DefLevel() const 
{
    return OcaRoot::DefLevel() + 1;
}

std::vector<Property> OcaWorker::GetProperties() const
{
    std::vector<Property> ret = OcaRoot::GetProperties();
    ret.push_back({
        OcaWorker::DefLevel(), 1 /* idx */, NanoOcp1::OCP1DATATYPE_BOOLEAN, "Enabled", 1 /* get */, 2 /* set */
        });
    return ret;
}

juce::Component* OcaWorker::CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction)
{
    if ((prop.m_defLevel == OcaWorker::DefLevel()) &&
        (prop.m_index == 1))
    {
        auto pComboBox = new juce::ComboBox("OcaWorker.Enabled");
        pComboBox->addItem("1: True", 1);
        pComboBox->addItem("0: False", 2);
        pComboBox->setSelectedId(1, juce::sendNotification);
        pComboBox->onChange = [=]()
        {
            onChangeFunction();
        };

        return pComboBox;
    }

    return OcaRoot::CreateComponentForProperty(prop, onChangeFunction);
}

std::vector<std::uint8_t> OcaWorker::CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop)
{
    if ((prop.m_defLevel == OcaWorker::DefLevel()) &&
        (prop.m_index == 1))
    {
        auto pComboBox = static_cast<const juce::ComboBox*>(component);
        std::uint8_t newValue = (pComboBox->getSelectedId() == 1) ? 1 : 0;
        return NanoOcp1::DataFromUint8(newValue);
    }

    return OcaRoot::CreateParamDataForComponent(component, prop);
}


//==============================================================================
// Class OcaActuator
//==============================================================================

//==============================================================================
// Class OcaSwitch
//==============================================================================

int OcaSwitch::DefLevel() const 
{
    return OcaActuator::DefLevel() + 1;
}

std::vector<Property> OcaSwitch::GetProperties() const 
{
    std::vector<Property> ret = OcaActuator::GetProperties();
    ret.push_back({
        OcaSwitch::DefLevel(), 1 /* idx */, NanoOcp1::OCP1DATATYPE_UINT16, "Position", 1 /* get */, 2 /* set */
        });
    return ret;
}

juce::Component* OcaSwitch::CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction)
{
    if ((prop.m_defLevel == OcaSwitch::DefLevel()) &&
        (prop.m_index == 1))
    {
        auto pComboBox = new juce::ComboBox("OcaSwitch.Position");
        for (int posIdx = 0; posIdx < 100; posIdx++)
            pComboBox->addItem(juce::String(posIdx), posIdx + 1);
        pComboBox->setSelectedId(1, juce::sendNotification);
        pComboBox->onChange = [=]()
        {
            onChangeFunction();
        };

        return pComboBox;
    }

    return OcaActuator::CreateComponentForProperty(prop, onChangeFunction);
}

std::vector<std::uint8_t> OcaSwitch::CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop)
{
    if ((prop.m_defLevel == OcaSwitch::DefLevel()) &&
        (prop.m_index == 1))
    {
        auto pComboBox = static_cast<const juce::ComboBox*>(component);
        std::uint16_t newValue = static_cast<std::uint16_t>(pComboBox->getSelectedId() - 1);
        return NanoOcp1::DataFromUint16(newValue);
    }

    return OcaActuator::CreateParamDataForComponent(component, prop);
}


//==============================================================================
// Class OcaMute
//==============================================================================

int OcaMute::DefLevel() const 
{
    return OcaActuator::DefLevel() + 1;
}

std::vector<Property> OcaMute::GetProperties() const 
{
    std::vector<Property> ret = OcaActuator::GetProperties();
    ret.push_back({
        OcaMute::DefLevel(), 1 /* idx */, NanoOcp1::OCP1DATATYPE_UINT8, "Mute", 1 /* get */, 2 /* set */
        });
    return ret;
}

juce::Component* OcaMute::CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction)
{
    if ((prop.m_defLevel == OcaMute::DefLevel()) &&
        (prop.m_index == 1))
    {
        auto pComboBox = new juce::ComboBox("OcaMute.Mute");
        pComboBox->addItem("1: Mute", 1);
        pComboBox->addItem("2: Unmute", 2);
        pComboBox->setSelectedId(1, juce::sendNotification);
        pComboBox->onChange = [=]()
        {
            onChangeFunction();
        };

        return pComboBox;
    }

    return OcaActuator::CreateComponentForProperty(prop, onChangeFunction);
}

std::vector<std::uint8_t> OcaMute::CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop)
{
    if ((prop.m_defLevel == OcaMute::DefLevel()) &&
        (prop.m_index == 1))
    {
        auto pComboBox = static_cast<const juce::ComboBox*>(component);
        std::uint8_t newValue = (pComboBox->getSelectedId() == 1) ? 1 : 2; // mute = 1, unmute = 2
        return NanoOcp1::DataFromUint8(newValue);
    }

    return OcaActuator::CreateParamDataForComponent(component, prop);
}


//==============================================================================
// Class OcaGain
//==============================================================================

int OcaGain::DefLevel() const 
{
    return OcaActuator::DefLevel() + 1;
}

std::vector<Property> OcaGain::GetProperties() const
{
    std::vector<Property> ret = OcaActuator::GetProperties();
    ret.push_back({
        OcaGain::DefLevel(), 1 /* idx */, NanoOcp1::OCP1DATATYPE_FLOAT32, "Gain", 1 /* get */, 2 /* set */
        });
    return ret;
}

juce::Component* OcaGain::CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction)
{
    if ((prop.m_defLevel == OcaGain::DefLevel()) &&
        (prop.m_index == 1))
    {
        auto pSlider = new juce::Slider(juce::Slider::LinearBar, juce::Slider::TextBoxBelow);
        pSlider->setHasFocusOutline(true);
        pSlider->setRange(-120.0, 12.0, 0.01);
        pSlider->setValue(0.0, juce::dontSendNotification);
        pSlider->onValueChange = [=]()
        {
            onChangeFunction();
        };

        return pSlider;
    }

    return OcaActuator::CreateComponentForProperty(prop, onChangeFunction);
}

std::vector<std::uint8_t> OcaGain::CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop)
{
    if ((prop.m_defLevel == OcaGain::DefLevel()) &&
        (prop.m_index == 1))
    {
        auto pSlider = static_cast<const juce::Slider*>(component);
        std::float_t newValue = static_cast<std::float_t>(pSlider->getValue());
        return NanoOcp1::DataFromFloat(newValue);
    }

    return OcaActuator::CreateParamDataForComponent(component, prop);
}


//==============================================================================
// Class OcaDelay
//==============================================================================

//==============================================================================
// Class OcaBasicActuator
//==============================================================================

//==============================================================================
// Class OcaStringActuator
//==============================================================================

//==============================================================================
// Class OcaInt32Actuator
//==============================================================================

//==============================================================================
// Class OcaSensor
//==============================================================================

//==============================================================================
// Class OcaBasicSensor
//==============================================================================

//==============================================================================
// Class OcaBooleanSensor
//==============================================================================

//==============================================================================
// Class OcaInt32Sensor
//==============================================================================

//==============================================================================
// Class OcaFloat32Sensor
//==============================================================================

//==============================================================================
// Class OcaStringSensor
//==============================================================================

//==============================================================================
// Class OcaLevelSensor
//==============================================================================

//==============================================================================
// Class OcaAudioLevelSensor
//==============================================================================

//==============================================================================
// Class OcaAgent
//==============================================================================

//==============================================================================
// Class OcaCustomClass
//==============================================================================

int OcaCustomClass::DefLevel() const
{
    jassertfalse; // Not expecting this method to be called.
    return 0;
}

std::vector<Property> OcaCustomClass::GetProperties() const
{
    // Custom class contains only one property: the one defined by the user via the GUI.
    return std::vector<Property>({ m_customProp });
}

juce::Component* OcaCustomClass::CreateComponentForProperty(const Property& prop, const std::function<void()>& onChangeFunction)
{
    juce::Component* ret(nullptr);

    switch (prop.m_type)
    {
        case NanoOcp1::OCP1DATATYPE_BOOLEAN:
            {
                auto pComboBox = new juce::ComboBox("OcaCustomClass Control");
                pComboBox->setHasFocusOutline(true);
                pComboBox->addItem("1: True", 1);
                pComboBox->addItem("0: False", 2);
                pComboBox->setSelectedId(1, juce::dontSendNotification);
                pComboBox->onChange = [=]()
                {
                    onChangeFunction();
                };
                ret = pComboBox;
            }
            break;
        case NanoOcp1::OCP1DATATYPE_UINT8:
            {
                auto pSlider = new juce::Slider(juce::Slider::LinearBar, juce::Slider::TextBoxBelow);
                pSlider->setHasFocusOutline(true);
                pSlider->setRange(std::numeric_limits<std::uint8_t>::min(), std::numeric_limits<std::uint8_t>::max(), 1);
                pSlider->setValue(std::numeric_limits<std::uint8_t>::min(), juce::dontSendNotification);
                pSlider->onValueChange = [=]()
                {
                    onChangeFunction();
                };
                ret = pSlider;
            }
            break;
        case NanoOcp1::OCP1DATATYPE_UINT16:
            {
                auto pSlider = new juce::Slider(juce::Slider::LinearBar, juce::Slider::TextBoxBelow);
                pSlider->setHasFocusOutline(true);
                pSlider->setRange(std::numeric_limits<std::uint16_t>::min(), std::numeric_limits<std::uint16_t>::max(), 1);
                pSlider->setValue(std::numeric_limits<std::uint16_t>::min(), juce::dontSendNotification);
                pSlider->onValueChange = [=]()
                {
                    onChangeFunction();
                };
                ret = pSlider;
            }
            break;
        case NanoOcp1::OCP1DATATYPE_UINT32:
            {
                juce::NormalisableRange<double> range(std::numeric_limits<std::uint32_t>::min(),
                    std::numeric_limits<std::uint32_t>::max(),
                    1);
                auto pSlider = new juce::Slider(juce::Slider::LinearBar, juce::Slider::TextBoxBelow);
                pSlider->setHasFocusOutline(true);
                pSlider->setNormalisableRange(range); // TODO: fix range display bug 
                //pSlider->setRange(std::numeric_limits<std::uint32_t>::min(), std::numeric_limits<std::uint32_t>::max(), 1);
                pSlider->setRange(0, 0x7fffffff, 1);
                pSlider->setValue(std::numeric_limits<std::uint32_t>::min(), juce::dontSendNotification);
                pSlider->onValueChange = [=]()
                {
                    onChangeFunction();
                };
                ret = pSlider;
            }
            break;
        case NanoOcp1::OCP1DATATYPE_FLOAT32:
            {
                //juce::NormalisableRange<double> range(std::numeric_limits<std::uint32_t>::min(),
                //    std::numeric_limits<std::uint32_t>::max(),
                //    1);            
                auto pSlider = new juce::Slider(juce::Slider::LinearBar, juce::Slider::TextBoxBelow);
                pSlider->setHasFocusOutline(true);
                //pSlider->setNormalisableRange(range); // TODO: fix range display bug 
                //pSlider->setRange(std::numeric_limits<std::float_t>::lowest(), std::numeric_limits<std::float_t>::max(), 1);
                //pSlider->setValue(std::numeric_limits<std::float_t>::min(), juce::dontSendNotification);
                pSlider->setRange(-255.0, 255.0, 0.01);
                pSlider->setValue(0.0, juce::dontSendNotification);
                pSlider->onValueChange = [=]()
                {
                    onChangeFunction();
                };
                ret = pSlider;
            }
            break;
        case NanoOcp1::OCP1DATATYPE_STRING:
            {
                auto pTextEditor = new juce::TextEditor("OcaCustomClass Control");
                pTextEditor->setHasFocusOutline(true);
                pTextEditor->setIndents(pTextEditor->getLeftIndent(), 0); // Hack for JUCE justification bug
                pTextEditor->setJustification(juce::Justification(juce::Justification::centredLeft));
                pTextEditor->setText("Some text");
                pTextEditor->onTextChange = [=]()
                {
                    onChangeFunction();
                };
                ret = pTextEditor;
            }
            break;
        default:
            break;
    }

    jassert(ret); // Missing implementation!
    return ret;
}

std::vector<std::uint8_t> OcaCustomClass::CreateParamDataForComponent(const juce::Component* component, const AES70::Property& prop)
{
    std::vector<std::uint8_t> ret;

    switch (prop.m_type)
    {
        case NanoOcp1::OCP1DATATYPE_BOOLEAN:
            {
                auto pComboBox = static_cast<const juce::ComboBox*>(component);
                std::uint8_t newValue = (pComboBox->getSelectedId() == 1) ? 1 : 0;
                ret = NanoOcp1::DataFromUint8(newValue);
            }
            break;
        case NanoOcp1::OCP1DATATYPE_UINT8:
            {
                auto pSlider = static_cast<const juce::Slider*>(component);
                std::uint8_t newValue = static_cast<std::uint8_t>(pSlider->getValue());
                ret = NanoOcp1::DataFromUint8(newValue);
            }
            break;
        case NanoOcp1::OCP1DATATYPE_UINT16:
            {
                auto pSlider = static_cast<const juce::Slider*>(component);
                std::uint16_t newValue = static_cast<std::uint16_t>(pSlider->getValue());
                ret = NanoOcp1::DataFromUint16(newValue);
            }
            break;
        case NanoOcp1::OCP1DATATYPE_UINT32:
            {
                auto pSlider = static_cast<const juce::Slider*>(component);
                std::uint32_t newValue = static_cast<std::uint32_t>(pSlider->getValue());
                ret = NanoOcp1::DataFromUint32(newValue);
            }
            break;
        case NanoOcp1::OCP1DATATYPE_FLOAT32:
            {
                auto pSlider = static_cast<const juce::Slider*>(component);
                std::float_t newValue = static_cast<std::float_t>(pSlider->getValue());
                ret = NanoOcp1::DataFromFloat(newValue);
            }
            break;
        case NanoOcp1::OCP1DATATYPE_STRING:
            {
                const auto& pTextEditor = static_cast<const juce::TextEditor*>(component);
                ret = NanoOcp1::DataFromString(pTextEditor->getText());
            }
            break;
        default:
            break;
    }

    jassert(ret.size() > 0); // Missing implementation!
    return ret;
}


}
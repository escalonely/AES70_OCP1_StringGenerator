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

#include "MainComponent.h"
#include "AES70.h"


static constexpr int WorkflowStepEnterOno = 1;
static constexpr int WorkflowStepSelectClass = 2;
static constexpr int WorkflowStepSelectProperty = 3;
static constexpr int WorkflowStepSelectCommand = 4;
static constexpr int WorkflowStepEnterSetValue = 5;
static constexpr int WorkflowStepEnterResponseStatus = 6;
static constexpr int WorkflowStepEnterResponseValue = 7;


MainComponent::MainComponent()
{
    m_ocaONoTextEditor = std::make_unique<juce::TextEditor>("OCA ONo");
    m_ocaONoTextEditor->setHasFocusOutline(true);
    m_ocaONoTextEditor->setInputRestrictions(0, "0123456789");
    m_ocaONoTextEditor->setJustification(juce::Justification(juce::Justification::centredRight));
    m_ocaONoTextEditor->setText("10000", juce::dontSendNotification);

    m_ocaClassComboBox = std::make_unique<juce::ComboBox>("OCA Class");
    m_ocaClassComboBox->setHasFocusOutline(true);

    m_ocaPropertyComboBox = std::make_unique<juce::ComboBox>("OCA Property");
    m_ocaPropertyComboBox->setHasFocusOutline(true);

    m_ocaCommandComboBox = std::make_unique<juce::ComboBox>("OCA Command");
    m_ocaCommandComboBox->setHasFocusOutline(true);

    m_ocaCommandLabel = std::make_unique<juce::Label>("OCA Command Label");
    m_ocaCommandLabel->setJustificationType(juce::Justification::centredLeft);

    m_ocaCommandTextEditor = std::make_unique<juce::TextEditor>("OCA Command String");
    m_ocaCommandTextEditor->setHasFocusOutline(true);
    m_ocaCommandTextEditor->setReadOnly(true);
    m_ocaCommandTextEditor->setCaretVisible(false);
    m_ocaCommandTextEditor->setMultiLine(true, true);

    m_ocaResponseLabel = std::make_unique<juce::Label>("OCA Response Label");
    m_ocaResponseLabel->setJustificationType(juce::Justification::centredLeft);

    m_ocaResponseStatusComboBox = std::make_unique<juce::ComboBox>("OCA Response Status");
    m_ocaResponseStatusComboBox->setHasFocusOutline(true);
    m_ocaResponseStatusComboBox->addItem("Ok", 1);
    m_ocaResponseStatusComboBox->addItem("Protocol Version Error", 2);
    m_ocaResponseStatusComboBox->addItem("Device Error", 3); // TODO add remaining statuses

    m_ocaResponseTextEditor = std::make_unique<juce::TextEditor>("OCA Response String");
    m_ocaResponseTextEditor->setHasFocusOutline(true);
    m_ocaResponseTextEditor->setReadOnly(true);
    m_ocaResponseTextEditor->setCaretVisible(false);
    m_ocaResponseTextEditor->setMultiLine(true, true);

    m_ocaClassComboBox->addItem(AES70::MapOfClassNames.at(AES70::OCA_ROOT), AES70::OCA_ROOT);
    m_ocaClassComboBox->addItem(AES70::MapOfClassNames.at(AES70::OCA_WORKER), AES70::OCA_WORKER);
    m_ocaClassComboBox->addItem(AES70::MapOfClassNames.at(AES70::OCA_ACTUATOR), AES70::OCA_ACTUATOR);
    m_ocaClassComboBox->addItem(AES70::MapOfClassNames.at(AES70::OCA_SWITCH), AES70::OCA_SWITCH);
    m_ocaClassComboBox->addItem(AES70::MapOfClassNames.at(AES70::OCA_MUTE), AES70::OCA_MUTE);
    m_ocaClassComboBox->addItem(AES70::MapOfClassNames.at(AES70::OCA_SENSOR), AES70::OCA_SENSOR);
    m_ocaClassComboBox->addItem(AES70::MapOfClassNames.at(AES70::OCA_BASIC_SENSOR), AES70::OCA_BASIC_SENSOR);
    m_ocaClassComboBox->addItem(AES70::MapOfClassNames.at(AES70::OCA_BOOLEAN_SENSOR), AES70::OCA_BOOLEAN_SENSOR);
    m_ocaClassComboBox->addItem(AES70::MapOfClassNames.at(AES70::OCA_AGENT), AES70::OCA_AGENT);

    m_ocaONoTextEditor->onTextChange = [=]()
    {
        UpdateBinaryStrings();
    };

    m_ocaClassComboBox->onChange = [=]()
    {
        ResetComponents(WorkflowStepSelectClass);

        m_ocaObject.reset();
        int classIdx = m_ocaClassComboBox->getSelectedId();
        if (classIdx > 0)
        {
            m_ocaObject = std::unique_ptr<AES70::OcaRoot>(AES70::OcaRoot::Create(classIdx));
            auto propertyList = m_ocaObject->GetProperties();
            for (int pIdx = 0; pIdx < propertyList.size(); pIdx++)
            {
                m_ocaPropertyComboBox->addItem(propertyList.at(pIdx).m_name, pIdx + 1);
            }
            m_ocaPropertyComboBox->setEnabled(true);
        }
    };

    m_ocaPropertyComboBox->onChange = [=]()
    {
        ResetComponents(WorkflowStepSelectProperty);

        int propIdx = m_ocaPropertyComboBox->getSelectedId();
        if (propIdx > 0)
        {
            auto propertyList = m_ocaObject->GetProperties();
            auto& prop = propertyList.at(propIdx - 1);

            if (prop.m_getMethodIdx != 0)
                m_ocaCommandComboBox->addItem("GetValue", prop.m_getMethodIdx);
            if (prop.m_setMethodIdx != 0)
                m_ocaCommandComboBox->addItem("SetValue", prop.m_setMethodIdx);
            m_ocaCommandComboBox->setEnabled(true);
        }
    };

    m_ocaCommandComboBox->onChange = [=]()
    {
        ResetComponents(WorkflowStepSelectCommand);
        CreateValueComponents();
    };

    m_ocaResponseStatusComboBox->onChange = [=]()
    {
        UpdateBinaryStrings();
    };

    addAndMakeVisible(m_ocaONoTextEditor.get());
    addAndMakeVisible(m_ocaClassComboBox.get());
    addAndMakeVisible(m_ocaPropertyComboBox.get());
    addAndMakeVisible(m_ocaCommandComboBox.get());
    addAndMakeVisible(m_ocaCommandLabel.get());
    addAndMakeVisible(m_ocaCommandTextEditor.get());
    addAndMakeVisible(m_ocaResponseLabel.get());
    addAndMakeVisible(m_ocaResponseStatusComboBox.get());
    addAndMakeVisible(m_ocaResponseTextEditor.get());
    setSize(640, 480);

    ResetComponents(WorkflowStepSelectClass);
}

MainComponent::~MainComponent()
{
}

void MainComponent::ResetComponents(int step)
{
    m_ocaCommandTextEditor->clear();
    m_ocaResponseTextEditor->clear();
    if (step < WorkflowStepEnterSetValue)
    {
        // Reset labels to show the default text
        m_ocaCommandLabel->setText("Use the following binary string to execute the selected command.", juce::dontSendNotification);
        m_ocaResponseLabel->setText("The following command can be expected.", juce::dontSendNotification);

        // Reset status back to OK
        m_ocaResponseStatusComboBox->setSelectedId(1, juce::dontSendNotification);

        // Reset value components to just be generic components.
        if (m_ocaSetCommandValueComponent)
        {
            this->removeChildComponent(m_ocaSetCommandValueComponent.get());
            m_ocaSetCommandValueComponent.reset();
        }
        if (m_ocaResponseValueComponent)
        {
            this->removeChildComponent(m_ocaResponseValueComponent.get());
            m_ocaResponseValueComponent.reset();
        }
    }
    if (step < WorkflowStepSelectCommand)
    {
        m_ocaCommandComboBox->clear();
        m_ocaCommandComboBox->setSelectedItemIndex(0, juce::dontSendNotification);
        m_ocaCommandComboBox->setEnabled(false);
        m_ocaResponseStatusComboBox->setEnabled(false);
    }
    if (step < WorkflowStepSelectProperty)
    {
        m_ocaPropertyComboBox->clear();
        m_ocaPropertyComboBox->setSelectedItemIndex(0, juce::dontSendNotification);
        m_ocaPropertyComboBox->setEnabled(false);
    }
    if (step < WorkflowStepSelectClass)
    {
        m_ocaClassComboBox->clear();
        m_ocaClassComboBox->setSelectedItemIndex(0, juce::dontSendNotification);
        m_ocaClassComboBox->setEnabled(false);
    }
}

void MainComponent::CreateValueComponents()
{
    int propIdx = m_ocaPropertyComboBox->getSelectedId();
    int methodIdx = m_ocaCommandComboBox->getSelectedId();
    if ((propIdx <= 0) || (methodIdx <= 0))
        return;

    if (!m_ocaObject)
        return;

    auto propertyList = m_ocaObject->GetProperties();
    if (propertyList.size() < propIdx)
        return;

    auto& prop = propertyList.at(propIdx - 1);
    if ((prop.m_getMethodIdx != 0) && (prop.m_getMethodIdx == methodIdx))
    {
        m_ocaResponseValueComponent = CreateValueComponentForType(prop.m_type);
    }
    else if ((prop.m_setMethodIdx != 0) && (prop.m_setMethodIdx == methodIdx))
    {
        m_ocaSetCommandValueComponent = CreateValueComponentForType(prop.m_type);
    }

    m_ocaResponseStatusComboBox->setEnabled(true);

    resized();
}

void MainComponent::UpdateBinaryStrings()
{
    juce::String commandString;
    juce::String responseString;
    CreateCommandAndResponseStrings(commandString, responseString);
    m_ocaCommandTextEditor->setText(commandString, juce::dontSendNotification);
    m_ocaResponseTextEditor->setText(responseString, juce::dontSendNotification);
}

bool MainComponent::CreateCommandAndResponseStrings(juce::String& commandString, juce::String& responseString)
{
    int propIdx = m_ocaPropertyComboBox->getSelectedId();
    int methodIdx = m_ocaCommandComboBox->getSelectedId();
    if ((propIdx <= 0) || (methodIdx <= 0))
        return false;

    if (!m_ocaObject)
        return false;

    auto propertyList = m_ocaObject->GetProperties();
    if (propertyList.size() < propIdx)
        return false;

    std::uint8_t commandParamCount(0);
    std::uint8_t responseParamCount(0);
    std::vector<std::uint8_t> commandParamData;
    std::vector<std::uint8_t> responseParamData;
    NanoOcp1::Ocp1CommandDefinition commandDefinition;

    auto& prop = propertyList.at(propIdx - 1);
    if ((prop.m_getMethodIdx != 0) && (prop.m_getMethodIdx == methodIdx))
    {
        responseParamCount = 1;
        responseParamData = GetParamDataFromValueComponent(m_ocaResponseValueComponent.get(), prop.m_type);
    }
    else if ((prop.m_setMethodIdx != 0) && (prop.m_setMethodIdx == methodIdx))
    {
        commandParamCount = 1;
        commandParamData = GetParamDataFromValueComponent(m_ocaSetCommandValueComponent.get(), prop.m_type);
    }

    std::uint32_t targetOno = static_cast<std::uint32_t>(m_ocaONoTextEditor->getText().getIntValue());
    std::uint8_t responseStatus = static_cast<std::uint32_t>(m_ocaResponseStatusComboBox->getSelectedId() - 1);

    std::uint32_t handle; // TODO: make handle settable
    commandDefinition = NanoOcp1::Ocp1CommandDefinition(targetOno, prop.m_type, prop.m_defLevel, prop.m_setMethodIdx, commandParamCount, commandParamData);
    auto commandMemBlock = NanoOcp1::Ocp1CommandResponseRequired(commandDefinition, handle).GetMemoryBlock();
    commandString = juce::String::toHexString(commandMemBlock.getData(), commandMemBlock.getSize());

    auto responseMemBlock = NanoOcp1::Ocp1Response(handle, responseStatus, responseParamCount, responseParamData).GetMemoryBlock();
    responseString = juce::String::toHexString(responseMemBlock.getData(), responseMemBlock.getSize());
    return true;
}

std::vector<std::uint8_t> MainComponent::GetParamDataFromValueComponent(juce::Component* component, int dataType)
{
    std::vector<std::uint8_t> paramData;

    switch (dataType)
    {
        case NanoOcp1::OCP1DATATYPE_BOOLEAN:
            {
                auto pComboBox = static_cast<juce::ComboBox*>(component);
                std::uint8_t newValue = (pComboBox->getSelectedId() == 1) ? 1 : 0;
                paramData = NanoOcp1::DataFromUint8(newValue);
            }
            break;
        case NanoOcp1::OCP1DATATYPE_INT8:
        case NanoOcp1::OCP1DATATYPE_INT16:
        case NanoOcp1::OCP1DATATYPE_INT32:
        case NanoOcp1::OCP1DATATYPE_INT64:
        case NanoOcp1::OCP1DATATYPE_UINT8:
        case NanoOcp1::OCP1DATATYPE_UINT16:
        case NanoOcp1::OCP1DATATYPE_UINT32:
        case NanoOcp1::OCP1DATATYPE_UINT64:
            {
                auto pTextEditor = static_cast<juce::TextEditor*>(m_ocaSetCommandValueComponent.get());
                // TODO: paramData
            }
            break;
        case NanoOcp1::OCP1DATATYPE_FLOAT32:
        case NanoOcp1::OCP1DATATYPE_FLOAT64:
            {
                auto pTextEditor = static_cast<juce::TextEditor*>(m_ocaSetCommandValueComponent.get());
                // TODO: paramData
            }
            break;
        case NanoOcp1::OCP1DATATYPE_STRING:
            {
                auto pTextEditor = static_cast<juce::TextEditor*>(m_ocaSetCommandValueComponent.get());
                // TODO: paramData
            }
            break;
        default:
            break;
    }

    return paramData;
}

std::unique_ptr<juce::Component> MainComponent::CreateValueComponentForType(int dataType)
{
    std::unique_ptr<juce::Component> valueComponent;

    switch (dataType)
    {
        case NanoOcp1::OCP1DATATYPE_BOOLEAN:
            {
                valueComponent = std::make_unique<juce::ComboBox>("OCA Set Command Value");
                auto pComboBox = static_cast<juce::ComboBox*>(valueComponent.get());
                pComboBox->addItem("True", 1);
                pComboBox->addItem("False", 2);
                pComboBox->setSelectedId(1, juce::sendNotification);
                pComboBox->onChange = [=]()
                {
                    UpdateBinaryStrings();
                };
            }
            break;
        case NanoOcp1::OCP1DATATYPE_INT8:
        case NanoOcp1::OCP1DATATYPE_INT16:
        case NanoOcp1::OCP1DATATYPE_INT32:
        case NanoOcp1::OCP1DATATYPE_INT64:
        case NanoOcp1::OCP1DATATYPE_UINT8:
        case NanoOcp1::OCP1DATATYPE_UINT16:
        case NanoOcp1::OCP1DATATYPE_UINT32:
        case NanoOcp1::OCP1DATATYPE_UINT64:
            {
                valueComponent = std::make_unique<juce::TextEditor>("OCA Set Command Value");
                auto pTextEditor = static_cast<juce::TextEditor*>(valueComponent.get());
                pTextEditor->setInputRestrictions(0, "0123456789");
                pTextEditor->onTextChange = [=]()
                {
                    UpdateBinaryStrings();
                };
            }
            break;
        case NanoOcp1::OCP1DATATYPE_FLOAT32:
        case NanoOcp1::OCP1DATATYPE_FLOAT64:
            {
                valueComponent = std::make_unique<juce::TextEditor>("OCA Set Command Value");
                auto pTextEditor = static_cast<juce::TextEditor*>(valueComponent.get());
                pTextEditor->setInputRestrictions(0, "0123456789.");
                pTextEditor->onTextChange = [=]()
                {
                    UpdateBinaryStrings();
                };
            }
            break;
        case NanoOcp1::OCP1DATATYPE_STRING:
            {
                valueComponent = std::make_unique<juce::TextEditor>("OCA Set Command Value");
                auto pTextEditor = static_cast<juce::TextEditor*>(valueComponent.get());
                pTextEditor->onTextChange = [=]()
                {
                    UpdateBinaryStrings();
                };
            }
            break;
        default:
            break;
    }

    if (valueComponent)
    {
        valueComponent->setEnabled(true);
        addAndMakeVisible(valueComponent.get());
    }

    return std::move(valueComponent);
}

void MainComponent::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);
    g.drawText ("AES70 OCP.1 binary string generator", getLocalBounds(), juce::Justification::topLeft, true);
}

void MainComponent::resized()
{
    int margin = 2;
    int controlHeight = 40;
    auto bounds = getLocalBounds();
    auto titleBounds = bounds.removeFromTop(controlHeight * 3);
    auto ocaComboBoxesBounds = bounds.removeFromTop(controlHeight);

    int comboBoxWidth = bounds.getWidth() / 5;
    auto ocaONoTextEditorBounds = ocaComboBoxesBounds.removeFromLeft(comboBoxWidth);
    auto ocaClassComboBoxBounds = ocaComboBoxesBounds.removeFromLeft(comboBoxWidth * 2);
    auto ocaPropertyComboBoxBounds = ocaComboBoxesBounds.removeFromLeft(comboBoxWidth);
    auto ocaCommandComboBoxBounds = ocaComboBoxesBounds.removeFromLeft(comboBoxWidth);

    m_ocaONoTextEditor->setBounds(ocaONoTextEditorBounds.reduced(margin));
    m_ocaClassComboBox->setBounds(ocaClassComboBoxBounds.reduced(margin));
    m_ocaPropertyComboBox->setBounds(ocaPropertyComboBoxBounds.reduced(margin));
    m_ocaCommandComboBox->setBounds(ocaCommandComboBoxBounds.reduced(margin));

    bounds.removeFromTop(controlHeight / 2); // Spacer
    auto ocaCommandLabelBounds = bounds.removeFromTop(controlHeight);
    auto ocaSetCommandValueBounds = ocaCommandLabelBounds.removeFromRight(comboBoxWidth);
    m_ocaCommandLabel->setBounds(ocaCommandLabelBounds.reduced(margin));
    if (m_ocaSetCommandValueComponent)
        m_ocaSetCommandValueComponent->setBounds(ocaSetCommandValueBounds.reduced(margin));

    auto ocaCommandBounds = bounds.removeFromTop(controlHeight * 2);
    m_ocaCommandTextEditor->setBounds(ocaCommandBounds.reduced(margin));

    bounds.removeFromTop(controlHeight / 2); // Spacer
    auto ocaResponseLabelBounds = bounds.removeFromTop(controlHeight);
    auto ocaResponseValueBounds = ocaResponseLabelBounds.removeFromRight(comboBoxWidth);
    auto ocaResponseStatusComboBoxBounds = ocaResponseLabelBounds.removeFromRight(comboBoxWidth);
    m_ocaResponseLabel->setBounds(ocaResponseLabelBounds.reduced(margin));
    m_ocaResponseStatusComboBox->setBounds(ocaResponseStatusComboBoxBounds.reduced(margin));
    if (m_ocaResponseValueComponent)
        m_ocaResponseValueComponent->setBounds(ocaResponseValueBounds.reduced(margin));

    auto ocaResponseBounds = bounds.removeFromTop(controlHeight * 2);
    m_ocaResponseTextEditor->setBounds(ocaResponseBounds.reduced(margin));
}

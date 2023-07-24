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

    m_ocaClassComboBox = std::make_unique<juce::ComboBox>("OCA Class");
    m_ocaClassComboBox->setHasFocusOutline(true);

    m_ocaPropertyComboBox = std::make_unique<juce::ComboBox>("OCA Property");
    m_ocaPropertyComboBox->setHasFocusOutline(true);

    m_ocaCommandComboBox = std::make_unique<juce::ComboBox>("OCA Command");
    m_ocaCommandComboBox->setHasFocusOutline(true);

    m_ocaCommandTextEditor = std::make_unique<juce::TextEditor>("OCA Command String");
    m_ocaCommandTextEditor->setHasFocusOutline(true);
    m_ocaCommandTextEditor->setReadOnly(true);
    m_ocaCommandTextEditor->setCaretVisible(false);
    m_ocaCommandTextEditor->setMultiLine(true, true);

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

    m_ocaClassComboBox->onChange = [=]()
    {
        ResetComponents(WorkflowStepSelectClass);

        m_ocaObject.reset();
        int classIdx = m_ocaClassComboBox->getSelectedId();
        if (classIdx > 0)
        {
            m_ocaObject = std::unique_ptr<AES70::OcaRoot>(AES70::OcaRoot::Create(classIdx));
            auto properties = m_ocaObject->GetProperties();
            for (int pIdx = 0; pIdx < properties.size(); pIdx++)
            {
                m_ocaPropertyComboBox->addItem(properties.at(pIdx).m_name, pIdx + 1);
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
            auto properties = m_ocaObject->GetProperties();
            auto& prop = properties.at(propIdx - 1);

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

        int propIdx = m_ocaPropertyComboBox->getSelectedId();
        int methodIdx = m_ocaCommandComboBox->getSelectedId();
        if ((propIdx > 0) && (methodIdx > 0))
        {
            auto properties = m_ocaObject->GetProperties();
            auto& prop = properties.at(propIdx - 1);

            //NanoOcp1::Ocp1CommandDefinition def(123 /* TODO: ONO */, prop.m_type, prop.m_defLevel, prop.m_index);

            m_ocaCommandTextEditor->setText("Temporary placeholder command content.", false);
            m_ocaResponseTextEditor->setText("Temporary placeholder response content.", false);
        }
    };

    addAndMakeVisible(m_ocaONoTextEditor.get());
    addAndMakeVisible(m_ocaClassComboBox.get());
    addAndMakeVisible(m_ocaPropertyComboBox.get());
    addAndMakeVisible(m_ocaCommandComboBox.get());
    addAndMakeVisible(m_ocaCommandTextEditor.get());
    addAndMakeVisible(m_ocaResponseTextEditor.get());
    setSize (600, 400);
}

MainComponent::~MainComponent()
{
}

void MainComponent::ResetComponents(int step)
{
    m_ocaCommandTextEditor->clear();
    m_ocaResponseTextEditor->clear();
    if (step < WorkflowStepSelectCommand)
    {
        m_ocaCommandComboBox->clear();
        m_ocaCommandComboBox->setSelectedItemIndex(0, juce::dontSendNotification);
        m_ocaCommandComboBox->setEnabled(false);
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
    auto titleBounds = bounds.removeFromTop(controlHeight);
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

    auto ocaCommandBounds = bounds.removeFromTop(controlHeight * 2);
    auto ocaResponseBounds = bounds.removeFromTop(controlHeight * 2);
    m_ocaCommandTextEditor->setBounds(ocaCommandBounds.reduced(margin));
    m_ocaResponseTextEditor->setBounds(ocaResponseBounds.reduced(margin));
}

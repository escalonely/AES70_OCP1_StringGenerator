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

/**
 * Command index used for AddSubscription.
 */
static constexpr int MethodIndexForAddSubscription = 100;

/**
 * Default height (in pixels) of m_container, the container component within the juce::Viewport.
 */
static constexpr int ContainerComponentDefaultHeight = 480;

/**
 * Expected user workflow, separated in discrete steps.
 */
enum WorkflowSteps
{
    WORKFLOW_STEP_ENTER_ONO = 1,
    WORKFLOW_STEP_SELECT_CLASS,
    WORKFLOW_STEP_SELECT_PROPERTY,
    WORKFLOW_STEP_SELECT_COMMAND,
    WORKFLOW_STEP_ENTER_SET_VALUE,
    WORKFLOW_STEP_ENTER_RESPONSE_STATUS,
    WORKFLOW_STEP_ENTER_RESPONSE_VALUE,
};

/**
 * Indeces of juce::Label components used on the GUI.
 */
enum GuiLabels
{
    LABELIDX_TITLE = 0,
    LABELIDX_ONO,
    LABELIDX_CLASS,
    LABELIDX_PROP,
    LABELIDX_COMMAND,
    LABELIDX_CMD_STRING,
    LABELIDX_CMD_VALUE,
    LABELIDX_RESP_STRING,
    LABELIDX_RESP_STATUS,
    LABELIDX_RESP_VALUE,
    LABELIDX_NOTIF_STRING,
    LABELIDX_NOTIF_VALUE,
    LABELIDX_MAX,
};

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

    m_ocaCommandTextEditor = std::make_unique<juce::TextEditor>("OCA Command String");
    m_ocaCommandTextEditor->setHasFocusOutline(true);
    m_ocaCommandTextEditor->setReadOnly(true);
    m_ocaCommandTextEditor->setCaretVisible(false);
    m_ocaCommandTextEditor->setMultiLine(true, true);

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

    for (int labelIdx = 0; labelIdx < LABELIDX_MAX; labelIdx++)
    {
        m_ocaLabels.emplace_back(std::make_unique<juce::Label>("OCA Label " + juce::String(labelIdx)));
        m_ocaLabels.back()->setJustificationType(juce::Justification::bottomLeft);
        m_container.addAndMakeVisible(m_ocaLabels.back().get());
    }

    for (int classIdx = AES70::OCA_ROOT; classIdx < AES70::OCA_MAX_CLASS_IDX; classIdx++)
    {
        m_ocaClassComboBox->addItem(AES70::MapOfClassNames.at(classIdx), classIdx);
    }

    m_ocaONoTextEditor->onTextChange = [=]()
    {
        UpdateBinaryStrings();
    };

    m_ocaClassComboBox->onChange = [=]()
    {
        ResetComponents(WORKFLOW_STEP_SELECT_CLASS);

        m_ocaObject.reset();
        int classIdx = m_ocaClassComboBox->getSelectedId();
        if (classIdx > 0)
        {
            m_ocaObject = std::unique_ptr<AES70::OcaRoot>(AES70::OcaRoot::Create(classIdx));
            auto propertyList = m_ocaObject->GetProperties();
            for (int propIdx = 0; propIdx < propertyList.size(); propIdx++)
            {
                juce::String propName = juce::String(propertyList.at(propIdx).m_defLevel) + "," +
                                        juce::String(propertyList.at(propIdx).m_index) + ": " +
                                        propertyList.at(propIdx).m_name;
                m_ocaPropertyComboBox->addItem(propName, propIdx + 1);
            }
            m_ocaPropertyComboBox->setEnabled(true);
        }
    };

    m_ocaPropertyComboBox->onChange = [=]()
    {
        ResetComponents(WORKFLOW_STEP_SELECT_PROPERTY);

        int propIdx = m_ocaPropertyComboBox->getSelectedId();
        if (propIdx > 0)
        {
            auto propertyList = m_ocaObject->GetProperties();
            auto& prop = propertyList.at(propIdx - 1);
            //bool enable(false);

            if (prop.m_getMethodIdx != 0)
            {
                juce::String cmdName = juce::String(prop.m_getMethodIdx) + ": GetValue";
                m_ocaCommandComboBox->addItem(cmdName, prop.m_getMethodIdx);
                //enable = true;
            }
            if (prop.m_setMethodIdx != 0)
            {
                juce::String cmdName = juce::String(prop.m_setMethodIdx) + ": SetValue";
                m_ocaCommandComboBox->addItem(cmdName, prop.m_setMethodIdx);
                //enable = true;
            }

            // In addition to the property-related commands, offer the AddSubscription command.
            m_ocaCommandComboBox->addSeparator();
            m_ocaCommandComboBox->addItem("AddSubscription", MethodIndexForAddSubscription);
            m_ocaCommandComboBox->setEnabled(true);
        }
    };

    m_ocaCommandComboBox->onChange = [=]()
    {
        ResetComponents(WORKFLOW_STEP_SELECT_COMMAND);

        // Special handling for the AddSubscription command
        if (m_ocaCommandComboBox->getSelectedId() == MethodIndexForAddSubscription)
        {
            m_ocaLabels.at(LABELIDX_NOTIF_STRING)->setText("The following Notification can be expected.", juce::dontSendNotification);

            m_ocaNotificationTextEditor = std::make_unique<juce::TextEditor>("OCA Notification String");
            m_ocaNotificationTextEditor->setHasFocusOutline(true);
            m_ocaNotificationTextEditor->setReadOnly(true);
            m_ocaNotificationTextEditor->setCaretVisible(false);
            m_ocaNotificationTextEditor->setMultiLine(true, true);
            m_container.addAndMakeVisible(m_ocaNotificationTextEditor.get());
        }

        CreateValueComponents();
        UpdateBinaryStrings();
    };

    m_ocaResponseStatusComboBox->onChange = [=]()
    {
        UpdateBinaryStrings();
    };

    m_container.addAndMakeVisible(m_ocaONoTextEditor.get());
    m_container.addAndMakeVisible(m_ocaClassComboBox.get());
    m_container.addAndMakeVisible(m_ocaPropertyComboBox.get());
    m_container.addAndMakeVisible(m_ocaCommandComboBox.get());
    m_container.addAndMakeVisible(m_ocaCommandTextEditor.get());
    m_container.addAndMakeVisible(m_ocaResponseStatusComboBox.get());
    m_container.addAndMakeVisible(m_ocaResponseTextEditor.get());
    setSize(640, 480);
    setViewedComponent(&m_container, false);

    ResetComponents(WORKFLOW_STEP_SELECT_CLASS);
}

MainComponent::~MainComponent()
{
}

void MainComponent::ResetComponents(int step)
{
    m_ocaCommandTextEditor->clear();
    m_ocaResponseTextEditor->clear();
    if (step < WORKFLOW_STEP_ENTER_SET_VALUE)
    {
        // Reset labels to show the default text
        m_ocaLabels.at(LABELIDX_ONO)->setText("ONo:", juce::dontSendNotification);
        m_ocaLabels.at(LABELIDX_CLASS)->setText("Class:", juce::dontSendNotification);
        m_ocaLabels.at(LABELIDX_PROP)->setText("Property:", juce::dontSendNotification);
        m_ocaLabels.at(LABELIDX_COMMAND)->setText("Command:", juce::dontSendNotification);
        m_ocaLabels.at(LABELIDX_CMD_STRING)->setText("Use the following binary string to execute the selected Command.", juce::dontSendNotification);
        m_ocaLabels.at(LABELIDX_RESP_STRING)->setText("The following Response can be expected.", juce::dontSendNotification);
        m_ocaLabels.at(LABELIDX_NOTIF_STRING)->setText("", juce::dontSendNotification);

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
        if (m_ocaNotificationValueComponent)
        {
            this->removeChildComponent(m_ocaNotificationValueComponent.get());
            m_ocaNotificationValueComponent.reset();
        }

        // Remove the TextEditor for displaying Notification messages.
        if (m_ocaNotificationTextEditor)
        {
            this->removeChildComponent(m_ocaNotificationTextEditor.get());
            m_ocaNotificationTextEditor.reset();
        }
    }
    if (step < WORKFLOW_STEP_SELECT_COMMAND)
    {
        m_ocaCommandComboBox->clear();
        m_ocaCommandComboBox->setSelectedItemIndex(0, juce::dontSendNotification);
        m_ocaCommandComboBox->setEnabled(false);
        m_ocaResponseStatusComboBox->setEnabled(false);
    }
    if (step < WORKFLOW_STEP_SELECT_PROPERTY)
    {
        m_ocaPropertyComboBox->clear();
        m_ocaPropertyComboBox->setSelectedItemIndex(0, juce::dontSendNotification);
        m_ocaPropertyComboBox->setEnabled(false);
    }
    if (step < WORKFLOW_STEP_SELECT_CLASS)
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

    // Create a component appropriate for displaying this properties value.
    auto& prop = propertyList.at(propIdx - 1);
    auto pComponent = m_ocaObject->CreateComponentForProperty(prop, 
        [this] {
            UpdateBinaryStrings();
        });
    jassert(pComponent); // Missing implementation!
    m_container.addAndMakeVisible(pComponent);

    // Depending on whether the Get, Set, or AddSubscription commands are selected,
    // assign this new component to the correct member variable.
    if (prop.m_getMethodIdx == methodIdx)
    {
        m_ocaResponseValueComponent = std::unique_ptr<juce::Component>(pComponent);
    }
    else if (prop.m_setMethodIdx == methodIdx)
    {
        m_ocaSetCommandValueComponent = std::unique_ptr<juce::Component>(pComponent);
    }
    else if (methodIdx == MethodIndexForAddSubscription)
    {
        m_ocaNotificationValueComponent = std::unique_ptr<juce::Component>(pComponent);
    }

    m_ocaResponseStatusComboBox->setEnabled(true);

    resized();
}

void MainComponent::UpdateBinaryStrings()
{
    DBG("UpdateBinaryStrings");
    juce::String commandString;
    juce::String responseString;
    juce::String notificationString;
    CreateBinaryStrings(commandString, responseString, notificationString);
    m_ocaCommandTextEditor->setText(commandString, juce::dontSendNotification);
    m_ocaResponseTextEditor->setText(responseString, juce::dontSendNotification);
    if (m_ocaNotificationTextEditor)
        m_ocaNotificationTextEditor->setText(notificationString, juce::dontSendNotification);
}

bool MainComponent::CreateBinaryStrings(juce::String& commandString, juce::String& responseString, juce::String& notificationString)
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

    std::uint32_t targetOno = static_cast<std::uint32_t>(m_ocaONoTextEditor->getText().getIntValue());
    std::uint8_t responseStatus = static_cast<std::uint8_t>(m_ocaResponseStatusComboBox->getSelectedId() - 1);

    std::uint8_t responseParamCount(0);
    std::vector<std::uint8_t> responseParamData;
    NanoOcp1::Ocp1CommandDefinition commandDefinition;
    notificationString.clear();

    auto& prop = propertyList.at(propIdx - 1);
    if (prop.m_getMethodIdx == methodIdx)
    {
        responseParamCount = 1;
        responseParamData = m_ocaObject->CreateParamDataForComponent(m_ocaResponseValueComponent.get(), prop);
        jassert(responseParamData.size() > 0);

        commandDefinition = NanoOcp1::Ocp1CommandDefinition(targetOno, 
                                                            static_cast<std::uint16_t>(prop.m_type),
                                                            static_cast<std::uint16_t>(prop.m_defLevel),
                                                            static_cast<std::uint16_t>(prop.m_getMethodIdx));
    }
    else if (prop.m_setMethodIdx == methodIdx)
    {
        auto commandParamData = m_ocaObject->CreateParamDataForComponent(m_ocaSetCommandValueComponent.get(), prop);
        jassert(commandParamData.size() > 0);

        commandDefinition = NanoOcp1::Ocp1CommandDefinition(targetOno, 
                                                            static_cast<std::uint16_t>(prop.m_type), 
                                                            static_cast<std::uint16_t>(prop.m_defLevel), 
                                                            static_cast<std::uint16_t>(prop.m_setMethodIdx), 
                                                            1, /* paramCount */ 
                                                            commandParamData);
    }
    else if (methodIdx == MethodIndexForAddSubscription)
    {
        commandDefinition = NanoOcp1::Ocp1CommandDefinition(targetOno,
                                                            static_cast<std::uint16_t>(prop.m_type),
                                                            static_cast<std::uint16_t>(prop.m_defLevel),
                                                            static_cast<std::uint16_t>(prop.m_index)).AddSubscriptionCommand();

        auto notificationParamData = m_ocaObject->CreateParamDataForComponent(m_ocaNotificationValueComponent.get(), prop);
        auto notificationMemBlock = NanoOcp1::Ocp1Notification(targetOno, 
                                                               static_cast<std::uint16_t>(prop.m_defLevel),
                                                               static_cast<std::uint16_t>(prop.m_index),
                                                               1 /* paramCount */, 
                                                               notificationParamData).GetMemoryBlock();
        notificationString = juce::String::toHexString(notificationMemBlock.getData(), static_cast<int>(notificationMemBlock.getSize()));
    }

    std::uint32_t handle; // TODO: make handle settable    
    auto commandMemBlock = NanoOcp1::Ocp1CommandResponseRequired(commandDefinition, handle).GetMemoryBlock();
    commandString = juce::String::toHexString(commandMemBlock.getData(), static_cast<int>(commandMemBlock.getSize()));

    auto responseMemBlock = NanoOcp1::Ocp1Response(handle, responseStatus, responseParamCount, responseParamData).GetMemoryBlock();
    responseString = juce::String::toHexString(responseMemBlock.getData(), static_cast<int>(responseMemBlock.getSize()));

    return true;
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
    // The height of the container component within the Viewport depends on how much 
    // content needs to be displayed. So m_container grows as more components are required. 
    int containerHeight = ContainerComponentDefaultHeight;
    if (m_ocaNotificationTextEditor)
        containerHeight += 140;

    // Remove a margin to account for the vertical scrollbar.
    m_container.setBounds(0, 0, getLocalBounds().getWidth() - 8, containerHeight);
    auto bounds = m_container.getLocalBounds();

    int margin = 2;
    int controlHeight = 40;
    int comboBoxWidth = bounds.getWidth() / 5;
    auto titleBounds = bounds.removeFromTop(controlHeight * 2);

    auto firstLabelRowBounds = bounds.removeFromTop(controlHeight);
    auto ocaONoLabelBounds = firstLabelRowBounds.removeFromLeft(comboBoxWidth);
    auto ocaClassLabelBounds = firstLabelRowBounds.removeFromLeft(comboBoxWidth * 2);
    auto ocaPropertyLabelBounds = firstLabelRowBounds.removeFromLeft(comboBoxWidth);
    auto ocaCommandLabelBounds = firstLabelRowBounds.removeFromLeft(comboBoxWidth);

    m_ocaLabels.at(LABELIDX_ONO)->setBounds(ocaONoLabelBounds.reduced(margin));
    m_ocaLabels.at(LABELIDX_CLASS)->setBounds(ocaClassLabelBounds.reduced(margin));
    m_ocaLabels.at(LABELIDX_PROP)->setBounds(ocaPropertyLabelBounds.reduced(margin));
    m_ocaLabels.at(LABELIDX_COMMAND)->setBounds(ocaCommandLabelBounds.reduced(margin));

    auto ocaComboBoxesBounds = bounds.removeFromTop(controlHeight);
    auto ocaONoTextEditorBounds = ocaComboBoxesBounds.removeFromLeft(comboBoxWidth);
    auto ocaClassComboBoxBounds = ocaComboBoxesBounds.removeFromLeft(comboBoxWidth * 2);
    auto ocaPropertyComboBoxBounds = ocaComboBoxesBounds.removeFromLeft(comboBoxWidth);
    auto ocaCommandComboBoxBounds = ocaComboBoxesBounds.removeFromLeft(comboBoxWidth);

    m_ocaONoTextEditor->setBounds(ocaONoTextEditorBounds.reduced(margin));
    m_ocaClassComboBox->setBounds(ocaClassComboBoxBounds.reduced(margin));
    m_ocaPropertyComboBox->setBounds(ocaPropertyComboBoxBounds.reduced(margin));
    m_ocaCommandComboBox->setBounds(ocaCommandComboBoxBounds.reduced(margin));

    bounds.removeFromTop(controlHeight / 2); // Spacer
    auto ocaCommandStringLabelBounds = bounds.removeFromTop(controlHeight);
    auto ocaSetCommandValueBounds = ocaCommandStringLabelBounds.removeFromRight(comboBoxWidth);
    m_ocaLabels.at(LABELIDX_CMD_STRING)->setBounds(ocaCommandStringLabelBounds.reduced(margin));
    if (m_ocaSetCommandValueComponent)
        m_ocaSetCommandValueComponent->setBounds(ocaSetCommandValueBounds.reduced(margin));

    auto ocaCommandBounds = bounds.removeFromTop(controlHeight * 2);
    m_ocaCommandTextEditor->setBounds(ocaCommandBounds.reduced(margin));

    bounds.removeFromTop(controlHeight / 2); // Spacer
    auto ocaResponseLabelBounds = bounds.removeFromTop(controlHeight);
    auto ocaResponseValueBounds = ocaResponseLabelBounds.removeFromRight(comboBoxWidth);
    auto ocaResponseStatusComboBoxBounds = ocaResponseLabelBounds.removeFromRight(comboBoxWidth);
    m_ocaLabels.at(LABELIDX_RESP_STRING)->setBounds(ocaResponseLabelBounds.reduced(margin));
    m_ocaResponseStatusComboBox->setBounds(ocaResponseStatusComboBoxBounds.reduced(margin));
    if (m_ocaResponseValueComponent)
        m_ocaResponseValueComponent->setBounds(ocaResponseValueBounds.reduced(margin));

    auto ocaResponseBounds = bounds.removeFromTop(controlHeight * 2);
    m_ocaResponseTextEditor->setBounds(ocaResponseBounds.reduced(margin));

    // Resize components which only exist if the AddSubscription command is selected.
    if (m_ocaNotificationTextEditor)
    {
        bounds.removeFromTop(controlHeight / 2); // Spacer
        auto ocaNotificationLabelBounds = bounds.removeFromTop(controlHeight);
        auto ocaNotificationValueBounds = ocaNotificationLabelBounds.removeFromRight(comboBoxWidth);
        m_ocaLabels.at(LABELIDX_NOTIF_STRING)->setBounds(ocaNotificationLabelBounds.reduced(margin));

        jassert(m_ocaNotificationValueComponent); // Expected since m_ocaNotificationTextEditor exists.
        m_ocaNotificationValueComponent->setBounds(ocaNotificationValueBounds.reduced(margin));

        auto ocaNotificationBounds = bounds.removeFromTop(controlHeight * 2);
        m_ocaNotificationTextEditor->setBounds(ocaNotificationBounds.reduced(margin));
    }

    // Call base class implementation which takes care of updating scrollbars etc.
    return juce::Viewport::resized();
}

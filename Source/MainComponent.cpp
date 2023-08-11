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
static constexpr int MethodIndexForAddSubscription = 256;

/**
 * Index used for custom (aka proprietary, non-standard) AES70 classes.
 */
static constexpr int ClassIndexForCustomClass = 256;

/**
 * Default height (in pixels) of m_container, the container component within the juce::Viewport.
 */
static constexpr int ContainerComponentDefaultHeight = 480;

static const juce::Colour AppBackgroundColour(43, 65, 77);          // App window background color
static const juce::Colour LabelEnabledTextColour(125, 182, 212);    // Color for labels of relevant controls
static const juce::Colour LabelDisabledTextColour(12, 12, 12);      // Color for labels of non-relevant controls

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
    LABELIDX_CLASS,
    LABELIDX_ONO,
    LABELIDX_PROP,          
    LABELIDX_PROP_DEFLEVEL, 
    LABELIDX_PROP_PARAMTYPE,
    LABELIDX_CMD,
    LABELIDX_CMD_DEFLEVEL,
    LABELIDX_CMD_HANDLE,
    LABELIDX_CMD_SET_VALUE,
    LABELIDX_RESP_STATUS,
    LABELIDX_RESP_VALUE,
    LABELIDX_NOTIF_VALUE,
    LABELIDX_MAX,
};

/**
 * Text content for all juce::Label components used on the GUI.
 */
static const std::vector<juce::String> GuiLabelsText = {
    /* LABELIDX_TITLE              */ "AES70 OCP.1 binary string generator",
    /* LABELIDX_CLASS,             */ "Class:",
    /* LABELIDX_ONO,               */ "ONo:",
    /* LABELIDX_PROP,              */ "Property:",
    /* LABELIDX_PROP_DEFLEVEL,     */ "Prop Def Level:",
    /* LABELIDX_PROP_PARAMTYPE,    */ "Prop Type:",
    /* LABELIDX_CMD,               */ "Command:",
    /* LABELIDX_CMD_DEFLEVEL,      */ "Cmd Def Level:",
    /* LABELIDX_CMD_HANDLE,        */ "Cmd Handle:",
    /* LABELIDX_CMD_SET_VALUE,     */ "Cmd Set Value:",
    /* LABELIDX_RESP_STATUS,       */ "Response Status:",
    /* LABELIDX_RESP_VALUE,        */ "Response Value:",
    /* LABELIDX_NOTIF_VALUE,       */ "Notif Value:",
    /* LABELIDX_MAX,               */ "MAX"
};

MainComponent::MainComponent()
    :   m_ocaONoTextEditor(juce::TextEditor("OCA ONo")), 
        m_ocaClassComboBox(juce::ComboBox("OCA Class")),
        m_ocaPropertyComboBox(juce::ComboBox("OCA Property Idx")),
        m_ocaPropertyDefLevelComboBox(juce::ComboBox("OCA Property DefLevel")),
        m_ocaPropertyParamTypeComboBox(juce::ComboBox("OCA Property ParamType")),
        m_ocaCommandComboBox(juce::ComboBox("OCA Command Idx")),
        m_ocaCommandDefLevelComboBox(juce::ComboBox("OCA Command DefLevel")),
        m_ocaCommandHandleTextEditor(juce::TextEditor("OCA Command Handle")),
        m_ocaCommandTextEditor(juce::TextEditor("OCA Command String")),
        m_ocaResponseStatusComboBox(juce::ComboBox("OCA Response Status")),
        m_ocaResponseTextEditor(juce::TextEditor("OCA Response String")),
        m_ocaNotificationTextEditor(juce::TextEditor("OCA Notification String"))
{
    m_container.addAndMakeVisible(&m_ocaONoTextEditor);
    m_container.addAndMakeVisible(&m_ocaClassComboBox);
    m_container.addAndMakeVisible(&m_ocaPropertyComboBox);
    m_container.addAndMakeVisible(&m_ocaPropertyDefLevelComboBox);
    m_container.addAndMakeVisible(&m_ocaPropertyParamTypeComboBox);
    m_container.addAndMakeVisible(&m_ocaCommandComboBox);
    m_container.addAndMakeVisible(&m_ocaCommandDefLevelComboBox);
    m_container.addAndMakeVisible(&m_ocaCommandHandleTextEditor);
    m_container.addAndMakeVisible(&m_ocaCommandTextEditor);
    m_container.addAndMakeVisible(&m_ocaResponseStatusComboBox);
    m_container.addAndMakeVisible(&m_ocaResponseTextEditor);
    m_container.addChildComponent(&m_ocaNotificationTextEditor); // Invisible until AddSubscription Cmd selected

    m_ocaONoTextEditor.setHasFocusOutline(true);
    m_ocaONoTextEditor.setInputRestrictions(0, "0123456789");
    m_ocaONoTextEditor.setJustification(juce::Justification(juce::Justification::centredRight));
    m_ocaONoTextEditor.setText("10000", juce::dontSendNotification);

    m_ocaClassComboBox.setHasFocusOutline(true);
    m_ocaPropertyComboBox.setHasFocusOutline(true);
    m_ocaPropertyDefLevelComboBox.setHasFocusOutline(true);
    m_ocaPropertyParamTypeComboBox.setHasFocusOutline(true);
    m_ocaCommandComboBox.setHasFocusOutline(true);
    m_ocaCommandDefLevelComboBox.setHasFocusOutline(true);

    m_ocaCommandHandleTextEditor.setHasFocusOutline(true);
    m_ocaCommandHandleTextEditor.setInputRestrictions(0, "0123456789");
    m_ocaCommandHandleTextEditor.setJustification(juce::Justification(juce::Justification::centredRight));
    m_ocaCommandHandleTextEditor.setText("1", juce::dontSendNotification);
    
    m_ocaCommandTextEditor.setHasFocusOutline(true);
    m_ocaCommandTextEditor.setReadOnly(true);
    m_ocaCommandTextEditor.setCaretVisible(false);
    m_ocaCommandTextEditor.setMultiLine(true, true);
    m_ocaCommandTextEditor.setTextToShowWhenEmpty("Use the following binary string to execute the selected Command.", LabelEnabledTextColour);

    m_ocaResponseStatusComboBox.setHasFocusOutline(true);
    m_ocaResponseStatusComboBox.addItem("0: Ok", 1);
    m_ocaResponseStatusComboBox.addItem("1: Protocol Version Error", 2);
    m_ocaResponseStatusComboBox.addItem("2: Device Error", 3); // TODO add remaining statuses

    m_ocaResponseTextEditor.setHasFocusOutline(true);
    m_ocaResponseTextEditor.setReadOnly(true);
    m_ocaResponseTextEditor.setCaretVisible(false);
    m_ocaResponseTextEditor.setMultiLine(true, true);
    m_ocaResponseTextEditor.setTextToShowWhenEmpty("The following Response can be expected.", LabelEnabledTextColour);

    m_ocaNotificationTextEditor.setHasFocusOutline(true);
    m_ocaNotificationTextEditor.setReadOnly(true);
    m_ocaNotificationTextEditor.setCaretVisible(false);
    m_ocaNotificationTextEditor.setMultiLine(true, true);
    m_ocaNotificationTextEditor.setVisible(false); // Invisible until AddSubscription Cmd selected
    m_ocaNotificationTextEditor.setTextToShowWhenEmpty("The following Notification can be expected, ...TODO", LabelEnabledTextColour);

    // Create and add all labels on the GUI
    for (int labelIdx = 0; labelIdx < LABELIDX_MAX; labelIdx++)
    {
        m_ocaLabels.emplace_back(std::make_unique<juce::Label>("OCA Label " + juce::String(labelIdx)));
        if (labelIdx == LABELIDX_TITLE)
            m_ocaLabels.back()->setJustificationType(juce::Justification::centredLeft);
        else
            m_ocaLabels.back()->setJustificationType(juce::Justification::centredRight);

        if (labelIdx < LABELIDX_ONO)
            m_ocaLabels.at(labelIdx)->setColour(juce::Label::textColourId, LabelEnabledTextColour);

        m_ocaLabels.at(labelIdx)->setText(GuiLabelsText.at(labelIdx), juce::dontSendNotification);
        m_container.addAndMakeVisible(m_ocaLabels.back().get());
    }

    for (int classIdx = AES70::OCA_ROOT; classIdx < AES70::OCA_MAX_CLASS_IDX; classIdx++)
    {
        m_ocaClassComboBox.addItem(AES70::MapOfClassNames.at(classIdx), classIdx);
    }
    m_ocaClassComboBox.addSeparator();
    m_ocaClassComboBox.addItem("Custom", ClassIndexForCustomClass);

    m_ocaONoTextEditor.onTextChange = [=]()
    {
        UpdateBinaryStrings();
    };

    m_ocaClassComboBox.onChange = [=]()
    {
        ResetComponents(WORKFLOW_STEP_SELECT_CLASS);

        m_ocaObject.reset();
        int classIdx = m_ocaClassComboBox.getSelectedId();
        if (classIdx == ClassIndexForCustomClass)
        {
            m_ocaObject = std::unique_ptr<AES70::OcaRoot>(AES70::OcaRoot::CreateCustom());

            // Misuse the m_ocaPropertyComboBox for the selection of the PropertyIdx.
            for (int propIdx = 1; propIdx < 16; propIdx++)
            {
                juce::String propName = "Property " + juce::String(propIdx);
                m_ocaPropertyComboBox.addItem(propName, propIdx);
            }
            m_ocaPropertyComboBox.setEnabled(true);

            // Enable labels
            m_ocaLabels.at(LABELIDX_ONO)->setColour(juce::Label::textColourId, LabelEnabledTextColour);
            m_ocaLabels.at(LABELIDX_PROP)->setColour(juce::Label::textColourId, LabelEnabledTextColour);
            m_ocaLabels.at(LABELIDX_PROP_DEFLEVEL)->setColour(juce::Label::textColourId, LabelEnabledTextColour);
            m_ocaLabels.at(LABELIDX_PROP_PARAMTYPE)->setColour(juce::Label::textColourId, LabelEnabledTextColour);
        }
        else if (classIdx > 0)
        {
            m_ocaObject = std::unique_ptr<AES70::OcaRoot>(AES70::OcaRoot::Create(classIdx));
            auto propertyList = m_ocaObject->GetProperties();
            for (int propIdx = 0; propIdx < propertyList.size(); propIdx++)
            {
                juce::String propName = juce::String(propertyList.at(propIdx).m_defLevel) + "," +
                                        juce::String(propertyList.at(propIdx).m_index) + ": " +
                                        propertyList.at(propIdx).m_name;
                m_ocaPropertyComboBox.addItem(propName, propIdx + 1);
            }
            m_ocaPropertyComboBox.setEnabled(true);

            // Enable labels
            m_ocaLabels.at(LABELIDX_ONO)->setColour(juce::Label::textColourId, LabelEnabledTextColour);
            m_ocaLabels.at(LABELIDX_PROP)->setColour(juce::Label::textColourId, LabelEnabledTextColour);
        }
    };

    m_ocaPropertyComboBox.onChange = [=]()
    {
        ResetComponents(WORKFLOW_STEP_SELECT_PROPERTY);

        int propIdx = m_ocaPropertyComboBox.getSelectedId();
        if (propIdx > 0)
        {
            m_ocaLabels.at(LABELIDX_CMD)->setColour(juce::Label::textColourId, LabelEnabledTextColour);

            // Usual case: a standard AES70 class (not proprietary) was selected.
            if (m_ocaClassComboBox.getSelectedId() != ClassIndexForCustomClass)
            {
                const auto& propertyList = m_ocaObject->GetProperties();
                const auto& prop = propertyList.at(propIdx - 1);

                // Update m_ocaCommandComboBox to contain the standard Get and Set methods, 
                // which "most" AES70 classes have at indexes 1 and 2, resp.
                if (prop.m_getMethodIdx != 0)
                {
                    m_ocaCommandComboBox.addItem(juce::String(prop.m_getMethodIdx) + ": GetValue", 
                                                 prop.m_getMethodIdx);
                }
                if (prop.m_setMethodIdx != 0)
                {
                    m_ocaCommandComboBox.addItem(juce::String(prop.m_setMethodIdx) + ": SetValue",
                                                 prop.m_setMethodIdx);
                }

                // Update m_ocaPropertyDefLevelComboBox to display the definiton level  
                // of the selected Property (Read-only!)
                m_ocaPropertyDefLevelComboBox.addItem(juce::String(prop.m_defLevel), prop.m_defLevel);
                m_ocaPropertyDefLevelComboBox.setSelectedId(prop.m_defLevel, juce::dontSendNotification);

                // Update m_ocaPropertyParamTypeComboBox to display the data type
                // of the selected Property (Read-only!)
                m_ocaPropertyParamTypeComboBox.addItem("uint8", 1); // TODO: get type from prop.
                m_ocaPropertyParamTypeComboBox.setSelectedId(1, juce::dontSendNotification);
            }

            // Custom AES70 class is selected.
            // NOTE: propertyList will be empty.
            else
            {
                // Fill m_ocaCommandComboBox with dummy command indeces.
                for (int commandIdx = 1; commandIdx < 16; commandIdx++)
                {
                    m_ocaCommandComboBox.addItem("Command " + juce::String(commandIdx), 
                                                 commandIdx);
                }

                // Fill m_ocaPropertyDefLevelComboBox with dummy definition levels
                // and then enable it to allow user changes.
                for (int propDefLevel = 1; propDefLevel < 16; propDefLevel++)
                {
                    m_ocaPropertyDefLevelComboBox.addItem(juce::String(propDefLevel), propDefLevel);
                }
                m_ocaPropertyDefLevelComboBox.setEnabled(true);
                m_ocaLabels.at(LABELIDX_PROP_DEFLEVEL)->setColour(juce::Label::textColourId, LabelEnabledTextColour);

                // Add available data types to m_ocaPropertyParamTypeComboBox and allow user changes.
                for (int dataTypeIdx = 1; dataTypeIdx < 16; dataTypeIdx++) // TODO: iterate all data types
                {
                    juce::String dataTypeName = "uint8 " + juce::String(dataTypeIdx);
                    m_ocaPropertyParamTypeComboBox.addItem(dataTypeName, dataTypeIdx);
                }
                m_ocaPropertyParamTypeComboBox.setEnabled(true);
                m_ocaLabels.at(LABELIDX_PROP_PARAMTYPE)->setColour(juce::Label::textColourId, LabelEnabledTextColour);
            }

            // In addition to the property-related commands, always offer the AddSubscription command.
            m_ocaCommandComboBox.addSeparator();
            m_ocaCommandComboBox.addItem("AddSubscription", MethodIndexForAddSubscription);
            m_ocaCommandComboBox.setEnabled(true);
        }
    };

    m_ocaCommandComboBox.onChange = [=]()
    {
        ResetComponents(WORKFLOW_STEP_SELECT_COMMAND);

        int commandIdx = m_ocaCommandComboBox.getSelectedId();
        if (commandIdx > 0)
        {
            // Enable command handle controls
            m_ocaLabels.at(LABELIDX_CMD_HANDLE)->setColour(juce::Label::textColourId, LabelEnabledTextColour);
            m_ocaCommandHandleTextEditor.setEnabled(true);

            // Usual case: Get or Set command was selected.
            if (m_ocaCommandComboBox.getSelectedId() != MethodIndexForAddSubscription)
            {
                // Usual case: a standard AES70 class (not proprietary) was selected.
                if (m_ocaClassComboBox.getSelectedId() != ClassIndexForCustomClass)
                {
                    const auto& propertyList = m_ocaObject->GetProperties();
                    const auto& prop = propertyList.at(m_ocaPropertyComboBox.getSelectedId() - 1);

                    // Update m_ocaCommandDefLevelComboBox to display the definiton level  
                    // of the selected Property (Read-only!)
                    m_ocaCommandDefLevelComboBox.addItem(juce::String(prop.m_defLevel), prop.m_defLevel);
                    m_ocaCommandDefLevelComboBox.setSelectedId(prop.m_defLevel, juce::dontSendNotification);
                }

                // Custom AES70 class is selected.
                else
                {
                    // Add dummy DefLevels to m_ocaCommandDefLevelComboBox and allow user changes.
                    for (int cmdDefLevel = 1; cmdDefLevel < 16; cmdDefLevel++)
                    {
                        m_ocaCommandDefLevelComboBox.addItem(juce::String(cmdDefLevel), cmdDefLevel);
                    }
                    m_ocaCommandDefLevelComboBox.setEnabled(true);
                }
            }

            // AddSubscription command selected: make m_ocaNotificationTextEditor visible.
            else
            {
                m_ocaNotificationTextEditor.setVisible(true);
            }

            CreateValueComponents();
            UpdateBinaryStrings();
        }
    };

    m_ocaResponseStatusComboBox.onChange = [=]()
    {
        UpdateBinaryStrings();
    };

    setSize(640, 480);
    setViewedComponent(&m_container, false);

    ResetComponents(WORKFLOW_STEP_SELECT_CLASS);
}

MainComponent::~MainComponent()
{
}

void MainComponent::ResetComponents(int step)
{
    DBG("ResetComponents step " + juce::String(step));

    bool resizeNeeded(false);
    m_ocaCommandTextEditor.clear();
    m_ocaResponseTextEditor.clear();
    if (step < WORKFLOW_STEP_ENTER_SET_VALUE)
    {
        // Reset status back to OK
        m_ocaResponseStatusComboBox.setSelectedId(1, juce::dontSendNotification);

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

        m_ocaLabels.at(LABELIDX_CMD_SET_VALUE)->setVisible(false);
        m_ocaLabels.at(LABELIDX_RESP_VALUE)->setVisible(false);
        m_ocaLabels.at(LABELIDX_NOTIF_VALUE)->setVisible(false);

        m_ocaCommandDefLevelComboBox.clear();
        m_ocaCommandDefLevelComboBox.setEnabled(false);

        if (m_ocaNotificationTextEditor.isVisible())
        {
            m_ocaNotificationTextEditor.setVisible(false);
            resizeNeeded = true;
        }
    }
    if (step < WORKFLOW_STEP_SELECT_COMMAND)
    {
        m_ocaPropertyDefLevelComboBox.clear();
        m_ocaPropertyDefLevelComboBox.setEnabled(false);
        m_ocaPropertyParamTypeComboBox.clear();
        m_ocaPropertyParamTypeComboBox.setEnabled(false);
        m_ocaCommandComboBox.clear();
        m_ocaCommandComboBox.setEnabled(false);
        //m_ocaCommandHandleTextEditor.setEnabled(false); // TODO: looks crappy
        m_ocaResponseStatusComboBox.setEnabled(false);
        m_ocaLabels.at(LABELIDX_CMD_HANDLE)->setColour(juce::Label::textColourId, LabelDisabledTextColour);
        m_ocaLabels.at(LABELIDX_RESP_STATUS)->setColour(juce::Label::textColourId, LabelDisabledTextColour);
    }
    if (step < WORKFLOW_STEP_SELECT_PROPERTY)
    {
        m_ocaPropertyComboBox.clear();
        m_ocaPropertyComboBox.setEnabled(false);

        // Disable initially all labels starting with LABELIDX_ONO 
        for (int labelIdx = LABELIDX_ONO; labelIdx < LABELIDX_MAX; labelIdx++)
            m_ocaLabels.at(labelIdx)->setColour(juce::Label::textColourId, LabelDisabledTextColour);
    }
    if (step < WORKFLOW_STEP_SELECT_CLASS)
    {
        m_ocaClassComboBox.clear();
        m_ocaClassComboBox.setEnabled(false);
    }

    // Hiding or removing components may change the size of the m_container in the Viewport.
    if (resizeNeeded)
        resized();
}

void MainComponent::CreateValueComponents()
{
    int propIdx = m_ocaPropertyComboBox.getSelectedId();
    int methodIdx = m_ocaCommandComboBox.getSelectedId();
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
        m_ocaLabels.at(LABELIDX_RESP_VALUE)->setVisible(true);
        m_ocaLabels.at(LABELIDX_RESP_VALUE)->setColour(juce::Label::textColourId, LabelEnabledTextColour);
    }
    else if (prop.m_setMethodIdx == methodIdx)
    {
        m_ocaSetCommandValueComponent = std::unique_ptr<juce::Component>(pComponent);
        m_ocaLabels.at(LABELIDX_CMD_SET_VALUE)->setVisible(true);
        m_ocaLabels.at(LABELIDX_CMD_SET_VALUE)->setColour(juce::Label::textColourId, LabelEnabledTextColour);
    }
    else if (methodIdx == MethodIndexForAddSubscription)
    {
        m_ocaNotificationValueComponent = std::unique_ptr<juce::Component>(pComponent);
        m_ocaLabels.at(LABELIDX_NOTIF_VALUE)->setVisible(true);
        m_ocaLabels.at(LABELIDX_NOTIF_VALUE)->setColour(juce::Label::textColourId, LabelEnabledTextColour);
    }

    m_ocaResponseStatusComboBox.setEnabled(true);
    m_ocaLabels.at(LABELIDX_RESP_STATUS)->setColour(juce::Label::textColourId, LabelEnabledTextColour);
    m_ocaLabels.at(LABELIDX_CMD_HANDLE)->setColour(juce::Label::textColourId, LabelEnabledTextColour);

    resized();
}

void MainComponent::UpdateBinaryStrings()
{
    DBG("UpdateBinaryStrings");
    juce::String commandString;
    juce::String responseString;
    juce::String notificationString;
    CreateBinaryStrings(commandString, responseString, notificationString);
    m_ocaCommandTextEditor.setText(commandString, juce::dontSendNotification);
    m_ocaResponseTextEditor.setText(responseString, juce::dontSendNotification);
    m_ocaNotificationTextEditor.setText(notificationString, juce::dontSendNotification);
}

bool MainComponent::CreateBinaryStrings(juce::String& commandString, juce::String& responseString, juce::String& notificationString)
{
    int propIdx = m_ocaPropertyComboBox.getSelectedId();
    int methodIdx = m_ocaCommandComboBox.getSelectedId();
    if ((propIdx <= 0) || (methodIdx <= 0))
        return false;

    if (!m_ocaObject)
        return false;

    // TODO: Custom AES70 class

    auto propertyList = m_ocaObject->GetProperties();
    if (propertyList.size() < propIdx)
        return false;

    std::uint32_t targetOno = static_cast<std::uint32_t>(m_ocaONoTextEditor.getText().getIntValue());
    std::uint8_t responseStatus = static_cast<std::uint8_t>(m_ocaResponseStatusComboBox.getSelectedId() - 1);

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
    g.fillAll(AppBackgroundColour);
}

void MainComponent::resized()
{
    // The height of the container component within the Viewport depends on how much 
    // content needs to be displayed. So m_container grows as more components are required. 
    int containerHeight = ContainerComponentDefaultHeight;
    if (m_ocaCommandComboBox.getSelectedId() == MethodIndexForAddSubscription)
        containerHeight += 140;

    // Remove a margin to account for the vertical scrollbar.
    m_container.setBounds(0, 0, getLocalBounds().getWidth() - 8, containerHeight);
    auto bounds = m_container.getLocalBounds();

    int margin = 2;
    int controlHeight = 40;
    int comboBoxWidth = bounds.getWidth() / 5; // 5 columns. 

    // Row 1
    auto rowBounds = bounds.removeFromTop(static_cast<int>(controlHeight * 1.5f));
    m_ocaLabels.at(LABELIDX_TITLE)->setBounds(rowBounds.reduced(margin));

    // Row 2
    rowBounds = bounds.removeFromTop(controlHeight);
    m_ocaLabels.at(LABELIDX_CLASS)->setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));
    m_ocaClassComboBox.setBounds(rowBounds.removeFromLeft(comboBoxWidth * 2).reduced(margin));
    m_ocaLabels.at(LABELIDX_ONO)->setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));
    m_ocaONoTextEditor.setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));

    // Row 3
    rowBounds = bounds.removeFromTop(controlHeight);
    m_ocaLabels.at(LABELIDX_PROP)->setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));
    m_ocaPropertyComboBox.setBounds(rowBounds.removeFromLeft(comboBoxWidth * 2).reduced(margin));
    m_ocaLabels.at(LABELIDX_PROP_DEFLEVEL)->setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));
    m_ocaPropertyDefLevelComboBox.setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));

    // Row 4
    rowBounds = bounds.removeFromTop(controlHeight);
    rowBounds.removeFromLeft(comboBoxWidth * 3); // Horizontal spacer
    m_ocaLabels.at(LABELIDX_PROP_PARAMTYPE)->setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));
    m_ocaPropertyParamTypeComboBox.setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));

    // Row 5
    rowBounds = bounds.removeFromTop(controlHeight);
    m_ocaLabels.at(LABELIDX_CMD)->setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));
    m_ocaCommandComboBox.setBounds(rowBounds.removeFromLeft(comboBoxWidth * 2).reduced(margin));
    m_ocaLabels.at(LABELIDX_CMD_DEFLEVEL)->setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));
    m_ocaCommandDefLevelComboBox.setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));

    // Vertical spacer
    bounds.removeFromTop(controlHeight / 2);

    // Row 6
    rowBounds = bounds.removeFromTop(controlHeight);
    m_ocaLabels.at(LABELIDX_CMD_SET_VALUE)->setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));
    if (m_ocaSetCommandValueComponent)
        m_ocaSetCommandValueComponent->setBounds(rowBounds.removeFromLeft(comboBoxWidth * 2).reduced(margin));
    else
        rowBounds.removeFromLeft(comboBoxWidth * 2); // Horizontal spacer
    m_ocaLabels.at(LABELIDX_CMD_HANDLE)->setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));
    m_ocaCommandHandleTextEditor.setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));

    // Row 7
    rowBounds = bounds.removeFromTop(static_cast<int>(controlHeight * 1.5f));
    m_ocaCommandTextEditor.setBounds(rowBounds.reduced(margin));

    // Vertical spacer
    bounds.removeFromTop(controlHeight / 2); 

    // Row 8
    rowBounds = bounds.removeFromTop(controlHeight);
    m_ocaLabels.at(LABELIDX_RESP_VALUE)->setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));
    if (m_ocaResponseValueComponent)
        m_ocaResponseValueComponent->setBounds(rowBounds.removeFromLeft(comboBoxWidth * 2).reduced(margin));
    else
        rowBounds.removeFromLeft(comboBoxWidth * 2); // Horizontal spacer
    m_ocaLabels.at(LABELIDX_RESP_STATUS)->setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));
    m_ocaResponseStatusComboBox.setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));

    // Row 9
    rowBounds = bounds.removeFromTop(static_cast<int>(controlHeight * 1.5f));
    m_ocaResponseTextEditor.setBounds(rowBounds.reduced(margin));

    // Row 10 and 11: Notification only relevant if AddSubscription command selected.
    if (m_ocaCommandComboBox.getSelectedId() == MethodIndexForAddSubscription)
    {
        bounds.removeFromTop(controlHeight / 2); // Vertical spacer
        rowBounds = bounds.removeFromTop(controlHeight); // Row 10
        m_ocaLabels.at(LABELIDX_NOTIF_VALUE)->setBounds(rowBounds.removeFromLeft(comboBoxWidth).reduced(margin));
        m_ocaNotificationValueComponent->setBounds(rowBounds.removeFromLeft(comboBoxWidth * 2).reduced(margin));

        rowBounds = bounds.removeFromTop(static_cast<int>(controlHeight * 1.5f)); // Row 11
        m_ocaNotificationTextEditor.setBounds(rowBounds.reduced(margin));
    }

    // Call base class implementation which takes care of updating scrollbars etc.
    return juce::Viewport::resized();
}

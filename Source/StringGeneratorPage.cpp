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

#include "StringGeneratorPage.h"
#include "MainTabbedComponent.h"
#include "AES70.h"


/**
 * Command index used for AddSubscription.
 */
static constexpr int MethodIndexForAddSubscription = 256;

/**
 * Index used for custom (aka proprietary, non-standard) AES70 classes.
 */
static constexpr int ClassIndexForCustomClass = 256;

static constexpr int AppWindowDefaultWidth = 640; // Default App window width.
static constexpr int AppWindowDefaultHeight = 480; // Default App window height, also used for m_container within the juce::Viewport.

static const juce::Colour AppBackgroundColour(43, 65, 77);          // App window background color
static const juce::Colour LabelEnabledTextColour(125, 182, 212);    // Color for labels of relevant controls
static const juce::Colour LabelDisabledTextColour(12, 12, 12);      // Color for labels of non-relevant controls

static const juce::String ProjectHostShortURL("https://github.com/escalonely"); // Web address of this project's host.
static const juce::String ProjectHostLongURL("https://github.com/escalonely/AES70_OCP1_StringGenerator"); // Complete web address of this project's host.

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
    /* LABELIDX_TITLE              */ "AES70 OCP.1 binary string generator v" + juce::String(JUCE_STRINGIFY(JUCE_APP_VERSION)),
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


//==============================================================================
// Class StringGeneratorPage
//==============================================================================

StringGeneratorPage::StringGeneratorPage(MainTabbedComponent* parent)
    :   m_parent(parent),
        m_ocaONoTextEditor(juce::TextEditor("OCA ONo")), 
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
        m_ocaNotificationTextEditor(juce::TextEditor("OCA Notification String")),
        m_sendButton(juce::TextButton("String Test Button")),
        m_hyperlink(juce::HyperlinkButton(ProjectHostShortURL, juce::URL(ProjectHostLongURL)))
{
    jassert(parent != nullptr);

    m_container.addAndMakeVisible(&m_hyperlink);
    m_container.addAndMakeVisible(&m_ocaONoTextEditor);
    m_container.addAndMakeVisible(&m_ocaClassComboBox);
    m_container.addAndMakeVisible(&m_ocaPropertyComboBox);
    m_container.addAndMakeVisible(&m_ocaPropertyDefLevelComboBox);
    m_container.addAndMakeVisible(&m_ocaPropertyParamTypeComboBox);
    m_container.addAndMakeVisible(&m_ocaCommandComboBox);
    m_container.addAndMakeVisible(&m_ocaCommandDefLevelComboBox);
    m_container.addAndMakeVisible(&m_ocaCommandHandleTextEditor);
    m_container.addAndMakeVisible(&m_ocaCommandTextEditor);
    m_container.addAndMakeVisible(&m_sendButton);
    m_container.addAndMakeVisible(&m_ocaResponseStatusComboBox);
    m_container.addAndMakeVisible(&m_ocaResponseTextEditor);
    m_container.addChildComponent(&m_ocaNotificationTextEditor); // Invisible until AddSubscription Cmd selected

    m_ocaONoTextEditor.setHasFocusOutline(true);
    m_ocaONoTextEditor.setInputRestrictions(0, "0123456789");
    m_ocaONoTextEditor.setIndents(m_ocaONoTextEditor.getLeftIndent(), 0); // Hack for JUCE justification bug
    m_ocaONoTextEditor.setJustification(juce::Justification(juce::Justification::centredRight));
    m_ocaONoTextEditor.setText("10000", juce::dontSendNotification);

    m_ocaClassComboBox.setHasFocusOutline(true);
    m_ocaPropertyComboBox.setHasFocusOutline(true);
    m_ocaPropertyDefLevelComboBox.setHasFocusOutline(true);
    m_ocaPropertyParamTypeComboBox.setHasFocusOutline(true);
    m_ocaCommandComboBox.setHasFocusOutline(true);
    m_ocaCommandDefLevelComboBox.setHasFocusOutline(true);
    m_ocaResponseStatusComboBox.setHasFocusOutline(true);

    m_ocaCommandHandleTextEditor.setHasFocusOutline(true);
    m_ocaCommandHandleTextEditor.setInputRestrictions(0, "0123456789");
    m_ocaCommandHandleTextEditor.setIndents(m_ocaCommandHandleTextEditor.getLeftIndent(), 0); // Hack for JUCE justification bug
    m_ocaCommandHandleTextEditor.setJustification(juce::Justification(juce::Justification::centredRight));
    m_ocaCommandHandleTextEditor.setText("1", juce::dontSendNotification);
    
    m_ocaCommandTextEditor.setHasFocusOutline(true);
    m_ocaCommandTextEditor.setReadOnly(true);
    m_ocaCommandTextEditor.setCaretVisible(false);
    m_ocaCommandTextEditor.setMultiLine(true, true);
    m_ocaCommandTextEditor.setTextToShowWhenEmpty("This field will show the specified Command string "
        "to be transmitted to an AES70-capable device.", 
        LabelEnabledTextColour);

    m_sendButton.setButtonText("Test");
    m_sendButton.setHasFocusOutline(true);
    m_sendButton.setTooltip("Test by transmitting the string to the device."); // TODO: tooltips don't work

    m_ocaResponseTextEditor.setHasFocusOutline(true);
    m_ocaResponseTextEditor.setReadOnly(true);
    m_ocaResponseTextEditor.setCaretVisible(false);
    m_ocaResponseTextEditor.setMultiLine(true, true);
    m_ocaResponseTextEditor.setTextToShowWhenEmpty("This field will show the expected Response "
        "from the device as a result of the above Command.", 
        LabelEnabledTextColour);

    m_ocaNotificationTextEditor.setHasFocusOutline(true);
    m_ocaNotificationTextEditor.setReadOnly(true);
    m_ocaNotificationTextEditor.setCaretVisible(false);
    m_ocaNotificationTextEditor.setMultiLine(true, true);
    m_ocaNotificationTextEditor.setVisible(false); // Invisible until AddSubscription Cmd selected
    m_ocaNotificationTextEditor.setTextToShowWhenEmpty("The following Notification can be expected, ...TODO", LabelEnabledTextColour);

    // Add all OCA status and status strings to m_ocaResponseStatusComboBox
    for (int statusIdx = 0; statusIdx < 16; statusIdx++)
    {
        juce::String statusName = juce::String(statusIdx) + ": " + NanoOcp1::StatusToString(static_cast<std::uint8_t>(statusIdx));
        m_ocaResponseStatusComboBox.addItem(statusName, statusIdx + 1);
    }

    // Create and add all labels on the GUI
    for (int labelIdx = 0; labelIdx < LABELIDX_MAX; labelIdx++)
    {
        m_ocaLabels.emplace_back(std::make_unique<juce::Label>("OCA Label " + juce::String(labelIdx)));
        if (labelIdx == LABELIDX_TITLE)
            m_ocaLabels.back()->setJustificationType(juce::Justification::bottomRight);
        else
            m_ocaLabels.back()->setJustificationType(juce::Justification::centredRight);

        if (labelIdx < LABELIDX_ONO)
            m_ocaLabels.at(labelIdx)->setColour(juce::Label::textColourId, LabelEnabledTextColour);

        m_ocaLabels.at(labelIdx)->setText(GuiLabelsText.at(labelIdx), juce::dontSendNotification);
        m_container.addAndMakeVisible(m_ocaLabels.back().get());
    }

    // Set same font as the labels.
    m_hyperlink.setFont(m_ocaLabels.at(LABELIDX_TITLE)->getFont(), false /* do not resize */);
    m_hyperlink.setJustificationType(juce::Justification::topRight);
    m_hyperlink.setTooltip(ProjectHostLongURL); // TODO: get tooltip to work

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
        }

        // Enable labels
        m_ocaLabels.at(LABELIDX_ONO)->setColour(juce::Label::textColourId, LabelEnabledTextColour);
        m_ocaLabels.at(LABELIDX_PROP)->setColour(juce::Label::textColourId, LabelEnabledTextColour);

        // Remove StartHere icon
        repaint();
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
                m_ocaPropertyParamTypeComboBox.addItem(NanoOcp1::DataTypeToString(prop.m_type), prop.m_type);
                m_ocaPropertyParamTypeComboBox.setSelectedId(prop.m_type, juce::dontSendNotification);
            }

            // Custom AES70 class is selected. 
            // NOTE: propertyList will be empty at this point.
            else
            {
                // Manually create a property based on the user's selected settings, 
                // and assign it to m_ocaObject to use as the single entry in its property list.
                AES70::Property customProp = { 
                    1, /*m_defLevel*/ propIdx, /*m_index*/ 1, /*m_type*/
                    "Custom", /*m_name*/ 0, /*m_getMethodIdx*/ 0 /*m_setMethodIdx*/ 
                };
                auto customObj = static_cast<AES70::OcaCustomClass*>(m_ocaObject.get());
                customObj->m_customProp = customProp;
                
                // Fill m_ocaPropertyDefLevelComboBox with dummy definition levels
                // and then enable it to allow user changes.
                for (int propDefLevel = 1; propDefLevel < 16; propDefLevel++)
                {
                    m_ocaPropertyDefLevelComboBox.addItem(juce::String(propDefLevel), propDefLevel);
                }
                m_ocaPropertyDefLevelComboBox.setSelectedId(1, juce::dontSendNotification);
                m_ocaPropertyDefLevelComboBox.setEnabled(true);
                m_ocaLabels.at(LABELIDX_PROP_DEFLEVEL)->setColour(juce::Label::textColourId, LabelEnabledTextColour);

                // Add supported data types defined in NanoOcp1 to m_ocaPropertyParamTypeComboBox and allow user changes.
                m_ocaPropertyParamTypeComboBox.addItem(NanoOcp1::DataTypeToString(NanoOcp1::OCP1DATATYPE_BOOLEAN), NanoOcp1::OCP1DATATYPE_BOOLEAN);
                m_ocaPropertyParamTypeComboBox.addItem(NanoOcp1::DataTypeToString(NanoOcp1::OCP1DATATYPE_INT32), NanoOcp1::OCP1DATATYPE_INT32);
                m_ocaPropertyParamTypeComboBox.addItem(NanoOcp1::DataTypeToString(NanoOcp1::OCP1DATATYPE_UINT8), NanoOcp1::OCP1DATATYPE_UINT8);
                m_ocaPropertyParamTypeComboBox.addItem(NanoOcp1::DataTypeToString(NanoOcp1::OCP1DATATYPE_UINT16), NanoOcp1::OCP1DATATYPE_UINT16);
                m_ocaPropertyParamTypeComboBox.addItem(NanoOcp1::DataTypeToString(NanoOcp1::OCP1DATATYPE_UINT32), NanoOcp1::OCP1DATATYPE_UINT32);
                m_ocaPropertyParamTypeComboBox.addItem(NanoOcp1::DataTypeToString(NanoOcp1::OCP1DATATYPE_FLOAT32), NanoOcp1::OCP1DATATYPE_FLOAT32);
                m_ocaPropertyParamTypeComboBox.addItem(NanoOcp1::DataTypeToString(NanoOcp1::OCP1DATATYPE_STRING), NanoOcp1::OCP1DATATYPE_STRING);
                m_ocaPropertyParamTypeComboBox.setSelectedId(NanoOcp1::OCP1DATATYPE_BOOLEAN, juce::dontSendNotification);
                m_ocaPropertyParamTypeComboBox.setEnabled(true);
                m_ocaLabels.at(LABELIDX_PROP_PARAMTYPE)->setColour(juce::Label::textColourId, LabelEnabledTextColour);

                // Fill m_ocaCommandComboBox with dummy Get commands
                for (int commandIdx = 1; commandIdx <= 8; commandIdx++)
                    m_ocaCommandComboBox.addItem(juce::String(commandIdx) + ": GetValue", commandIdx);

                // Fill m_ocaCommandComboBox with dummy Set commands
                m_ocaCommandComboBox.addSeparator();
                for (int commandIdx = 1; commandIdx <= 8; commandIdx++)
                    m_ocaCommandComboBox.addItem(juce::String(commandIdx) + ": SetValue", commandIdx + 8);
            }

            // In addition to the property-related commands, always offer the AddSubscription command.
            m_ocaCommandComboBox.addSeparator();
            m_ocaCommandComboBox.addItem("AddSubscription", MethodIndexForAddSubscription);
            m_ocaCommandComboBox.setEnabled(true);
        }
    };

    m_ocaPropertyDefLevelComboBox.onChange = [=]()
    {
        if (m_ocaClassComboBox.getSelectedId() == ClassIndexForCustomClass)
        {
            // Update the custom property and use it to update the binary strings.
            auto customObj = static_cast<AES70::OcaCustomClass*>(m_ocaObject.get());
            customObj->m_customProp.m_defLevel = m_ocaPropertyDefLevelComboBox.getSelectedId();

            UpdateBinaryStrings();
        }
    };

    m_ocaPropertyParamTypeComboBox.onChange = [=]()
    {
        if (m_ocaClassComboBox.getSelectedId() == ClassIndexForCustomClass)
        {
            // Update the custom property and use it to update the binary strings.
            auto customObj = static_cast<AES70::OcaCustomClass*>(m_ocaObject.get());
            customObj->m_customProp.m_type = m_ocaPropertyParamTypeComboBox.getSelectedId();

            CreateValueComponents();
            UpdateBinaryStrings();
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
                    m_ocaCommandDefLevelComboBox.setSelectedId(1, juce::dontSendNotification);
                    m_ocaCommandDefLevelComboBox.setEnabled(true);
                    m_ocaLabels.at(LABELIDX_CMD_DEFLEVEL)->setColour(juce::Label::textColourId, LabelEnabledTextColour);
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

    m_ocaCommandDefLevelComboBox.onChange = [=]()
    {
        UpdateBinaryStrings();
    };

    m_ocaCommandHandleTextEditor.onTextChange = [=]()
    {
        UpdateBinaryStrings();
    };

    m_ocaResponseStatusComboBox.onChange = [=]()
    {
        UpdateBinaryStrings();
    };

    m_sendButton.onClick = [=]()
    {
        DBG("m_sendButton.onClick");
        
        juce::MemoryBlock commandMemBlock;
        juce::MemoryBlock responseMemBlock;
        juce::MemoryBlock notificationMemBlock;
        CreateBinaryStrings(commandMemBlock, responseMemBlock, notificationMemBlock);

        // Pass command MemoryBlock to the parent MainTabbedComponent.
        m_parent->SendCommandToDevice(commandMemBlock);
    };

    setSize(AppWindowDefaultWidth, AppWindowDefaultHeight);
    setViewedComponent(&m_container, false);

    ResetComponents(WORKFLOW_STEP_SELECT_CLASS);
}

StringGeneratorPage::~StringGeneratorPage()
{
}

void StringGeneratorPage::ResetComponents(int step)
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
        //m_ocaCommandHandleTextEditor.setEnabled(false); // TODO: looks crappy, set style instead
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

void StringGeneratorPage::CreateValueComponents()
{
    int propIdx = m_ocaPropertyComboBox.getSelectedId();
    int methodIdx = m_ocaCommandComboBox.getSelectedId();
    if ((propIdx <= 0) || (methodIdx <= 0))
        return;

    if (!m_ocaObject)
        return;

    AES70::Property prop;
    bool getMethodSelected(false);
    bool setMethodSelected(false);

    // Usual case: a standard AES70 class (not proprietary) was selected.
    if (m_ocaClassComboBox.getSelectedId() != ClassIndexForCustomClass)
    {
        auto propertyList = m_ocaObject->GetProperties();
        if (propertyList.size() < propIdx)
            return;

        prop = propertyList.at(propIdx - 1);
        getMethodSelected = (methodIdx == prop.m_getMethodIdx);
        setMethodSelected = (methodIdx == prop.m_setMethodIdx);
    }

    // Custom AES70 class is selected. NOTE: GetProperties will return exactly one entry.
    else
    {
        prop = m_ocaObject->GetProperties().back(); 
        getMethodSelected = (methodIdx <= 8);
        setMethodSelected = (methodIdx > 8) && (methodIdx <= 16);
    }

    // Create a component appropriate for displaying this properties value.
    auto pComponent = m_ocaObject->CreateComponentForProperty(prop, 
        [this] {
            UpdateBinaryStrings();
        });
    jassert(pComponent); // Missing implementation!
    m_container.addAndMakeVisible(pComponent);

    // Depending on whether the Get, Set, or AddSubscription commands are selected, assign this new component 
    // to the correct member variable. The old component, if any, will be deleted automatically.
    if (getMethodSelected)
    {
        m_ocaResponseValueComponent = std::unique_ptr<juce::Component>(pComponent);
        m_ocaLabels.at(LABELIDX_RESP_VALUE)->setVisible(true);
        m_ocaLabels.at(LABELIDX_RESP_VALUE)->setColour(juce::Label::textColourId, LabelEnabledTextColour);
    }
    else if (setMethodSelected)
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

void StringGeneratorPage::UpdateBinaryStrings()
{
    DBG("UpdateBinaryStrings");

    juce::MemoryBlock commandMemBlock;
    juce::MemoryBlock responseMemBlock;
    juce::MemoryBlock notificationMemBlock;
    CreateBinaryStrings(commandMemBlock, responseMemBlock, notificationMemBlock);

    // Convert juce::MemoryBlock to juce::String
    // TODO: add support for user-defined byte-separators
    juce::String commandString = juce::String::toHexString(commandMemBlock.getData(), static_cast<int>(commandMemBlock.getSize()));
    juce::String responseString = juce::String::toHexString(responseMemBlock.getData(), static_cast<int>(responseMemBlock.getSize()));
    juce::String notificationString = juce::String::toHexString(notificationMemBlock.getData(), static_cast<int>(notificationMemBlock.getSize()));

    m_ocaCommandTextEditor.setText(commandString, juce::dontSendNotification);
    m_ocaResponseTextEditor.setText(responseString, juce::dontSendNotification);
    m_ocaNotificationTextEditor.setText(notificationString, juce::dontSendNotification);
}

bool StringGeneratorPage::CreateBinaryStrings(juce::MemoryBlock& commandMemBlock, juce::MemoryBlock& responseMemBlock, juce::MemoryBlock& notificationMemBlock)
{
    int propIdx = m_ocaPropertyComboBox.getSelectedId();
    int methodIdx = m_ocaCommandComboBox.getSelectedId();
    if ((propIdx <= 0) || (methodIdx <= 0))
        return false;

    if (!m_ocaObject)
        return false;

    AES70::Property prop;
    int commandDefLevel;
    bool getMethodSelected(false);
    bool setMethodSelected(false);

    // Usual case: a standard AES70 class (not proprietary) was selected.
    if (m_ocaClassComboBox.getSelectedId() != ClassIndexForCustomClass)
    {
        auto propertyList = m_ocaObject->GetProperties();
        if (propertyList.size() < propIdx)
            return false;

        prop = propertyList.at(propIdx - 1);
        commandDefLevel = prop.m_defLevel;
        getMethodSelected = (methodIdx == prop.m_getMethodIdx);
        setMethodSelected = (methodIdx == prop.m_setMethodIdx);
    }

    // Custom AES70 class is selected. NOTE: GetProperties will return exactly one entry.
    else
    {
        prop = m_ocaObject->GetProperties().back();
        commandDefLevel = m_ocaCommandDefLevelComboBox.getSelectedId();
        getMethodSelected = (methodIdx <= 8);
        setMethodSelected = (methodIdx > 8) && (methodIdx <= 16);
    }

    std::uint32_t targetOno = static_cast<std::uint32_t>(m_ocaONoTextEditor.getText().getIntValue());
    std::uint8_t responseStatus = static_cast<std::uint8_t>(m_ocaResponseStatusComboBox.getSelectedId() - 1);

    std::uint8_t responseParamCount(0);
    std::vector<std::uint8_t> responseParamData;
    NanoOcp1::Ocp1CommandDefinition commandDefinition;
    notificationMemBlock.reset();

    // Depending on whether the Get, Set, or AddSubscription commands are selected,
    // the commandDefinition will be defined differently.
    if (getMethodSelected)
    {
        responseParamCount = 1;
        responseParamData = m_ocaObject->CreateParamDataForComponent(m_ocaResponseValueComponent.get(), prop);
        jassert(responseParamData.size() > 0);

        commandDefinition = NanoOcp1::Ocp1CommandDefinition(targetOno, 
                                                            static_cast<std::uint16_t>(prop.m_type),
                                                            static_cast<std::uint16_t>(commandDefLevel),
                                                            static_cast<std::uint16_t>(methodIdx));
    }
    else if (setMethodSelected)
    {
        auto commandParamData = m_ocaObject->CreateParamDataForComponent(m_ocaSetCommandValueComponent.get(), prop);
        jassert(commandParamData.size() > 0);

        commandDefinition = NanoOcp1::Ocp1CommandDefinition(targetOno, 
                                                            static_cast<std::uint16_t>(prop.m_type), 
                                                            static_cast<std::uint16_t>(commandDefLevel),
                                                            static_cast<std::uint16_t>(methodIdx),
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
        notificationMemBlock = NanoOcp1::Ocp1Notification(targetOno, 
                                                          static_cast<std::uint16_t>(prop.m_defLevel),
                                                          static_cast<std::uint16_t>(prop.m_index),
                                                          1 /* paramCount */, 
                                                          notificationParamData).GetMemoryBlock();
    }

    std::uint32_t dummyHandle; // Auto-generated by Ocp1CommandResponseRequired, will be thrown away.
    NanoOcp1::Ocp1CommandResponseRequired ocp1Command(commandDefinition, dummyHandle);

    // Replace the auto-generated handle with the user-defined one.
    std::uint32_t userHandle(static_cast<std::uint32_t>(m_ocaCommandHandleTextEditor.getText().getIntValue()));
    ocp1Command.SetHandle(userHandle);

    commandMemBlock = ocp1Command.GetMemoryBlock();
    responseMemBlock = NanoOcp1::Ocp1Response(userHandle, responseStatus, responseParamCount, responseParamData).GetMemoryBlock();

    return true;
}

void StringGeneratorPage::paint(juce::Graphics& g)
{
    g.fillAll(AppBackgroundColour);

    if (m_ocaClassComboBox.getSelectedId() == 0)
    {
        std::unique_ptr<Drawable> startHereIcon = Drawable::createFromImageData(BinaryData::StartHere_png, BinaryData::StartHere_pngSize);
        float startHereIconXPos = (float)(m_ocaPropertyComboBox.getBounds().getCentreX() - 50);
        startHereIcon->drawWithin(g, Rectangle<float>(startHereIconXPos, 15.0f, 90.0f, 45.0f), 
            RectanglePlacement::stretchToFit, 0.75f);
    }
}

void StringGeneratorPage::resized()
{
    // The height of the container component within the Viewport depends on how much 
    // content needs to be displayed. So m_container grows as more components are required. 
    int containerHeight = AppWindowDefaultHeight;
    if (m_ocaCommandComboBox.getSelectedId() == MethodIndexForAddSubscription)
        containerHeight += 140;

    // Remove a margin to account for the vertical scrollbar.
    m_container.setBounds(0, 0, getLocalBounds().getWidth() - 8, containerHeight);
    auto bounds = m_container.getLocalBounds();
    bounds.removeFromLeft(4); // Left margin

    int margin = 2;
    int controlHeight = 40;
    int comboBoxWidth = bounds.getWidth() / 5; // 5 columns. 

    // Row 1
    auto rowBounds = bounds.removeFromTop(static_cast<int>(controlHeight * 0.75));
    rowBounds.removeFromLeft(comboBoxWidth); // Horizontal spacer
    m_ocaLabels.at(LABELIDX_TITLE)->setBounds(rowBounds.reduced(margin));
    rowBounds = bounds.removeFromTop(static_cast<int>(controlHeight * 0.5));
    m_hyperlink.changeWidthToFitText();
    m_hyperlink.setBounds(rowBounds.removeFromRight(m_hyperlink.getWidth()).reduced(margin));

    // Vertical spacer
    bounds.removeFromTop(controlHeight / 2);

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
    m_sendButton.setBounds(rowBounds.removeFromRight(static_cast<int>(comboBoxWidth * 0.5f)).reduced(margin));
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


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


/**
 * Forward declarations.
 */
namespace AES70
{
    struct Property;
    struct OcaRoot;
}


/**
 * Main component displayed on the application's UI.
 * It inherits from juce::Viewport to allow scrolling through the contents inside.
 */
class MainComponent  : public juce::Viewport
{
public:
    MainComponent();
    ~MainComponent() override;

    // Reimplemented from juce::Component

    void paint (juce::Graphics&) override;
    void resized() override;


protected:
    /**
     * Reset GUI components.
     * @param[in] step  Determines which GUI controls are reset. 
     */
    void ResetComponents(int step);

    /**
     * Re-create the components for entering the values for Set commands and Get responses,
     * m_ocaSetCommandValueComponent and m_ocaResponseValueComponent.
     */
    void CreateValueComponents();

    /**
     * Re-create the Command and Response binary strings, and display them on both TextEditors:
     * m_ocaCommandTextEditor and m_ocaResponseTextEditor.
     */
    void UpdateBinaryStrings();

    /**
     * Re-create the Command, Response, and Notification binary strings, based on the current 
     * configuration of the OCA class, ONo, Property, and Command GUI controls.
     * 
     * @parameter[out] commandString        The resulting Command binary string.
     * @parameter[out] responseString       The resulting Response binary string.
     * @parameter[out] notificationString   The Notification binary string that would result from a PropertyChanged event
     *                                      in the device. Will only be non-empty if the AddSubscription command is selected.
     * @return  True if all strings could be generated successfully.
     */
    bool CreateBinaryStrings(juce::String& commandString, juce::String& responseString, juce::String& notificationString);


private:
    // Component inside the main juce::Viewport, set with setViewedComponent.
    juce::Component m_container;

    // Hyperlink to the project webpage.
    juce::HyperlinkButton m_hyperlink;

    // TextEditor to enter the AES70/OCA object number
    juce::TextEditor m_ocaONoTextEditor;

    // ComboBox to select AES70/OCA class
    juce::ComboBox m_ocaClassComboBox;

    // ComboBox to select AES70/OCA class property
    juce::ComboBox m_ocaPropertyComboBox;

    // ComboBox to display the definition level of the selected AES70/OCA class property
    juce::ComboBox m_ocaPropertyDefLevelComboBox;

    // ComboBox to display the datatype of the selected AES70/OCA class property
    juce::ComboBox m_ocaPropertyParamTypeComboBox;

    // ComboBox to select the AES70/OCA Command to send
    juce::ComboBox m_ocaCommandComboBox;

    // ComboBox to display the definition level of the selected AES70/OCA command
    juce::ComboBox m_ocaCommandDefLevelComboBox;

    // Component to enter values for Set methods
    std::unique_ptr<juce::Component> m_ocaSetCommandValueComponent;

    // TextEditor to set the handle of the AES70/OCA Command to send
    juce::TextEditor m_ocaCommandHandleTextEditor;

    // TextEditor to display the AES70/OCA Command to send
    juce::TextEditor m_ocaCommandTextEditor;

    // ComboBox to select the AES70/OCA Response status
    juce::ComboBox m_ocaResponseStatusComboBox;

    // Component to enter values for the Get method Response message
    std::unique_ptr<juce::Component> m_ocaResponseValueComponent;

    // TextEditor to display the AES70/OCA Response message
    juce::TextEditor m_ocaResponseTextEditor;

    // Component to enter values for Notifications
    std::unique_ptr<juce::Component> m_ocaNotificationValueComponent;

    // TextEditor to display AES70/OCA Notification messages
    juce::TextEditor m_ocaNotificationTextEditor;

    // Labels on the UI. NOTE: use a vector of pointers because juce::Label is non-copyable.
    std::vector<std::unique_ptr<juce::Label>> m_ocaLabels;

    // AES70/OCA object representing the current configuraion on the GUI
    std::unique_ptr<AES70::OcaRoot> m_ocaObject;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

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
namespace NanoOcp1
{
    class NanoOcp1Client;
}


/**
 * Custom juce::TabbedComponent, a component with a TabbedButtonBar along the top.
 * The first tab will always be a TestPage component, while the rest of the tabs 
 * will be StringGeneratorPage components.
 */
class MainTabbedComponent : public juce::TabbedComponent
{
public:
    MainTabbedComponent();
    ~MainTabbedComponent() override;

    /**
     * Opens a dialog window to select a config file to load.
     * This clears all current tabs, and creates new tabs based on the selected config file.
     */
    bool LoadFileViaDialog();

    /**
     * Opens a dialog window to select a config file to save current configuration to.
     * This calls CreateConfigFileFromPages internally and then writes the result to the selected file.
     */
    bool SaveFileViaDialog();

    /**
     * Create the initial tabs and pages under this TabbedComponent. This will add one TestPage tab, 
     * and then a number of StringGeneratorPage tabs depending on whether a config file was provided.
     * If no config file is provided, the default page configuration will be created.
     *
     * @param[in] configFile    Optional file containing page configuration as XML.
     * @return  True if the given file was used and successfully parsed.
     *          False if just the default pages were created instead.
     */
    bool InitializePages(const juce::File& configFile = juce::File());

    /**
     * Create a file containing the configuration of all pages under this TabbedComponent. 
     *
     * @param[in] configFile    File which will be weitten with page configuration as XML.
     * @return  True if the file could be written to.
     */
    bool CreateConfigFileFromPages(juce::File& configFile) const;

    /**
     * Use m_nanoOcp1Client to send a given MemoryBlock to the connected remote device.
     * 
     * @param[in] data  MemoryBlock to send.
     * @return  True if the message could be sent successfully.
     */
    bool SendCommandToDevice(const juce::MemoryBlock& data);

    /**
     * Get the ip address and port currently used by m_nanoOcp1Client.
     *
     * @param[out] address  Current IP address
     * @param[out] port     Current IP port
     * @return  True if the parameters could be retrieved successfully.
     */
    bool GetConnectionParameters(juce::String& address, int& port) const;


    // Reimplemented from juce::TabbedComponent

    TabBarButton* createTabButton(const String& tabName, int tabIndex) override;
    void currentTabChanged(int newCurrentTabIndex, const String& newCurrentTabName) override;


    // Reimplemented from juce::Component

    void paint(juce::Graphics&) override;
    void resized() override;


protected:
    /**
     * Initialize NanoOcp1Client, which will send and receive messages to & from the device
     * whenever m_sendButton is pressed.
     */
    void StartNanoOcpClient();

    /**
     * Read the given file, parse the XML content, and create StringGeneratorPage tabs based on the file.
     * 
     * @param[in] configFile        File containing a valid configuration as XML.
     * @return  True if the file could be parsed and at least one StringGeneratorPage was created.
     */
    bool CreatePagesFromConfigFile(const juce::File& configFile);


private:
    /**
     * OCP1 Client to handle AES70 communication with device.
     * TODO: why unique_ptr? Just put on the stack.
     */
    std::unique_ptr<NanoOcp1::NanoOcp1Client> m_nanoOcp1Client;

    /**
     * Number of messages that have been passed to the TestPage for displaying
     * since the last time that the TestPage has been the active tab (See currentTabChanged).
     */
    int m_numUnreadMessages;

    /**
     * File open dialog.
     */
    std::unique_ptr<juce::FileChooser> m_fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainTabbedComponent)
};




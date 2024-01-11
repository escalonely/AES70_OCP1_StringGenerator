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
#include "AbstractPage.h"



/**
 * Component for testing the generated OCP.1 binary strings against real AES70-capable
 * devices via TCP/IP. This page provides GUI elements for TCP connection configuration,
 * as well as fields for monitoring the incoming OCP.1 Notifications and Responses.
 */
class TestPage : public AbstractPage
{
public:
    TestPage(MainTabbedComponent* parent);
    ~TestPage() override;

    /**
     * Display an incoming message on the m_incomingMessageDisplayEdit
     * 
     * @param[in] message   Message which will be converted to String for displaying.
     */
    void AddMessage(const juce::MemoryBlock& message);

    /**
     * Callback method that is triggered whenever the IP address or port are changed on the GUI by the user.
     *
     * @param[in] ipAddress New IP address string.
     * @param[in] ipPort    New port number.
     */
    std::function<void(const juce::String& ipAddress, int ipPort)> OnDeviceIpAddressChanged;


    // Reimplemented from AbstractPage

    void UpdateConnectionStatus(ConnectionStatus status) override;


    // Reimplemented from juce::Component

    void paint(juce::Graphics&) override;
    void resized() override;


protected:



private:
    // Parent TabbedComponent which contains this page component as one or more of its tabs. 
    MainTabbedComponent* m_parent;

    // Hyperlink to the project webpage.
    juce::HyperlinkButton m_hyperlink;

    // TextEditor to enter the IP address of the remote test device.
    juce::TextEditor m_ipAddressEdit;

    // TextEditor to enter the port of the remote test device.
    juce::TextEditor m_ipPortEdit;

    // TextButton misused as LED for displaying the connection status with the remote test device.
    juce::TextButton m_stateLed;

    // TextEditor to display incoming messages (Responses and Notifications).
    juce::TextEditor m_incomingMessageDisplayEdit;

    // Labels on the UI. NOTE: use a vector of pointers because juce::Label is non-copyable.
    std::vector<std::unique_ptr<juce::Label>> m_ocaLabels;

    // Placeholder button for saving the current page configuration
    juce::TextButton m_saveButton;

    // Placeholder button for loading a previously saved page configuration
    juce::TextButton m_loadButton;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestPage)
};
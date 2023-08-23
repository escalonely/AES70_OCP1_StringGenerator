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

    bool SendCommandToDevice(const juce::MemoryBlock& data);

    // Reimplemented from juce::TabbedComponent

    TabBarButton* createTabButton(const String& tabName, int tabIndex) override;
    //void currentTabChanged(int newCurrentTabIndex, const String& newCurrentTabName) override;
    //void popupMenuClickOnTab(int tabIndex, const String& tabName) override;


    // Reimplemented from juce::Component

    void paint(juce::Graphics&) override;
    void resized() override;


protected:
    /**
     * Initialize NanoOcp1Client, which will send and receive messages to & from the device
     * whenever m_sendButton is pressed.
     */
    void StartNanoOcpClient();


private:
    // OCP1 Client to handle AES70 communication with device.
    // TODO: why unique_ptr? Just put on the stack.
    std::unique_ptr<NanoOcp1::NanoOcp1Client> m_nanoOcp1Client;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainTabbedComponent)
};




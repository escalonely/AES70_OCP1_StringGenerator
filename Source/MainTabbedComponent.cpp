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

#include "MainTabbedComponent.h"
#include "TestPage.h"
#include "StringGeneratorPage.h"
#include <NanoOcp1.h>


static const juce::Colour AppBackgroundColour(43, 65, 77); // TODO: have common definition somewhere


MainTabbedComponent::MainTabbedComponent()
    : juce::TabbedComponent(juce::TabbedButtonBar::TabsAtTop)
{
    // First tab is always the "Test" tab
    addTab("Test", AppBackgroundColour, new TestPage(this), true);

    // TODO: load tabs from config file

    // Add one StringGeneratorPage tab per default.
    auto firstMainPage = new StringGeneratorPage(this);
    auto mainPageBounds = firstMainPage->getBounds();
    addTab("Page " + juce::String(getNumTabs()), AppBackgroundColour, firstMainPage, true);

    // Last tab is always the "+" tab
    addTab("+", AppBackgroundColour, new juce::Component(), true);
    getTabbedButtonBar().getTabButton(getNumTabs() - 1)->onClick = [=]()
    {
        // Clicking on the "+" tab will add a new StringGeneratorPage tab, 
        // ensuring that the "+" tab remains as the rightmost tab.
        int newTabNumber = getNumTabs() - 1;
        addTab("Page " + juce::String(newTabNumber), AppBackgroundColour, new StringGeneratorPage(this), true, newTabNumber);
        setCurrentTabIndex(newTabNumber);
    };

    // Set the first StringGeneratorPage tab as the active one per default
    setCurrentTabIndex(1);

    // Resize the MainTabbedComponent based on the size of a StringGeneratorPage component,
    // taking into account the height of the TabBar itself. TODO: get rid of magic numbers.
    setSize(mainPageBounds.getWidth() + 3, mainPageBounds.getHeight() + getTabBarDepth() + 2);

    // Initialize the NanoOcp1::NanoOcp1Client m_nanoOcp1Client. 
    StartNanoOcpClient();
}

MainTabbedComponent::~MainTabbedComponent()
{

}

TabBarButton* MainTabbedComponent::createTabButton(const String& tabName, int tabIndex)
{
    // TODO: create custom TabBarButton components depending on the tabIndex

    return juce::TabbedComponent::createTabButton(tabName, tabIndex);
}

void MainTabbedComponent::paint(juce::Graphics& g)
{
    // TODO: anything to do here? Just call base implementation for now.

    juce::TabbedComponent::paint(g);
}

void MainTabbedComponent::resized()
{
    // TODO: anything to do here? Just call base implementation for now.

    juce::TabbedComponent::resized();
}

void MainTabbedComponent::StartNanoOcpClient()
{
    m_nanoOcp1Client = std::make_unique<NanoOcp1::NanoOcp1Client>("127.0.0.1", 50014);
    m_nanoOcp1Client->onDataReceived = [=](const juce::MemoryBlock& message)
    {
        auto receivedStr = juce::String::toHexString(message.getData(), static_cast<int>(message.getSize()));

        // TODO display message on the GUI

        DBG("onDataReceived: " + receivedStr);

        return true;
    };

    m_nanoOcp1Client->onConnectionEstablished = [=]()
    {
        DBG("onConnectionEstablished");

        // TODO: show on the GUI
    };

    m_nanoOcp1Client->onConnectionLost = [=]()
    {
        DBG("onConnectionLost");

        // TODO: show on the GUI
    };

    m_nanoOcp1Client->start();
}

bool MainTabbedComponent::SendCommandToDevice(const juce::MemoryBlock& data)
{
    if (m_nanoOcp1Client && m_nanoOcp1Client->isConnected())
    {
        //m_nanoOcp1Client->sendMessage(commandMemBlock); TODO make method protected?? 
        return m_nanoOcp1Client->sendData(data);
    }

    return false;
}

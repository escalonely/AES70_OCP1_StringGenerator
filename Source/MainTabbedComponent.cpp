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
#include "Common.h"
#include <NanoOcp1.h>


static const juce::String TestPageDefaultName("Test"); // Default name to show on the page's tab.
static constexpr int TestPageTabIndex(0); // TestPage will always be the leftmost tab.


MainTabbedComponent::MainTabbedComponent()
    :   juce::TabbedComponent(juce::TabbedButtonBar::TabsAtTop),
        m_numUnreadMessages(0)
{
    // Initialize the NanoOcp1::NanoOcp1Client m_nanoOcp1Client. 
    StartNanoOcpClient();

    // Resize the MainTabbedComponent taking into account the height of the TabBar itself. 
    // TODO: get rid of magic numbers.
    setSize(AppWindowDefaultWidth + 3, AppWindowDefaultHeight + getTabBarDepth() + 2);
}

MainTabbedComponent::~MainTabbedComponent()
{
}

bool MainTabbedComponent::LoadFileViaDialog()
{
    m_fileChooser.reset(new juce::FileChooser("Select config file to LOAD...", 
                                              File::getCurrentWorkingDirectory(), 
                                              "*.xml",
                                              true /* use native */));

    auto fileChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
    m_fileChooser->launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
        {
            juce::File configFile = chooser.getResult();
            if (configFile != juce::File())
            {
                // First need to remove all current tabs.
                clearTabs();

                // Create tabs on a blank slate, based on the contents of the file.
                InitializePages(configFile);
            }
        });

    return true;
}

bool MainTabbedComponent::SaveFileViaDialog()
{
    m_fileChooser.reset(new juce::FileChooser("Select config file to SAVE to...",
                                              File::getCurrentWorkingDirectory(),
                                              "*.xml",
                                              true /* use native */));

    auto fileChooserFlags = juce::FileBrowserComponent::saveMode | 
                            juce::FileBrowserComponent::canSelectFiles | 
                            juce::FileBrowserComponent::warnAboutOverwriting;
    m_fileChooser->launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
        {
            juce::File configFile = chooser.getResult();
            if (configFile != juce::File())
            {
                CreateConfigFileFromPages(configFile);
            }
        });

    return true;
}

bool MainTabbedComponent::InitializePages(const juce::File& configFile)
{
    // First tab is always the "Test" tab
    auto testPage = new TestPage(this);
    testPage->SetConnectionStatus(0);
    addTab(TestPageDefaultName, AppBackgroundColour, testPage, true);
    testPage->OnDeviceIpAddressChanged = [=](const juce::String& ipAddress, int ipPort)
    {
        if ((ipAddress != m_nanoOcp1Client->getAddress()) || (ipPort != m_nanoOcp1Client->getPort()))
        {
            m_nanoOcp1Client->stop();
            m_nanoOcp1Client->setAddress(ipAddress);
            m_nanoOcp1Client->setPort(ipPort);
            m_nanoOcp1Client->start();
        }
    };

    // Attempt to load tabs and pages from the passed config file.
    bool parsedConfigFromFile(false);
    if (configFile.getFullPathName().isNotEmpty())
    {
        parsedConfigFromFile = CreatePagesFromConfigFile(configFile);
    }

    if (!parsedConfigFromFile)
    {
        // Add one StringGeneratorPage tab per default.
        auto genPage = new StringGeneratorPage(this);
        genPage->setName("Page " + juce::String(getNumTabs()));
        addTab(genPage->getName(), AppBackgroundColour, genPage, true);
    }

    // Last tab is always the "+" tab
    addTab("+", AppBackgroundColour, new juce::Component(), true);
    getTabbedButtonBar().getTabButton(getNumTabs() - 1)->onClick = [=]()
    {
        // Clicking on the "+" tab will add a new StringGeneratorPage tab.
        auto genPage = new StringGeneratorPage(this);
        int newTabNumber = getNumTabs() - 1;
        genPage->setName("Page " + juce::String(newTabNumber));

        // Ensure that the "+" tab remains as the rightmost tab.
        addTab(genPage->getName(), AppBackgroundColour, genPage, true, newTabNumber);
        setCurrentTabIndex(newTabNumber);
    };

    // Set the first StringGeneratorPage tab as the active one per default
    setCurrentTabIndex(1);

    return parsedConfigFromFile;
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

bool MainTabbedComponent::GetConnectionParameters(juce::String& address, int& port) const
{
    if (m_nanoOcp1Client)
    {
        address = m_nanoOcp1Client->getAddress();
        port = m_nanoOcp1Client->getPort();

        return true;
    }

    return false;
}

TabBarButton* MainTabbedComponent::createTabButton(const String& tabName, int tabIndex)
{
    // TODO: create custom TabBarButton components depending on the tabIndex

    return juce::TabbedComponent::createTabButton(tabName, tabIndex);
}

void MainTabbedComponent::currentTabChanged(int newCurrentTabIndex, const String& /*newCurrentTabName*/)
{
    if (newCurrentTabIndex == TestPageTabIndex)
    {
        // Reset tab's name to clear the "unread messages" mark.
        setTabName(TestPageTabIndex, TestPageDefaultName);
        m_numUnreadMessages = 0;
    }
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
        // Pass message to the TestPage tab for displaying.
        auto testPage = static_cast<TestPage*>(getTabContentComponent(TestPageTabIndex));
        if (testPage)
            testPage->AddMessage(message);

#if JUCE_DEBUG
        auto receivedStr = juce::String::toHexString(message.getData(), static_cast<int>(message.getSize()));
        DBG("onDataReceived: " + receivedStr);
#endif

        // Mark the TestPage's tab with "unread messages".
        if (getCurrentTabIndex() != TestPageTabIndex)
        {
            m_numUnreadMessages++;
            setTabName(TestPageTabIndex, TestPageDefaultName +
                                         juce::String(" (" + 
                                         juce::String(m_numUnreadMessages) + 
                                         juce::String(")")));
        }

        return true;
    };

    m_nanoOcp1Client->onConnectionEstablished = [=]()
    {
        DBG("onConnectionEstablished");

        // Pass connection status to TestPage tab in order to update status LED on the GUI
        auto testPage = static_cast<TestPage*>(this->getTabContentComponent(TestPageTabIndex));
        if (testPage)
            testPage->SetConnectionStatus(3);
    };

    m_nanoOcp1Client->onConnectionLost = [=]()
    {
        DBG("onConnectionLost");

        // Pass connection status to TestPage tab in order to update status LED on the GUI
        auto testPage = static_cast<TestPage*>(this->getTabContentComponent(TestPageTabIndex));
        if (testPage)
            testPage->SetConnectionStatus(0);
    };

    m_nanoOcp1Client->start();
}

bool MainTabbedComponent::CreatePagesFromConfigFile(const juce::File& configFIle)
{
    if (configFIle.existsAsFile())
    {
        auto rootXmlElement = parseXMLIfTagMatches(configFIle, "AES70CommandSet");
        if (rootXmlElement)
        {
            bool atLeastOneAdded(false);

            juce::String configVersion;
            if (rootXmlElement->hasAttribute("version"))
                configVersion = rootXmlElement->getStringAttribute("version");

            auto childXmlElement = rootXmlElement->getChildByName("AES70Command");
            while (childXmlElement)
            {
                // Create one StringGeneratorPage for each AES70Command.
                auto page = StringGeneratorPage::CreatePageFromXmlElement(childXmlElement, this);
                jassert(page != nullptr);
                if (page != nullptr)
                {
                    // Add the StringGeneratorPage tab. Use the page's component name as tabName.
                    addTab(page->getName(), AppBackgroundColour, page, true);
                    atLeastOneAdded = true;
                }

                // Advance to next AES70Command tag.
                childXmlElement = childXmlElement->getNextElementWithTagName("AES70Command");
            }

            if (atLeastOneAdded)
                return true;
        }
    }

    return false;
}

bool MainTabbedComponent::CreateConfigFileFromPages(juce::File& configFile) const
{
    XmlElement rootElement("AES70CommandSet");
    rootElement.setAttribute("version", JUCEApplicationBase::getInstance()->getApplicationVersion());
    rootElement.setAttribute("date", Time::getCurrentTime().formatted("%d.%m.%Y"));

    for (int tIdx = 0; tIdx < getNumTabs(); tIdx++)
    {
        auto genPage = dynamic_cast<StringGeneratorPage*>(getTabContentComponent(tIdx));
        if (genPage)
        {
            rootElement.addChildElement(genPage->CreateXmlElementFromPage());
        }
    }

    return rootElement.writeTo(configFile);
}
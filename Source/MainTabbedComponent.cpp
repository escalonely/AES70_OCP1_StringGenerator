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
#include "MainComponent.h"


static const juce::Colour AppBackgroundColour(43, 65, 77); // TODO: have common definition somewhere


MainTabbedComponent::MainTabbedComponent()
    : juce::TabbedComponent(juce::TabbedButtonBar::TabsAtTop)
{
    // First tab is always the "Test" tab
    addTab("Test", AppBackgroundColour, new TestPage(), true);

    // TODO: load tabs from config file

    // Add one StringGeneratorPage tab per default.
    auto firstMainPage = new MainComponent();
    auto mainPageBounds = firstMainPage->getBounds();
    addTab("Page " + juce::String(getNumTabs()), AppBackgroundColour, firstMainPage, true);

    // Last tab is always the "+" tab
    addTab("+", AppBackgroundColour, new juce::Component(), true);
    getTabbedButtonBar().getTabButton(getNumTabs() - 1)->onClick = [=]()
    {
        // Clicking on the "+" tab will add a new StringGeneratorPage tab, 
        // ensuring that the "+" tab remains as the rightmost tab.
        int newTabNumber = getNumTabs() - 1;
        addTab("Page " + juce::String(newTabNumber), AppBackgroundColour, new MainComponent(), true, newTabNumber);
        setCurrentTabIndex(newTabNumber);
    };

    // Set the first StringGeneratorPage tab as the active one per default
    setCurrentTabIndex(1);

    // Resize the MainTabbedComponent based on the size of a StringGeneratorPage component,
    // taking into account the height of the TabBar itself. TODO: get rid of magic numbers.
    setSize(mainPageBounds.getWidth() + 3, mainPageBounds.getHeight() + getTabBarDepth() + 2);
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
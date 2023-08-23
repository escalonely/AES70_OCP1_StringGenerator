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

#include "TestPage.h"
#include "MainTabbedComponent.h"


TestPage::TestPage(MainTabbedComponent* parent)
    :   m_parent(parent)
{
    jassert(parent != nullptr);

    // TODO: create GUI elements

    setSize(10, 10);
}

TestPage::~TestPage()
{

}

void TestPage::paint(juce::Graphics& g)
{
    // TODO: anything to do here? Just call base implementation for now.

    juce::Component::paint(g);
}

void TestPage::resized()
{
    // TODO: anything to do here? Just call base implementation for now.

    juce::Component::resized();
}
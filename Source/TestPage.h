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
 * Component for testing the generated OCP.1 binary strings against real AES70-capable
 * devices via TCP/IP. This page provides GUI elements for TCP connection configuration,
 * as well as fields for monitoring the incoming OCP.1 Notifications and Responses.
 */
class TestPage : public juce::Component
{
public:
    TestPage();
    ~TestPage() override;


    // Reimplemented from juce::Component

    void paint(juce::Graphics&) override;
    void resized() override;


protected:



private:


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestPage)
};
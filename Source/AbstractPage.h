/*
===============================================================================

 Copyright (C) 2024 Bernardo Escalona. All Rights Reserved.

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
class MainTabbedComponent;
enum class ConnectionStatus;


/**
 * Base abstract class for all tab pages in the app.
 * It inherits from juce::Viewport to allow scrolling through the contents inside.
 */
class AbstractPage : public juce::Viewport
{
public:
    AbstractPage(MainTabbedComponent* const /*parent*/)
    {
        // TODO: parent as member
    };

    ~AbstractPage() override
    {
    };

    /**
     * Update any GUI elements which depend on the current ConnectionStatus 
     * with the remote test devce.
     *
     * @param[in] status ConnectionStatus of the internal NanoOcpClient.
     */
    virtual void UpdateConnectionStatus(ConnectionStatus status) = 0;
};


/**
 * Dummy empty page. Used for the "+" tab.
 */
class DummyPage : public AbstractPage
{
public:
    DummyPage(MainTabbedComponent* const parent)
        : AbstractPage(parent)
    {
    };

    ~DummyPage() override
    {
    };


    // Reimplemented from AbstractPage

    void UpdateConnectionStatus(ConnectionStatus /*status*/) override
    {
    };
};

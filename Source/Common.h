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


static const juce::Colour AppBackgroundColour(43, 65, 77);          // App window background color
static const juce::Colour LabelEnabledTextColour(125, 182, 212);    // Color for labels of relevant controls
static const juce::Colour LabelDisabledTextColour(12, 12, 12);      // Color for labels of non-relevant controls

static const juce::String ProjectHostShortURL("https://github.com/escalonely"); // Web address of this project's host.
static const juce::String ProjectHostLongURL("https://github.com/escalonely/AES70_OCP1_StringGenerator"); // Complete web address of this project's host.

static constexpr int AppWindowDefaultWidth = 640; // Default App window width.
static constexpr int AppWindowDefaultHeight = 480; // Default App window height, also used for m_container within the juce::Viewport.

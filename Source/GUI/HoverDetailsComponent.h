/*
===============================================================================

 Copyright (C) 2025 Bernardo Escalona. All Rights Reserved.

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


// Forward declarations.
class HoverDetailsComponent;


/**
 * Struct describing the byte position and the type of PDU field 
 * currently hovered with the mouse over the HoverSensitiveTextEditor Component 
 * and whose Details and then shown on the DetailsDisplay Component.
 */
struct Details
{
    /**
     * Start and end position of the featured field, in bytes.
     */
    juce::Range<int> m_position = juce::Range<int>(0, 0);

    /**
     * Type of field. See field codes defined in CustomOcp1Message.h. 
     */
    std::uint8_t m_fieldType;
};


/**
 * TextEditor which calls its UpdateDetails method every time it receives a MouseEvent.
 */
class HoverSensitiveTextEditor : public juce::TextEditor
{
public:
    /**
     * Constructor.
     *
     * @param[in] parent    Parent Component.
     */
    HoverSensitiveTextEditor(HoverDetailsComponent& parent);

protected:
    // Overriden from juce::TextEditor

    void mouseMove(const MouseEvent& event) override;
    void mouseEnter(const MouseEvent& event) override;
    void mouseExit(const MouseEvent& event) override;

    /**
     * After a mouse movement, triggers the parent Component to compute the
     * PDU field code details at the new mouse position.
     * 
     * @param[in] event Contains the new mouse position.
     */
    void UpdateDetails(const MouseEvent& event);

private:
    /**
     * Index of the PDU string character closest to the current mouse position.
     */
    int m_nearestCharIdx;

    /**
     * Reference to the parent Component.
     */
    HoverDetailsComponent& m_parent;
};


/**
 * Read-only TextEditor specialized for displaying PDU field code Details.
 */
class DetailsDisplay : public juce::TextEditor
{
public:
    /**
     * Constructor.
     * 
     * @param[in] parent    Parent Component.
     */
    DetailsDisplay(HoverDetailsComponent& parent);

    /**
     * Sets the field code Details to be displayed.
     */
    void SetDetails(const Details& d);

private:
    /**
     * Reference to the parent Component.
     */
    HoverDetailsComponent& m_parent;
};


/**
 * Component made out of a HoverSensitiveTextEditor and a DetailsDisplay 
 */
class HoverDetailsComponent : public juce::Component
{
public:
    /**
     * Constructor.
     */
    HoverDetailsComponent(const String& componentName);

    /**
     * Sets the "field code" representation of the same PDU currently 
     * displayed by m_editor.
     * 
     * @param[in] fieldCodeData MamoryBlock containing the field code representation 
     *                          of the PDU.
     */
    void SetFieldCodeData(const juce::MemoryBlock& fieldCodeData)
    {
        m_fieldCodeData = fieldCodeData;
    }

    /**
     * Determines the Details about the PDU field at the given position in bytes,
     * displays these Details in m_display, and then returns them. 
     *
     * @param[in] position  The desired position in bytes within the PDU.
     * @return Details about the PDU field at the given position.
     */
    Details DisplayDetailsAt(int position);

    /**
     * TextEditor sensitive to hovering with the mouse.
     */
    HoverSensitiveTextEditor m_editor;

    /**
     * Sub-component to show the Details of the PDU field hovered.
     */
    DetailsDisplay m_display;

protected:
    // Reimplemented from juce::Component
    void resized() override;

private:
    /**
     * Buffer which always holds the same PDU as m_editor, but in "field code"
     * representation. See field codes defined in CustomOcp1Message.h
     */
    juce::MemoryBlock m_fieldCodeData;
};
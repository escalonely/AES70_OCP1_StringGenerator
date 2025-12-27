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

#include "HoverDetailsComponent.h"
#include "../CustomOcp1Message.h"


// ---- HoverSensitiveTextEditor ---- //

juce::String Details::ToString() const
{
    juce::String ret = FieldPrefixString(m_fieldType) + "." +
                       FieldNameString(m_fieldType) + "\n" +
                       "Length: " +
                       juce::String(m_position.getLength() + 1);

    return ret;
}


// ---- HoverSensitiveTextEditor ---- //

HoverSensitiveTextEditor::HoverSensitiveTextEditor(HoverDetailsComponent& parent)
    :   juce::TextEditor(),
        m_parent(parent),
        m_nearestCharIdx(-1)
{

}

void HoverSensitiveTextEditor::mouseMove(const MouseEvent& event)
{
    UpdateDetails(event);

    return juce::TextEditor::mouseMove(event);
}

void HoverSensitiveTextEditor::mouseEnter(const MouseEvent& event)
{
    UpdateDetails(event);

    return juce::TextEditor::mouseEnter(event);
}

void HoverSensitiveTextEditor::mouseExit(const MouseEvent& event)
{
    UpdateDetails(event);
    //m_nearestCharIdx = -1;
    //setHighlightedRegion({ 0, 0 });

    return juce::TextEditor::mouseExit(event);
}

void HoverSensitiveTextEditor::UpdateDetails(const MouseEvent& event)
{
    // If TextEditor is empty, nothing to do.
    if (getText().length() < 1)
        return;

    // If mouse is not hovering over any of the text inside the TextEditor, nothing to do.
    auto textRectList = getTextBounds(juce::Range<int>(0, getText().length()));
    if (!textRectList.containsPoint(event.getPosition()))
    {
        //DBG("UpdateDetails: clearing details.");
        setHighlightedRegion({ 0, 0 });
        m_parent.ClearDetails();
        return; 
    }

    // No change
    auto nearestCharIdx = getCharIndexForPoint(event.getPosition());
    if (nearestCharIdx == m_nearestCharIdx)
        return;

    // Account for empty space every 3rd character.
    m_nearestCharIdx = nearestCharIdx;
    int bytePos = static_cast<int>((m_nearestCharIdx - (m_nearestCharIdx / 3.0f)) / 2); 

    auto details = m_parent.DisplayDetailsAt(bytePos);

    //DBG(juce::String(": m_nearestCharIdx=") + juce::String(m_nearestCharIdx) + 
    //    ", bytePos=" + juce::String(bytePos) + 
    //    ", fieldType=" + juce::String(details.m_fieldType));

    // TODO: move to helper, use also to clear
    // Determine the text region to highlight.
    int startHl = details.m_position.getStart() * 3;
    int stopHl = details.m_position.getEnd() * 3 + 2;
    setHighlightedRegion({ startHl, stopHl });
}


// ---- DetailsDisplay ---- //

DetailsDisplay::DetailsDisplay(HoverDetailsComponent& parent)
    :   juce::TextEditor(),
        m_parent(parent)
{
    setHasFocusOutline(true);
    setReadOnly(true);
    setCaretVisible(false);
    //setColour(juce::TextEditor::highlightColourId, juce::Colours::red);
    setMultiLine(true, true);
}

void DetailsDisplay::SetDetails(const Details& d)
{
    setText(d.ToString());
}


// ---- HoverDetailsComponent ---- //

HoverDetailsComponent::HoverDetailsComponent(const String& componentName)
    :   juce::Component(componentName),
        m_editor(HoverSensitiveTextEditor(*this)),
        m_display(DetailsDisplay(*this))
{
    addAndMakeVisible(m_editor);
    addAndMakeVisible(m_display);
}

Details HoverDetailsComponent::DisplayDetailsAt(int position)
{
    Details ret;
    if ((position < 0) || (position >= m_fieldCodeData.getSize()))
    {
        DBG("DisplayDetailsAt: out of bounds -> clearing.");
        m_display.clear();
        return ret;
    }

    // Walk m_fieldCodeData in both directions in order to find the range of the field at position.
    auto fieldType = m_fieldCodeData[position];
    int start(position);
    int end(position);
    while ((start > 0) && (m_fieldCodeData[start - 1] == fieldType))
        start--;
    while ((end < m_fieldCodeData.getSize() - 1) && (m_fieldCodeData[end + 1] == fieldType))
        end++;

    ret.m_position.setStart(start);
    ret.m_position.setEnd(end);
    ret.m_fieldType = fieldType;

    m_display.SetDetails(ret);

    return ret;
}

void HoverDetailsComponent::ClearDetails()
{
    m_display.clear();
}

void HoverDetailsComponent::resized()
{
    auto bounds = getLocalBounds();
    //auto controlHeight = bounds.getHeight();
    auto editorWidth = int(bounds.getWidth() * 0.771);
    auto margin = 2;

    auto editorBounds = bounds.removeFromRight(editorWidth + margin);
    m_editor.setBounds(editorBounds.reduced(margin));
    m_display.setBounds(bounds.reduced(margin));
}
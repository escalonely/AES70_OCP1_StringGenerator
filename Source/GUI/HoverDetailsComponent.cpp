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
                       "Length: " + juce::String(m_position.getLength() + 1) + "\n" +
                       "Value: " + m_fieldValue.ToString();

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
    addChildComponent(m_display);

    // TODO: setting below does not combine well with setVisible
    setPaintingIsUnclipped(true); // To allow m_display to be drawn outside bounds.
}

void HoverDetailsComponent::SetData(const juce::MemoryBlock& pduData, const juce::MemoryBlock& fieldCodeData)
{
    m_pduData = pduData;
    m_fieldCodeData = fieldCodeData;

    // Convert PDU data to hex string representation for display.
    juce::String commandString = juce::String::toHexString(pduData.getData(), static_cast<int>(pduData.getSize()));
    m_editor.setText(commandString, false);
}

Details HoverDetailsComponent::DisplayDetailsAt(int position)
{
    Details ret;
    if ((position < 0) || (position >= m_fieldCodeData.getSize()))
    {
        DBG("DisplayDetailsAt: out of bounds -> clearing.");
        ClearDetails();
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


    // Extract field value from m_pduData.
    std::vector<std::uint8_t> parameterData = std::vector<std::uint8_t>(
        static_cast<std::uint8_t*> (m_pduData.getData()) + start,
        static_cast<std::uint8_t*> (m_pduData.getData()) + end + 1
    );

    bool ok = false;
    switch(fieldType)
    {
        // 1-byte fields
        case Char_Hdr_SyncVal:
        case Char_Cmd_ParamCount:
        case Char_Hdr_MessageType:
        {
            auto intValue = NanoOcp1::DataToUint8(parameterData, &ok);
            ret.m_fieldValue = NanoOcp1::Variant(intValue);
            break;
        }

        // 2-byte fields
        case Char_Hdr_ProtoVers:
        case Char_Hdr_MessageCount:
        case Char_Cmd_MethodDefLevel:
        case Char_Cmd_MethodIndex:
        {
            auto intValue = NanoOcp1::DataToUint16(parameterData, &ok);
            ret.m_fieldValue = NanoOcp1::Variant(intValue);
            break;
        }

        // 4-byte fields
        case Char_Hdr_MessageSize:
        case Char_Cmd_Size:
        case Char_Cmd_Handle:
        case Char_Cmd_ONo:
        {
            auto intValue = NanoOcp1::DataToUint32(parameterData, &ok);
            ret.m_fieldValue = NanoOcp1::Variant(intValue);
            break;
        }

        case Char_Cmd_ParamData:
        {
            ok = true;
            ret.m_fieldValue = NanoOcp1::Variant(parameterData /*, NanoOcp1::OCP1DATATYPE_STRING*/);
            break;
        }
        break;
    }

    jassert(ok);
    m_display.setVisible(true);
    m_display.SetDetails(ret);

    return ret;
}

void HoverDetailsComponent::ClearDetails()
{
    m_display.clear();
    m_display.setVisible(false);
}

void HoverDetailsComponent::resized()
{
    auto bounds = getLocalBounds();
    auto margin = 2;

    auto editorBounds = bounds.reduced(margin);
    m_editor.setBounds(editorBounds);

    //auto detailsHeight = juce::jmax(32, juce::roundToInt(m_display.getTextHeight() * 3.0f));
    auto displayBounds = editorBounds.removeFromRight(static_cast<int>(bounds.getWidth() * 0.4f)).reduced(margin);
    m_display.setBounds(displayBounds);
}
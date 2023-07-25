/*
===============================================================================

Copyright (C) 2023 Bernardo Escalona. All Rights Reserved.

  This file is part of AES70_OCP1_StringGenerator, found at:
  https://github.com/escalonely/AES70_OCP1_StringGenerator

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

===============================================================================
*/

#pragma once

#include <JuceHeader.h>


/**
 * Forward declarations.
 */
namespace AES70
{
    struct OcaRoot;
}


/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    // Reimplemented from juce::Component

    void paint (juce::Graphics&) override;
    void resized() override;


protected:
    /**
     * Reset GUI components.
     * @param[in] step  Determines which GUI controls are reset. 
     */
    void ResetComponents(int step);

    /**
     * Re-create the components for entering the values for Set commands and Get responses,
     * m_ocaSetCommandValueComponent and m_ocaResponseValueComponent.
     */
    void CreateValueComponents();

    /**
     * Re-create the Command and Response binary strings, and display them on both TextEditors:
     * m_ocaCommandTextEditor and m_ocaResponseTextEditor.
     */
    void UpdateBinaryStrings();

    /**
     * Re-create the Command and Response binary strings, based on the current configuration of the
     * OCA class, ONo, Property, and Command GUI controls.
     * 
     * @parameter[out] commandString    The resulting Command binary string.
     * @parameter[out] responseString   The resulting Response binary string.
     * @return  True if both strings could be generated successfully.
     */
    bool CreateCommandAndResponseStrings(juce::String& commandString, juce::String& responseString);


private:
    // TextEditor to enter the AES70/OCA object number
    std::unique_ptr<juce::TextEditor> m_ocaONoTextEditor;

    // ComboBox to select AES70/OCA class
    std::unique_ptr<juce::ComboBox> m_ocaClassComboBox;

    // ComboBox to select AES70/OCA class property
    std::unique_ptr<juce::ComboBox> m_ocaPropertyComboBox;

    // ComboBox to select AES70/OCA command
    std::unique_ptr<juce::ComboBox> m_ocaCommandComboBox;

    // Label above the m_ocaCommandTextEditor
    std::unique_ptr<juce::Label> m_ocaCommandLabel;

    // Component to enter values for Set methods
    std::unique_ptr<juce::Component> m_ocaSetCommandValueComponent;

    // TextEditor to display AES70/OCA command
    std::unique_ptr<juce::TextEditor> m_ocaCommandTextEditor;

    // Label above the m_ocaResponseTextEditor
    std::unique_ptr<juce::Label> m_ocaResponseLabel;

    // ComboBox to select AES70/OCA response status
    std::unique_ptr<juce::ComboBox> m_ocaResponseStatusComboBox;

    // Component to enter values for Get method responses
    std::unique_ptr<juce::Component> m_ocaResponseValueComponent;

    // TextEditor to display AES70/OCA response
    std::unique_ptr<juce::TextEditor> m_ocaResponseTextEditor;

    // AES70/OCA object representing the current configuraion on the GUI
    std::unique_ptr<AES70::OcaRoot> m_ocaObject;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

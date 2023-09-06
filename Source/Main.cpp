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

#include <JuceHeader.h>
#include "MainTabbedComponent.h"

class AES70_OCP1_StringGeneratorApplication  : public juce::JUCEApplication
{
public:
    AES70_OCP1_StringGeneratorApplication() {}

    const juce::String getApplicationName() override       { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    void initialise (const juce::String& /*commandLine*/) override
    {
        m_mainWindow.reset(new MainWindow());

        // Create and initialize the tabs in the mainWindow's TabbedComponent.
        m_mainWindow->InitializeTabbedComponent();
    }

    void shutdown() override
    {
        m_mainWindow = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const juce::String& /*commandLine*/) override
    {
    }

    /*
        This class implements the desktop window that contains an instance of
        our MainTabbedComponent class.
    */
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow()
            : DocumentWindow(juce::String(),
                             juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                         .findColour(juce::ResizableWindow::backgroundColourId),
                             DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainTabbedComponent(), true);

           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen (true);
           #else
            setResizable (true, true);
            setResizeLimits(getWidth(), getHeight(), 3840, 2160);
            centreWithSize (getWidth(), getHeight());
           #endif

            setVisible (true);
        }

        void closeButtonPressed() override
        {
            // This is called when the user tries to close this window. 
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

        void InitializeTabbedComponent()
        {
            // Check if a config file was given in via the commandline.
            juce::File configFile;
            juce::ArgumentList argList("executable", JUCEApplicationBase::getCommandLineParameterArray());
            if (argList.containsOption("-o"))
                configFile = argList.getFileForOption("-o");

            // Create tabs and pages (using the config file if available).
            auto mainComponent = static_cast<MainTabbedComponent*>(getContentComponent());
            bool parsedFile = mainComponent->InitializePages(configFile);

            // Set the app window name. If successfully opened, add the name of the config file.
            juce::String windowName(JUCEApplicationBase::getInstance()->getApplicationName());
            if (parsedFile)
                windowName = configFile.getFileName() + " - " + windowName;
            setName(windowName);
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> m_mainWindow;
};

// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (AES70_OCP1_StringGeneratorApplication)

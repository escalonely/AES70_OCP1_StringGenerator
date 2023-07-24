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
#include "Ocp1Message.h"


namespace AES70
{

enum ClassIdx
{
    OCA_ROOT = 1,
    OCA_WORKER,
    OCA_ACTUATOR,
    OCA_SWITCH,
    OCA_MUTE,
    OCA_SENSOR,
    OCA_BASIC_SENSOR,
    OCA_BOOLEAN_SENSOR,
    OCA_AGENT
};

static const std::map<int, juce::String> MapOfClassNames = {
    { OCA_ROOT,             "OcaRoot" },
    { OCA_WORKER,           " +-OcaWorker" },
    { OCA_ACTUATOR,         " |  +-OcaActuator" },
    { OCA_SWITCH,           " |  |  +-OcaSwitch" },
    { OCA_MUTE,             " |  |  +-OcaMute" },
    { OCA_SENSOR,           " |  +-OcaSensor" },
    { OCA_BASIC_SENSOR,     " |  |  +-OcaBasicSensor" },
    { OCA_BOOLEAN_SENSOR,   " |  |  |  +-OcaBooleanSensor" },
    { OCA_AGENT,            " +-OcaAgent" }
};

struct Property
{
    int m_defLevel;
    int m_index;
    int m_type;
    juce::String m_name;
    int m_getMethodIdx;
    int m_setMethodIdx;
};

struct OcaRoot
{
    static OcaRoot* Create(int classIdx);
 
    virtual juce::String Name() const
    {
        return "OcaRoot";
    }

    virtual int DefLevel() const
    {
        return 1;
    }

    virtual std::vector<Property> GetProperties() const
    {
        std::vector<Property> ret;
        ret.push_back({
            OcaRoot::DefLevel(), 5, NanoOcp1::OCP1DATATYPE_STRING, "Role", 5, 1
            });
        return ret;
    }
};


struct OcaWorker : public OcaRoot
{
    juce::String Name() const override
    {
        return "OcaWorker";
    }

    int DefLevel() const override
    {
        return OcaRoot::DefLevel() + 1;
    }

    std::vector<Property> GetProperties() const override
    {
        std::vector<Property> ret = OcaRoot::GetProperties();
        ret.push_back({
            OcaWorker::DefLevel(), 1, NanoOcp1::OCP1DATATYPE_BOOLEAN, "Enabled", 1, 2
            });
        return ret;
    }
};




OcaRoot* OcaRoot::Create(int classIdx)
{
    OcaRoot* obj(nullptr);
    switch (classIdx)
    {
    case OCA_ROOT:
        obj = new OcaRoot;
        break;
    case OCA_WORKER:
        obj = new OcaWorker;
        break;
    }

    jassert(obj != nullptr); // Missing implementation!
    return obj;
}


}
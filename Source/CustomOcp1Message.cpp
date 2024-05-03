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

#include "CustomOcp1Message.h"


CustomOcp1CommandResponseRequired::CustomOcp1CommandResponseRequired(const NanoOcp1::Ocp1CommandDefinition& def,
                                                                     std::uint32_t& handle)
    : Ocp1CommandResponseRequired(def, handle)
{
}

std::vector<std::uint8_t> CustomOcp1CommandResponseRequired::GetSerializedData()
{
    std::vector<std::uint8_t> serializedData = GetDummyHeaderSerializedData();

    serializedData.push_back(Char_Cmd_Size);
    serializedData.push_back(Char_Cmd_Size);
    serializedData.push_back(Char_Cmd_Size);
    serializedData.push_back(Char_Cmd_Size);
    serializedData.push_back(Char_Cmd_Handle);
    serializedData.push_back(Char_Cmd_Handle);
    serializedData.push_back(Char_Cmd_Handle);
    serializedData.push_back(Char_Cmd_Handle);
    serializedData.push_back(Char_Cmd_ONo);
    serializedData.push_back(Char_Cmd_ONo);
    serializedData.push_back(Char_Cmd_ONo);
    serializedData.push_back(Char_Cmd_ONo);
    serializedData.push_back(Char_Cmd_MethodDefLevel);
    serializedData.push_back(Char_Cmd_MethodDefLevel);
    serializedData.push_back(Char_Cmd_MethodIndex);
    serializedData.push_back(Char_Cmd_MethodIndex);
    serializedData.push_back(Char_Cmd_ParamCount);
    for (size_t i = 0; i < m_parameterData.size(); i++)
    {
        serializedData.push_back(Char_Cmd_ParamData);
    }

    return serializedData;
}

std::vector<std::uint8_t> CustomOcp1CommandResponseRequired::GetDummyHeaderSerializedData() const
{
    return 
    {
        Char_Hdr_SyncVal,
        Char_Hdr_ProtoVers,
        Char_Hdr_ProtoVers,
        Char_Hdr_MessageSize,
        Char_Hdr_MessageSize,
        Char_Hdr_MessageSize,
        Char_Hdr_MessageSize,
        Char_Hdr_MessageType,
        Char_Hdr_MessageCount,
        Char_Hdr_MessageCount,
    };
}

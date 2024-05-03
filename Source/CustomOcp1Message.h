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

#include <Ocp1Message.h>


static constexpr std::uint8_t Char_Hdr_SyncVal = static_cast<std::uint8_t>('3');
static constexpr std::uint8_t Char_Hdr_ProtoVers = static_cast<std::uint8_t>('V');
static constexpr std::uint8_t Char_Hdr_MessageSize = static_cast<std::uint8_t>('S');
static constexpr std::uint8_t Char_Hdr_MessageType = static_cast<std::uint8_t>('T');
static constexpr std::uint8_t Char_Hdr_MessageCount = static_cast<std::uint8_t>('C');

static constexpr std::uint8_t Char_Cmd_Size = static_cast<std::uint8_t>('s');
static constexpr std::uint8_t Char_Cmd_Handle = static_cast<std::uint8_t>('h');
static constexpr std::uint8_t Char_Cmd_ONo = static_cast<std::uint8_t>('o');
static constexpr std::uint8_t Char_Cmd_MethodDefLevel = static_cast<std::uint8_t>('l');
static constexpr std::uint8_t Char_Cmd_MethodIndex = static_cast<std::uint8_t>('i');
static constexpr std::uint8_t Char_Cmd_ParamCount = static_cast<std::uint8_t>('c');
static constexpr std::uint8_t Char_Cmd_ParamData = static_cast<std::uint8_t>('d');


/**
 * Subclass of NanoOcp1::Ocp1CommandResponseRequired with custom GetSerializedData implementation.
 */
class CustomOcp1CommandResponseRequired : public NanoOcp1::Ocp1CommandResponseRequired
{
public:
    /**
     * Class constructor that takes parameters via a Ocp1CommandDefinition struct.
     */
    CustomOcp1CommandResponseRequired(const NanoOcp1::Ocp1CommandDefinition& def,
                                      std::uint32_t& handle);

    /**
     * Reimplemented from Ocp1Message to write a byte verctor containing ASCII codes 
     * to represent the OCP1 fields instead of actual data.
     */
    std::vector<std::uint8_t> GetSerializedData() override;


protected:
    /**
     * Returns a byte verctor containing ASCII codes to represent the fields 
     * in an OCP1 header instead of actual data.
     */
    std::vector<std::uint8_t> GetDummyHeaderSerializedData() const;
};

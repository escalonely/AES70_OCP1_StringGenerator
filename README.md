![TitleBar.png](Resources/Documentation/TitleBar.png "0x3b")

# AES70 OCP.1 PDU Generator

The AES70 OCP.1 PDU Generator is a privately created and driven project.

## Description

- The AES70 OCP.1 PDU Generator is a companion and debugging tool for integrating AES70-capable devices into a remote control solution.
- It is able to generate AES70/OCA protocol data units (PDUs) for TCP-network-based control (OCP.1) quickly and on the fly after adjusting only a handful of parameters.
- The textual representation of the resulting PDUs can be used by integrators for controlling and monitoring AES70-capable devices, e.g. by copy-pasting them into a script.

For more info in AES70 and/or OCA, visit https://ocaalliance.com/what-is-aes70/

## Quick Start

![GuiOverview01.png](Resources/Documentation/GuiOverview01.png "GUI Overview")

1. Start by selecting the parameters of the AES70 Command that you wish to generate.
2. Enter the unique object number (ONo) of the AES70 object within the device.
3. Copy the resulting binary string into your integration environment so that it can be transmitted via TCP/IP to the AES70-capable device in your network.
4. Use the generated Response string to help decode the reply from the device to the above Command.

## Video Tutorial

Video Tutorial Part 1: Quick Start

[![Video Tutorial Part 1: Quick Start](https://img.youtube.com/vi/GOEGQvrscU8/0.jpg)](https://www.youtube.com/watch?v=GOEGQvrscU8)

## User Guide

For a more in-depth look please check the [User Guide](Resources/Documentation/UserGuide.md).

## Submodules

- JUCE: https://github.com/juce-framework/JUCE
- NanoOcp: https://github.com/ChristianAhrens/NanoOcp


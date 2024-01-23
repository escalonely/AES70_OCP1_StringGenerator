![TitleBar.png](Resources/Documentation/TitleBar.png "0x3b")

# AES70 OCP.1 PDU Generator - User Guide

## What is this tool for?

The AES70 OCP.1 PDU Generator is a companion and debugging tool for integrating AES70-capable devices into remote control solutions.

It generates binary strings or PDUs for a variety of common Commands defined by the AES70 standard. 

These PDUs are represented as text in hexadecimal notation. This text can then be copy-and-pasted into the control environment being used. A LUA or python script may be used to send these PDUs over a TCP connection, for example.

## Getting started

An AES70-capable device hosts a number of AES70 "objects" which can be accessible over the network. These objects represent the state of the device, and can be used as an interface to interact with the device.

### Class

Every AES70 object belongs to a specific class. The class of an object determines what functionality it offers. 
There are many classes defined by AES70. This tool currently only supports the most common ones, but more will be added in the future.

> It is important to know that the AES70 class structure is hierarchical. This means that every class is a specialization of a more generic "parent" class. 
> A sub-class inherits all the functionality and Properties from its parent class.

### ONo

The next attribute required to interact with a given object is its object number, or ONo for short.

Every object hosted by an AES70-capable device has a unique ONo that is used to identify that object. 

> Keep in mind that a device can host several objects of the same class, but each instance of that class will have its own ONo.
> For example, a two-channel amplifier could have two OcaGain objects, one for channel A with the ONo 10000, and one for channel B with the ONo 10001. 

Aside from a few reserved numbers / number range, it is up to the manufacturer to choose whatever ONos they want to use in their AES70-capable device. 

In order to integrate with a particular device, the required ONos need to be obtained from the manufacturer.

### Property

Next, the Property which is to be queried or modified needs to be specified.

Each AES70 class defines a number of so-called Properties that are used to represent a state or a setting in the physical device.

Additionally, each AES70 class inherits all the Properties defined by its parent classes.

Selecting a Property from the list will show the level within the AES70 class hierarchy at which this Property was defined, as well as the data type used.

> For example, OcaGain defines the "Gain" Property used to represent a gain value in dB. But it also defines the "Enabled" Property which is inherited from OcaWorker, and the "Role" Property which is inherited from OcaRoot. 

### SetValue Command

Each AES70 class also defines a number of Commands or methods, which can be invoked on objects of that class. The most common Commands are to get and to set the value of each Property. 

Having selected the SetValue Command, the tool will provide a UI element that can be used to specify the value which the selected Property should be set to. 

### Binary string / PDU

The first of the two large fields at the bottom of the tool contains a textual, hex-based representation of the resulting TCP/IP binary string or PDU. This is what would need to be sent to the device in order to perform the desired Command.

> Note that the generated PDU needs to be sent as binary and not as text, as done in other protocols such as OSC.

The binary string or PDU can be copied over into a script, a piece of code, or whatever integration environment is being used.

### Response

In addition to the Command PDU, the AES70 OCP.1 PDU Generator tool also generates a simulated Response PDU, shown on the lowermost textfield.

This Response is what a real AES70-capable device is expected to send back as an answer to the above Command.

The Response for a SetValue Command contains really only one piece of interesting information: the Response Status. 

> For example: if an invalid or non-existing ONo was used in the Command, the device is expected to respond with a "Bad Ono" Status.
> This Status can be selected on the tool's UI in order to simulate such an error scenario.
> This reverse-engineering of Responses can help to debug Commands until the "OK" Status is obtained, meaning that the device was able to perform the Command successfully.

### GetValue Command and Response

If the GetValue Command is selected instead, the Command's PDU becomes very simple: please provide the Property's current value. 

The Response PDU on the other hand, is now expected to contain the requested value. 

The tool provides a UI element that can be used to set the value which the Response should simulate. This feature of the tool is meant to help decode actual Responses received from real AES70-capable devices.

> Note that the requested value will not be included in a Response if the Status is not OK.

## Testing Commands

Next to the Command PDU the tool features a Test button, which is initially disabled. 

To enable it, the AES70 OCP.1 PDU Generator tool needs to be connected to a test device on the network. 

This can be configured on the Test page. Simply enter the device's IP address and the TCP port number that the device uses to listen to AES70/OCA messages.

> AES70 defines a recommended range to be used by manufacturers for the OCP.1 listening port of their devices. 
> Information about the port used by the actual test device needs to be obtained from the device's manufacturer.

The tool will attempt to establish a connection and will display the connection state on the Test page at all times. 

Once the test device is Online, the Test button on the string generator page will become enabled.

Clicking on the Test button will send the configured Command to the remote device.

If the device at the other end is AES70-capable, it will respond to this Command. 

The number shown on the Test tab will indicate how many AES70 Responses have been received since the last time the Test page was viewed. 

Switching to the Test tab will reset the number of unread Responses. 

On the field at the bottom of the Test page, all received Responses will be displayed with their corresponding time stamps. 

Keep in mind that the Responses shown here were actually sent by the test device, and not simply generated or simulated by the tool, like the expected Response PDU shown below the Command.

Comparing the real and the expected Responses can help to debug Commands and reverse-engineer Responses.

### Command Handles

*Coming soon...*

## Subscriptions and Notifications

*Coming soon...*

## Custom Classes

*Coming soon...*

## Configuration Files

*Coming soon...*


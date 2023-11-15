
# AES70 OCP.1 Binary String Generator - User Guide

## What is this tool for?

If we are trying to integrate AES70-capable devices into our remote control solution, this tool will generate the binary strings or PDUs necessary for whatever commands we want to send to them.

These PDUs are represented in hex notation as text that can be copy-and-pasted into the control environment being used. We may need to use a LUA or python script to send these over a TCP socket connection, for example.

## Getting started

An AES70-capable device hosts a number of AES70 "objects" which can be accessible over the network. These objects represent the state of the device, and can be used as an interface to interact with the device.

To start using this tool, we need to know exactly which object we wish to interact with. 

### Class

Every AES70 object belongs to a specific class. The class of an object determines what functionality it offers. 
There are many classes defined by AES70. This tool currently only supports the most common ones, but more will be added in the future.

> It is important to know that the AES70 class structure is hierchical. This means that every class is a specialization of a more generic "parent" class. 
> A sub-class inherits all the functionality and properties from its parent class.

So let's say we want to use this tool to generate a command to set the ouput channel gain setting on our AES70-capable amplifier. In this case we select OcaGain from the list.

### ONo

The next thing we need to know about the object we wish to interact with is its object number, or ONo for short.

Every object hosted by a AES70-capable device has a unique ONo that is used to identify that object. We can think of it as a kind of object ID.

> Keep in mind that a device can host several objects of the same class, but each instance of that class needs to have its own ONo.
> For example, a two-channel amplifier could have two OcaGain objects, one for channel A with the ONo 10000, and one for channel B with the ONo 10001. 

Aside from a few reserved numbers / number range, it is up to the manufacturer to choose whatever ONos they want to use in their AES70-capable device. So if we want to integrate with a particular device we need to obtain the required ONos from the manufacturer.

### Property

Next we need to specify which property we want to query or modify in the specified object.

Each AES70 class defines a number of so-called properties that are used to represent a state or a setting in the physical device.
Additionally, each AES70 class inherits all the properties defined by its parent classes.

> TODO: number coordinates
> 

Selecting a property from the list will show us the level within the AES70 class hierarchy at which this property was defined, as well as the data type used by the property.

For example, OcaGain defines the "Gain" property used to represent a gain value in dB. But it also defines the "Enabled" property which is inherited from OcaWorker, and the "Role" property which is inherited from OcaRoot. Let's stay with Gain for now.

### SetValue Command

Each AES70 class also defines a number of commands or methods, which can be invoked on objects of that class. The most common commands are to get and to set the value of the selected property. 

If we select SetValue, we can then specify the value which we want to set the selected property to. Drag the slider to adjust the value, or click on the slider to type in an exact number.

As you change the value you can already see the binary string below changing aswell.

### Binary string

The first of the two large fields below is the result of our configuration above. It contains a textual hex-based representation of the TCP/IP message, which needs to be sent to the device to perform the desired command.

The binary string or PDU will need to be copied from here into a script, a piece of code, or whatever integration environment we are using.

This tool offers the possibility to immediately test the command with a real connected device. But we will come back to this later. TODO where?

### Response

Assuming we were to send this command to a real AES70-capable device, the bottom field shows the PDU that we would receive from the device as a response to our command.

The response for a SetValue command contains really only one piece of interesting information: the response status. If we used an invalid ONo in our command, for example, the device would respond with a "Bad Ono" status, in which case the response PDU would look like what the tool shows. This reverse-engineering of received responses helps us debug our commands until we obtain the "OK" status, meaning that the device has performed the desired SetValue command successfully.

### GetValue Command and Response

If we choose GetValue instead, the command's PDU becomes very simple: just give me the value, please. 

Assuming the device understood our command and is able to send us the current value of our OcaGain object, the Response PDU contains the value. 

We know that it is of type Float32 which means it takes four bytes. And in the bottom field you can see those four bytes changing as we drag the slider to adjust the value. 

> Note that the value will not be included in the Response if the status is not OK.
> TODO: confirm

## Testing Commands

*Coming soon...*

### Command Handles

*Coming soon...*

## Subscriptions and Notifications

*Coming soon...*

## Custom Classes

*Coming soon...*

## Configuration Files

*Coming soon...*


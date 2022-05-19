
<p align="center">
<a href="https://dotnet.microsoft.com/en-us/apps/SmartBin">
    <img alt="SmartBin" title="SmartBin Strater Project" src="/pics/logo.png" width="300"><br/> + <br/>
  <a href="https://dotnet.microsoft.com/en-us/apps/SmartBin">
    <img alt="SmartBin" title="SmartBin Strater Project" src="https://upload.wikimedia.org/wikipedia/commons/thumb/8/87/Arduino_Logo.svg/1024px-Arduino_Logo.svg.png" width="90"><br/>+ <br/><img alt="Android" title="SmartBin Strater Project" src="https://www.hivemq.com/img/svg/hivemq-logo.svg" width="130">
  </a>
</p>

<p align="center">
  An arduino smartbin project based on HiveMQ Cloud and an CI4 backend.
</p>


<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Feedback](#feedback)
- [Build Process](#build-process)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## Introduction

Transform a normal trash bin into a smart one. Reading data from ultrasonic sensors placed on the lid of the trash and send it over MQTT Protocol to the Waste Management Platform. I use ESP8266 but i would work for EP32 too with minors changements.

Get live notifications of filled bins and their locations.
<p align="center">
  <img src = "/pics/device.jpeg" width=350>
</p>

## Features

A few of the things you can do with this Repository:

* Reading data from Ultrasonic sensor✅
* Calculate the bins capacity and waste ✅
* Send these data Trougth MQTT Protocol (HiveMQ Cloud) ✅
* Show data on Map✅
* Send Removing Alert when full ❌
* more...

The device connecting to internet over wifi get NTP clock(need to check certificate) and send Json data to predifined topic (**poubelles/$DeviceID/**) on HiveMQ Cloud. The device send data evry 5 min and go to deep sleep when evrything done well.
<p align="center">
  <img src = "/pics/arduino.jpg" width=700>
</p>

An php script on waste management server side act like mqtt client and subcribe to topic (**poubelles/#**). Data is received and processed.

Device is created if not exit and it just updated with the new data if exist!
<p align="center">
  <img src = "/pics/mqttSubResponse.jpg" width=700>
</p>

<p align="center">
  <img src = "/pics/mqttSubResponse2.jpg" width=700>
</p>
The data updated on the waste managment plateform
<p align="center">
  <img src = "/pics/platform.png" width=700>
</p>

Finally the data showed on map
<p align="center">
  <img src = "/pics/device_onMap.jpg" width=700>
</p>

## Feedback

Feel free to send us feedback on [Twitter](https://twitter.com/GnOtaku) or [file an issue](https://github.com/devnullone/SmartBin/issues/new). Feature requests are always welcome. If you wish to contribute, please take a quick look at the [guidelines](./CONTRIBUTING.md)!

<!-- 
## Contributors

This project follows the [all-contributors](https://github.com/kentcdodds/all-contributors) specification and is brought to you by these [awesome contributors](./CONTRIBUTORS.md). -->

## Build Process
Add ESP8266/ESP32 board to your arduino IDE. Follow this tutorial ->
Install all library the project need:
- NTPClient 
- PubSubClient
- LittleFS
- ArduinoJson

Then connect your ultrasonnic sensor I use the hc-sr04 who is good for testing purpose and realy cheap.

Sensor <------> ESP8266
Vcc<------> vv
Trig<------> D8
Echo<------>D7
Gnd<------>GND after A0
<p align="center">
  <img src = "/pics/ESP8266-Ultrasonic-Sensor-Wiring-Fritzing-Diagram.webp" width=300>
</p>

Connect the D0 to RST pin to use deep sleep mode.
<p align="center">
  <img src = "/pics/enable_deep_sleep_esp8266.webp" width=300>
</p>

Connect your ESP8266 to the arduino IDE via USB cable and verify the code and upload it on the board.

Don't forget to change values on config.h file according yours!

Enjoy It!

<p align="center"> If you like what I do, maybe consider buying me a coffee </p>

<p align="center"><a href="https://www.buymeacoffee.com/devnull" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-red.png" alt="Buy Me A Coffee" width="120" ></a>
<!-- ## Acknowledgments

Thanks to [JetBrains](https://www.jetbrains.com) for supporting us with a [free Open Source License](https://www.jetbrains.com/buy/opensource). -->
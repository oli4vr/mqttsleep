# MQTT Wake and Sleep

## Overview

This project consists of two programs, `mqttwake` and `mqttsleep`, designed to work together using MQTT messaging. The `mqttwake` program sends a signal to wake up a `mqttsleep` process that is waiting for a specific message on a given MQTT topic.

## Usage

### mqttwake

Attempts to wake a `mqttsleep` process by publishing an MQTT message.

**Syntax:**
```
mqttwake [-i HostnameOrIP] Signal ID
```

### mqttsleep

Subscribes to an MQTT topic and waits for a specific message to terminate.

**Syntax:**
```
mqttsleep Signal ID
```

## Build Instructions

To build the `mqttwake` and `mqttsleep` programs, you need to have the `mosquitto` library installed. You can build the programs using the provided Makefile.

### Steps

**Install dependencies:**
   ```sh
   sudo apt-get install libmosquitto-dev
   ```

**Clone the repository:**
   ```sh
   git clone https://github.com/oli4vr/mqttsleep.git
   cd mqttsleep
   ```

**Build the programs:**
   ```sh
   make all
   ```

**Build and install the rpm package :**
   ```sh
   make rpm
   rpm -ivh mqttsleep*.rpm
   ```

**Or Install the programs:**
   ```sh
   sudo make install
   ```

**Clean up build files:**
   ```sh
   make clean
   ```

## Logging

Logs are written to:
- Info logs: `/var/log/mqttsleep.info`
- Error logs: `/var/log/mqttsleep.err`

## Credits

This project is developed by Olivier Van Rompuy (Westpole Belgium).


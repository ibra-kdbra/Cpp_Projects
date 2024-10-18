# Mothbar

A small bar utility written in C for [FreeBSD](https://en.wikipedia.org/wiki/FreeBSD), which displays information such as the current desktop, clock, and volume. It works by creating FIFO pipes to receive data, and updates the bar in real-time using XCB for interaction with the X server.

## Features

### 1. Clock Display
- Displays the current time in `HH:MM` format.
- The clock is updated every few seconds and is displayed in the bar.

### 2. Desktop Indicator
- Displays the current active desktop.
- Monitors the state of desktops, identifying the current one and other active ones.

### 3. Volume Display
- Displays the current volume level.
- It dynamically updates the bar with the volume percentage or shows "mute" if the volume is set to 0.

### 4. FIFO Communication
- The project uses FIFO (First In, First Out) special files for communication between different components.
- The bar reads strings in the format `DESK@`, `VOL@`, or `CLOCK@` from the FIFO and updates the display accordingly.

### 5. Real-time Bar Updates
- The bar updates in real-time, showing the latest status of the clock, desktop, and volume.
- The project ensures that only meaningful updates are displayed, reducing unnecessary redraws.

## Key Components

### 1. **Clock Module (`m_clock.c`)**
- This module fetches the current time, formats it, and sends the result to the FIFO.
- The clock updates every 3 seconds to provide a near real-time display of the current time.

### 2. **Desktop Module (`m_desk.c`)**
- Monitors the active desktops using XCB properties.
- Sends information about the current desktop to the bar through the FIFO.

### 3. **Volume Module (`mixer.c`)**
- Reads the volume level from the system mixer device and formats it as a percentage.
- If the volume is muted, it displays "mute" instead of the percentage.

### 4. **Main Bar Application (`mothbar.c`)**
- This is the core of the project, which manages the bar window.
- It spawns processes to handle the clock and desktop updates.
- Reads data from the FIFO and updates the bar with current desktop, volume, and time information.

## Makefile Overview

- The `Makefile` automates the compilation and installation of the bar.
- Key targets include:
  - **`all`**: Builds all the components of the project.
  - **`install`**: Installs the bar and its components to `/usr/local/bin`.
  - **`uninstall`**: Removes the installed files from the system.
  - **`clean`**: Cleans up the built binaries.

## Installation

1. Compile and install the bar by running the following commands:
   ```bash
   make
   sudo make install
   ```

2. This will install the binaries (`mothbar`, `m_desk`, `m_clock`) to `/usr/local/bin`.

## Running the Bar

- To start the bar, simply run the `mothbar` binary. The bar will automatically update with the desktop, clock, and volume information.

## Uninstallation

- To remove the bar from your system, run:
   ```bash
   sudo make uninstall
   ```

This will delete the installed binaries from `/usr/local/bin`.

## Summary

This FreeBSD bar project provides a lightweight status bar that integrates clock, desktop, and volume information. By leveraging FIFO for communication, and XCB for desktop monitoring, and it ensures efficient real-time updates.

# TitanReset - Programming Setup Tutorial

## TitanReset - LemLib/EZ-Template Setup

This section goes over how to setup TitanReset with LemLib & EZ-Template specifically. This will not go over LemLib & EZ-Template setup and will assume that either is already setup for your robot.

**If you need help setting up LemLib**, I suggest the following resource:
 - [LemLib Setup (Website)](https://lemlib.readthedocs.io/en/stable/tutorials/1_getting_started.html)

**If you need help setting up EZ-Template**, I suggest the following resource:
 - [EZ-Template Setup (Website)](https://ez-robotics.github.io/EZ-Template/tutorials/installation) 

## Installation

To begin using TitanReset, you first need to install it. This can be done through the command line and the pros conductor.

First, you should click the pros icon on the sidebar of Visual Studio Code and from there press the integrated terminal button. After that a terminal should appear **on the bottom** of Visual Studio Code.

![Visual Guide](Guide.png)

After that, run the following commands in order to **install TitanReset**.

> [!WARNING]
> TitanReset uses two different versions for EZ-Template and LemLib. Use the right commands to install TitanReset otherwise it will not work.

#### LemLib Install Commands

```pros c add-depot TitanReset https://raw.githubusercontent.com/tubaplayerdis/TitanReset/refs/heads/main/depot.json```

```pros c apply TitanReset@0.0.1```

#### EZ-Template Install Commands

```pros c add-depot TitanReset-EZ https://raw.githubusercontent.com/tubaplayerdis/TitanReset-EZ/refs/heads/main/depot.json```

```pros c apply TitanReset-EZ@0.0.1```

## Programming Setup

> [!NOTE]
> You do not literally have to paste the all of the following code into `main.cpp`. This guide assume that you have your template objects defined in main.cpp,
> **If your LemLib/EZ-Template objects are defined somewhere else, use the file that contains those definitions instead.**

### Adding TitanReset to your file

After that, include TitanReset into your `main.cpp` file.

```cpp
#include "TitanReset/TitanReset.hpp"
```

### Setting Up Distance Sensors

Next, paste the following code into your `main.cpp` file.

```cpp
/**
 * North sensor
 * X is your parrallel offset
 * Y is your perpendicular offset
 * P is the port of your sensor
 */
tr_sensor north({X, Y}, P);

/**
 * East sensor
 * X is your parrallel offset
 * Y is your perpendicular offset
 * P is the port of your sensor
 */
tr_sensor east({X, Y}, P);

/**
 * South sensor
 * X is your parrallel offset
 * Y is your perpendicular offset
 * P is the port of your sensor
 */
tr_sensor south({X, Y}, P);

/**
 * West sensor
 * X is your parrallel offset
 * Y is your perpendicular offset
 * P is the port of your sensor
 */
tr_sensor west({X, Y}, P);
```

#### Populate the X,Y, and P objects with the offsets you measured in the [previous tutorial](PhysicalSetup.MD).

 - The X value if the parrallel offset for that sensor
 - The Y value is the perpendicular offset for that sensor
 - The P value is the port of that sensor

#### Completed Example (matches the previous tutorial)

```cpp
/**
 * North sensor
 * X is your parrallel offset
 * Y is your perpendicular offset
 * P is the port of your sensor
 */
tr_sensor north({5.823, -4.694}, 10);

/**
 * East sensor
 * X is your parrallel offset
 * Y is your perpendicular offset
 * P is the port of your sensor
 */
tr_sensor east({5.137, 3.23}, 11);

/**
 * South sensor
 * X is your parrallel offset
 * Y is your perpendicular offset
 * P is the port of your sensor
 */
tr_sensor south({4.861, 5.25}, 12);

/**
 * West sensor
 * X is your parrallel offset
 * Y is your perpendicular offset
 * P is the port of your sensor
 */
tr_sensor west({5.137, 3.744}, 13);
```

### Setting up your TitanReset chassis

To make all the sensors work together, you have to setup a TitanReset chassis. Keep this in the same place as your `tr_sensor` objects.

This looks slightly different for LemLib vs EZ-Template users, so both are included below

#### Lemlib Usage

Paste the following code into your `main.cpp` file to setup the TitanReset chassis:

```cpp
/**
 * TitanReset Chassis
 * &imu - pointer to IMU
 * &chassis - pointer to LemLib chassis
 * {&north, &east, &south, &west} - Array of pointers to sensors.
 */
tr_chassis dsr_system(&imu, &chassis, {&north, &east, &south, &west});
```
Notes:
 - If your imu object or lemlib chassis object are named something else, change the parameter names to match the name of the chassis or imu. (default called chassis & imu)

#### EZ-Template Usage

Paste the following code into your `main.cpp` file to setup the TitanReset chassis:

```cpp
/**
 * TitanReset Chassis
 * &chassis.imu - pointer to IMU
 * &chassis - pointer to EZ-Template Drive
 * {&north, &east, &south, &west} - Array of pointers to sensors.
 */
tr_chassis dsr_system(&chassis.imu, &chassis, {&north, &east, &south, &west});
```

Notes:
 - If your EZ-Template drive object is named something else, change the parameter names to match the name of the drive (default called chassis)

Paste the following code into your `subsystems.hpp` file to allow your autons to access the TitanReset chassis. **Paste this below** the `extern Drive chassis` code.

```cpp
extern tr_chassis dsr_system;
```

## Thats all for setup!

Read the [next tutorial](ProgrammingUsage.MD) on how to accurately and reliably use DSR in your autons!

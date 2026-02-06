# TitanReset - Programming Usage Tutorial

This tutorial will go over the best practices and usage of TitanReset.

## Intoroduction - Coordinates, JerryIO, etc

#### Notice - This assumes you are using coordinates to create autonomous routines as is standard pracice with LemLib and EZ-Template

#### TitanReset is a DSR implementation.

Just the like VEX GPS Sensor, DSR (Distance Sensor Reset) is based off absolute positioning. This is done since it is too convoluted to have DSR be based off of other coordinate systems.

Most autonomous routines absent of DSR or GPS use local coordinates. This means the positioning is based of where the robot starts. This is opposite to system used by DSR.

TitanReset (DSR) uses absolute potitioning, this means that the coordinates are based off the center of the field and not where the robot starts.

### Your new best friend - [JerryIO](path.jerryio.com)

The coordinate system used by TitanReset is the same as the VEX Game Posititing System. Since JerryIO also supports this positioning system, it is in your best intrest to use JerryIO in the aid of the construction of your autons.

### Quadants

TitanReset uses alot of refrences to quadrants. This is representative of qudrants of the field. The quadrants are as follows in the picture below.

## Starting location

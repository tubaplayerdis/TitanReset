# TitanReset - Programming Usage Tutorial

This tutorial will go over the best practices and usage of TitanReset.

## Intoroduction - Coordinates, JerryIO, etc

#### Notice - This assumes you are using coordinates to create autonomous routines as is standard pracice with LemLib and EZ-Template

#### TitanReset is a DSR implementation.

Just the like VEX GPS Sensor, DSR (Distance Sensor Reset) is based off absolute positioning. This is done since it is too convoluted to have DSR be based off of other coordinate systems.

Most autonomous routines absent of DSR or GPS use local coordinates. This means the positioning is based of where the robot starts. This is opposite to system used by DSR.
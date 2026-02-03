#pragma once

#include "TRTypes.hpp"
#include "../pros/distance.hpp"

/**
 * @brief Distance sensor wrapper class used for distance sensor resets.
 */
class tr_sensor
{
    /**
     * Offset vector of the TitanReset sensor.
     * X is the parrallel offset (the way it is facing) of the sensor from the center of the robot to the sensor
     * Y is the perpendicular offset (left or right of the way the sensor is facing) of the sensor from the center of the robot to the sensor
     */
    const tr_vector2 offset;

    /**
     * Pros distance sensor object.
     */
    pros::Distance sensor;

public:

    /**
     * @brief Constructor for TitanReset sensor.
     * @note Offsets should be done in inches.
     * @param off Offset of the sensor from the origin of the robot with X being in the parallel direction of the sensors facing, and Y being the perpendicular
     * @param port Port of the distance sensor.
     */
    tr_sensor(tr_vector2 off, int port);

    /**
     * @brief Distance read from the distance sensor as a confidence pair with angle of robot factored in and offset.
     * @note Data returned is in inches.
     * @return confidence and the distance reading and calculation.
     */
    tr_distance distance(float heading);

    /**
     * @brief Distance read from the distance sensor as a confidence pair without factoring in heading.
     * @note Data returned is in inches.
     * @return confidence and the distance reading.
     */
    tr_distance distance();

public:

    /**
     * @breif Recursive function mapping the heading into the domain of -45 to 45 degrees to use with trigonometric functions
     */
    static float octant_recursive(float heading);
};
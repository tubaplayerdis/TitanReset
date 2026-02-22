#pragma once

/*
* Every constant used by TitanReset. Values are compiled into TitanReset and changing will have no effect. They should only be used as refrence.
*/

/**
 * Erroneous reading value when the V5 Distance Sensor cannot read a distance
 */
static constexpr int err_reading_value = 9999;

/**
 * MM to IN conversion factor.
 */
static constexpr float mm_inch_conversion_factor = 0.0393701;

/**
 * Degree to Radian conversion factor
 */
static constexpr float deg_rad_conversion_factor = 0.0174532;

/**
 * Radian to Degree conversion factor
 */
static constexpr float rad_deg_conversion_factor = 57.2958;

/**
 * Domain of the confidence readings from the V5 Distance Sensor
 */
static constexpr float confidence_domain = 63.0;
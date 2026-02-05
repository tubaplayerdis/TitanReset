#include "../../include/TitanReset/TRSensor.hpp"
#include "../../include/TitanReset/TRConstants.hpp"

tr_sensor::tr_sensor(tr_vector2 offset, int port) :
            offset(offset),
            sensor(port)
{}

float tr_sensor::relative_square(float heading)
{
    float wrapped = fmod(heading, 360.0f);
    if (wrapped < 0) wrapped += 360.0f;

    // Find distance to the nearest 90-degree increment
    // This gives you how "un-square" the robot is to the wall
    float relative = fmod(wrapped + 45.0f, 90.0f) - 45.0f;
    return relative;
}

tr_conf_pair<float> tr_sensor::distance()
{
    int sensor_reading = sensor.get_distance();
    float sensor_confidence = (sensor.get_confidence() / confidence_domain);
    if (sensor_reading == err_reading_value) return tr_conf_pair<float>(err_reading_value, 0.0);

    return tr_conf_pair<float>(sensor_reading * mm_inch_conversion_factor, sensor_confidence);
}

tr_conf_pair<float> tr_sensor::distance(float heading)
{
    int sensor_reading = sensor.get_distance();
    float sensor_confidence = (sensor.get_confidence() / confidence_domain);
    if (sensor_reading == err_reading_value) return tr_conf_pair<float>(err_reading_value, 0.0);

    auto reading = sensor_reading * mm_inch_conversion_factor;

    heading = tr_sensor::relative_square(heading);

    float heading_err_rad = heading * deg_rad_conversion_factor;

    float actual_reading = cos(heading_err_rad) * reading;
    float parallel_offset = cos(heading_err_rad) * offset.x;
    float perpendicular_offset = sin(heading_err_rad) * offset.y;

    return tr_conf_pair<float>(actual_reading + parallel_offset - perpendicular_offset, sensor_confidence);
}
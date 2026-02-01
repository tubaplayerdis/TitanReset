#include "../../include/TitanReset/TRSensor.hpp"
#include "../../include/TitanReset/TRConstants.hpp"

tr_sensor::tr_sensor(tr_vector2 offset, int port) :
            offset(offset),
            sensor(port)
{}

float tr_sensor::octant_recursive(float heading)
{
    if (static_cast<int>(heading) % 45 == 0) return 1;

    if (heading < -45.0)
    {
        return octant_recursive(heading + 90.0);
    }

    if (heading > 45.0)
    {
        return octant_recursive(heading - 90.0);
    }

    return heading;
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

    heading = tr_sensor::octant_recursive(heading);

    float heading_err_rad = heading * deg_rad_conversion_factor;

    float actual_reading = cos(heading_err_rad) * reading;
    float parallel_offset = cos(heading_err_rad) * offset.x;
    float perpendicular_offset = sin(heading_err_rad) * offset.y;

    return tr_conf_pair<float>(actual_reading + parallel_offset + perpendicular_offset, sensor_confidence);
}
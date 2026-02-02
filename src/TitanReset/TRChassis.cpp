#include "../../include/TitanReset/TRChassis.hpp"
#include "../../include/TitanReset/TRConstants.hpp"
#include "../../include/pros/imu.hpp"
#include "../../include/pros/llemu.hpp"
#include <fstream>

float tr_chassis::quadrant_recursive(float heading)
{
    if (heading < 0.0f)
    {
        return quadrant_recursive(heading + 360.0f);
    }

    if (heading > 360.0f)
    {
        return quadrant_recursive(heading - 360.0f);
    }

    return heading;
}

static const tr_rectangle north_goal_exclusion(-22, 44, 22, 49);
static const tr_rectangle south_goal_exclusion(-22, -44, 22, -49);
static const tr_rectangle mid_goal_exclusion(-7, -7, 7, -7);

static const tr_circle pos_pos_ml_exclusion(67.5, 47, 5);
static const tr_circle neg_pos_ml_exclusion(-67.5, 47, 5);
static const tr_circle neg_neg_ml_exclusion(-67.5, -47, 5);
static const tr_circle pos_neg_ml_exclusion(-67.5, 47, 5);

bool tr_chassis::can_position_exist(tr_vector3 pose)
{
    tr_vector2 loc = tr_vector2(pose.x, pose.y);

    if (pose.x > wall_coord || pose.y > wall_coord || pose.x < -wall_coord || pose.y < -wall_coord) return false;

    if (pose.x > 0 && pose.y > 0)
    {
        return !(north_goal_exclusion.inside(loc) && mid_goal_exclusion.inside(loc) && pos_pos_ml_exclusion.inside(loc));
    }

    if (pose.x > 0 && pose.y < 0)
    {
        return !(north_goal_exclusion.inside(loc) && mid_goal_exclusion.inside(loc) && neg_pos_ml_exclusion.inside(loc));
    }

    if (pose.x < 0 && pose.y < 0)
    {
        return !(south_goal_exclusion.inside(loc) && mid_goal_exclusion.inside(loc) && neg_neg_ml_exclusion.inside(loc));
    }

    if (pose.x > 0 && pose.y < 0)
    {
        return !(south_goal_exclusion.inside(loc) && mid_goal_exclusion.inside(loc) && pos_neg_ml_exclusion.inside(loc));
    }

    return false;
}

std::string tr_chassis::get_quadrant_string(tr_quadrant quadr)
{
    switch (quadr)
    {
        case POS_POS:
            return "POS_POS";
        case NEG_POS:
            return "NEG_POS";
        case NEG_NEG:
            return "NEG_NEG";
        case POS_NEG:
            return "POS_NEG";
    }
    return "";
}

void tr_chassis::set_active_sensors(int sensors)
{
    active_sensors = 0;
    active_sensors |= sensors;
}

tr_chassis::tr_chassis(tr_options settings, pros::Imu *inertial, tr_drivebase_generic* chas ,std::array<tr_sensor *,4> sensors) : options(settings), active_sensors(0), b_display(false), location_task(nullptr)
{
    north = sensors.at(0);
    east = sensors.at(1);
    south = sensors.at(2);
    west = sensors.at(3);
    imu = inertial;
    chassis = chas;
}

tr_quadrant tr_chassis::sensor_relevancy()
{
    float heading = quadrant_recursive(chassis->getPose().z);

    if ((heading >= 0 && heading <= 45) || (heading <= 360 && heading > 315))
    {
        return tr_quadrant::POS_POS;
    }

    if (heading > 45 && heading <= 135)
    {
        return tr_quadrant::NEG_POS;
    }

    if (heading > 135 && heading <= 225)
    {
        return tr_quadrant::NEG_NEG;
    }

    if (heading > 225 && heading <= 315)
    {
        return tr_quadrant::POS_NEG;
    }

    return tr_quadrant::NEG_NEG;
}

tr_quadrant tr_chassis::sensor_relevancy(float heading)
{
    if ((heading >= 0 && heading <= 45) || (heading <= 360 && heading > 315))
    {
        return tr_quadrant::POS_POS;
    }

    if (heading > 45 && heading <= 135)
    {
        return tr_quadrant::NEG_POS;
    }

    if (heading > 135 && heading <= 225)
    {
        return tr_quadrant::NEG_NEG;
    }

    if (heading > 225 && heading <= 315)
    {
        return tr_quadrant::POS_NEG;
    }

    return tr_quadrant::NEG_NEG;
}

tr_quadrant tr_chassis::get_quadrant()
{
    tr_vector3 cur_pose = chassis->getPose();

    if (cur_pose.x > 0 && cur_pose.y > 0)
    {
        return tr_quadrant::POS_POS;
    }

    if (cur_pose.x > 0 && cur_pose.y < 0)
    {
        return tr_quadrant::NEG_POS;
    }

    if (cur_pose.x < 0 && cur_pose.y < 0)
    {
        return tr_quadrant::NEG_NEG;
    }

    if (cur_pose.x > 0 && cur_pose.y < 0)
    {
        return tr_quadrant::POS_NEG;
    }

    return tr_quadrant::POS_POS;
}
//n_p, n_p
tr_conf_pair<tr_vector3> tr_chassis::get_position_calculation(tr_quadrant quadrant)
{
    return get_position_calculation(quadrant, chassis->getPose().z);
}

tr_conf_pair<tr_vector3> tr_chassis::get_position_calculation(tr_quadrant quadrant, float heading)
{
    float normal_heading = quadrant_recursive(heading);
    tr_quadrant theta_quad = sensor_relevancy(normal_heading);

    tr_conf_pair<float> n_dist = north->distance(normal_heading);
    tr_conf_pair<float> e_dist = east->distance(normal_heading);
    tr_conf_pair<float> s_dist = south->distance(normal_heading);
    tr_conf_pair<float> w_dist = west->distance(normal_heading);

    tr_conf_pair<tr_vector3> ret = tr_conf_pair<tr_vector3>();

    float x = 0;
    float y = 0;

    if (quadrant == POS_POS)
    {
        switch (theta_quad)
        {
            case POS_POS:
            {
                ret.set_confidence(conf_avg(e_dist, n_dist));
                x = wall_coord - e_dist.get_value();
                y = wall_coord - n_dist.get_value();
                set_active_sensors(NORTH | EAST);
                break;
            }

            case NEG_POS:
            {
                ret.set_confidence(conf_avg(n_dist, w_dist));
                x = wall_coord - n_dist.get_value();
                y = wall_coord - w_dist.get_value();
                set_active_sensors(NORTH | WEST);
                break;
            }

            case NEG_NEG:
            {
                ret.set_confidence(conf_avg(w_dist, s_dist));
                x = wall_coord - w_dist.get_value();
                y = wall_coord - s_dist.get_value();
                set_active_sensors(WEST | SOUTH);
                break;
            }

            case POS_NEG:
            {
                ret.set_confidence(conf_avg(s_dist, e_dist));
                x = wall_coord - s_dist.get_value();
                y = wall_coord - e_dist.get_value();
                set_active_sensors(SOUTH | EAST);
                break;
            }
        }
    } else if (quadrant == NEG_POS)
    {
        switch (theta_quad)
        {
            case POS_POS:
            {
                ret.set_confidence(conf_avg(n_dist, w_dist));
                x = -wall_coord + w_dist.get_value();
                y = wall_coord - n_dist.get_value();
                set_active_sensors(WEST | NORTH);
                break;
            }

            case NEG_POS:
            {
                ret.set_confidence(conf_avg(s_dist, w_dist));
                x = -wall_coord + s_dist.get_value();
                y = wall_coord - w_dist.get_value();
                set_active_sensors(SOUTH | WEST);
                break;
            }

            case NEG_NEG:
            {
                ret.set_confidence(conf_avg(e_dist, s_dist));
                x = -wall_coord + e_dist.get_value();
                y = wall_coord - s_dist.get_value();
                set_active_sensors(WEST | SOUTH);
                break;
            }

            case POS_NEG:
            {
                ret.set_confidence(conf_avg(e_dist, n_dist));
                x = -wall_coord + n_dist.get_value();
                y = wall_coord - e_dist.get_value();
                set_active_sensors(NORTH | EAST);
                break;
            }
        }
    } else if (quadrant == NEG_NEG)
    {
        switch (theta_quad)
        {
            case POS_POS:
            {
                ret.set_confidence(conf_avg(w_dist, s_dist));
                x = -wall_coord + w_dist.get_value();
                y = -wall_coord + s_dist.get_value();
                set_active_sensors(WEST | SOUTH);
                break;
            }

            case NEG_POS:
            {
                ret.set_confidence(conf_avg(s_dist, e_dist));
                x = -wall_coord + s_dist.get_value();
                y = -wall_coord + e_dist.get_value();
                set_active_sensors(SOUTH | EAST);
                break;
            }

            case NEG_NEG:
            {
                ret.set_confidence(conf_avg(e_dist, n_dist));
                x = -wall_coord + e_dist.get_value();
                y = -wall_coord + n_dist.get_value();
                set_active_sensors(EAST | NORTH);
                break;
            }

            case POS_NEG:
            {
                ret.set_confidence(conf_avg(n_dist, w_dist));
                x = -wall_coord + n_dist.get_value();
                y = -wall_coord + w_dist.get_value();
                set_active_sensors(NORTH | WEST);
                break;
            }
        }
    } else if (quadrant == POS_NEG)
    {
        switch (theta_quad)
        {
            case POS_POS:
            {
                ret.set_confidence(conf_avg(e_dist, s_dist));
                x = wall_coord - e_dist.get_value();
                y = -wall_coord + s_dist.get_value();
                set_active_sensors(EAST | SOUTH);
                break;
            }

            case NEG_POS:
            {
                ret.set_confidence(conf_avg(n_dist, e_dist));
                x = wall_coord - n_dist.get_value();
                y = -wall_coord + e_dist.get_value();
                set_active_sensors(NORTH | EAST);
                break;
            }

            case NEG_NEG:
            {
                ret.set_confidence(conf_avg(w_dist, n_dist));
                x = wall_coord - w_dist.get_value();
                y = -wall_coord + n_dist.get_value();
                set_active_sensors(WEST | NORTH);
                break;
            }

            case POS_NEG:
            {
                ret.set_confidence(conf_avg(s_dist, w_dist));
                x = wall_coord - s_dist.get_value();
                y = -wall_coord + w_dist.get_value();
                set_active_sensors(SOUTH | WEST);
                break;
            }
        }
    } else
    {
        x = 0;
        y = 0;
        ret.set_confidence(0);
    }

    ret.set_value(tr_vector3(x, y, normal_heading));

    if (!can_position_exist(tr_vector3(x, y, normal_heading))) ret.set_confidence(0);

    return ret;
}

float tr_chassis::conf_avg(tr_distance one, tr_distance two)
{
    return (one.get_confidence() + two.get_confidence()) / 2.0f;
}

bool tr_chassis::perform_dsr(bool trust_sensors)
{
    return perform_dsr_quad(get_quadrant(), trust_sensors);
}

bool tr_chassis::perform_dsr_quad(tr_quadrant quadrant, bool trust_sensors)
{
    tr_vector3 pose = chassis->getPose();
    tr_conf_pair<tr_vector3> coords = get_position_calculation(quadrant);

    if (coords.get_confidence() > options.sensor_trust && trust_sensors == false) return false;

    pose.x = coords.get_value().x;
    pose.y = coords.get_value().y;
    chassis->setPose(pose);

    return true;
}

void tr_chassis::perform_dsr_init(tr_quadrant quadrant, float heading)
{
    tr_vector3 pose = chassis->getPose();
    imu->set_heading(heading);
    chassis->setPose(tr_vector3(0,0,heading));
    tr_conf_pair<tr_vector3> coords = get_position_calculation(quadrant, heading);

    pose.x = coords.get_value().x;
    pose.y = coords.get_value().y;
    chassis->setPose(pose);
}

void tr_chassis::init_display()
{
    pros::lcd::initialize();
}

void tr_chassis::update_display(tr_chassis* chassis)
{
    bool north = chassis->is_sensor_used(NORTH);
    bool east = chassis->is_sensor_used(EAST);
    bool south = chassis->is_sensor_used(SOUTH);
    bool west = chassis->is_sensor_used(WEST);

    float heading = quadrant_recursive(chassis->imu->get_heading());

    float confidence_n = chassis->north->distance(heading).get_confidence();
    float confidence_e = chassis->east->distance(heading).get_confidence();
    float confidence_s = chassis->south->distance(heading).get_confidence();
    float confidence_w = chassis->west->distance(heading).get_confidence();

    float dis_n = chassis->north->distance(heading).get_value();
    float dis_e = chassis->east->distance(heading).get_value();
    float dis_s = chassis->south->distance(heading).get_value();
    float dis_w = chassis->west->distance(heading).get_value();
    

    bool use_pose = chassis->perform_dsr();
    tr_conf_pair<tr_vector3> position = chassis->get_position_calculation(tr_quadrant::NEG_POS);
    std::string quads = chassis->get_quadrant_string(chassis->get_quadrant());
    std::string squad = chassis->get_quadrant_string(chassis->sensor_relevancy());

    tr_vector3 pose_lem = chassis->chassis->getPose();

    pros::lcd::print(0, "SQ: %s, %s", quads.c_str(), squad.c_str());
    pros::lcd::print(1, "SU: N %i, E %i, S %i, W %i", north, east, south, west);
    pros::lcd::print(2, "SR: N %.2f, E %.2f, S %.2f, W %.2f", dis_n, dis_e, dis_s, dis_w);
    pros::lcd::print(3, "SC: N %.2f, E %.2f, S %.2f, W %.2f", confidence_n, confidence_e, confidence_s, confidence_w);
    pros::lcd::print(4, "PS: X: %.2f,Y: %.2f,H: %.2f,C: %.2f", position.get_value().x, position.get_value().y, heading, position.get_confidence());
    pros::lcd::print(5, "LC: X: %.2f,Y: %.2f,H: %.2f,S: %i", pose_lem.x, pose_lem.y, pose_lem.z, use_pose);
}

void tr_chassis::shutdown_display()
{
    pros::lcd::shutdown();
}

void tr_chassis::start_location_recording(std::string date, std::string time)
{
    if (location_task != nullptr) location_task->suspend();
    delete location_task;
    location_task = new pros::Task([this, time, date]() -> void
    {
        std::ofstream output("odom_data.txt", std::ios::app);
        std::ofstream output2("dist_data.txt", std::ios::app);

        double mil = pros::millis();

        output << "\n" << "Timestamp: " << date << " " << time << " " << mil << "\n";
        output2 << "\n" << "Timestamp: " << date << " " << time << " " << mil << "\n";

        while (true)
        {
            tr_vector3 pose = chassis->getPose();
            tr_vector3 pose1 = get_position_calculation(get_quadrant()).get_value();
            output << pose.x << ", " << pose.y << ", " << pose.z << "\n";
            output2 << pose1.x << ", " << pose1.y << ", " << pose1.z << "\n";
            pros::Task::delay(50);
        }
        output.close();
    });
}

void tr_chassis::stop_location_recording()
{
    if (location_task != nullptr)
    {
        location_task->suspend();
        delete location_task;
    }
}
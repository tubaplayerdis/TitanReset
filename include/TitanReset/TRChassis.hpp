#include "TRSensor.hpp"
#include "../pros/imu.hpp"
#include "../lemlib/chassis/chassis.hpp"

/**
 * Options used by TitanReset when initializing the TitanReset chassis.
 * 
 * @note Subject to change as more modifiable options are added.
 */
struct tr_options
{
    /**
     * Sensor trust threshold on whether to use dsr if called. 0 is least trust and 1 is full trust.
     */
    const float sensor_trust = 0.5;
};

/**
 * TitanReset chassis object. Used to perform distance sensor resets
 */
class tr_chassis
{
public:

    /**
     * @brief Initialize the localization chassis
     * @note ONLY INITIALIZE THIS WHEN YOUR ROBOT IS NOT MOVING!
     *
     * @param settings customizable trust and gain options for the localization algorithm
     * @param inertial pointer to the inertial sensor on the robot
     * @param base pointer to the lemlib chassis of the robot
     * @param sensors array of pointers to the localization sensors of the robot
     */
    tr_chassis(tr_options settings, pros::Imu* inertial, lemlib::Chassis* base, std::array<tr_sensor*,4> sensors);

    /**
     * @brief Performs a distance sensor reset using the sensors on the robot given the robot already knows where it is and where it is facing.
     *
     * @param trust_sensors Whether to ignore the sensor trust threshold defined in the options
     * @return Whether the function decided to reset the location of the robot
     */
    bool perform_dsr(bool trust_sensors = false);

    /**
     * @brief Performs a distance sensor reset using the sensors on the robot given the robot does not know which quadrant it is in.
     * 
     * @note Use this function after a movement that performs an action such as driving over a parking zone which crosses quadrants.
     *
     * @param quad The quadrant the robot is currently in
     * @return Whether the function decided to reset the location of the robot
     */
    bool perform_dsr_quad(tr_quadrant quadrant, bool trust_sensors = false);

    /**
     * @brief Performs a distance sensor reset using the sensors on the robot given the robot does not know where it is and the sensors are fully trusted.
     * 
     * @note This will set the heading of the chassis and imu as it performs a distance sensor reset. 
     * @warning This will always set the location of the robot. Use only in a situation where the robot starts in familliar place each time like the start of an auton.
     *
     * @param quadrant The quadrant the robot is currently in
     * @param heading The heading of the robot
     */
    void perform_dsr_init(tr_quadrant quadrant, float heading);

    /**
     * @breif Gets the robots quadrant based on its coordinates
     * @return The quadrant of the robot
     */
    tr_quadrant get_quadrant();

    /**
     * @brief Starts an odometry system and distance sensor system recording in a background task
     *
     * @param filename name of the file for the recording
     */
    void start_location_recording(std::string date, std::string time);

    /**
     * @brief Stops the current odometry system recording
     */
    void stop_location_recording();

    /*
    *   Note - Everything below this line is either utilities to aid with the implementation of TitanReset and are most likey irrelevant to your goals.
    */

private:

    /**
     * Whether the display is active and being displayed.
     */
    bool b_display;

    /*
    * Active sensors being used by the robot.
    */
    int active_sensors;

    void set_active_sensors(int sensors);

public:

    /**
     * @brief Normalizes heading to the domain of 0-360. Also called finding the coterminal angle
     * @param heading heading to normalize.
     * @return Normalized heading
     */
    static float quadrant_recursive(float heading);

    /**
     * @breif Compares the location against locations the robot physically cannot exist at such as inside the match loader or out of bounds.
     *
     * @param pose current location vector
     * @returns whether the location can physically eist.
     */
    static bool can_position_exist(tr_vector3 pose);

    static std::string get_quadrant_string(tr_quadrant quadrant);

    /**
     * @brief Returns the relevant sensors based on the heading of the robot.
     */
    tr_quadrant sensor_relevancy();

    /**
     * @brief Returns the relevant sensors based on the heading of the robot.
     *
     * Requires normalized heading 0-360 degrees.
     *
     * For 315 - 45 degrees: ++
     * For 45 - 135 degrees: -+
     * For 135 - 225 degrees: --
     * For 225 - 315 degrees: +-
     */
    tr_quadrant sensor_relevancy(float heading);

    /**
     * @brief Average confidence of value pair
     * @param one first confidence pair
     * @param two second confidence pair
     * @return Average confidence of value pair
     */
    static float conf_avg(tr_distance one, tr_distance two);


    /**
     * @brief Returns the confidence pair of a coordinate pair representing the robots location gathered from the sensors.
     * @param quad Current quadrant of the robot
     * @return Confidence pair of a coordinate pair representing the robots location gathered from the sensors.
     */
    tr_conf_pair<tr_vector3> get_position_calculation(tr_quadrant quadrant);

    tr_conf_pair<tr_vector3> get_position_calculation(tr_quadrant quadrant, float heading);

    /**
     * Initializes the debug screen.
     */
    static void init_display();

    /**
     * Renders the debug screen. Use in a loop.
     */
    static void update_display(tr_chassis* chassis);

    /**
     * Shutdown the debug screen
     */
    static void shutdown_display();

    /**
     * @breif Uses flags to return whether a sensor is being used.
     * @note Active sensors are set by performing a distance sensor reset.
     * @param r_sensor sensor flags
     * @return whether that sensor is being used.
     */
    bool is_sensor_used(int r_sensor)
    {
        return active_sensors & (r_sensor);
    }

private:

    /*
    * Private objects to be used by TitanReset
    */

    /**
     * Location recording task pointer
     */
    pros::Task* location_task;

    /** 
     * Sensors
     */
    tr_sensor* north;
    tr_sensor* east;
    tr_sensor* south;
    tr_sensor* west;
    pros::Imu* imu;

    /** 
     * LemLib reference
     */
    lemlib::Chassis* chassis;

    /**
     * Provided options
     */
    tr_options options;
};
#include <math.h>

/**
 * TitanReset Quadrant enumeration.
 */
enum tr_quadrant
{
    POS_POS,
    NEG_POS,
    NEG_NEG,
    POS_NEG,
};

/**
 * TitanReset Sensor flags.
 */
enum tr_sensors
{
    NORTH = 1,
    EAST = 2,
    SOUTH = 4,
    WEST = 8,
};

/**
 * Standard probability type definition
 */
typedef float tr_probability;

/**
 * Confidence Pair
 *
 * Templated pair abstraction with the confidence value.
 */
template<typename T>
class tr_conf_pair
{
private:
    /**
     * Internal pair value
     */
    std::pair<T, tr_probability>value;

public:

    /**
     * @brief confidence pair default constructor
     */
    tr_conf_pair()
    {
        value = std::pair<T, tr_probability>(T(), 0);
    }

    /**
     * @brief confidence pair passing the confidence as a float
     *
     * @param principal value of the pair
     * @param confidence confidence as a float
     */
    tr_conf_pair(T principal, tr_probability confidence)
    {
        value = std::pair<T, tr_probability>(principal, confidence);
    }

    /**
     * @brief Sets the confidence of the confidence pair
     * @param confidence new confidence to set
     */
    void set_confidence(tr_probability confidence)
    {
        value.second = confidence;
    }

    /**
     * @breif Sets the value of the confidence pair
     * @param principal new value to set
     */
    void set_value(T principal)
    {
        value.first = principal;
    }


    /**
     * @brief Gets the value of the confidence pair as T
     * @return first value of the internal pair as T
     */
    T get_value()
    {
        return value.first;
    }

    /**
     * @brief Gets the confidence of the confidence pair as a float
     * @return confidence of the internal pair as a float
     */
    tr_probability get_confidence()
    {
        return value.second;
    }
};

/**
 * Standard distance confidence pair definition
 */
typedef tr_conf_pair<float> tr_distance;


/*
 * 3D vector data structure. Z is expressed as theta for TitanReset
 */
struct tr_vector3
{
    float x;
    float y;
    float z;

    /**
     * @breif Default constructor for vector, initializes X, Y, and Z to zero.
     */
    tr_vector3()
    {
        x = 0;
        y = 0;
        z = 0;
    }

    /**
     * @brief Standard constructor for vector, initializes values to input parameters
     * @param X x value of vector
     * @param Y y value of vector
     * @param Z z or theta value of vector depending on plane interpreted
     */
    tr_vector3(float X, float Y, float Z)
    {
        x = X;
        y = Y;
        z = Z;
    }

    /**
     * @brief Standard constructor for vector, initializes values to input parameters
     * @param arr
     */
    tr_vector3(std::array<float, 3> arr)
    {
        x = arr[0];
        y = arr[1];
        z = arr[2];
    }
};

/**
 * Two dimensional vector object used by TitanReset
 */
struct tr_vector2
{
    float x;
    float y;

    tr_vector2()
    {
        x = 0.0f;
        y = 0.0f;
    }

    tr_vector2(float X, float Y)
    {
        x = X;
        y = Y;
    }

    /**
     * @brief Standard constructor for vector, initializes values to input parameters
     * @param arr
     */
    tr_vector2(std::array<float, 2> arr)
    {
        x = arr[0];
        y = arr[1];
    }
};

/**
 * @brief Generic drivebase class to allow support of any template.
 */
class tr_drivebase_generic
{
public:
    tr_drivebase_generic() {}

    virtual tr_vector3 getPose() = 0;
    virtual void setPose(tr_vector3 new_pose) = 0;
};

/**
 * Rctangle object used by TitanReset to determine if a reset is reads a valid location on the field.
 */
struct tr_rectangle
{
    tr_vector2 lower_bound;
    tr_vector2 upper_bound;

    tr_rectangle(const float lx, const float ly, const float ux, const float uy) : lower_bound(lx, ly), upper_bound(ux, uy) {}
    tr_rectangle(tr_vector2 l, tr_vector2 u) : lower_bound(l), upper_bound(u) {}

    bool inside(float x, float y) const
    {
        return inside(tr_vector2(x, y));
    }

    bool inside(tr_vector2 pos) const
    {
        return (pos.x >= lower_bound.x && pos.x <= upper_bound.x) && (pos.y >= lower_bound.y && pos.y <= upper_bound.y);
    }
};

/**
 * Circle object used by TitanReset to determine if a reset is reads a valid location on the field.
 */
struct tr_circle
{
    tr_vector2 center;
    float radius;

    tr_circle(float x, float y, float r) : center(x, y), radius(r) {}
    tr_circle(tr_vector2 c, float r) : center(c), radius(r) {}

    bool inside(float x, float y)
    {
        return inside(tr_vector2(x, y));
    }

    bool inside(tr_vector2 pos) const
    {
        //distance formula
        return sqrt(std::pow(pos.x - center.x,2) + std::pow(pos.y - center.y, 2)) <= radius;
    }
};
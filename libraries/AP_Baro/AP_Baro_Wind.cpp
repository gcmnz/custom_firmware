#include "AP_Baro.h"
#include <AP_AHRS/AP_AHRS.h>

#if HAL_BARO_WIND_COMP_ENABLED

// table of compensation coefficient parameters for one baro
const AP_Param::GroupInfo AP_Baro::WindCoeff::var_info[] = {

    // @Param: ENABLE
    // @DisplayName: Wind coefficient enable
    // @Description: This enables the use of wind coefficients for barometer compensation
    // @Values: 0:Disabled, 1:Enabled
    // @User: Advanced
    AP_GROUPINFO_FLAGS("ENABLE", 1, WindCoeff, enable, 0, AP_PARAM_FLAG_ENABLE),

    // @Param: FWD
    // @DisplayName: Pressure error coefficient in positive X direction (forward)
    // @Description: This is the ratio of static pressure error to dynamic pressure generated by a positive wind relative velocity along the X body axis. If the baro height estimate rises during forwards flight, then this will be a negative number. Multirotors can use this feature only if using EKF3 and if the EK3_DRAG_BCOEF_X and EK3_DRAG_BCOEF_Y parameters have been tuned.
    // @Range: -1.0 1.0
    // @Increment: 0.05
    // @User: Advanced
    AP_GROUPINFO("FWD", 2, WindCoeff, xp, 0.0),

    // @Param: BCK
    // @DisplayName: Pressure error coefficient in negative X direction (backwards)
    // @Description: This is the ratio of static pressure error to dynamic pressure generated by a negative wind relative velocity along the X body axis. If the baro height estimate rises during backwards flight, then this will be a negative number. Multirotors can use this feature only if using EKF3 and if the EK3_DRAG_BCOEF_X and EK3_DRAG_BCOEF_Y parameters have been tuned.
    // @Range: -1.0 1.0
    // @Increment: 0.05
    // @User: Advanced
    AP_GROUPINFO("BCK", 3, WindCoeff, xn, 0.0),

    // @Param: RGT
    // @DisplayName: Pressure error coefficient in positive Y direction (right)
    // @Description: This is the ratio of static pressure error to dynamic pressure generated by a positive wind relative velocity along the Y body axis. If the baro height estimate rises during sideways flight to the right, then this should be a negative number. Multirotors can use this feature only if using EKF3 and if the EK3_DRAG_BCOEF_X and EK3_DRAG_BCOEF_Y parameters have been tuned.
    // @Range: -1.0 1.0
    // @Increment: 0.05
    // @User: Advanced
    AP_GROUPINFO("RGT", 4, WindCoeff, yp, 0.0),

    // @Param: LFT
    // @DisplayName: Pressure error coefficient in negative Y direction (left)
    // @Description: This is the ratio of static pressure error to dynamic pressure generated by a negative wind relative velocity along the Y body axis. If the baro height estimate rises during sideways flight to the left, then this should be a negative number. Multirotors can use this feature only if using EKF3 and if the EK3_DRAG_BCOEF_X and EK3_DRAG_BCOEF_Y parameters have been tuned.
    // @Range: -1.0 1.0
    // @Increment: 0.05
    // @User: Advanced
    AP_GROUPINFO("LFT", 5, WindCoeff, yn, 0.0),

    AP_GROUPEND
};


/*
  return pressure correction for wind based on GND_WCOEF parameters
 */
float AP_Baro::wind_pressure_correction(uint8_t instance)
{
    const WindCoeff &wcoef = sensors[instance].wind_coeff;

    if (!wcoef.enable) {
        return 0;
    }

    // correct for static pressure position errors
    Vector3f airspeed_vec_bf;
    if (!AP::ahrs().airspeed_vector_true(airspeed_vec_bf)) {
        return 0;
    }

    float error = 0.0;
    const float sqx = sq(airspeed_vec_bf.x);
    const float sqy = sq(airspeed_vec_bf.y);

    if (is_positive(airspeed_vec_bf.x)) {
        error += wcoef.xp * sqx;
    } else {
        error += wcoef.xn * sqx;
    }
    if (is_positive(airspeed_vec_bf.y)) {
        error += wcoef.yp * sqy;
    } else {
        error += wcoef.yn * sqy;
    }

    return error * 0.5 * SSL_AIR_DENSITY * get_air_density_ratio();
}
#endif // HAL_BARO_WIND_COMP_ENABLED

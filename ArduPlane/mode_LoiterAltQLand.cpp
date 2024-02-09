#include "mode.h"
#include "Plane.h"

#if HAL_QUADPLANE_ENABLED

bool ModeLoiterAltQLand::_enter()
{
    if (plane.previous_mode->is_vtol_mode() || plane.quadplane.in_vtol_mode()) {
        plane.set_mode(plane.mode_qland, ModeReason::LOITER_ALT_IN_VTOL);
        return true;
    }

    ModeLoiter::_enter();

    plane.next_WP_loc.alt = quadplane.get_landable_alt_cm();
    plane.setup_terrain_target_alt(plane.next_WP_loc);

    switch_qland();

    return true;
}

void ModeLoiterAltQLand::navigate()
{
    switch_qland();

    ModeLoiter::navigate();
}

void ModeLoiterAltQLand::switch_qland()
{
    if (plane.height_above_target() < 10.0f && plane.nav_controller->reached_loiter_target()) {
        plane.set_mode(plane.mode_qland, ModeReason::LOITER_ALT_REACHED_QLAND);
    }
}

bool ModeLoiterAltQLand::handle_guided_request(Location target_loc)
{
    // setup altitude
    target_loc.alt = quadplane.get_landable_alt_cm();
    plane.setup_terrain_target_alt(target_loc);

    plane.set_guided_WP(target_loc);

    return true;
}

#endif

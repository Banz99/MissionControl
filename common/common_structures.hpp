/*
 * Copyright (c) 2020-2021 ndeadly
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

namespace ams::controller {

    struct RGBColour {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    } __attribute__ ((__packed__));

    struct SwitchControllerColours  {
        RGBColour body;
        RGBColour buttons;
        RGBColour left_grip;
        RGBColour right_grip;
    } __attribute__ ((__packed__));

    const SwitchControllerColours g_default_colours_pro_controller = {
        .body       = {0x32, 0x32, 0x32},
        .buttons    = {0xe6, 0xe6, 0xe6},
        .left_grip  = {0x46, 0x46, 0x46},
        .right_grip = {0x46, 0x46, 0x46}
    };

    const SwitchControllerColours g_default_colours_joycon = {
        .body       = {0x82, 0x82, 0x82},
        .buttons    = {0x0f, 0x0f, 0x0f},
        .left_grip  = {0xff, 0xff, 0xff},
        .right_grip = {0xff, 0xff, 0xff}
    };

}

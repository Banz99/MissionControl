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
#include "switch_analog_stick.hpp"
#include "../../../common/bluetooth_types.hpp"

namespace ams::controller {

    Result OpenSPIFile(const bluetooth::Address *address, fs::FileHandle *spi_flash_file, SwitchAnalogStickFactoryCalibration *lstick_factory_calib, SwitchAnalogStickFactoryCalibration *rstick_factory_calib);
    Result CloseSPIFile(fs::FileHandle spi_flash_file);
}

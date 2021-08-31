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
#include <switch.h>
#include <stratosphere.hpp>
#include "../../../../common/bluetooth_types.hpp"

namespace ams::bluetooth::ble {

    bool IsInitialized(void);
    void SignalInitialized(void);
    void WaitInitialized(void);

    os::SystemEvent *GetSystemEvent(void);
    os::SystemEvent *GetForwardEvent(void);
    os::SystemEvent *GetUserForwardEvent(void);

    Result GetEventInfo(bluetooth::BleEventType *type, void *buffer, size_t size);
    void HandleEvent(void);

}

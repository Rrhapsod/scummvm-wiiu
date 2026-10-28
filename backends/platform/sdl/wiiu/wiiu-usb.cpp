/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#define FORBIDDEN_SYMBOL_ALLOW_ALL
#include "backends/platform/sdl/wiiu/wiiu-usb.h"

#include <fat.h>
#include <mocha/mocha.h>
#include <sys/iosupport.h>

WiiUUSBStorage::WiiUUSBStorage() : _attempted(false), _initialized(false), _mounted(false),
	_status("not attempted") {
}

void WiiUUSBStorage::mount() {
	if (_attempted)
		return;
	_attempted = true;

	// fatMount accepts an existing name as success. Never claim or unmount
	// another owner's device, including a mount registered by a loader.
	if (FindDevice("usb:") >= 0) {
		_status = "usb: is already owned by another driver; skipped";
		return;
	}
	if (Mocha_InitLibrary() != MOCHA_RESULT_SUCCESS) {
		_status = "Mocha API unavailable; SD remains available";
		return;
	}
	_initialized = true;
	// One FAT USB drive, first valid partition, bounded 128 KiB sector cache.
	// Do not call fatInitDefault: it would also try to own/remount the SD.
	_mounted = fatMount("usb", &Mocha_usb_disc_interface, 0, 4, 64);
	_status = _mounted ? "FAT USB mounted at usb:/ (games and optional saves)" :
		"no readable FAT USB partition; SD remains available";
	if (!_mounted) {
		// fatMount can fail after opening the raw device.
		Mocha_usb_disc_interface.shutdown();
		Mocha_DeInitLibrary();
		_initialized = false;
	}
}

WiiUUSBStorage::~WiiUUSBStorage() {
	if (_mounted) {
		// main keeps this object alive until engines, save manager, mixer,
		// timers and SDL have all finished. Flush before closing raw access.
		fatUnmount("usb:");
		Mocha_usb_disc_interface.shutdown();
	}
	if (_initialized)
		Mocha_DeInitLibrary();
}

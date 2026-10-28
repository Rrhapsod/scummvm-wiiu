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


#ifndef PLATFORM_SDL_WIIU_USB_H
#define PLATFORM_SDL_WIIU_USB_H

// Own only our FAT devoptab and raw USB client. The WUT SD mount and SDL's
// ProcUI/GX2 lifecycle belong to their existing owners.
class WiiUUSBStorage {
public:
	WiiUUSBStorage();
	~WiiUUSBStorage();
	void mount();
	const char *root() const { return _mounted ? "usb:/" : nullptr; }
	const char *status() const { return _status; }
private:
	WiiUUSBStorage(const WiiUUSBStorage &) = delete;
	WiiUUSBStorage &operator=(const WiiUUSBStorage &) = delete;
	bool _attempted;
	bool _initialized;
	bool _mounted;
	const char *_status;
};

#endif

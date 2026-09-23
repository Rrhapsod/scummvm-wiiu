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


#include "backends/platform/sdl/wiiu/wiiu.h"
#include "base/main.h"

int main(int argc, char *argv[]) {
	// WUT's CRT initializes libc and the SD filesystem. SDL owns ProcUI/GX2.
	g_system = new OSystem_WiiU();
	g_system->init();
	const int result = scummvm_main(argc, argv);
	g_system->destroy();
	return result;
}

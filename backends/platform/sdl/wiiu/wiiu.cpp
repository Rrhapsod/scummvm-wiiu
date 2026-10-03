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
#include "backends/platform/sdl/wiiu/wiiu-diagnostics.h"
#include "backends/platform/sdl/wiiu/wiiu-events.h"
#include "backends/fs/posix-drives/posix-drives-fs-factory.h"
#include "backends/saves/default/default-saves.h"
#include "common/config-manager.h"
#include "common/fs.h"
#include "common/textconsole.h"

static const char *const kDataRoot = "/vol/external01/scummvm";

OSystem_WiiU::~OSystem_WiiU() {
	// Discard text while the event source/window still exist, before SDL's
	// base destructor tears down graphics, audio and finally the video device.
	setFeatureState(kFeatureVirtualKeyboard, false);
	debug(0, "Wii U backend teardown: entering SDL cleanup");
}

void OSystem_WiiU::engineDone() {
	debug(0, "Wii U engineDone: clearing keyboard input");
	setFeatureState(kFeatureVirtualKeyboard, false);
	OSystem_SDL::engineDone();
	debug(0, "Wii U engineDone: hook complete; engine destruction still pending");
}

void OSystem_WiiU::quit() {
	debug(0, "Wii U quit requested");
	setFeatureState(kFeatureVirtualKeyboard, false);
	OSystem_SDL::quit();
}

void OSystem_WiiU::init() {
	DrivesPOSIXFilesystemFactory *factory = new DrivesPOSIXFilesystemFactory();
	factory->addDrive("/vol/external01");
	_fsFactory = factory;

	// Do not depend on Aroma's current working directory or HOME.
	const Common::FSNode root(kDataRoot);
	if (!root.exists() && !root.createDirectory())
		warning("Could not create %s", kDataRoot);
	const Common::FSNode saves(Common::Path(kDataRoot).join("saves"));
	if (!saves.exists() && !saves.createDirectory())
		warning("Could not create the Wii U save directory");

	OSystem_SDL::init();
}

void OSystem_WiiU::initBackend() {
	ConfMan.registerDefault("joystick_num", 0);
	ConfMan.registerDefault("touchpad_mouse_mode", false);
	ConfMan.registerDefault("output_rate", 48000);
	ConfMan.registerDefault("fullscreen", true);
	ConfMan.registerDefault("browser_lastpath", Common::Path("/vol/external01"));
	ConfMan.registerDefault("extrapath", Common::Path(kDataRoot).join("data"));

	ConfMan.setBool("fullscreen", true);
	// Wii U SDL generates touch events from the opened GamePad joystick.
	if (ConfMan.getInt("joystick_num") < 0)
		ConfMan.setInt("joystick_num", 0);

	if (!_savefileManager)
		_savefileManager = new DefaultSaveFileManager(Common::Path(kDataRoot).join("saves"));
	_eventSource = new WiiUEventSource();
	OSystem_SDL::initBackend();
	WIIU_TRACE("BUILD lifecycle diagnostic 20261002; automatic tracing enabled; not a freeze fix");
	debug(0, "Wii U SDL backend initialized; config and saves under %s", kDataRoot);
}

bool OSystem_WiiU::hasFeature(Feature f) {
	if (f == kFeatureVirtualKeyboard)
		return SDL_HasScreenKeyboardSupport() == SDL_TRUE;
	if (f == kFeatureFullscreenMode || f == kFeatureIconifyWindow ||
	    f == kFeatureClipboardSupport || f == kFeatureOpenUrl)
		return false;
	return OSystem_SDL::hasFeature(f);
}

void OSystem_WiiU::setFeatureState(Feature f, bool enable) {
	if (f == kFeatureVirtualKeyboard) {
		if (_eventSource)
			static_cast<WiiUEventSource *>(_eventSource)->setKeyboardVisible(enable);
		return;
	}
	OSystem_SDL::setFeatureState(f, enable);
}

bool OSystem_WiiU::getFeatureState(Feature f) {
	if (f == kFeatureVirtualKeyboard)
		return _eventSource && static_cast<WiiUEventSource *>(_eventSource)->isKeyboardActive();
	return OSystem_SDL::getFeatureState(f);
}

Common::Path OSystem_WiiU::getDefaultConfigFileName() {
	return Common::Path(kDataRoot).join("scummvm.ini");
}

Common::Path OSystem_WiiU::getDefaultLogFileName() {
	return Common::Path(kDataRoot).join("scummvm.log");
}

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

#ifndef PLATFORM_SDL_WIIU_EVENTS_H
#define PLATFORM_SDL_WIIU_EVENTS_H

#include "backends/events/sdl/sdl-events.h"
#include "common/ustr.h"
#include "common/queue.h"

class WiiUEventSource : public SdlEventSource {
public:
	WiiUEventSource();
	~WiiUEventSource() override;
	bool pollEvent(Common::Event &event) override;
	bool allowMapping() const override { return !_nativeKeyEvent; }
	void setKeyboardVisible(bool visible);
	void setKeyboardText(const Common::U32String &text);
	bool isKeyboardActive() const { return _keyboardActive; }

protected:
	bool dispatchSDLEvent(SDL_Event &ev, Common::Event &event) override;

private:
	// SDL's Wii U driver exposes one GamePad and four WPAD channels. Keep all
	// open: closing the GamePad also stops its touchscreen samples.
	struct Controller {
		SDL_Joystick *joystick = nullptr;
		SDL_JoystickID id = -1;
		bool remote = false;
		uint32 buttons = 0;
		int16 axes[6] = {};
	};
	Controller _controllers[5];
	uint32 _controllerButtons = 0;
	int16 _controllerAxes[6] = {};
	Common::Queue<Common::Event> _controllerEvents;
	void openController(int index);
	void removeController(SDL_JoystickID id);
	void refreshPrimaryJoystick();
	Controller *findController(SDL_JoystickID id);
	void updateControllerState();
	bool dispatchControllerEvent(const SDL_Event &ev, Common::Event &event);
	bool popControllerEvent(Common::Event &event);
#ifdef WIIU_REMOTE_TEST
	uint32 _pointerReports = 0;
	uint32 _trackingReports = 0;
#endif
	bool pollKeyboardEvent(Common::Event &event);
	void closeKeyboard();
	bool _keyboardActive = false;
	bool _keyboardWasShown = false;
	bool _inSubmission = false;
	bool _acceptText = false;
	bool _keyUpPending = false;
	bool _nativeKeyEvent = false;
	Common::KeyState _lastKey;
	Common::U32String _text;
	Common::U32String _fieldText, _openingText;
	bool _hasFieldText = false;
	bool _hasOpeningText = false;
	bool _moveToEnd = false;
	uint32 _eraseCount = 0;
	uint32 _textPos = 0;
	uint32 _nextKeyTime = 0;
};

#endif

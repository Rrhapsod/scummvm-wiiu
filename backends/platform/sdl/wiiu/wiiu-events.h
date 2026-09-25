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

class WiiUEventSource : public SdlEventSource {
public:
	WiiUEventSource();
	bool pollEvent(Common::Event &event) override;
	bool allowMapping() const override { return !_nativeKeyEvent; }
	void setKeyboardVisible(bool visible);
	bool isKeyboardActive() const { return _keyboardActive; }

protected:
	bool dispatchSDLEvent(SDL_Event &ev, Common::Event &event) override;

private:
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
	uint32 _textPos = 0;
	uint32 _nextKeyTime = 0;
};

#endif

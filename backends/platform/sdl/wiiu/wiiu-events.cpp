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

#include "backends/platform/sdl/wiiu/wiiu-events.h"
#include "backends/graphics/sdl/sdl-graphics.h"
#include "backends/platform/sdl/sdl-window.h"
#include "common/system.h"
#include "common/textconsole.h"
#include <SDL_syswm.h>

WiiUEventSource::WiiUEventSource() {
	// SDL's Wii U keyboard brackets its UTF-8 submission with SYSWM events.
	SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
}

void WiiUEventSource::closeKeyboard() {
	_keyboardActive = false;
	SDL_StopTextInput();
}

void WiiUEventSource::setKeyboardVisible(bool visible) {
	if (!visible) {
		closeKeyboard();
		// The receiving field may have lost focus; do not type into the next one.
		_text.clear();
		_textPos = 0;
		_acceptText = false;
		return;
	}

	SDL_Window *window = SDL_GetKeyboardFocus();
	if (!window || _keyboardActive || SDL_IsScreenKeyboardShown(window))
		return;
	_text.clear();
	_textPos = 0;
	SDL_StartTextInput();
	_keyboardActive = SDL_IsScreenKeyboardShown(window) == SDL_TRUE;
	if (!_keyboardActive)
		warning("Wii U native keyboard did not open");
}

bool WiiUEventSource::pollEvent(Common::Event &event) {
	_nativeKeyEvent = false;
	if (_graphicsManager) {
		SDL_Window *window = _graphicsManager->getWindow()->getSDLWindow();
		bool shown = SDL_IsScreenKeyboardShown(window) == SDL_TRUE;
		// SDL draws swkbd during RenderPresent. Even an idle save dialog must
		// present while it is visible, and redraw once after it disappears.
		if (shown || _keyboardWasShown)
			_graphicsManager->notifyVideoExpose();
		_keyboardWasShown = shown;
	}
	if (pollKeyboardEvent(event))
		return true;
	return SdlEventSource::pollEvent(event);
}

bool WiiUEventSource::pollKeyboardEvent(Common::Event &event) {
	if (_keyUpPending) {
		event = Common::Event();
		event.type = Common::EVENT_KEYUP;
		event.kbd = _lastKey;
		_keyUpPending = false;
		_nativeKeyEvent = true;
		return true;
	}
	if (_inSubmission || _textPos >= _text.size())
		return false;
	uint32 now = g_system->getMillis();
	if ((int32)(now - _nextKeyTime) < 0)
		return false;

	// KeyState stores 16-bit characters. Never turn a supplementary Unicode
	// character into a truncated key (possibly a control character).
	while (_textPos < _text.size()) {
		uint32 c = _text[_textPos++];
		if (c < 32 || c == 127 || c > 0xffff)
			continue;
		event = Common::Event();
		event.type = Common::EVENT_KEYDOWN;
		// Original SCUMM save dialogs reject KEYCODE_INVALID even when ascii
		// is set. Supply printable keycodes, but bypass key mapping so text
		// cannot trigger game actions (Space = pause, for example).
		Common::KeyCode key = Common::KEYCODE_INVALID;
		if (c < 127)
			key = (Common::KeyCode)((c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c);
		if (c == '~')
			key = Common::KEYCODE_TILDE;
		event.kbd = Common::KeyState(key, (uint16)c, 0);
		_lastKey = event.kbd;
		_keyUpPending = true;
		_nativeKeyEvent = true;
		// Original SCUMM dialogs retain just one key per engine tick. Do not
		// flood them with the entire name in one poll cycle. No blocking sleep.
		_nextKeyTime = now + 100;
		return true;
	}
	return false;
}

bool WiiUEventSource::dispatchSDLEvent(SDL_Event &ev, Common::Event &event) {
	if (ev.type == SDL_SYSWMEVENT && ev.syswm.msg && ev.syswm.msg->subsystem == SDL_SYSWM_WIIU) {
		switch (ev.syswm.msg->msg.wiiu.event) {
		case SDL_WIIU_SYSWM_SWKBD_OK_START_EVENT:
			_inSubmission = true;
			_acceptText = _keyboardActive;
			if (_acceptText) {
				_text.clear();
				_textPos = 0;
			}
			return false;
		case SDL_WIIU_SYSWM_SWKBD_OK_FINISH_EVENT:
			_inSubmission = false;
			if (_acceptText) {
				// Close only AFTER all SDL_TEXTINPUT chunks have been collected:
				// StopTextInput disables and flushes queued SDL text events.
				closeKeyboard();
				debug(0, "Wii U keyboard accepted %u characters", (unsigned int)_text.size());
				_acceptText = false;
				_nextKeyTime = g_system->getMillis();
			}
			// No synthetic Enter: OK accepts text, not the underlying dialog.
			return pollKeyboardEvent(event);
		case SDL_WIIU_SYSWM_SWKBD_CANCEL_EVENT:
			debug(0, "Wii U keyboard cancelled");
			setKeyboardVisible(false);
			_inSubmission = false;
			return false;
		}
	}
	if (ev.type == SDL_TEXTINPUT && _inSubmission) {
		if (_acceptText)
			_text += Common::U32String(ev.text.text);
		// Discard repeated submissions during the keyboard's closing animation.
		return false;
	}
	return SdlEventSource::dispatchSDLEvent(ev, event);
}

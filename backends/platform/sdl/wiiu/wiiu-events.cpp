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
#include <SDL_system.h>

WiiUEventSource::WiiUEventSource() {
	// SDL's Wii U keyboard brackets its UTF-8 submission with SYSWM events.
	SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
	SDL_SetHint("SDL_WIIU_REMOTE_POINTER", "1");
	// A Remote is a mouse, not a synthetic touchscreen contact.
	SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "0");
	closeJoystick();
	for (int i = 0; i < SDL_NumJoysticks(); ++i)
		openController(i);
	refreshPrimaryJoystick();
}

WiiUEventSource::~WiiUEventSource() {
	closeKeyboard();
	closeJoystick();
	for (auto &controller : _controllers) {
		if (controller.joystick)
			SDL_JoystickClose(controller.joystick);
	}
	SDL_SetHint("SDL_WIIU_REMOTE_POINTER", "0");
}

WiiUEventSource::Controller *WiiUEventSource::findController(SDL_JoystickID id) {
	for (auto &controller : _controllers) {
		if (controller.joystick && controller.id == id)
			return &controller;
	}
	return nullptr;
}

void WiiUEventSource::openController(int index) {
	const SDL_JoystickID id = SDL_JoystickGetDeviceInstanceID(index);
	if (id < 0 || findController(id))
		return;
	for (auto &controller : _controllers) {
		if (controller.joystick)
			continue;
		controller.joystick = SDL_JoystickOpen(index);
		if (!controller.joystick) {
			warning("Wii U: could not open controller %d: %s", index, SDL_GetError());
			return;
		}
		controller.id = id;
		const char *name = SDL_JoystickName(controller.joystick);
		controller.remote = name && !strncmp(name, "Wii Remote #", 12);
		debug(0, "Wii U input: opened instance %d: %s%s", (int)id,
			name ? name : "unknown", controller.remote ? " (IR mouse A/B)" : "");
		return;
	}
}

void WiiUEventSource::refreshPrimaryJoystick() {
	// The base class uses this handle to advertise joystick/keymapper support.
	// Raw events from all five devices are normalized below, with no duplicate
	// GameController events and no dependency on device-index ordering.
	if (_joystick)
		return;
	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
		if (findController(SDL_JoystickGetDeviceInstanceID(i))) {
			_joystick = SDL_JoystickOpen(i);
			return;
		}
	}
}

void WiiUEventSource::removeController(SDL_JoystickID id) {
	Controller *controller = findController(id);
	if (!controller)
		return;
	if (_joystick && SDL_JoystickInstanceID(_joystick) == id)
		closeJoystick();
	SDL_JoystickClose(controller->joystick);
	*controller = Controller();
	updateControllerState(); // Release held keys/sticks before selecting another device.
	refreshPrimaryJoystick();
	debug(0, "Wii U input: removed instance %d", (int)id);
}

void WiiUEventSource::updateControllerState() {
	// These raw indices are the Wii U SDL driver's common button layout, NOT
	// the desktop SDL joystick fallback layout. 8/9 are digital triggers.
	static const Common::JoystickButton buttons[16] = {
		Common::JOYSTICK_BUTTON_A, Common::JOYSTICK_BUTTON_B,
		Common::JOYSTICK_BUTTON_X, Common::JOYSTICK_BUTTON_Y,
		Common::JOYSTICK_BUTTON_LEFT_STICK, Common::JOYSTICK_BUTTON_RIGHT_STICK,
		Common::JOYSTICK_BUTTON_LEFT_SHOULDER, Common::JOYSTICK_BUTTON_RIGHT_SHOULDER,
		Common::JOYSTICK_BUTTON_INVALID, Common::JOYSTICK_BUTTON_INVALID,
		Common::JOYSTICK_BUTTON_START, Common::JOYSTICK_BUTTON_BACK,
		Common::JOYSTICK_BUTTON_DPAD_LEFT, Common::JOYSTICK_BUTTON_DPAD_UP,
		Common::JOYSTICK_BUTTON_DPAD_RIGHT, Common::JOYSTICK_BUTTON_DPAD_DOWN
	};
	uint32 held = 0;
	int16 axes[6] = {};
	for (const auto &controller : _controllers) {
		uint32 buttons = controller.buttons;
		if (controller.remote) {
			// Swap Remote 2 (raw 7/R) with Nunchuk C (raw 3/Y). Remote 2
			// shares GamePad Y/Escape; C shares GamePad R/slow analog mouse.
			// Retain physical state until aggregation across all controllers.
			const uint32 swap = (1u << 3) | (1u << 7);
			buttons = (buttons & ~swap) | ((buttons & (1u << 7)) >> 4) |
				((buttons & (1u << 3)) << 4);
		}
		held |= buttons;
		for (int i = 0; i < 6; ++i) {
			// An idle device must not cancel another device's deflected stick.
			const int candidate = controller.axes[i], current = axes[i];
			if ((candidate < 0 ? -candidate : candidate) > (current < 0 ? -current : current))
				axes[i] = controller.axes[i];
		}
	}
	for (int i = 0; i < 16; ++i) {
		if (buttons[i] == Common::JOYSTICK_BUTTON_INVALID || !((held ^ _controllerButtons) & (1u << i)))
			continue;
		Common::Event event;
		event.type = (held & (1u << i)) ? Common::EVENT_JOYBUTTON_DOWN : Common::EVENT_JOYBUTTON_UP;
		event.joystick.button = buttons[i];
		_controllerEvents.push(event);
	}
	_controllerButtons = held;
	for (int i = 0; i < 6; ++i) {
		if (axes[i] == _controllerAxes[i])
			continue;
		Common::Event event;
		event.type = Common::EVENT_JOYAXIS_MOTION;
		event.joystick.axis = i;
		event.joystick.position = axes[i];
		_controllerEvents.push(event);
		_controllerAxes[i] = axes[i];
	}
}

bool WiiUEventSource::popControllerEvent(Common::Event &event) {
	if (_controllerEvents.empty())
		return false;
	event = _controllerEvents.pop();
	return true;
}

bool WiiUEventSource::dispatchControllerEvent(const SDL_Event &ev, Common::Event &event) {
	if (ev.type == SDL_JOYDEVICEADDED || ev.type == SDL_JOYDEVICEREMOVED) {
		if (ev.type == SDL_JOYDEVICEADDED) {
			openController(ev.jdevice.which);
			refreshPrimaryJoystick();
		} else {
			removeController(ev.jdevice.which);
		}
		Common::Event changed;
		changed.type = Common::EVENT_INPUT_CHANGED;
		_controllerEvents.push(changed);
		return popControllerEvent(event);
	}
	const bool button = ev.type == SDL_JOYBUTTONDOWN || ev.type == SDL_JOYBUTTONUP;
	Controller *controller = findController(button ? ev.jbutton.which : ev.jaxis.which);
	if (!controller)
		return false;
	if (button) {
		const unsigned int index = ev.jbutton.button;
		// The opted-in SDL driver emits real mouse buttons for Remote A/B.
		if (index >= 16 || (controller->remote && index < 2))
			return false;
		const bool down = ev.type == SDL_JOYBUTTONDOWN;
		if (down)
			controller->buttons |= 1u << index;
		else
			controller->buttons &= ~(1u << index);
		if (index == 8 || index == 9)
			controller->axes[index - 4] = down ? 32767 : 0;
	} else if (ev.jaxis.axis < 4) {
		controller->axes[ev.jaxis.axis] = ev.jaxis.value;
	}
	updateControllerState();
	return popControllerEvent(event);
}

void WiiUEventSource::closeKeyboard() {
	if (!_keyboardActive)
		return;
	_keyboardActive = false;
	SDL_StopTextInput();
}

void WiiUEventSource::setKeyboardText(const Common::U32String &text) {
	_fieldText = text;
	_hasFieldText = true;
}

void WiiUEventSource::setKeyboardVisible(bool visible) {
	if (!visible) {
		closeKeyboard();
		// The receiving field may have lost focus; do not type into the next one.
		_text.clear();
		_textPos = 0;
		_acceptText = false;
		_inSubmission = false;
		_hasFieldText = _hasOpeningText = _moveToEnd = false;
		_fieldText.clear();
		_openingText.clear();
		_eraseCount = 0;
		return;
	}

	SDL_Window *window = SDL_GetKeyboardFocus();
	if (!window || _keyboardActive || SDL_IsScreenKeyboardShown(window))
		return;
	_text.clear();
	_textPos = 0;
	_eraseCount = 0;
	_moveToEnd = false;
	_hasOpeningText = _hasFieldText;
	_openingText = _fieldText;
	SDL_WiiUSetSWKBDInitialText(_hasOpeningText ? _openingText.encode().c_str() : nullptr);
	debug(0, "Wii U keyboard opening");
#ifdef WIIU_COMPAT_DIAGNOSTICS
	const int controllers = SDL_NumJoysticks();
	debug(0, "Wii U keyboard: %d detected controller(s)", controllers);
	for (int i = 0; i < controllers && i < 8; ++i) {
		const char *name = SDL_JoystickNameForIndex(i);
		debug(0, "Wii U keyboard: controller %d = %s", i, name ? name : "unknown");
	}
#endif
	SDL_StartTextInput();
	_keyboardActive = SDL_IsScreenKeyboardShown(window) == SDL_TRUE;
	if (!_keyboardActive)
		warning("Wii U native keyboard did not open");
}

bool WiiUEventSource::pollEvent(Common::Event &event) {
	_nativeKeyEvent = false;
	if (popControllerEvent(event))
		return true;
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
	if (SdlEventSource::pollEvent(event))
		return true;
	return popControllerEvent(event);
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
	if (_inSubmission || (!_moveToEnd && !_eraseCount && _textPos >= _text.size()))
		return false;
	uint32 now = g_system->getMillis();
	if ((int32)(now - _nextKeyTime) < 0)
		return false;

	// Only change the field after OK. Keep the common prefix, delete the old
	// suffix, then insert the new suffix. Original SCUMM labels edit at their
	// end and ignore End; GUI widgets use it to clear selection/move the caret.
	if (_moveToEnd || _eraseCount) {
		const bool end = _moveToEnd;
		_moveToEnd = false;
		if (!end)
			--_eraseCount;
		event = Common::Event();
		event.type = Common::EVENT_KEYDOWN;
		event.kbd = Common::KeyState(end ? Common::KEYCODE_END : Common::KEYCODE_BACKSPACE,
			end ? 0 : 8, 0);
		_lastKey = event.kbd;
		_keyUpPending = _nativeKeyEvent = true;
		_nextKeyTime = now + 100;
		return true;
	}

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
	if (ev.type == SDL_USEREVENT && ev.user.code >= 0x57495200 && ev.user.code < 0x57495208) {
#ifdef WIIU_REMOTE_TEST
		if (_trackingReports++ < 64)
			debug(0, "Wii U IR: channel=%u tracking=%s", (unsigned int)((ev.user.code - 0x57495200) / 2),
				(ev.user.code & 1) ? "valid" : "lost");
#endif
		return false;
	}
	switch (ev.type) {
	case SDL_JOYDEVICEADDED:
	case SDL_JOYDEVICEREMOVED:
	case SDL_JOYBUTTONDOWN:
	case SDL_JOYBUTTONUP:
	case SDL_JOYAXISMOTION:
		return dispatchControllerEvent(ev, event);
	case SDL_CONTROLLERBUTTONDOWN:
	case SDL_CONTROLLERBUTTONUP:
	case SDL_CONTROLLERAXISMOTION:
	case SDL_CONTROLLERDEVICEADDED:
	case SDL_CONTROLLERDEVICEREMOVED:
	case SDL_CONTROLLERDEVICEREMAPPED:
		return false; // Each physical input is delivered once, through SDL_JOY*.
	default:
		break;
	}
#ifdef WIIU_REMOTE_TEST
	if ((ev.type == SDL_MOUSEMOTION || ev.type == SDL_MOUSEBUTTONDOWN || ev.type == SDL_MOUSEBUTTONUP) &&
		(ev.motion.which & 0xfffffffcU) == 0x57494900U && _pointerReports < 32) {
		++_pointerReports;
		debug(0, "Wii U IR event: channel=%u type=%u x=%d y=%d button=%u",
			(unsigned int)(ev.motion.which & 3), (unsigned int)ev.type,
			(int)(ev.type == SDL_MOUSEMOTION ? ev.motion.x : ev.button.x),
			(int)(ev.type == SDL_MOUSEMOTION ? ev.motion.y : ev.button.y),
			ev.type == SDL_MOUSEMOTION ? 0 : ev.button.button);
	}
#endif
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
				if (_hasOpeningText) {
					uint32 prefix = 0;
					while (prefix < _openingText.size() && prefix < _text.size() &&
						_openingText[prefix] == _text[prefix])
						++prefix;
					_eraseCount = _openingText.size() - prefix;
					_textPos = prefix;
					_moveToEnd = _eraseCount || prefix < _text.size();
				}
				debug(0, "Wii U keyboard accepted %u characters", (unsigned int)_text.size());
				_acceptText = false;
				_nextKeyTime = g_system->getMillis();
			}
			// No synthetic Enter: OK accepts text, not the underlying dialog.
			return pollKeyboardEvent(event);
		case SDL_WIIU_SYSWM_SWKBD_CANCEL_EVENT:
			debug(0, "Wii U keyboard cancelled");
			// Keep the focused field context for a manual reopen, but never
			// deliver partial text or deletion keys on Cancel.
			closeKeyboard();
			_text.clear();
			_textPos = _eraseCount = 0;
			_moveToEnd = _acceptText = false;
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

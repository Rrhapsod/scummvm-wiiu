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

#include "backends/keymapper/action.h"
#include "backends/keymapper/keymap.h"
#include "backends/keymapper/standard-actions.h"
#include "base/plugins.h"
#include "engines/advancedDetector.h"
#include "common/savefile.h"
#include "common/system.h"
#include "common/translation.h"

#include "mm/detection.h"
#ifdef ENABLE_MM1
#include "mm/mm1/mm1.h"
#endif
#ifdef ENABLE_XEEN
#include "mm/xeen/xeen.h"
#include "mm/xeen/metaengine.h"
#include "mm/xeen/worldofxeen/worldofxeen.h"
#include "mm/xeen/swordsofxeen/swordsofxeen.h"
#endif

static const ADExtraGuiOptionsMap optionsList[] = {
	{
		GAMEOPTION_COPY_PROTECTION,
		{
			_s("Enable copy protection"),
			_s("Enable any copy protection that would otherwise be bypassed by default."),
			"copy_protection",
			false,
			0,
			0
		},
	},
	{
		GAMEOPTION_SHOW_ITEM_COSTS,
		{
			_s("Show item costs in standard inventory mode"),
			_s("Shows item costs in standard inventory mode, allowing the value of items to be compared"),
			"ShowItemCosts",
			false,
			0,
			0
		}
	},
	{
		GAMEOPTION_DURABLE_ARMOR,
		{
			_s("More durable armor"),
			_s("Armor won't break until character is at -80HP, rather than merely -10HP"),
			"DurableArmor",
			false,
			0,
			0
		}
	},
	{
		GAMEOPTION_SHOW_HP_SP_BARS,
		{
			_s("Hitpoint bars"),
			_s("Replace a colored gem with bars for hit points and spell points."),
			"ShowHpSpBars",
			false,
			0,
			0
		}
	},
#ifdef USE_TTS
	{
		GAMEOPTION_TTS,
		{
			_s("Enable Text to Speech"),
			_s("Use TTS to read text in the game (if TTS is available)"),
			"tts_enabled",
			false,
			0,
			0
		}
	},
#endif
	AD_EXTRA_GUI_OPTIONS_TERMINATOR
};

class MMMetaEngine : public AdvancedMetaEngine<MM::MightAndMagicGameDescription> {
private:
	/**
	 * Gets the game Id given a target string
	 */
	static Common::String getGameId(const Common::String &target);

	/**
	 * Returns true if the game is a Xeen game
	 */
	static bool isXeenGame(const Common::String &target);
public:
	const char *getName() const override {
		return "mm";
	}

	bool hasFeature(MetaEngineFeature f) const override;
	Common::Error createInstance(OSystem *syst, Engine **engine, const MM::MightAndMagicGameDescription *desc) const override;
	SaveStateList listSaves(const char *target) const override;
	SaveStateDescriptor querySaveMetaInfos(const char *target, int slot) const override;
	Common::KeymapArray initKeymaps(const char *target) const override;

	const ADExtraGuiOptionsMap *getAdvancedExtraGuiOptions() const override {
		return optionsList;
	}
};

bool MMMetaEngine::hasFeature(MetaEngineFeature f) const {
	return checkExtendedSaves(f) ||
		(f == kSupportsLoadingDuringStartup);
}

Common::Error MMMetaEngine::createInstance(OSystem *syst, Engine **engine, const MM::MightAndMagicGameDescription *gd) const {
	switch (gd->gameID) {
#ifdef ENABLE_MM1
	case MM::GType_MightAndMagic1:
		*engine = new MM::MM1::MM1Engine(syst, gd);
		break;
#endif
#ifdef ENABLE_XEEN
	case MM::GType_Clouds:
	case MM::GType_DarkSide:
	case MM::GType_WorldOfXeen:
		*engine = new MM::Xeen::WorldOfXeen::WorldOfXeenEngine(syst, gd);
		break;
	case MM::GType_Swords:
		*engine = new MM::Xeen::SwordsOfXeen::SwordsOfXeenEngine(syst, gd);
		break;
#endif
	default:
		return Common::kUnsupportedGameidError;
	}

	return Common::kNoError;
}

SaveStateList MMMetaEngine::listSaves(const char *target) const {
#ifdef ENABLE_XEEN
	if (isXeenGame(target))
		// Fallback original code for Xeen
		return MM::Xeen::XeenMetaEngine::listSaves(this, target);
#endif
	return AdvancedMetaEngine::listSaves(target);
}

SaveStateDescriptor MMMetaEngine::querySaveMetaInfos(const char *target, int slot) const {
#ifdef ENABLE_XEEN
	if (isXeenGame(target))
		// Fallback original code for Xeen
		return MM::Xeen::XeenMetaEngine::querySaveMetaInfos(this, target, slot);
#endif

	return AdvancedMetaEngine::querySaveMetaInfos(target, slot);
}

Common::KeymapArray MMMetaEngine::initKeymaps(const char *target) const {
	using namespace Common;

	KeymapArray keymaps;

	Keymap *mainKeymap = new Keymap(Keymap::kKeymapTypeGame, "mm-main", _("Might and Magic"));
	Action *act;

	act = new Action(kStandardActionLeftClick, _("Left click"));
	act->setLeftClickEvent();
	act->addDefaultInputMapping("MOUSE_LEFT");
	act->addDefaultInputMapping("JOY_A");
	mainKeymap->addAction(act);

	act = new Action(kStandardActionRightClick, _("Right click"));
	act->setRightClickEvent();
	act->addDefaultInputMapping("MOUSE_RIGHT");
	act->addDefaultInputMapping("JOY_B");
	mainKeymap->addAction(act);

	keymaps.push_back(mainKeymap);

#ifdef ENABLE_XEEN
	if (isXeenGame(target)) {
		// Xeen handles arrow keys (not the generic keypad movement bindings).
		// Keep text confirmation separate from clicking so the native keyboard
		// can submit text without implicitly closing the game's input dialog.
		Keymap *xeenKeymap = new Keymap(Keymap::kKeymapTypeGame, "xeen-controls", _("Xeen controls"));
		xeenKeymap->setPartialMatchAllowed(false);

		act = new Action(kStandardActionMoveUp, _("Move forward"));
		act->setKeyEvent(KEYCODE_UP);
		act->addDefaultInputMapping("UP");
		act->addDefaultInputMapping("JOY_UP");
		act->allowKbdRepeats();
		xeenKeymap->addAction(act);

		act = new Action(kStandardActionMoveDown, _("Move backward"));
		act->setKeyEvent(KEYCODE_DOWN);
		act->addDefaultInputMapping("DOWN");
		act->addDefaultInputMapping("JOY_DOWN");
		act->allowKbdRepeats();
		xeenKeymap->addAction(act);

		act = new Action(kStandardActionMoveLeft, _("Turn left"));
		act->setKeyEvent(KEYCODE_LEFT);
		act->addDefaultInputMapping("LEFT");
		act->addDefaultInputMapping("JOY_LEFT");
		act->allowKbdRepeats();
		xeenKeymap->addAction(act);

		act = new Action(kStandardActionMoveRight, _("Turn right"));
		act->setKeyEvent(KEYCODE_RIGHT);
		act->addDefaultInputMapping("RIGHT");
		act->addDefaultInputMapping("JOY_RIGHT");
		act->allowKbdRepeats();
		xeenKeymap->addAction(act);

		act = new Action("RETURN", _("Confirm"));
		act->setKeyEvent(KeyState(KEYCODE_RETURN, ASCII_RETURN));
		act->addDefaultInputMapping("RETURN");
		act->addDefaultInputMapping("JOY_X");
		xeenKeymap->addAction(act);

		act = new Action(kStandardActionSkip, _("Cancel"));
		act->setKeyEvent(KeyState(KEYCODE_ESCAPE, ASCII_ESCAPE));
		act->addDefaultInputMapping("ESCAPE");
		act->addDefaultInputMapping("JOY_Y");
		xeenKeymap->addAction(act);

		keymaps.push_back(xeenKeymap);
	}
#endif

#ifdef ENABLE_MM1
	const Common::String gameId = getGameId(target);
	if (gameId == "mm1" || gameId == "mm1_enh") {
		KeymapArray mm1Keymaps = MM::MM1::MetaEngine::initKeymaps();

		for (KeymapArray::iterator it = mm1Keymaps.begin(); it != mm1Keymaps.end(); ++it)
			keymaps.push_back(*it);
	}
#endif

	return keymaps;
}

Common::String MMMetaEngine::getGameId(const Common::String &target) {
	// Store a copy of the active domain
	Common::String currDomain = ConfMan.getActiveDomainName();

	// Switch to the given target domain and get it's game Id
	ConfMan.setActiveDomain(target);
	Common::String gameId = ConfMan.get("gameid");

	// Switch back to the original domain and return the game Id
	ConfMan.setActiveDomain(currDomain);
	return gameId;
}

bool MMMetaEngine::isXeenGame(const Common::String &target) {
	Common::String gameId = getGameId(target);
	if (gameId == "mm1" || gameId == "mm1_enh")
		return false;

	return true;
}

#if PLUGIN_ENABLED_DYNAMIC(MM)
	REGISTER_PLUGIN_DYNAMIC(MM, PLUGIN_TYPE_ENGINE, MMMetaEngine);
#else
	REGISTER_PLUGIN_STATIC(MM, PLUGIN_TYPE_ENGINE, MMMetaEngine);
#endif

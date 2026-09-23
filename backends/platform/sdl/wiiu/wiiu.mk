# Use the normal ScummVM linker rule; only package the resulting ELF.
WIIU_ELF2RPL := $(DEVKITPRO)/tools/bin/elf2rpl
WIIU_WUHBTOOL := $(DEVKITPRO)/tools/bin/wuhbtool

scummvm.rpx: $(EXECUTABLE)
	$(WIIU_ELF2RPL) $< $@

ScummVM.wuhb: scummvm.rpx
	$(WIIU_WUHBTOOL) $< $@ --name="ScummVM $(VERSION)" --short-name="ScummVM" --author="ScummVM Team"

# Staging contains no user configuration, savegames or games.
wiiu_release: ScummVM.wuhb
	mkdir -p wiiu_release/wiiu/apps wiiu_release/scummvm/data wiiu_release/scummvm/doc
	cp ScummVM.wuhb wiiu_release/wiiu/apps/
	cp $(DIST_FILES_THEMES) wiiu_release/scummvm/data/
	if test -n "$(DIST_FILES_ENGINEDATA)"; then cp $(DIST_FILES_ENGINEDATA) wiiu_release/scummvm/data/; fi
	if test -n "$(DIST_FILES_VKEYBD)"; then cp $(DIST_FILES_VKEYBD) wiiu_release/scummvm/data/; fi
	cp $(srcdir)/COPYING $(srcdir)/COPYRIGHT $(srcdir)/AUTHORS wiiu_release/scummvm/doc/
	cp $(srcdir)/backends/platform/sdl/wiiu/README.WIIU wiiu_release/scummvm/doc/

.PHONY: wiiu_release

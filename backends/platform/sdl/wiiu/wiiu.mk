# Use the normal ScummVM linker rule; only package the resulting ELF.
WIIU_ELF2RPL := $(DEVKITPRO)/tools/bin/elf2rpl
WIIU_WUHBTOOL := $(DEVKITPRO)/tools/bin/wuhbtool
# Reuse the upstream ScummVM icon, already at WUHB's required 128x128 size.
WIIU_ICON := $(srcdir)/dists/psp2/icon0.png

scummvm.rpx: $(EXECUTABLE)
	$(WIIU_ELF2RPL) $< $@

ScummVM.wuhb: scummvm.rpx $(WIIU_ICON) $(srcdir)/backends/platform/sdl/wiiu/wiiu.mk
	$(WIIU_WUHBTOOL) $< $@ --name="ScummVM" --short-name="ScummVM" --author="ScummVM Team" --icon="$(WIIU_ICON)"

# Staging contains no user configuration, savegames or games.
wiiu_release: ScummVM.wuhb
	mkdir -p wiiu_release/wiiu/apps wiiu_release/scummvm/data wiiu_release/scummvm/doc
	cp ScummVM.wuhb wiiu_release/wiiu/apps/
	cp $(DIST_FILES_THEMES) wiiu_release/scummvm/data/
	if test -n "$(DIST_FILES_ENGINEDATA)"; then cp $(DIST_FILES_ENGINEDATA) wiiu_release/scummvm/data/; fi
	if test -n "$(DIST_FILES_VKEYBD)"; then cp $(DIST_FILES_VKEYBD) wiiu_release/scummvm/data/; fi
	cp $(srcdir)/COPYING $(srcdir)/COPYRIGHT $(srcdir)/AUTHORS wiiu_release/scummvm/doc/
	cp $(srcdir)/README.md $(srcdir)/README.scummvm.md wiiu_release/scummvm/doc/
	mkdir -p wiiu_release/scummvm/doc/dists/psp2 wiiu_release/scummvm/doc/backends/platform/sdl/wiiu
	cp $(WIIU_ICON) wiiu_release/scummvm/doc/dists/psp2/icon0.png
	cp $(srcdir)/backends/platform/sdl/wiiu/README.WIIU $(srcdir)/backends/platform/sdl/wiiu/sdl2-swkbd-input.patch wiiu_release/scummvm/doc/backends/platform/sdl/wiiu/
	cp $(srcdir)/backends/platform/sdl/wiiu/README.WIIU wiiu_release/scummvm/doc/
	cp $(srcdir)/backends/platform/sdl/wiiu/sdl2-swkbd-input.patch wiiu_release/scummvm/doc/

.PHONY: wiiu_release

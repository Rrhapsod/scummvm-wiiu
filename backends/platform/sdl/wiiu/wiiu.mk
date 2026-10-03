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
	cp $(srcdir)/backends/platform/sdl/wiiu/README.RPX.md wiiu_release/scummvm/doc/backends/platform/sdl/wiiu/
	cp $(srcdir)/backends/platform/sdl/wiiu/EXPERIMENTAL.md $(srcdir)/backends/platform/sdl/wiiu/sdl2-swkbd-lifecycle.patch $(srcdir)/backends/platform/sdl/wiiu/build-expanded.sh wiiu_release/scummvm/doc/backends/platform/sdl/wiiu/
	cp $(srcdir)/backends/platform/sdl/wiiu/EXPERIMENTAL.md wiiu_release/scummvm/doc/
	cp $(srcdir)/backends/platform/sdl/wiiu/sdl2-renderer-lifecycle.patch $(srcdir)/backends/platform/sdl/wiiu/RENDERER_FIX.md wiiu_release/scummvm/doc/backends/platform/sdl/wiiu/
	mkdir -p wiiu_release/scummvm/doc/LICENSES
	cp $(srcdir)/LICENSES/* $(srcdir)/backends/platform/sdl/wiiu/licenses/* wiiu_release/scummvm/doc/LICENSES/

# Keep the RPX test distribution separate to avoid duplicate launcher entries
# when Aroma users extract their package. It uses the same executable/data.
wiiu_rpx_release: wiiu_release
	mkdir -p wiiu_rpx_release/wiiu/apps/scummvm wiiu_rpx_release/scummvm
	cp scummvm.rpx wiiu_rpx_release/wiiu/apps/scummvm/ScummVM.rpx
	cp $(srcdir)/backends/platform/sdl/wiiu/meta-rpx.xml wiiu_rpx_release/wiiu/apps/scummvm/meta.xml
	cp -R wiiu_release/scummvm/. wiiu_rpx_release/scummvm/
	cp $(srcdir)/backends/platform/sdl/wiiu/README.RPX.md wiiu_rpx_release/README-RPX.md

wiiu_release_all: wiiu_release wiiu_rpx_release

.PHONY: wiiu_release wiiu_rpx_release wiiu_release_all

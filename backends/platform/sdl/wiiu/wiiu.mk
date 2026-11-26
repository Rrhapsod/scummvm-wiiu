# Use the normal ScummVM linker rule; only package the resulting ELF.
WIIU_ELF2RPL := $(DEVKITPRO)/tools/bin/elf2rpl
WIIU_WUHBTOOL := $(DEVKITPRO)/tools/bin/wuhbtool
# Reuse the upstream ScummVM icon, already at WUHB's required 128x128 size.
WIIU_ICON := $(srcdir)/dists/psp2/icon0.png
WIIU_RELEASE_NOTES ?= $(srcdir)/backends/platform/sdl/wiiu/RELEASE-0.9.0.md
WIIU_RPX_META ?= $(srcdir)/backends/platform/sdl/wiiu/meta-rpx.xml

WIIU_ORDER_RPX := $(srcdir)/backends/platform/sdl/wiiu/order-rpx.py

scummvm.rpx: $(EXECUTABLE) $(WIIU_ORDER_RPX)
	$(WIIU_ELF2RPL) $< $@.raw
	python3 $(WIIU_ORDER_RPX) $@.raw $@.new
	mv $@.new $@

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
	cp $(WIIU_RELEASE_NOTES) wiiu_release/RELEASE-NOTES.md
	mkdir -p wiiu_release/scummvm/doc/dists/psp2 wiiu_release/scummvm/doc/backends/platform/sdl/wiiu
	cp $(WIIU_ICON) wiiu_release/scummvm/doc/dists/psp2/icon0.png
	cp $(srcdir)/backends/platform/sdl/wiiu/README.WIIU $(srcdir)/backends/platform/sdl/wiiu/sdl2-swkbd-input.patch wiiu_release/scummvm/doc/backends/platform/sdl/wiiu/
	cp $(srcdir)/backends/platform/sdl/wiiu/README.WIIU wiiu_release/scummvm/doc/
	cp $(srcdir)/backends/platform/sdl/wiiu/sdl2-swkbd-input.patch wiiu_release/scummvm/doc/
	cp $(srcdir)/backends/platform/sdl/wiiu/README.RPX.md wiiu_release/scummvm/doc/backends/platform/sdl/wiiu/
	cp $(srcdir)/backends/platform/sdl/wiiu/EXPERIMENTAL.md $(srcdir)/backends/platform/sdl/wiiu/sdl2-swkbd-lifecycle.patch $(srcdir)/backends/platform/sdl/wiiu/build-expanded.sh wiiu_release/scummvm/doc/backends/platform/sdl/wiiu/
	cp $(srcdir)/backends/platform/sdl/wiiu/EXPERIMENTAL.md wiiu_release/scummvm/doc/
	cp $(srcdir)/backends/platform/sdl/wiiu/sdl2-renderer-lifecycle.patch $(srcdir)/backends/platform/sdl/wiiu/RENDERER_FIX.md wiiu_release/scummvm/doc/backends/platform/sdl/wiiu/
	cp $(srcdir)/backends/platform/sdl/wiiu/build-release.sh wiiu_release/scummvm/doc/backends/platform/sdl/wiiu/
	cp $(srcdir)/backends/platform/sdl/wiiu/UPSTREAM-2026.4.0.md wiiu_release/scummvm/doc/backends/platform/sdl/wiiu/
	cp $(srcdir)/backends/platform/sdl/wiiu/RELEASE-0.7.1.md wiiu_release/scummvm/doc/backends/platform/sdl/wiiu/
	cp $(srcdir)/backends/platform/sdl/wiiu/RELEASE-0.6.0.md wiiu_release/scummvm/doc/backends/platform/sdl/wiiu/
	cp $(srcdir)/backends/platform/sdl/wiiu/RELEASE-0.6.0.md wiiu_release/scummvm/doc/
	cp $(srcdir)/backends/platform/sdl/wiiu/RELEASE-0.7.0.md wiiu_release/scummvm/doc/backends/platform/sdl/wiiu/
	cp $(srcdir)/backends/platform/sdl/wiiu/RELEASE-0.7.0.md wiiu_release/scummvm/doc/
	cp $(srcdir)/backends/platform/sdl/wiiu/sdl2-timer-zero-timeout.patch $(srcdir)/backends/platform/sdl/wiiu/sdl2-swkbd-controller.patch $(srcdir)/backends/platform/sdl/wiiu/sdl2-rgba565-blit.patch wiiu_release/scummvm/doc/backends/platform/sdl/wiiu/
	cp $(srcdir)/backends/platform/sdl/wiiu/sdl2-wiimote-pointer.patch wiiu_release/scummvm/doc/backends/platform/sdl/wiiu/
	cp $(srcdir)/backends/platform/sdl/wiiu/build-vorbis-sdk.sh $(srcdir)/backends/platform/sdl/wiiu/prepare-mm-data.py wiiu_release/scummvm/doc/backends/platform/sdl/wiiu/
	cp $(srcdir)/backends/platform/sdl/wiiu/RELEASE-0.8.0.md $(srcdir)/backends/platform/sdl/wiiu/meta-rpx.xml wiiu_release/scummvm/doc/backends/platform/sdl/wiiu/
	cp $(srcdir)/backends/platform/sdl/wiiu/sdl2-wiimote-precision.patch $(srcdir)/backends/platform/sdl/wiiu/sdl2-wiimote-stability.patch wiiu_release/scummvm/doc/backends/platform/sdl/wiiu/
	cp $(srcdir)/backends/platform/sdl/wiiu/USB-SDK.md $(srcdir)/backends/platform/sdl/wiiu/POINTER-SDK.md $(srcdir)/backends/platform/sdl/wiiu/build-usb-sdk.sh $(srcdir)/backends/platform/sdl/wiiu/build-pointer-sdk.sh $(srcdir)/backends/platform/sdl/wiiu/libfat-wiiu-usb.patch $(srcdir)/backends/platform/sdl/wiiu/libmocha-wiiu-usb.patch wiiu_release/scummvm/doc/backends/platform/sdl/wiiu/
	mkdir -p wiiu_release/scummvm/doc/LICENSES
	cp $(srcdir)/LICENSES/* $(srcdir)/backends/platform/sdl/wiiu/licenses/* wiiu_release/scummvm/doc/LICENSES/

	cp $(srcdir)/backends/platform/sdl/wiiu/order-rpx.py wiiu_release/scummvm/doc/backends/platform/sdl/wiiu/
	if test -n "$(USE_FLUIDSYNTH)"; then \
	    cp $(addprefix $(srcdir)/backends/platform/sdl/wiiu/,RELEASE-0.9.0.md PC-PARITY-2026.4.0.md MEDIA-SDK.md build-media-sdk.sh media-sources.json flac-newlib-int32.patch media-zlib-module.patch fluidsynth-wiiu.patch fluid_sys_wiiu.c fluid_sys_wiiu.h) wiiu_release/scummvm/doc/backends/platform/sdl/wiiu/ && \
	    cp -R "$(DEVKITPRO)/portlibs/ppc/licenses/." wiiu_release/scummvm/doc/LICENSES/media/; \
	fi

# Keep the RPX test distribution separate to avoid duplicate launcher entries
# when Aroma users extract their package. It uses the same executable/data.
wiiu_rpx_release: wiiu_release
	mkdir -p wiiu_rpx_release/wiiu/apps/scummvm wiiu_rpx_release/scummvm
	cp scummvm.rpx wiiu_rpx_release/wiiu/apps/scummvm/ScummVM.rpx
	cp $(WIIU_RPX_META) wiiu_rpx_release/wiiu/apps/scummvm/meta.xml
	cp -R wiiu_release/scummvm/. wiiu_rpx_release/scummvm/
	cp $(srcdir)/backends/platform/sdl/wiiu/README.RPX.md wiiu_rpx_release/README-RPX.md
	cp wiiu_release/RELEASE-NOTES.md wiiu_rpx_release/RELEASE-NOTES.md

wiiu_release_all: wiiu_release wiiu_rpx_release

.PHONY: wiiu_release wiiu_rpx_release wiiu_release_all

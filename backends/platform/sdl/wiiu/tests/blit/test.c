/* Actual SDL public blits plus the original format dispatcher. Host only. */
#include "SDL_blit.h"
#include "SDL.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

extern SDL_BlitFunc Baseline_CalculateBlitN(SDL_Surface *surface);

static void compare(SDL_Surface *src, SDL_Surface *dst, SDL_Rect sr, SDL_Rect dr) {
    SDL_Rect a = sr, b = dr;
    size_t bytes = dst->pitch * dst->h;
    void *expected = SDL_malloc(bytes);
    memset(dst->pixels, 0xa5, bytes);
    assert(SDL_BlitSurface(src, &a, dst, &b) == 0);
    SDL_BlitFunc fast = (SDL_BlitFunc)src->map->data;
    memcpy(expected, dst->pixels, bytes);
    memset(dst->pixels, 0xa5, bytes);
    SDL_BlitFunc reference = Baseline_CalculateBlitN(src);
    if (!(src->map->info.flags & SDL_COPY_BLEND) && reference)
        src->map->data = (void *)reference;
    a = sr; b = dr;
    assert(SDL_BlitSurface(src, &a, dst, &b) == 0);
    assert(memcmp(expected, dst->pixels, bytes) == 0);
    src->map->data = (void *)fast;
    SDL_free(expected);
}

int main(void) {
    SDL_Surface *src = SDL_CreateRGBSurfaceWithFormat(0, 257, 256, 32, SDL_PIXELFORMAT_RGBA8888);
    SDL_Surface *dst = SDL_CreateRGBSurfaceWithFormat(0, 259, 258, 16, SDL_PIXELFORMAT_RGB565);
    assert(src && dst);
    SDL_SetSurfaceBlendMode(src, SDL_BLENDMODE_NONE);
    for (unsigned red = 0; red < 256; ++red) {
        for (int y = 0; y < 256; ++y) {
            Uint32 *row = (Uint32 *)((Uint8 *)src->pixels + y * src->pitch);
            for (int x = 0; x < 257; ++x)
                row[x] = (red << 24) | ((unsigned)y << 16) | ((unsigned)(x & 255) << 8) | ((x + y + red) & 255);
        }
        compare(src, dst, (SDL_Rect){0, 0, 257, 256}, (SDL_Rect){1, 1, 0, 0});
    }
    /* Exact format selection; the baseline uses the generic conversion. */
    assert((SDL_BlitFunc)src->map->data != Baseline_CalculateBlitN(src));
    for (int mode = 0; mode < 4; ++mode) {
        SDL_SetSurfaceBlendMode(src, mode == 1 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
        SDL_SetColorKey(src, mode == 2, 0xff020307);
        SDL_SetSurfaceColorMod(src, mode == 3 ? 64 : 255, 255, 255);
        for (int x = -3; x < 7; ++x)
            compare(src, dst, (SDL_Rect){x, -1, 253, 255}, (SDL_Rect){-x, 2, 0, 0});
    }
    SDL_FreeSurface(src); SDL_FreeSurface(dst);
    puts("PASS: actual SDL conversion, all 16M RGB values/varying alpha, odd pitches, clipped/offset rectangles, color key/modulation and blend dispatch");
#ifdef BENCHMARK
    src = SDL_CreateRGBSurfaceWithFormat(0, 640, 480, 32, SDL_PIXELFORMAT_RGBA8888);
    dst = SDL_CreateRGBSurfaceWithFormat(0, 640, 480, 16, SDL_PIXELFORMAT_RGB565);
    assert(src && dst);
    SDL_SetSurfaceBlendMode(src, SDL_BLENDMODE_NONE);
    SDL_FillRect(src, NULL, 0xa1b2c3ff);
    assert(SDL_BlitSurface(src, NULL, dst, NULL) == 0);
    SDL_BlitFunc fast = (SDL_BlitFunc)src->map->data;
    SDL_BlitFunc old = Baseline_CalculateBlitN(src);
    for (int mode = 0; mode < 2; ++mode) {
        src->map->data = (void *)(mode ? fast : old);
        Uint64 start = SDL_GetPerformanceCounter();
        for (int frame = 0; frame < 1000; ++frame)
            assert(SDL_BlitSurface(src, NULL, dst, NULL) == 0);
        double ms = (SDL_GetPerformanceCounter() - start) * 1000.0 / SDL_GetPerformanceFrequency();
        printf("HOST ONLY: %s 640x480 conversion: %.3f ms/frame\n", mode ? "specialized" : "original", ms / 1000);
    }
    SDL_FreeSurface(src); SDL_FreeSurface(dst);
#endif
    return 0;
}

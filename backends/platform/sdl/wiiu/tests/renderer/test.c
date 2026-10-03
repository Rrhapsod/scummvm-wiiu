/* Host mocks for the actual SDL Wii U lifecycle functions, not GPU emulation. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
typedef int SDL_ScaleMode;
enum { SDL_ScaleModeNearest, SDL_ScaleModeLinear, SDL_ScaleModeBest };
#define SDL_HINT_RENDER_SCALE_QUALITY "scale"
#define SDL_PIXELFORMAT_RGBA8888 1
#define WIIU_TEXTURE_MEM1_MAGIC ((void *)1)
#define SDL_strcasecmp strcasecmp
#define SDL_atoi atoi
#define SDL_free free
typedef struct { int format; void *driverdata; SDL_ScaleMode scaleMode; int w, h; } SDL_Texture;
typedef struct { int w, h; } SDL_Window;
typedef struct { SDL_Texture windowTex; void *ctx; } WIIU_RenderData;
typedef struct { void *driverdata; SDL_Window *window; } SDL_Renderer;
typedef struct { int hasForeground; } WIIU_VideoData;
typedef struct { void *driverdata; } SDL_VideoDevice;
static WIIU_VideoData video = {1};
static SDL_VideoDevice device = {&video};
static int liveTextures, failAllocation, pendingGPU, shaderRefs, queueFrees;
static const char *scaleHint;
static SDL_VideoDevice *SDL_GetVideoDevice(void) { return &device; }
static const char *SDL_GetHint(const char *name) { (void)name; return scaleHint; }
static void SDL_GetWindowSize(SDL_Window *w, int *x, int *y) { *x = w->w; *y = w->h; }
static void GX2DrawDone(void) { pendingGPU = 0; }
static void GX2SetContextState(void *ctx) { assert(ctx == NULL); }
static void WIIU_FreeRenderData(WIIU_RenderData *data, int list) {
    (void)data; assert(list == 0 || list == 1); ++queueFrees;
}
static void WIIU_SDL_DestroyShaders(void) { assert(shaderRefs > 0); --shaderRefs; }
static int WIIU_SDL_CreateTexture(SDL_Renderer *r, SDL_Texture *t) {
    (void)r;
    assert(t->driverdata == WIIU_TEXTURE_MEM1_MAGIC);
    if (failAllocation) return -1;
    t->driverdata = malloc(1);
    assert(t->driverdata);
    ++liveTextures;
    return 0;
}
static void WIIU_SDL_DestroyTexture(SDL_Renderer *r, SDL_Texture *t) {
    (void)r;
    if (!t->driverdata) return;
    assert(t->driverdata != WIIU_TEXTURE_MEM1_MAGIC);
    assert(!video.hasForeground || !pendingGPU);
    free(t->driverdata);
    --liveTextures;
}
void WIIU_SDL_DestroyWindowTex(SDL_Renderer *, SDL_Window *);
/* The runner inserts the real SDL function bodies here. */
#include "extracted.c"

static SDL_Renderer makeRenderer(SDL_Window *window) {
    WIIU_RenderData *data = calloc(1, sizeof(*data));
    assert(data);
    data->ctx = malloc(1);
    assert(data->ctx);
    ++shaderRefs;
    return (SDL_Renderer){data, window};
}

int main(void) {
    SDL_Window window = {1280, 720};
#ifdef TEST_BASELINE
    SDL_Renderer r = makeRenderer(&window);
    WIIU_RenderData *d = r.driverdata;
    d->windowTex.driverdata = WIIU_TEXTURE_MEM1_MAGIC;
    assert(WIIU_SDL_CreateTexture(&r, &d->windowTex) == 0);
    void *leaked = d->windowTex.driverdata;
    WIIU_SDL_DestroyRenderer(&r);
    int detected = liveTextures != 0;
    free(leaked); /* Release mock memory after measuring the old destructor. */
    printf("Baseline leak detected: %d private texture(s) remain\n", liveTextures);
    return detected ? 0 : 1;
#else
    for (int i = 0; i < 100; ++i) {
        SDL_Renderer r = makeRenderer(&window);
        WIIU_RenderData *d = r.driverdata;
        failAllocation = 1;
        assert(WIIU_SDL_CreateWindowTex(&r, &window) == -1);
        assert(!d->windowTex.driverdata && liveTextures == 0);
        failAllocation = 0;
        scaleHint = (i % 2) ? "linear" : "nearest";
        assert(WIIU_SDL_CreateWindowTex(&r, &window) == 0);
        assert(d->windowTex.w == 1280 && d->windowTex.h == 720);
        assert(liveTextures == 1);
        void *old = d->windowTex.driverdata;
        failAllocation = 1;
        assert(WIIU_SDL_CreateWindowTex(&r, &window) == -1);
        assert(d->windowTex.driverdata == old && liveTextures == 1);
        failAllocation = 0;
        pendingGPU = 1;
        assert(WIIU_SDL_CreateWindowTex(&r, &window) == 0);
        assert(liveTextures == 1 && !pendingGPU);
        pendingGPU = 1;
        WIIU_SDL_DestroyWindowTex(&r, &window);
        WIIU_SDL_DestroyWindowTex(&r, &window);
        assert(!d->windowTex.driverdata && liveTextures == 0);
        assert(WIIU_SDL_CreateWindowTex(&r, &window) == 0);
        pendingGPU = 1;
        WIIU_SDL_DestroyRenderer(&r);
        assert(!r.driverdata && liveTextures == 0 && shaderRefs == 0);
    }
    assert(queueFrees == 200);
    // Failed initial texture allocation followed by renderer unwind.
    SDL_Renderer r = makeRenderer(&window);
    failAllocation = 1;
    assert(WIIU_SDL_CreateWindowTex(&r, &window) == -1);
    WIIU_SDL_DestroyRenderer(&r);
    assert(liveTextures == 0 && shaderRefs == 0);
    // Background cleanup must not wait for a foreground-only GPU operation.
    r = makeRenderer(&window);
    failAllocation = 0;
    assert(WIIU_SDL_CreateWindowTex(&r, &window) == 0);
    video.hasForeground = 0;
    pendingGPU = 1;
    WIIU_SDL_DestroyRenderer(&r);
    assert(liveTextures == 0 && shaderRefs == 0 && pendingGPU == 1);
    puts("PASS: 100 lifecycles, failed initial/resize allocation, replacement, idempotent texture cleanup, renderer unwind");
    return 0;
#endif
}

/* FluidSynth SDL2 OS abstraction for the native ScummVM Wii U port.
 * Copyright (C) 2026 Rrhapsod
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ScummVM owns audio/MIDI scheduling. SDL owns timers, mutexes and threads;
 * this adapter does not initialize another audio or platform subsystem.
 */
#include "fluid_sys.h"
#include <SDL.h>
#include <limits.h>

static SDL_SpinLock init_lock;
fluid_mutex_t _atomic_lock = NULL;

void fluid_msleep(unsigned int msecs) { SDL_Delay(msecs); }
double fluid_utime(void) {
    Uint64 counter = SDL_GetPerformanceCounter();
    Uint64 frequency = SDL_GetPerformanceFrequency();
    return (double)counter * 1000000.0 / (double)frequency;
}

void _fluid_mutex_init(fluid_mutex_t *mutex) {
    *mutex = SDL_CreateMutex();
    if (!*mutex) FLUID_LOG(FLUID_PANIC, "SDL mutex allocation failed");
}
void _fluid_mutex_lock(fluid_mutex_t *mutex) {
    /* FluidSynth has static mutexes and may first use them on either thread. */
    SDL_AtomicLock(&init_lock);
    if (!*mutex) _fluid_mutex_init(mutex);
    SDL_AtomicUnlock(&init_lock);
    if (SDL_LockMutex((SDL_mutex *)*mutex) != 0)
        FLUID_LOG(FLUID_PANIC, "SDL mutex lock failed");
}
void fluid_mutex_unlock(fluid_mutex_t mutex) { SDL_UnlockMutex((SDL_mutex *)mutex); }
void fluid_mutex_destroy(fluid_mutex_t mutex) { SDL_DestroyMutex((SDL_mutex *)mutex); }
void _fluid_rec_mutex_init(fluid_rec_mutex_t *mutex) { _fluid_mutex_init(mutex); }
void fluid_rec_mutex_lock(fluid_rec_mutex_t mutex) {
    if (SDL_LockMutex((SDL_mutex *)mutex) != 0)
        FLUID_LOG(FLUID_PANIC, "SDL recursive mutex lock failed");
}
void fluid_rec_mutex_unlock(fluid_rec_mutex_t mutex) { fluid_mutex_unlock(mutex); }
void fluid_rec_mutex_destroy(fluid_rec_mutex_t mutex) { fluid_mutex_destroy(mutex); }

fluid_cond_mutex_t *new_fluid_cond_mutex(void) { return SDL_CreateMutex(); }
void delete_fluid_cond_mutex(fluid_cond_mutex_t *mutex) { SDL_DestroyMutex((SDL_mutex *)mutex); }
void fluid_cond_mutex_lock(fluid_cond_mutex_t *mutex) { SDL_LockMutex((SDL_mutex *)mutex); }
void fluid_cond_mutex_unlock(fluid_cond_mutex_t *mutex) { SDL_UnlockMutex((SDL_mutex *)mutex); }
fluid_cond_t new_fluid_cond(void) { return SDL_CreateCond(); }
void delete_fluid_cond(fluid_cond_t cond) { SDL_DestroyCond((SDL_cond *)cond); }
void fluid_cond_signal(fluid_cond_t cond) { SDL_CondSignal((SDL_cond *)cond); }
void fluid_cond_broadcast(fluid_cond_t cond) { SDL_CondBroadcast((SDL_cond *)cond); }
void fluid_cond_wait(fluid_cond_t cond, fluid_cond_mutex_t *mutex) {
    SDL_CondWait((SDL_cond *)cond, (SDL_mutex *)mutex);
}

/* SDL TLS IDs are never recycled. Each synth owns its ID holder. */
void _fluid_private_init(fluid_private_t *priv) {
    SDL_AtomicLock(&init_lock);
    if (!*priv) {
        SDL_TLSID *id = FLUID_NEW(SDL_TLSID);
        if (id) {
            *id = SDL_TLSCreate();
            if (*id) *priv = id;
            else FLUID_FREE(id);
        }
        if (!*priv) FLUID_LOG(FLUID_PANIC, "SDL TLS allocation failed");
    }
    SDL_AtomicUnlock(&init_lock);
}
void fluid_private_free(fluid_private_t priv) {
    if (priv) {
        SDL_TLSSet(*(SDL_TLSID *)priv, NULL, NULL);
        FLUID_FREE(priv);
    }
}
void *fluid_private_get(fluid_private_t priv) { return priv ? SDL_TLSGet(*(SDL_TLSID *)priv) : NULL; }
void fluid_private_set(fluid_private_t priv, void *value) {
    if (priv) SDL_TLSSet(*(SDL_TLSID *)priv, value, NULL);
}

typedef struct {
    SDL_Thread *thread;
    fluid_thread_func_t func;
    void *data;
    int priority;
    int detached;
} WiiUFluidThread;

static int wiiu_fluid_thread(void *arg) {
    WiiUFluidThread *info = (WiiUFluidThread *)arg;
    fluid_thread_func_t func = info->func;
    void *data = info->data;
    int detached = info->detached;
    if (info->priority > 0) SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH);
    func(data);
    if (detached) FLUID_FREE(info);
    return 0;
}
fluid_thread_t *new_fluid_thread(const char *name, fluid_thread_func_t func, void *data, int priority, int detach) {
    WiiUFluidThread *info;
    SDL_Thread *thread;
    if (!func) return NULL;
    info = FLUID_NEW(WiiUFluidThread);
    if (!info) return NULL;
    info->func = func;
    info->data = data;
    info->priority = priority;
    info->detached = detach;
    thread = SDL_CreateThread(wiiu_fluid_thread, name, info);
    if (!thread) { FLUID_FREE(info); return NULL; }
    if (detach) {
        /* No access to info here: the detached worker may already be done.
         * Its opaque token must not be joined/deleted, per FluidSynth's API. */
        SDL_DetachThread(thread);
    } else info->thread = thread;
    return info;
}
int fluid_thread_join(fluid_thread_t *thread) {
    WiiUFluidThread *info = (WiiUFluidThread *)thread;
    if (!info) return FLUID_FAILED;
    if (info->thread) { SDL_WaitThread(info->thread, NULL); info->thread = NULL; }
    return FLUID_OK;
}
void delete_fluid_thread(fluid_thread_t *thread) {
    if (thread) { fluid_thread_join(thread); FLUID_FREE(thread); }
}
void fluid_thread_self_set_prio(int priority) {
    SDL_SetThreadPriority(priority > 0 ? SDL_THREAD_PRIORITY_HIGH : SDL_THREAD_PRIORITY_NORMAL);
}
int fluid_stat(const char *path, fluid_stat_buf_t *buffer) {
    struct stat info;
    time_t stamp;
    if (stat(path, &info) != 0) { buffer->st_mtime = 0; return FLUID_FAILED; }
    stamp = info.st_mtim.tv_sec;
    buffer->st_mtime = stamp > INT_MAX ? INT_MAX : stamp < INT_MIN ? INT_MIN : (int)stamp;
    return FLUID_OK;
}
bool fluid_file_test(const char *path, int flags) {
    struct stat info;
    if (stat(path, &info) != 0) return false;
    return (flags & FLUID_FILE_TEST_EXISTS) ||
           ((flags & FLUID_FILE_TEST_IS_REGULAR) && S_ISREG(info.st_mode));
}

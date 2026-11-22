/* Test the actual FluidSynth SDL OSAL and SF2 mixer with host sanitizers.
 * SPDX-License-Identifier: GPL-3.0-or-later */
#include "fluid_sys.h"
#include <SDL.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static fluid_private_t tls;
static fluid_atomic_int_t count;
static void *lock_worker(void *arg) {
    fluid_private_set(tls,arg);
    for (int i=0;i<5000;i++) {
        assert(fluid_private_get(tls)==arg);
        fluid_atomic_int_inc(&count);
    }
    return NULL;
}
static void *midi_worker(void *arg) {
    fluid_synth_t *synth=(fluid_synth_t *)arg;
    for(int i=0;i<200;i++) {
        assert(fluid_synth_noteon(synth,0,60+(i%8),100)==FLUID_OK);
        assert(fluid_synth_noteoff(synth,0,60+(i%8))==FLUID_OK);
        if(i%8==7) fluid_synth_all_sounds_off(synth,0);
        SDL_Delay(1);
    }
    return NULL;
}
static fluid_cond_t cond;
static fluid_cond_mutex_t *cond_mutex;
static int ready;
static void *cond_worker(void *arg) {
    (void)arg;
    fluid_cond_mutex_lock(cond_mutex);ready=1;
    fluid_cond_signal(cond);fluid_cond_mutex_unlock(cond_mutex);
    return NULL;
}
static SDL_atomic_t detached_done;
static void *detached_worker(void *arg) {
    (void)arg;SDL_AtomicSet(&detached_done,1);return NULL;
}
int main(int argc,char **argv) {
    assert(argc==2);assert(SDL_Init(0)==0);
    fluid_set_log_function(FLUID_DBG,NULL,NULL);
    assert(FLUID_LE32TOH(0x12345678u)==SDL_SwapLE32(0x12345678u));
    fluid_stat_buf_t stamp;assert(fluid_stat(argv[1],&stamp)==FLUID_OK && stamp.st_mtime>0);
    assert(fluid_file_test(argv[1],FLUID_FILE_TEST_IS_REGULAR));
    assert(!fluid_file_test("/nonexistent-wiiu-test.sf2",FLUID_FILE_TEST_EXISTS));
    double before=fluid_utime();fluid_msleep(2);assert(fluid_utime()>before);
    fluid_private_init(tls);fluid_private_set(tls,&count);
    fluid_thread_t *workers[4];int ids[4]={1,2,3,4};
    for(int i=0;i<4;i++) {workers[i]=new_fluid_thread("atomic/TLS",lock_worker,&ids[i],0,0);assert(workers[i]);}
    for(int i=0;i<4;i++) {assert(fluid_thread_join(workers[i])==FLUID_OK);delete_fluid_thread(workers[i]);}
    assert(fluid_atomic_int_get(&count)==20000 && fluid_private_get(tls)==&count);
    fluid_private_free(tls);
    cond=new_fluid_cond();cond_mutex=new_fluid_cond_mutex();assert(cond && cond_mutex);
    fluid_cond_mutex_lock(cond_mutex);
    fluid_thread_t *thread=new_fluid_thread("cond",cond_worker,NULL,0,0);assert(thread);
    while(!ready) fluid_cond_wait(cond,cond_mutex);
    fluid_cond_mutex_unlock(cond_mutex);fluid_thread_join(thread);delete_fluid_thread(thread);
    delete_fluid_cond(cond);delete_fluid_cond_mutex(cond_mutex);
    assert(new_fluid_thread("detached",detached_worker,NULL,0,1));
    for(int i=0;i<1000 && !SDL_AtomicGet(&detached_done);i++) SDL_Delay(1);
    assert(SDL_AtomicGet(&detached_done));SDL_Delay(5);
    for(int cycle=0;cycle<5;cycle++) {
        fluid_settings_t *settings=new_fluid_settings();assert(settings);
        assert(fluid_settings_setnum(settings,"synth.sample-rate",48000)==FLUID_OK);
        fluid_settings_setint(settings,"synth.threadsafe-api",1);
        fluid_settings_setnum(settings,"synth.gain",0.25);
        fluid_synth_t *synth=new_fluid_synth(settings);assert(synth);
        int bank_id=fluid_synth_sfload(synth,argv[1],1);assert(bank_id>=0);
        fluid_sfont_t *bank=fluid_synth_get_sfont_by_id(synth,bank_id);assert(bank);
        fluid_sfont_iteration_start(bank);fluid_preset_t *preset=fluid_sfont_iteration_next(bank);assert(preset);
        assert(fluid_synth_program_select(synth,0,bank_id,fluid_preset_get_banknum(preset),fluid_preset_get_num(preset))==FLUID_OK);
        assert(fluid_synth_noteon(synth,0,60,110)==FLUID_OK);
        short pcm[8192];unsigned active=0;
        for(int j=0;j<8;j++) {
            assert(fluid_synth_write_s16(synth,4096,pcm,0,2,pcm,1,2)==FLUID_OK);
            for(unsigned i=0;i<8192;i++) if(pcm[i]) active++;
        }
        assert(active>100);
        thread=new_fluid_thread("MIDI",midi_worker,synth,0,0);assert(thread);
        for(int j=0;j<30;j++) assert(fluid_synth_write_s16(synth,4096,pcm,0,2,pcm,1,2)==FLUID_OK);
        fluid_thread_join(thread);delete_fluid_thread(thread);
        assert(fluid_synth_sfunload(synth,bank_id,1)==FLUID_OK);
        delete_fluid_synth(synth);delete_fluid_settings(settings);
    }
    SDL_Quit();
    puts("PASS SDL OSAL: atomic/TLS/condition/thread lifetime; SF2 load/render/concurrent MIDI/unload in 5 cycles");
    return 0;
}

/* Host-only smoke test using synthetic audio and Xiph's test encoder. */
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <vorbis/vorbisfile.h>
#include "write_read.h"

int main(void) {
    float samples[8192];
    for (int i = 0; i < 8192; ++i)
        samples[i] = 0.5f * sinf(i * 0.07f);
    for (int channels = 1; channels <= 2; ++channels) {
        write_vorbis_data_or_die("synthetic.ogg", 44100, 0.4f, samples, 8192, channels);
        OggVorbis_File file;
        assert(ov_fopen("synthetic.ogg", &file) == 0);
        assert(ov_info(&file, -1)->rate == 44100);
        assert(ov_info(&file, -1)->channels == channels);
        char pcm[4096];
        long count, total = 0;
        int section, nonzero = 0;
        while ((count = ov_read(&file, pcm, sizeof(pcm), 1, 2, 1, &section)) != 0) {
            assert(count > 0);
            total += count;
            for (long i = 0; i < count; ++i) nonzero |= pcm[i];
        }
        assert(total == 8192 * channels * 2 && nonzero);
        assert(ov_pcm_seek(&file, 4096) == 0 && ov_pcm_tell(&file) == 4096);
        assert(ov_read(&file, pcm, sizeof(pcm), 1, 2, 1, &section) > 0);
        assert(ov_pcm_seek(&file, 0) == 0);
        assert(ov_clear(&file) == 0);
    }
    puts("PASS: synthetic mono/stereo Ogg Vorbis -> signed big-endian PCM, sample count, non-silence, EOF and seek (host, not Wii U)");
    return 0;
}

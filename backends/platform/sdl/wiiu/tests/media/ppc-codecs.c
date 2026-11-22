/* Execute actual devkitPPC codec archives under Linux/PPC with memory IO only.
 * SPDX-License-Identifier: GPL-3.0-or-later */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FLAC/stream_encoder.h>
#include <FLAC/stream_decoder.h>
#include <mpeg2dec/mpeg2.h>
#include <theora/theoradec.h>

static unsigned char encoded[131072];
static size_t encoded_size, read_pos;
static FLAC__int32 samples[8192];
static unsigned decoded_frames, decoded_errors;
static FLAC__StreamEncoderWriteStatus encode_cb(const FLAC__StreamEncoder *e, const FLAC__byte *p, size_t n, unsigned count, unsigned frame, void *data) {
    (void)e; (void)count; (void)frame; (void)data;
    assert(encoded_size+n <= sizeof(encoded));
    memcpy(encoded+encoded_size,p,n); encoded_size+=n;
    return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
}
static FLAC__StreamDecoderReadStatus read_cb(const FLAC__StreamDecoder *d, FLAC__byte *p, size_t *n, void *data) {
    (void)d; (void)data;
    if (*n > encoded_size-read_pos) *n=encoded_size-read_pos;
    if (!*n) return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
    memcpy(p,encoded+read_pos,*n);read_pos+=*n;
    return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}
static FLAC__StreamDecoderWriteStatus decode_cb(const FLAC__StreamDecoder *d, const FLAC__Frame *f, const FLAC__int32 *const b[], void *data) {
    (void)d; (void)data;
    assert(f->header.channels==2);
    for (unsigned i=0;i<f->header.blocksize;i++,decoded_frames++) {
        assert(decoded_frames<4096);
        assert(b[0][i]==samples[2*decoded_frames]);
        assert(b[1][i]==samples[2*decoded_frames+1]);
    }
    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}
static void error_cb(const FLAC__StreamDecoder *d, FLAC__StreamDecoderErrorStatus s, void *data) {
    (void)d;(void)s;(void)data;decoded_errors++;
}
static void flac_roundtrip(unsigned bits, unsigned block) {
    FLAC__StreamEncoder *enc=FLAC__stream_encoder_new(); assert(enc);
    encoded_size=read_pos=decoded_frames=decoded_errors=0;
    unsigned mask=(1u<<bits)-1, rng=42;
    for(unsigned i=0;i<8192;i++) {
        rng=rng*1664525u+1013904223u;
        samples[i]=(FLAC__int32)((rng>>8)&mask)-(FLAC__int32)(1u<<(bits-1));
        if(i<1024) samples[i]=(FLAC__int32)(i%17)-8;
    }
    assert(FLAC__stream_encoder_set_channels(enc,2));
    assert(FLAC__stream_encoder_set_bits_per_sample(enc,bits));
    assert(FLAC__stream_encoder_set_sample_rate(enc,22050));
    assert(FLAC__stream_encoder_set_blocksize(enc,block));
    assert(FLAC__stream_encoder_set_compression_level(enc,5));
    assert(FLAC__stream_encoder_init_stream(enc,encode_cb,NULL,NULL,NULL,NULL)==FLAC__STREAM_ENCODER_INIT_STATUS_OK);
    assert(FLAC__stream_encoder_process_interleaved(enc,samples,4096));
    assert(FLAC__stream_encoder_finish(enc));FLAC__stream_encoder_delete(enc);
    FLAC__StreamDecoder *dec=FLAC__stream_decoder_new();assert(dec);
    assert(FLAC__stream_decoder_init_stream(dec,read_cb,NULL,NULL,NULL,NULL,decode_cb,NULL,error_cb,NULL)==FLAC__STREAM_DECODER_INIT_STATUS_OK);
    assert(FLAC__stream_decoder_process_until_end_of_stream(dec));
    assert(decoded_frames==4096 && !decoded_errors);
    assert(FLAC__stream_decoder_finish(dec));FLAC__stream_decoder_delete(dec);
}
/* Memory callbacks never invoke newlib stdio. Fail if that boundary is crossed.
 * No long-double values cross the SDK/Linux ABI boundary. */
void *__getreent(void) { abort(); }
int main(void) {
    for(unsigned bits=16;bits<=24;bits+=8)
        for(unsigned block=16;block<=1024;block*=8) flac_roundtrip(bits,block);
    mpeg2dec_t *mpeg=mpeg2_init();assert(mpeg);mpeg2_close(mpeg);
    th_info info;th_info_init(&info);th_comment comment;th_comment_init(&comment);
    th_setup_info *setup=NULL;ogg_packet packet;memset(&packet,0,sizeof(packet));
    unsigned char invalid[]={0,1,2,3};packet.packet=invalid;packet.bytes=4;
    assert(th_decode_headerin(&info,&comment,&setup,&packet)<0);
    th_setup_free(setup);th_comment_clear(&comment);th_info_clear(&info);
    puts("PASS PowerPC: 16/24-bit FLAC PCM roundtrips, MPEG2 init and invalid Theora header");
    return 0;
}

#!/usr/bin/env python3
"""Actual AUD decoder/factory and Common buffer, synthetic chunks; no Wii U timing."""
from pathlib import Path
import subprocess
import tempfile
root=Path(__file__).resolve().parents[6]
s=(root/'common/stream.cpp').read_text()
a=s.index('class BufferedReadStream :')
b=s.index('\n#pragma mark -',s.index('SeekableReadStream *wrapBufferedSeekableReadStream('))
buffer='namespace Common { namespace {\n'+s[a:b]+'\n}\n'
aud=(root/'engines/kyra/sound/drivers/audstream.cpp').read_text()
aud='\n'.join(line for line in aud.splitlines() if not line.startswith('#include'))
prefix=r'''
#include <cassert>
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
using byte=uint8_t; using uint8=uint8_t; using int8=int8_t;
using uint16=uint16_t; using int16=int16_t; using uint32=uint32_t; using int32=int32_t; using int64=int64_t; using uint=unsigned;
#define MIN std::min
template<class T> T CLIP(T a,T b,T c){return std::max(b,std::min(c,a));}
#define debugC(...) ((void)0)
#define warning(...) ((void)0)
namespace DisposeAfterUse { enum Flag { NO, YES }; }
namespace Common {
struct ReadStream {
 virtual ~ReadStream()=default;
 virtual bool eos() const=0; virtual bool err() const=0; virtual void clearErr()=0;
 virtual uint32 read(void*,uint32)=0;
 uint8 readByte() { uint8 b=0; read(&b,1); return b; }
 uint16 readUint16LE() { uint8 b[2]={}; read(b,2); return b[0]|(b[1]<<8); }
 uint32 readUint32LE() { uint8 b[4]={}; read(b,4); return b[0]|(b[1]<<8)|(b[2]<<16)|(uint32(b[3])<<24); }
};
struct SeekableReadStream : virtual ReadStream {
 virtual int64 pos() const=0; virtual int64 size() const=0; virtual bool seek(int64,int=SEEK_SET)=0;
};
template<class T> struct DisposablePtr {
 T *p; DisposeAfterUse::Flag own;
 DisposablePtr(T*v,DisposeAfterUse::Flag o):p(v),own(o){}
 ~DisposablePtr(){ if(own==DisposeAfterUse::YES) delete p; }
 T *operator->() const { return p; }
};
}
namespace Audio {
struct Timestamp {
 uint32 frames; int rate;
 Timestamp(uint32 f,int r):frames(f),rate(r){}
 Timestamp addFrames(uint32 n) const { return Timestamp(frames+n,rate); }
 uint32 totalNumberOfFrames() const { return frames; }
};
Timestamp convertTimeToStreamPos(const Timestamp&t,int,bool) {return t;}
struct SeekableAudioStream {
 virtual ~SeekableAudioStream()=default;
 virtual int readBuffer(int16*,int)=0; virtual bool isStereo() const=0; virtual bool endOfData() const=0;
 virtual int getRate() const=0; virtual bool seek(const Timestamp&)=0; virtual Timestamp getLength() const=0;
};
}
static unsigned deleted=0, reads=0, seeks=0;
struct Disk : Common::SeekableReadStream {
 std::vector<byte> data; int64 at=0; bool ended=false;
 ~Disk(){++deleted;}
 bool eos()const override{return ended;} bool err()const override{return false;} void clearErr()override{ended=false;}
 int64 pos()const override{return at;} int64 size()const override{return data.size();}
 bool seek(int64 n,int mode=SEEK_SET)override{
  ++seeks; n+= mode==SEEK_CUR?at:mode==SEEK_END?size():0; assert(n>=0&&n<=size()); at=n; ended=false;return true;
 }
 uint32 read(void*out,uint32 n)override{
  ++reads; if(n>size()-at){n=size()-at;ended=true;} memcpy(out,data.data()+at,n);at+=n;return n;
 }
 void word(uint32 n,int bytes){while(bytes--){data.push_back(n&255);n>>=8;}}
};
'''
checks=r'''
int main(){
 for(unsigned chunks:{1u,17u,2000u}) for(unsigned chunkSize:{256u,2048u}){
  auto*d=new Disk; d->word(22050,2);d->word(chunks*(chunkSize+8),4);d->word(0,1);d->word(1,1);
  for(unsigned c=0;c<chunks;++c){d->word(chunkSize,2);d->word(chunkSize,2);d->word(0xDEAF,4);for(unsigned j=0;j<chunkSize;++j)d->data.push_back((c*chunkSize+j)%251);}
  reads=seeks=0;
  auto*a=Kyra::makeAUDStream(d,DisposeAfterUse::YES);
  unsigned scanSeeks=seeks,scanReads=reads;
  assert(a->getRate()==22050 && a->getLength().frames==chunks*chunkSize);
  assert(!a->isStereo() && !a->endOfData());
#ifdef __WIIU__
  assert(scanSeeks<=1+(chunks*(chunkSize+8)+65535)/65536);
  assert(scanReads<=2+(chunks*(chunkSize+8)+65535)/65536);
#else
  assert(scanSeeks==chunks+1 && scanReads==2*chunks+4);
#endif
  std::vector<int16> samples(chunks*chunkSize+1);
  assert(a->readBuffer(samples.data(),samples.size())==int(chunks*chunkSize));
  for(unsigned i=0;i<chunks*chunkSize;++i)assert(samples[i]==int16(((i%251)<<8)^0x8000));
  assert(a->endOfData());
  if(chunks>1) for(unsigned at:{0u,chunkSize/2,chunkSize}){
   assert(a->seek(Audio::Timestamp(at,22050)));
   assert(a->readBuffer(samples.data(),1)==(at<chunks*chunkSize?1:0));
   if(at<chunks*chunkSize)assert(samples[0]==int16(((at%251)<<8)^0x8000));
  }
  printf("PASS: %u AUD chunks x %u bytes; duration scan %u reads, %u seeks; PCM/duration/seek/rewind/ownership\n",chunks,chunkSize,scanReads,scanSeeks);
  delete a;
 }
 assert(deleted==6);
}
'''
with tempfile.TemporaryDirectory(prefix='scummvm-kyra-') as temp:
 p=Path(temp); src=p/'test.cpp';src.write_text(prefix+buffer+aud+checks)
 for mode in ('baseline','wiiu'):
  cmd=['g++','-std=c++11','-Wall','-Wextra','-Wno-reorder','-Wno-unused-variable','-Wno-unused-parameter','-Wno-unknown-pragmas','-fsanitize=address,undefined','-no-pie']
  if mode=='wiiu':cmd+=['-D__WIIU__']
  subprocess.run(cmd+[str(src),'-o',str(p/mode)],check=True)
  print(mode,flush=True);subprocess.run([str(p/mode)],check=True)

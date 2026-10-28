#!/usr/bin/env python3
"""Actual patched libfat on synthetic images; mocks only newlib/USB/OS transport.

Usage: fat-image.py LIBFAT_SOURCE LIBMOCHA_SOURCE SDK_PROFILE
Requires gcc, mkfs.fat, fsck.fat and mtools. Never accesses a block device.
"""
from pathlib import Path
import subprocess
import sys
import tempfile
fat,mocha,sdk=map(lambda s:Path(s).resolve(),sys.argv[1:])
with tempfile.TemporaryDirectory(prefix='scummvm-fat-image-') as temp:
 p=Path(temp);(p/'sys').mkdir();(p/'coreinit').mkdir()
 (p/'sys/syslimits.h').write_text('#include <limits.h>\n')
 (p/'sys/reent.h').write_text('#pragma once\n#include <sys/types.h>\nstruct _reent { int _errno; void *deviceData; };\n')
 h=(sdk/'devkitPPC/powerpc-eabi/include/sys/iosupport.h').read_text()
 a=h.index('enum\t{'); b=h.index('extern const devoptab_t *devoptab_list[];')
 (p/'sys/iosupport.h').write_text('#pragma once\n#include <stddef.h>\n#include <sys/types.h>\n#include <sys/stat.h>\n#include <sys/statvfs.h>\n#include <sys/reent.h>\n'+h[a:b]+'''
int AddDevice(const devoptab_t *);
int RemoveDevice(const char *);
int FindDevice(const char *);
const devoptab_t *GetDeviceOpTab(const char *);
int setDefaultDevice(int);
int getDefaultDevice(void);
''')
 (p/'coreinit/mutex.h').write_text('''#pragma once
#include <pthread.h>
typedef pthread_mutex_t OSMutex;
static inline void OSInitMutex(OSMutex*m){pthread_mutexattr_t a;pthread_mutexattr_init(&a);pthread_mutexattr_settype(&a,PTHREAD_MUTEX_RECURSIVE);pthread_mutex_init(m,&a);pthread_mutexattr_destroy(&a);}
static inline void OSLockMutex(OSMutex*m){pthread_mutex_lock(m);}
static inline void OSUnlockMutex(OSMutex*m){pthread_mutex_unlock(m);}
''')
 (p/'test.c').write_text(r'''
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fat.h>
#include <sys/iosupport.h>
static int image; static const devoptab_t *dev;
int AddDevice(const devoptab_t*d){assert(!dev);dev=d;return 3;}
int RemoveDevice(const char*s){(void)s;dev=NULL;return 0;}
int FindDevice(const char*s){(void)s;return dev?3:-1;}
const devoptab_t *GetDeviceOpTab(const char*s){(void)s;return dev;}
int setDefaultDevice(int d){return d;}
int getDefaultDevice(void){return 0;}
static bool yes(void){return true;}
static bool readS(uint32_t sec,uint32_t n,void*b){return pread(image,b,(size_t)n*512,(off_t)sec*512)==(ssize_t)n*512;}
static bool writeS(uint32_t sec,uint32_t n,const void*b){return pwrite(image,b,(size_t)n*512,(off_t)sec*512)==(ssize_t)n*512;}
const DISC_INTERFACE Mocha_usb_disc_interface={0,3,yes,yes,readS,writeS,yes,yes};
const DISC_INTERFACE Mocha_sdio_disc_interface={0,3,yes,yes,readS,writeS,yes,yes};
static void *gameReader(void*arg){
 (void)arg;struct _reent r={0};void*f=calloc(1,dev->structSize);assert(f);
 assert(dev->open_r(&r,f,"usb:/game.bin",O_RDONLY,0)!=-1);
 uint8_t b[4096];struct stat st;assert(dev->fstat_r(&r,f,&st)==0 && st.st_size==400003);
 for(unsigned i=0;i<240;++i){unsigned pos=(i*3571)%390000;assert(dev->seek_r(&r,f,pos,SEEK_SET)==pos);assert(dev->read_r(&r,f,(char*)b,sizeof(b))==sizeof(b));for(unsigned j=0;j<sizeof(b);++j)assert(b[j]==(pos+j)%251);}
 assert(!dev->close_r(&r,f));free(f);return NULL;
}
int main(int argc,char**argv){
 assert(argc==2);image=open(argv[1],O_RDWR);assert(image>=0);
 assert(fatMount("usb",&Mocha_usb_disc_interface,0,4,64));assert(dev);
 struct _reent r={0};assert(!dev->mkdir_r(&r,"usb:/ScummVM saves",0777));
 pthread_t threads[2];for(int i=0;i<2;++i)assert(!pthread_create(&threads[i],NULL,gameReader,NULL));
 uint8_t data[262147];for(unsigned j=0;j<sizeof(data);++j)data[j]=j%251;
 for(int i=0;i<12;++i){
  void*f=calloc(1,dev->structSize);assert(f);
  assert(dev->open_r(&r,f,"usb:/ScummVM saves/slot.sav.tmp",O_WRONLY|O_CREAT|O_TRUNC,0666)!=-1);
  assert(dev->write_r(&r,f,(char*)data,sizeof(data))==sizeof(data));
  assert(!dev->fsync_r(&r,f));assert(!dev->close_r(&r,f));free(f);
  if(i)assert(!dev->unlink_r(&r,"usb:/ScummVM saves/slot.sav"));
  assert(!dev->rename_r(&r,"usb:/ScummVM saves/slot.sav.tmp","usb:/ScummVM saves/slot.sav"));
 }
 for(int i=0;i<2;++i)assert(!pthread_join(threads[i],NULL));
 void*state=calloc(1,dev->dirStateSize);DIR_ITER it={.dirStruct=state};assert(dev->diropen_r(&r,&it,"usb:/ScummVM saves"));
 int count=0;char name[1024];struct stat st;while(!dev->dirnext_r(&r,&it,name,&st)){if(!strcmp(name,"slot.sav")){assert(st.st_size==sizeof(data));++count;}}
 assert(count==1);assert(!dev->dirclose_r(&r,&it));free(state);
 fatUnmount("usb:");assert(!dev);assert(!fsync(image));assert(!close(image));
 puts("PASS: libfat game reads/seeks/stat, simultaneous readers + 12 save writes/fsync/close/replacements, long names, directory listing, unmount");
}
''')
 binary=p/'test'
 sources=list((fat/'source').glob('*.c'))
 subprocess.run(['gcc','-std=gnu11','-g','-O1','-D__WIIU__','-Wall','-Wextra','-Wno-unused-parameter','-Wno-pointer-to-int-cast','-Wno-missing-field-initializers','-Wno-sign-compare',
                 '-fsanitize=address,undefined','-no-pie','-pthread','-I'+str(p),'-I'+str(fat/'include'),'-I'+str(mocha/'include'),
                 str(p/'test.c'),*map(str,sources),'-o',str(binary)],check=True)
 seed=bytes(i%251 for i in range(400003));(p/'seed').write_bytes(seed)
 for partitioned in (False,True):
  image=p/('mbr.img' if partitioned else 'vbr.img')
  with image.open('wb') as f:f.truncate(256*1024*1024)
  offset=2048 if partitioned else 0
  cmd=['mkfs.fat','-F','32']
  if offset:cmd+=['--offset='+str(offset)]
  subprocess.run(cmd+[str(image)],check=True,stdout=subprocess.DEVNULL)
  if offset:
   import struct
   mbr=bytearray(512);mbr[446+4]=0x0c;struct.pack_into('<II',mbr,446+8,offset,256*1024*1024//512-offset);mbr[510:]=b'\x55\xaa'
   with image.open('r+b') as f:f.write(mbr)
  mpath=str(image)+'@@'+str(offset*512)
  subprocess.run(['mcopy','-i',mpath,str(p/'seed'),'::/game.bin'],check=True)
  subprocess.run([str(binary),str(image)],check=True)
  out=p/'out';subprocess.run(['mcopy','-i',mpath,'::/ScummVM saves/slot.sav',str(out)],check=True)
  assert out.read_bytes()==seed[:262147];out.unlink()
  subprocess.run(['mcopy','-i',mpath,'::/game.bin',str(out)],check=True);assert out.read_bytes()==seed;out.unlink()
  if offset:
   # fsck.fat accepts a volume image, not a partition offset. Copy test bytes only.
   with image.open('rb') as f:f.seek(offset*512);data=f.read()
   vol=p/'partition.img';vol.write_bytes(data)
  else:vol=image
  subprocess.run(['fsck.fat','-n',str(vol)],check=True)
  print('PASS: independent mtools byte comparison and fsck.fat,', 'MBR' if offset else 'superfloppy')

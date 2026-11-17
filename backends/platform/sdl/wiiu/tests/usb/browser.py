#!/usr/bin/env python3
"""Actual drive nodes/factory/normalizer with mocked libc and stream transport.

The factory receives WUT-style cwd strings; no physical devices are accessed.
Pass --before to use the baseline Git versions of the two changed sources.
"""
from pathlib import Path
import subprocess
import sys
import tempfile

root = Path(__file__).resolve().parents[6]

def source(rel):
    if '--before' in sys.argv and rel.startswith('backends/fs/posix-drives/'):
        return subprocess.check_output(['git', 'show', '87031d09:'+rel], cwd=root, text=True)
    return (root/rel).read_text()

def method(rel, signature):
    s=source(rel);start=s.index(signature);end=s.index('{',start)+1;depth=1
    while depth:
        depth+=(s[end]=='{')-(s[end]=='}');end+=1
    return s[start:end]

support=r'''
#pragma once
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/param.h>
#include <cstdint>
using uint32=uint32_t;
namespace Common {
struct FSNode {enum ListMode {kListAll,kListFilesOnly,kListDirectoriesOnly};};
class String : public std::string {
public:
 using std::string::string;
 String(const std::string &s):std::string(s){}
 bool hasPrefix(const char *s)const{return compare(0,strlen(s),s)==0;}
 bool hasSuffix(const char *s)const{return size()>=strlen(s)&&compare(size()-strlen(s),strlen(s),s)==0;}
 bool contains(char c)const{return find(c)!=npos;}
 char lastChar()const{return back();}
};
using U32String=String;
template<class T> using Array=std::vector<T>;
template<class T> using List=std::list<T>;
using std::find;
String normalizePath(const String&,char);
String lastPathComponent(const String &s,char c){return s.substr(s.find_last_of(c)+1);}
struct SeekableReadStream {virtual ~SeekableReadStream()=default;};
struct SeekableWriteStream {virtual ~SeekableWriteStream()=default;};
}
namespace DisposeAfterUse {enum Flag {NO,YES};}
class StdioStream: public Common::SeekableReadStream,public Common::SeekableWriteStream {
public:enum WriteMode {WriteMode_Read,WriteMode_Write,WriteMode_WriteAtomic};
 void setBufferSize(unsigned){}
};
struct PosixIoStream {
 using WriteMode=StdioStream::WriteMode;
 static constexpr WriteMode WriteMode_Read=StdioStream::WriteMode_Read,WriteMode_Write=StdioStream::WriteMode_Write,WriteMode_WriteAtomic=StdioStream::WriteMode_WriteAtomic;
 static StdioStream *makeFromPath(const Common::String&,WriteMode){return nullptr;}
};
namespace Common {
SeekableReadStream *wrapBufferedSeekableReadStream(StdioStream*p,unsigned,DisposeAfterUse::Flag){return p;}
SeekableWriteStream *wrapBufferedWriteStream(StdioStream*p,unsigned,DisposeAfterUse::Flag=DisposeAfterUse::YES){return p;}
}
class AbstractFSNode;
using AbstractFSList=Common::Array<AbstractFSNode*>;
class AbstractFSNode {
public:
 using ListMode=Common::FSNode::ListMode;
 static constexpr ListMode kListAll=Common::FSNode::kListAll,kListFilesOnly=Common::FSNode::kListFilesOnly,kListDirectoriesOnly=Common::FSNode::kListDirectoriesOnly;
 virtual ~AbstractFSNode()=default;
 virtual AbstractFSNode *getChild(const Common::String&)const=0;
 virtual AbstractFSNode *getParent()const=0;
 virtual bool getChildren(AbstractFSList&,ListMode,bool)const=0;
 virtual bool isDirectory()const=0;
 virtual Common::String getPath()const=0;
 virtual Common::String getName()const=0;
 virtual Common::SeekableReadStream *createReadStream(){return nullptr;}
 virtual Common::SeekableWriteStream *createWriteStream(bool){return nullptr;}
};
class POSIXFilesystemNode:public AbstractFSNode {
protected:
 Common::String _path,_displayName;
 bool _isDirectory=false,_isValid=false;
 virtual AbstractFSNode *makeNode(const Common::String &p)const{return new POSIXFilesystemNode(p);}
 POSIXFilesystemNode()=default;
 void setFlags();
public:
 POSIXFilesystemNode(const Common::String&);
 bool exists()const;
 bool isReadable()const;
 bool isWritable()const;
 bool isDirectory()const override{return _isDirectory;}
 Common::String getPath()const override{return _path;}
 Common::String getName()const override{return _displayName;}
 AbstractFSNode *getChild(const Common::String&)const override{return nullptr;}
 bool getChildren(AbstractFSList&,ListMode,bool)const override{return false;}
 AbstractFSNode *getParent()const override;
};
class FilesystemFactory {
public:
 virtual ~FilesystemFactory()=default;
 virtual AbstractFSNode *makeRootFileNode()const=0;
 virtual AbstractFSNode *makeCurrentDirectoryFileNode()const=0;
 virtual AbstractFSNode *makeFileNodePath(const Common::String&)const=0;
};
static const char *fakeCwd="fs:/vol/external01";
static int libcCalls,opendirCalls;
static char *fakeGetcwd(char *out,size_t n){strncpy(out,fakeCwd,n);return out;}
static bool knownDirectory(const char *s){
 Common::String p=Common::normalizePath(s,'/');
 if(p=="fs:")p="/";
 else if(p.hasPrefix("fs:/"))p=p.substr(3);
 return p=="/"||p=="/vol"||p=="/vol/external01"||p=="/vol/external01/games"||p=="usb:"||p=="usb:/games"||p=="usb:/games/nested";
}
static int fakeStat(const char*p,struct stat *st){++libcCalls;memset(st,0,sizeof(*st));st->st_mode=S_IFDIR;return knownDirectory(p)?0:-1;}
static int fakeAccess(const char*p,int){return knownDirectory(p)?0:-1;}
static int entryIndex;
static Common::String openedPath;
static DIR *fakeOpendir(const char*p){++libcCalls;++opendirCalls;openedPath=Common::normalizePath(p,'/');entryIndex=0;return knownDirectory(p)?reinterpret_cast<DIR*>(1):nullptr;}
static struct dirent *fakeReaddir(DIR*){
 static struct dirent d;
 if(entryIndex++>0)return nullptr;
 memset(&d,0,sizeof(d));strcpy(d.d_name,openedPath=="usb:/games"?"nested":"games");d.d_type=DT_DIR;return &d;
}
static int fakeClosedir(DIR*){return 0;}
#define getcwd(...) fakeGetcwd(__VA_ARGS__)
#define stat(...) fakeStat(__VA_ARGS__)
#define access(...) fakeAccess(__VA_ARGS__)
#define opendir(...) fakeOpendir(__VA_ARGS__)
#define readdir(...) fakeReaddir(__VA_ARGS__)
#define closedir(...) fakeClosedir(__VA_ARGS__)
'''

checks=r'''
static void drop(AbstractFSList &nodes){for(auto*p:nodes)delete p;nodes.clear();}
#ifdef __WIIU__
int main(){
 DrivesPOSIXFilesystemFactory drives;
 drives.addDrive("/vol/external01");drives.addDrive("usb:/");
 FilesystemFactory &f=drives;
 for(const char *start:{"fs:/vol/external01/games","fs:/vol/external01/","/vol/external01/games","/vol/external01/","/vol","/","fs:/","fs:"}){
  AbstractFSNode *node=f.makeFileNodePath(start);
  AbstractFSList entries;bool listed=false;
  for(int steps=0;steps<8;++steps){
   entries.clear();assert(node->getChildren(entries,AbstractFSNode::kListAll,false));
   if(entries.size()==2 && entries[0]->getPath()=="/vol/external01/" && entries[1]->getPath()=="usb:/"){
    listed=true;drop(entries);break;
   }
   drop(entries);
   auto*parent=node->getParent();
   if(!parent)break;
   delete node;node=parent;
  }
  printf("Browser start %s: drive list %s\n",start,listed?"reachable":"UNREACHABLE");fflush(stdout);
  assert(listed);
  const Common::String persisted=node->getPath();assert(persisted=="/");
  delete node;node=f.makeFileNodePath(persisted);
  const int before=opendirCalls;
  assert(node->getChildren(entries,AbstractFSNode::kListAll,false));
  assert(opendirCalls==before && entries.size()==2);drop(entries);delete node;
 }
 // Real USB naming must keep its devoptab prefix. Descending/enumerating
 // exercises both d_type fast path and stat fallback in separate builds.
 for(const char *start:{"usb:","usb:/","usb:/games"}){
  auto*node=f.makeFileNodePath(start);assert(node->getPath().hasPrefix("usb:"));
  AbstractFSList entries;assert(node->getChildren(entries,AbstractFSNode::kListDirectoriesOnly,false));
  assert(entries.size()==1 && entries[0]->isDirectory());drop(entries);
  bool listed=false;
  for(int i=0;i<4;++i){auto*parent=node->getParent();if(!parent)break;delete node;node=parent;
   if(node->getPath()=="/"){listed=true;break;}}
  assert(listed);delete node;
 }
 fakeCwd="fs:/vol/external01/games";
 auto*node=f.makeCurrentDirectoryFileNode();assert(node->getPath()=="/");delete node;
 // SD-only startup must still offer a usable root without a phantom USB.
 DrivesPOSIXFilesystemFactory sd;sd.addDrive("/vol/external01");
 node=static_cast<FilesystemFactory&>(sd).makeRootFileNode();AbstractFSList entries;
 assert(node->getChildren(entries,AbstractFSNode::kListAll,false)&&entries.size()==1);
 drop(entries);delete node;
 puts("PASS: actual drive factory/nodes: FSA aliases, physical roots, Up to SD/USB, root persistence, typed paths, child types and SD-only list (host libc mocks)");
}
#else
int main(){
 DrivesPOSIXFilesystemFactory drives;drives.addDrive("/vol/external01");
 FilesystemFactory &f=drives;
 auto*node=f.makeRootFileNode();assert(node->getPath().empty());delete node;
 node=f.makeCurrentDirectoryFileNode();assert(node->getPath()==fakeCwd);delete node;
 node=f.makeFileNodePath("/");assert(node->getParent()==nullptr);
 AbstractFSList entries;assert(node->getChildren(entries,AbstractFSNode::kListAll,false));
 assert(entries.size()==1);drop(entries);delete node;
 puts("PASS: non-WiiU POSIX retains physical /, cwd prefixes and empty virtual-root semantics");
}
#endif
'''

with tempfile.TemporaryDirectory(prefix='scummvm-browser-') as temp:
    p=Path(temp)
    (p/'support.h').write_text(support)
    for rel in ['backends/fs/posix/posix-fs.h','backends/fs/posix/posix-iostream.h',
                'backends/fs/fs-factory.h','common/algorithm.h','common/bufferedstream.h']:
        dest=p/rel;dest.parent.mkdir(parents=True,exist_ok=True);dest.write_text('#include "support.h"\n')
    body='#include "support.h"\nnamespace Common {\n'+method('common/str.cpp','String normalizePath(')+'\n}\n'
    for sig in ['bool POSIXFilesystemNode::exists()', 'bool POSIXFilesystemNode::isReadable()',
                'bool POSIXFilesystemNode::isWritable()', 'void POSIXFilesystemNode::setFlags()',
                'POSIXFilesystemNode::POSIXFilesystemNode(const Common::String &p)',
                'AbstractFSNode *POSIXFilesystemNode::getParent()']:
        body+=method('backends/fs/posix/posix-fs.cpp',sig)+'\n'
    for rel in ['backends/fs/posix-drives/posix-drives-fs.cpp','backends/fs/posix-drives/posix-drives-fs-factory.cpp']:
        body+=source(rel)+'\n'
    (p/'test.cpp').write_text(body+checks)
    for platform,fallback in [('__WIIU__',False),('__WIIU__',True),('POSIX',False)]:
        cmd=['g++','-std=c++11','-Wall','-Wextra','-Wno-unused-parameter','-Wno-unused-function','-Wno-reorder','-Werror','-g',
             '-fsanitize=address,undefined','-fno-omit-frame-pointer','-no-pie','-D'+platform,
             '-I'+str(p),'-I'+str(root),str(p/'test.cpp'),'-o',str(p/'test')]
        if fallback:cmd.insert(1,'-DSYSTEM_NOT_SUPPORTING_D_TYPE')
        subprocess.run(cmd,check=True);subprocess.run([str(p/'test')],check=True)

#!/usr/bin/env python3
"""Actual SAGA keymap factory, panel switch and input consumers; mocked event transport.

Synthetic events only. Does not validate hardware, cutaway rendering or game data.
"""
from pathlib import Path
import subprocess
import tempfile
root=Path(__file__).resolve().parents[6]
def function(file,name):
 s=(root/file).read_text();a=s.index(name);i=s.index('{',a);depth=1;j=i+1
 while depth:
  if s[j]=='{':depth+=1
  elif s[j]=='}':depth-=1
  j+=1
 return s[a:j]+'\n'
saga=(root/'engines/saga/saga.cpp').read_text()
header=(root/'engines/saga/saga.h').read_text();actions=header[header.index('enum SAGAAction {'):header.index('extern const char *engineKeyMapId;')]
h=(root/'engines/saga/interface.h').read_text();panels=h[h.index('enum PanelModes {'):h.index('enum FadeModes {')]
ids=saga[saga.index('const char *engineKeyMapId ='):saga.index('static const GameResourceDescription ITE_Resources_GermanAGACD')]
factory=function('engines/saga/metaengine.cpp','Common::KeymapArray SagaMetaEngine::initKeymaps(')
switch=function('engines/saga/saga.cpp','void SagaEngine::enableKeyMap(')
intro=function('engines/saga/introproc_ihnm.cpp','bool Scene::checkKey(')
input_fn=function('engines/saga/input.cpp','int SagaEngine::processInput(')
assert '_currentPanelMode = -1;' in saga
prefix=r'''
#include <cassert>
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>
#include <deque>
#define _(x) x
#define SUCCESS 0
#define RF_RENDERPAUSE 1
using uint=unsigned;
void debug(int,const char*,...){}
namespace Common {
using String=std::string; using CustomEventType=unsigned;
enum EventType {EVENT_INVALID, EVENT_KEYDOWN, EVENT_KEYUP, EVENT_CUSTOM_ENGINE_ACTION_START,
 EVENT_CUSTOM_ENGINE_ACTION_END,EVENT_RETURN_TO_LAUNCHER,EVENT_QUIT,EVENT_LBUTTONDOWN,EVENT_LBUTTONUP,
 EVENT_RBUTTONDOWN,EVENT_RBUTTONUP,EVENT_WHEELUP,EVENT_WHEELDOWN,EVENT_MOUSEMOVE,EVENT_JOYBUTTON_DOWN};
enum Language {EN_ANY,JA_JPN};
struct KeyState {int keycode=0;int ascii=0;};
struct Point {int x=0,y=0;};
struct Event {EventType type=EVENT_INVALID; CustomEventType customType=0;KeyState kbd;Point mouse;String physical;};
const char *kStandardActionLeftClick="LCLK",*kStandardActionMiddleClick="MCLK",*kStandardActionRightClick="RCLK";
struct Action {
 String id;Event event;std::vector<String> mappings;
 Action(const char*i,const char*):id(i){}
 void addDefaultInputMapping(const char*m){mappings.push_back(m);}
 void setLeftClickEvent(){event.type=EVENT_LBUTTONDOWN;}
 void setMiddleClickEvent(){event.type=EVENT_INVALID;}
 void setRightClickEvent(){event.type=EVENT_RBUTTONDOWN;}
 void setCustomEngineActionEvent(unsigned action){event.type=EVENT_CUSTOM_ENGINE_ACTION_START;event.customType=action;}
};
struct Keymap {
 enum KeymapType {kKeymapTypeGame,kKeymapTypeGlobal};
 KeymapType type;String id;bool enabled=true;std::vector<Action*> actions;
 Keymap(KeymapType t,const char*i,const char*):type(t),id(i){}
 ~Keymap(){for(auto*a:actions)delete a;}
 void addAction(Action*a){actions.push_back(a);}
 void setEnabled(bool b){enabled=b;}bool isEnabled()const{return enabled;}
 KeymapType getType()const{return type;}const String&getId()const{return id;}
};
using KeymapArray=std::vector<Keymap*>;
struct Keymapper {
 KeymapArray maps;
 const KeymapArray&getKeymaps()const{return maps;}
 Keymap*getKeymap(const char*n){for(auto*m:maps)if(m->id==n)return m;assert(false);return nullptr;}
};
struct EventManager {
 Keymapper mapper;std::deque<Event> pending;
 Keymapper*getKeymapper(){return &mapper;}
 bool pollEvent(Event&out){
  if(pending.empty())return false;out=pending.front();pending.pop_front();
  for(auto*m:mapper.maps)if(m->enabled)for(auto*a:m->actions)
   if(std::find(a->mappings.begin(),a->mappings.end(),out.physical)!=a->mappings.end()){
    auto mouse=out.mouse;out=a->event;out.mouse=mouse;return true;
   }
  return true;
 }
};
}
struct System {Common::EventManager events;Common::EventManager*getEventManager(){return &events;}};
System systemObj;System*g_system=&systemObj;
namespace Saga {
'''
prefix+=actions+panels+ids+r'''
struct Interface {
 bool _textInput=false,_statusTextInput=false;int mode=kPanelNull,updates=0;Common::Point lastMouse;
 std::vector<unsigned> received;
 int getMode(){return mode;}int getSaveReminderState(){return 1;}void setMode(int m){mode=m;}
 void keyBoss(){}void draw(){}
 void processAscii(Common::KeyState,unsigned action){received.push_back(action);}
 void update(Common::Point p,int){++updates;lastMouse=p;}
};
struct Actor {int aborted=0;void abortSpeech(){++aborted;}};
struct Render {void toggleFlag(int){}};
enum {UPDATE_LEFTBUTTONCLICK,UPDATE_RIGHTBUTTONCLICK,UPDATE_WHEELUP,UPDATE_WHEELDOWN};
struct SagaEngine {
 int _currentPanelMode;Interface*_interface;Actor*_actor;Render*_render;
 Common::EventManager*_eventMan;bool _leftMouseButtonPressed=false,_rightMouseButtonPressed=false;
 Common::Language language=Common::EN_ANY;
 SagaEngine(Interface*i,Actor*a,Render*r):_interface(i),_actor(a),_render(r),_eventMan(&systemObj.events){_currentPanelMode=-1;}
 Common::Language getLanguage(){return language;}
 static Common::EventManager*getEventManager(){return &systemObj.events;}
 void enableKeyMap(int);int processInput();
};
struct Scene {SagaEngine*_vm;bool checkKey();};
}
struct SagaMetaEngine {Common::KeymapArray initKeymaps(const char*)const;};
'''
checks=r'''
int main(){
 using namespace Common;using namespace Saga;
 SagaMetaEngine meta;auto maps=meta.initKeymaps("ihnm");systemObj.events.mapper.maps=maps;
 Interface ui;Actor actor;Render render;SagaEngine vm(&ui,&actor,&render);Scene scene{&vm};
 auto press=[&](const char*physical,EventType type=EVENT_JOYBUTTON_DOWN,int ascii=0){Event e;e.type=type;e.physical=physical;e.kbd.ascii=ascii;systemObj.events.pending.push_back(e);};
 // The existing action IDs remain stable, including game-shortcuts/ESCAPE.
 assert(maps.size()==7&&maps[0]->id=="engine-default"&&maps[1]->id=="game-shortcuts");
 auto bound=[&](const char*id,const char*key){unsigned n=0;for(auto*m:maps)for(auto*a:m->actions)if(a->id==id&&std::find(a->mappings.begin(),a->mappings.end(),key)!=a->mappings.end())++n;return n;};
 assert(bound("ESCAPE","ESCAPE")==1&&bound("ESCAPE","JOY_Y")==1);
 assert(bound("ABRTSPEECH","SPACE")==1&&bound("ABRTSPEECH","JOY_X")==1);
 assert(bound("SHOWOPTION","JOY_LEFT_SHOULDER")==1);
 // First press in the separately polled initial intro: no priming event.
 for(int mode:{kPanelVideo,kPanelCutaway,kPanelNull}){
  ui.mode=mode;press("JOY_Y");assert(scene.checkKey());assert(vm._currentPanelMode==mode);
  press("JOY_X");assert(scene.checkKey());press("ESCAPE",EVENT_KEYDOWN,27);assert(scene.checkKey());
  press("",EVENT_KEYDOWN,13);assert(scene.checkKey());press("",EVENT_KEYDOWN,0);assert(!scene.checkKey());
  press("",EVENT_CUSTOM_ENGINE_ACTION_END);assert(!scene.checkKey());
  press("",EVENT_QUIT);assert(scene.checkKey());press("",EVENT_RETURN_TO_LAUNCHER);assert(scene.checkKey());
 }
 press("JOY_A");
#ifdef __WIIU__
 assert(scene.checkKey());
#else
 assert(!scene.checkKey());
#endif
 // Once normal input processing starts, Y/2 must reach the cutaway handler
 // on the first press after a script switches away from an options panel.
 ui.mode=kPanelOption;vm.enableKeyMap(ui.mode);assert(!maps[1]->enabled);
 ui.mode=kPanelCutaway;ui.received.clear();press("JOY_Y");vm.processInput();
 assert(ui.received.size()==1&&ui.received[0]==kActionEscape);
 ui.mode=kPanelVideo;press("JOY_Y");vm.processInput();assert(ui.received.back()==kActionEscape);
 // A/B mouse path in character selection and gameplay, including release.
 for(int mode:{kPanelChapterSelection,kPanelMain}){
  ui.mode=mode;Event e;e.type=EVENT_JOYBUTTON_DOWN;e.physical="JOY_A";e.mouse.x=321;e.mouse.y=211;
  systemObj.events.pending.push_back(e);vm.processInput();assert(vm._leftMouseButtonPressed&&ui.lastMouse.x==321&&ui.lastMouse.y==211);
  press("",EVENT_LBUTTONUP);vm.processInput();assert(!vm._leftMouseButtonPressed);
  press("JOY_B");vm.processInput();assert(vm._rightMouseButtonPressed);
  press("",EVENT_RBUTTONUP);vm.processInput();assert(!vm._rightMouseButtonPressed);
 }
 press("JOY_X");vm.processInput();assert(actor.aborted==1);
 press("JOY_LEFT_SHOULDER");vm.processInput();assert(ui.mode==kPanelOption);
 // Panel routing remains exclusive, and does not disable global menu/keyboard.
 auto*global=new Keymap(Keymap::kKeymapTypeGlobal,"global","Global");systemObj.events.mapper.maps.push_back(global);
 for(auto pair:std::vector<std::pair<int,const char*>>{{kPanelOption,"option-panel"},{kPanelSave,"save-panel"},{kPanelLoad,"load-panel"},{kPanelQuit,"quit-panel"},{kPanelConverse,"converse-panel"}}){
  vm.enableKeyMap(pair.first);assert(global->enabled&&maps[0]->enabled);
  for(unsigned i=1;i<maps.size();++i)assert(maps[i]->enabled==(maps[i]->id==pair.second));
 }
 // Existing native text path does not consume letters as game shortcuts.
 ui.mode=kPanelSave;ui._textInput=true;maps[3]->setEnabled(false);
 for(const char*letter:{"w","c","s"}){press(letter,EVENT_KEYDOWN,letter[0]);vm.processInput();assert(ui.received.back()==0&&!maps[1]->enabled&&!maps[3]->enabled);}
 ui._textInput=false;ui.mode=kPanelMain;press("JOY_Y");vm.processInput();
 assert(ui.received.back()==kActionEscape&&maps[1]->enabled&&!maps[3]->enabled);
 press("JOY_Y");vm.processInput();assert(maps[1]->enabled&&!maps[3]->enabled);
 ui._statusTextInput=true;press("w",EVENT_KEYDOWN,'w');vm.processInput();assert(ui.received.back()==0&&!maps[1]->enabled);
 ui._statusTextInput=false;press("JOY_Y");vm.processInput();assert(ui.received.back()==kActionEscape&&maps[1]->enabled);
 for(auto*m:systemObj.events.mapper.maps)delete m;
 puts("PASS: actual SAGA factory/panel switching/input consumers; first-press Y/2, custom intro keys, cutaways, clicks/releases, options, text and global map preservation (host mocks)");
}
'''
with tempfile.TemporaryDirectory(prefix='scummvm-saga-input-') as temp:
 p=Path(temp);src=p/'test.cpp';src.write_text(prefix+factory+'namespace Saga {\n'+switch+intro+input_fn+'}\n'+checks)
 for flags in ([],['-D__WIIU__'],['-D__WIIU__','-DWIIU_SAGA_INPUT_DIAGNOSTICS']):
  subprocess.run(['g++','-std=c++11','-Wall','-Wextra','-Werror','-Wno-unused-parameter','-Wno-misleading-indentation','-fsanitize=address,undefined','-no-pie',*flags,str(src),'-o',str(p/'test')],check=True)
  subprocess.run([str(p/'test')],check=True)
# Preserve the engine's deliberate cutscene cursor policy; rendering is not mocked here.
s=(root/'engines/saga/animation.cpp').read_text()
assert '_vm->_gfx->showCursor(false);' in s and '_vm->_gfx->showCursor(true);' in s
print('PASS: cutscene cursor hide/restore source retained; no claim of physical pointer/game validation')

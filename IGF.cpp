/*
Indie framework was create by Popov Evgeniy Alekseyevich
Some code bases on code from SVGALib(http://www.svgalib.org/).

Indie game framework license

Copyright � 2017-2018, Popov Evgeniy Alekseyevich

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Third�party code license

SVGALib is public domain. SVGALib homepage: http://www.svgalib.org/
*/

#include "IGF.h"

unsigned char IGF_Keys[IGF_KEYBOARD];
unsigned char IGF_Buttons[IGF_MOUSE];

LRESULT CALLBACK IGF_Process_Message(HWND window,UINT Message,WPARAM wParam,LPARAM lParam)
{
 switch (Message)
 {
  case WM_CLOSE:
  DestroyWindow(window);
  break;
  case WM_DESTROY:
  PostQuitMessage(0);
  break;
  case WM_CREATE:
  memset(IGF_Keys,IGFKEY_NONE,IGF_KEYBOARD);
  memset(IGF_Buttons,IGFKEY_NONE,IGF_MOUSE);
  break;
  case WM_LBUTTONDOWN:
  IGF_Buttons[IGF_MOUSE_LEFT]=IGFKEY_PRESS;
  break;
  case WM_LBUTTONUP:
  IGF_Buttons[IGF_MOUSE_LEFT]=IGFKEY_RELEASE;
  break;
  case WM_RBUTTONDOWN:
  IGF_Buttons[IGF_MOUSE_RIGHT]=IGFKEY_PRESS;
  break;
  case WM_RBUTTONUP:
  IGF_Buttons[IGF_MOUSE_RIGHT]=IGFKEY_RELEASE;
  break;
  case WM_MBUTTONDOWN:
  IGF_Buttons[IGF_MOUSE_MIDDLE]=IGFKEY_PRESS;
  break;
  case WM_MBUTTONUP:
  IGF_Buttons[IGF_MOUSE_MIDDLE]=IGFKEY_RELEASE;
  break;
  case WM_KEYDOWN:
  IGF_Keys[(lParam >> 16)&0x7f]=IGFKEY_PRESS;
  break;
  case WM_KEYUP:
  IGF_Keys[(lParam >> 16)&0x7f]=IGFKEY_RELEASE;
  break;
 }
 return DefWindowProc(window,Message,wParam,lParam);
}

IGF_Base::IGF_Base()
{
 HRESULT status;
 status=CoInitialize(NULL);
 if(status!=S_OK)
 {
  if(status!=S_FALSE)
  {
   puts("Can't initialize COM");
   exit(EXIT_FAILURE);
  }

 }

}

IGF_Base::~IGF_Base()
{
 CoUninitialize();
}

IGF_Synchronization::IGF_Synchronization()
{
 timer=NULL;
}

IGF_Synchronization::~IGF_Synchronization()
{
 if(timer==NULL)
 {
  CancelWaitableTimer(timer);
  CloseHandle(timer);
 }

}

void IGF_Synchronization::create_timer()
{
 timer=CreateWaitableTimer(NULL,FALSE,NULL);
 if (timer==NULL)
 {
  puts("Can't create synchronization timer");
  exit(EXIT_FAILURE);
 }

}

void IGF_Synchronization::set_timer(const unsigned long int interval)
{
 LARGE_INTEGER start;
 start.QuadPart=0;
 if(SetWaitableTimer(timer,&start,interval,NULL,NULL,FALSE)==FALSE)
 {
  puts("Can't set timer");
  exit(EXIT_FAILURE);
 }

}

void IGF_Synchronization::wait_timer()
{
 WaitForSingleObject(timer,INFINITE);
}

IGF_Engine::IGF_Engine()
{
 window_class.lpszClassName=TEXT("IGF");
 window_class.hInstance=GetModuleHandle(NULL);
 window_class.style=CS_HREDRAW|CS_VREDRAW;
 window_class.lpfnWndProc=(WNDPROC)IGF_Process_Message;
 window_class.hbrBackground=NULL;
 window_class.hIcon=LoadIcon(NULL,IDI_APPLICATION);
 window_class.hCursor=LoadCursor(NULL,IDC_ARROW);
 window_class.cbClsExtra=0;
 window_class.cbWndExtra=0;
 if (window_class.hCursor==NULL)
 {
  puts("Can't load the standart cursor");
  exit(EXIT_FAILURE);
 }
 if (window_class.hIcon==NULL)
 {
  puts("Can't load the standart program icon");
  exit(EXIT_FAILURE);
 }
 if (RegisterClass(&window_class)==0)
 {
  puts("Can't register window class");
  exit(EXIT_FAILURE);
 }
 width=GetSystemMetrics(SM_CXSCREEN);
 height=GetSystemMetrics(SM_CYSCREEN);
}

IGF_Engine::~IGF_Engine()
{
 if(window!=NULL) CloseWindow(window);
 UnregisterClass(window_class.lpszClassName,window_class.hInstance);
}

void IGF_Engine::create_window()
{
 window=CreateWindow(window_class.lpszClassName,NULL,WS_VISIBLE|WS_POPUP,0,0,width,height,NULL,NULL,window_class.hInstance,NULL);
 if (window==NULL)
 {
  puts("Can't create window");
  exit(EXIT_FAILURE);
 }
 EnableWindow(window,TRUE);
 SetFocus(window);
}

void IGF_Engine::capture_mouse()
{
 RECT border;
 if(GetClientRect(window,&border)==FALSE)
 {
  puts("Can't capture window");
  exit(EXIT_FAILURE);
 }
 if(ClipCursor(&border)==FALSE)
 {
  puts("Can't capture cursor");
  exit(EXIT_FAILURE);
 }

}

bool IGF_Engine::process_message()
{
 bool quit;
 MSG Message;
 quit=false;
 while(PeekMessage(&Message,window,0,0,PM_NOREMOVE)==TRUE)
 {
  if(GetMessage(&Message,window,0,0)==TRUE)
  {
   TranslateMessage(&Message);
   DispatchMessage(&Message);
  }
  else
  {
   quit=true;
   break;
  }

 }
 return quit;
}

unsigned long int IGF_Engine::get_width()
{
 return width;
}

unsigned long int IGF_Engine::get_height()
{
 return height;
}

IGF_Frame::IGF_Frame()
{
 frame_width=512;
 frame_height=512;
 frame_line=0;
 buffer=NULL;
}

IGF_Frame::~IGF_Frame()
{
 if(buffer!=NULL)
 {
  free(buffer);
  buffer=NULL;
 }

}

void IGF_Frame::create_render_buffer()
{
 frame_line=frame_width*sizeof(unsigned long int);
 length=frame_height*frame_line;
 buffer=(unsigned long int*)calloc(length,1);
 if(buffer==NULL)
 {
  puts("Can't allocate memory for render buffer");
  exit(EXIT_FAILURE);
 }

}

void IGF_Frame::draw_pixel(const unsigned long int x,const unsigned long int y,const unsigned long int red,const unsigned long int green,const unsigned long int blue)
{
 if((x<frame_width)&&(y<frame_height))
 {
  buffer[x+y*frame_width]=blue+256*green+65536*red; //This code bases on code from SVGALib
 }

}

void IGF_Frame::clear_screen()
{
 memset(buffer,0,length);
}

unsigned long int IGF_Frame::get_frame_width()
{
 return frame_width;
}

unsigned long int IGF_Frame::get_frame_height()
{
 return frame_height;
}

IGF_Render::IGF_Render()
{
 render=NULL;
 target=NULL;
 surface=NULL;
}

IGF_Render::~IGF_Render()
{
 if(surface!=NULL) surface->Release();
 if(target!=NULL) target->Release();
 if(render!=NULL) render->Release();
}

void IGF_Render::create_factory()
{
 if(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,&render)!=S_OK)
 {
  puts("Can't create render");
  exit(EXIT_FAILURE);
 }

}

void IGF_Render::create_target()
{
 if(render->CreateHwndRenderTarget(setting,configuration,&target)!=S_OK)
 {
  puts("Can't create render target");
  exit(EXIT_FAILURE);
 }
 target->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
}

void IGF_Render::create_surface()
{
 if(target->CreateBitmap(D2D1::SizeU(frame_width,frame_height),buffer,frame_line,D2D1::BitmapProperties(setting.pixelFormat,96.0,96.0),&surface)!=S_OK)
 {
  puts("Can't create render surface");
  exit(EXIT_FAILURE);
 }

}

void IGF_Render::set_render_setting()
{
 setting=D2D1::RenderTargetProperties();
 setting.type=D2D1_RENDER_TARGET_TYPE_HARDWARE;
 setting.pixelFormat=D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_IGNORE);
 setting.usage=D2D1_RENDER_TARGET_USAGE_NONE;
 setting.minLevel=D2D1_FEATURE_LEVEL_9;
 configuration.hwnd=window;
 configuration.pixelSize=D2D1::SizeU(width,height);
 configuration.presentOptions=D2D1_PRESENT_OPTIONS_IMMEDIATELY;
}

void IGF_Render::set_render()
{
 this->set_render_setting();
 this->create_factory();
 this->create_target();
 this->create_surface();
}

void IGF_Render::destroy_resource()
{
 if(surface!=NULL)
 {
  surface->Release();
  surface=NULL;
 }
 if(target!=NULL)
 {
  target->Release();
  target=NULL;
 }

}

void IGF_Render::recreate_render()
{
 this->destroy_resource();
 this->create_target();
 this->create_surface();
}

void IGF_Render::prepare_surface()
{
 source=D2D1::RectU(0,0,frame_width,frame_height);
 destanation=D2D1::RectF(0,0,width,height);
 texture=D2D1::RectF(0,0,frame_width,frame_height);
}

void IGF_Render::create_render()
{
 this->set_render_setting();
 this->set_render();
 this->prepare_surface();
 this->create_render_buffer();
}

void IGF_Render::refresh()
{
 target->BeginDraw();
 target->DrawBitmap(surface,destanation,1.0,D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,texture);
 if(target->EndDraw()==(HRESULT)D2DERR_RECREATE_TARGET) this->recreate_render();
 surface->CopyFromMemory(&source,buffer,frame_line);
}

void IGF_Screen::initialize()
{
 this->create_render_buffer();
 this->create_timer();
 this->create_window();
 this->capture_mouse();
 this->set_render();
 this->create_render();
 this->set_timer(17);
}

bool IGF_Screen::sync()
{
 bool quit;
 this->refresh();
 quit=this->process_message();
 this->wait_timer();
 return quit;
}

IGF_Screen* IGF_Screen::get_handle()
{
 return this;
}

IGF_Keyboard::IGF_Keyboard()
{
 preversion=NULL;
}

IGF_Keyboard::~IGF_Keyboard()
{
 if(preversion!=NULL) free(preversion);
}

void IGF_Keyboard::initialize()
{
 preversion=(unsigned char*)calloc(IGF_KEYBOARD,1);
 if(preversion==NULL)
 {
  puts("Can't allocate memory for keyboard state buffer");
  exit(EXIT_FAILURE);
 }

}

bool IGF_Keyboard::check_hold(const unsigned char code)
{
 bool result;
 result=false;
 if(IGF_Keys[code]==IGFKEY_PRESS) result=true;
 return result;
}

bool IGF_Keyboard::check_press(const unsigned char code)
{
 bool result;
 result=false;
 if(IGF_Keys[code]==IGFKEY_PRESS)
 {
  if(preversion[code]!=IGFKEY_PRESS) result=true;
 }
 preversion[code]=IGF_Keys[code];
 return result;
}

bool IGF_Keyboard::check_release(const unsigned char code)
{
 bool result;
 result=false;
 if(IGF_Keys[code]==IGFKEY_RELEASE)
 {
  result=true;
  IGF_Keys[code]=IGFKEY_NONE;
 }
 return result;
}

IGF_Mouse::IGF_Mouse()
{
 memset(preversion,IGFKEY_NONE,IGF_MOUSE);
}

IGF_Mouse::~IGF_Mouse()
{
 while(ShowCursor(TRUE)<1) ;
}

void IGF_Mouse::show()
{
 while(ShowCursor(TRUE)<1) ;
}

void IGF_Mouse::hide()
{
 while(ShowCursor(FALSE)>-2) ;
}

void IGF_Mouse::set_position(const unsigned long int x,const unsigned long int y)
{
 if(SetCursorPos(x,y)==FALSE)
 {
  puts("Can't set the mouse cursor position");
  exit(EXIT_FAILURE);
 }

}

unsigned long int IGF_Mouse::get_x()
{
 POINT position;
 if(GetCursorPos(&position)==FALSE)
 {
  puts("Can't get the mouse cursor position");
  exit(EXIT_FAILURE);
 }
 return position.x;
}

unsigned long int IGF_Mouse::get_y()
{
 POINT position;
 if(GetCursorPos(&position)==FALSE)
 {
  puts("Can't get the mouse cursor position");
  exit(EXIT_FAILURE);
 }
 return position.y;
}

bool IGF_Mouse::check_hold(const unsigned char button)
{
 bool result;
 result=false;
 if(button<=IGF_MOUSE_MIDDLE)
 {
  if(IGF_Buttons[button]==IGFKEY_PRESS) result=true;
 }
 return result;
}

bool IGF_Mouse::check_press(const unsigned char button)
{
 bool result;
 result=false;
 if(button<=IGF_MOUSE_MIDDLE)
 {
  if(IGF_Buttons[button]==IGFKEY_PRESS)
  {
   if(preversion[button]!=IGFKEY_PRESS) result=true;
  }

 }
 preversion[button]=IGF_Buttons[button];
 return result;
}

bool IGF_Mouse::check_release(const unsigned char button)
{
 bool result;
 result=false;
 if(button<=IGF_MOUSE_MIDDLE)
 {
  if(IGF_Buttons[button]==IGFKEY_RELEASE)
  {
   result=true;
   IGF_Buttons[button]=IGFKEY_NONE;
  }

 }
 return result;
}

IGF_Gamepad::IGF_Gamepad()
{
 length=sizeof(XINPUT_STATE);
 XInputEnable(TRUE);
 memset(&current,0,length);
 memset(&preversion,0,length);
 memset(&vibration,0,sizeof(XINPUT_VIBRATION));
 memset(&battery,0,sizeof(XINPUT_BATTERY_INFORMATION));
 active=0;
}

IGF_Gamepad::~IGF_Gamepad()
{
 XInputEnable(FALSE);
}

bool IGF_Gamepad::read_battery_status()
{
 bool result;
 result=false;
 if(XInputGetBatteryInformation(active,BATTERY_DEVTYPE_GAMEPAD,&battery)==ERROR_SUCCESS) return result;
 return result;
}

void IGF_Gamepad::clear_state()
{
 memset(&current,0,length);
 memset(&preversion,0,length);
}

bool IGF_Gamepad::read_state()
{
 bool result;
 result=false;
 if(XInputGetState(active,&current)==ERROR_SUCCESS) result=true;
 return result;
}

bool IGF_Gamepad::write_state()
{
 bool result;
 result=false;
 if(XInputSetState(active,&vibration)==ERROR_SUCCESS) result=true;
 return result;
}

void IGF_Gamepad::set_motor(const unsigned short int left,const unsigned short int right)
{
 vibration.wLeftMotorSpeed=left;
 vibration.wRightMotorSpeed=right;
}

bool IGF_Gamepad::check_button(XINPUT_STATE &target,const unsigned short int button)
{
 bool result;
 result=false;
 if(target.Gamepad.wButtons&button) result=true;
 return result;
}

bool IGF_Gamepad::check_trigger(XINPUT_STATE &target,const unsigned char trigger)
{
 bool result;
 result=false;
 if((trigger==IGF_GAMEPAD_LEFT_TRIGGER)&&(target.Gamepad.bLeftTrigger>=XINPUT_GAMEPAD_TRIGGER_THRESHOLD)) result=true;
 if((trigger==IGF_GAMEPAD_RIGHT_TRIGGER)&&(target.Gamepad.bRightTrigger>=XINPUT_GAMEPAD_TRIGGER_THRESHOLD)) result=true;
 return result;
}

void IGF_Gamepad::set_active(const unsigned long int gamepad)
{
 if(active<4)
 {
  this->clear_state();
  active=gamepad;
 }

}

unsigned long int IGF_Gamepad::get_active()
{
 return active;
}

unsigned long int IGF_Gamepad::get_amount()
{
 unsigned long int old,result;
 result=0;
 old=active;
 for(active=0;active<4;active++)
 {
  if(this->read_state()==true)
  {
   this->clear_state();
   result++;
  }

 }
 active=old;
 return result;
}

bool IGF_Gamepad::check_connection()
{
 return this->read_state();
}

bool IGF_Gamepad::is_wireless()
{
 bool result;
 result=false;
 if(this->read_battery_status()==true)
 {
  if(battery.BatteryType!=BATTERY_TYPE_DISCONNECTED)
  {
   if(battery.BatteryType!=BATTERY_TYPE_WIRED) result=true;
  }

 }
 return result;
}

unsigned char IGF_Gamepad::get_battery_type()
{
 unsigned char result;
 result=IGF_GAMEPAD_BATTERY_ERROR;
 if(this->read_battery_status()==true)
 {
  switch (battery.BatteryType)
  {
   case BATTERY_TYPE_ALKALINE:
   result=IGF_GAMEPAD_BATTERY_ALKAINE;
   break;
   case BATTERY_TYPE_NIMH:
   result=IGF_GAMEPAD_BATTERY_NIMH;
   break;
   case BATTERY_TYPE_UNKNOWN:
   result=IGF_GAMEPAD_BATTERY_UNKNOW;
   break;
  }

 }
 return result;
}

unsigned char IGF_Gamepad::get_battery_level()
{
 unsigned char result;
 result=IGF_GAMEPAD_BATTERY_ERROR;
 if(this->read_battery_status()==true)
 {
  switch (battery.BatteryType)
  {
   case BATTERY_LEVEL_EMPTY:
   result=IGF_GAMEPAD_BATTERY_EMPTY;
   break;
   case BATTERY_LEVEL_LOW:
   result=IGF_GAMEPAD_BATTERY_LOW;
   break;
   case BATTERY_LEVEL_MEDIUM:
   result=IGF_GAMEPAD_BATTERY_MEDIUM;
   break;
   case BATTERY_LEVEL_FULL:
   result=IGF_GAMEPAD_BATTERY_FULL;
   break;
  }
  if((battery.BatteryType==BATTERY_TYPE_WIRED)||(battery.BatteryType==BATTERY_TYPE_DISCONNECTED)) result=IGF_GAMEPAD_BATTERY_ERROR;
 }
 return result;
}

void IGF_Gamepad::update()
{
 preversion=current;
 if(this->read_state()==false) this->clear_state();
}

bool IGF_Gamepad::check_button_hold(const unsigned short int button)
{
 return this->check_button(current,button);
}

bool IGF_Gamepad::check_button_press(const unsigned short int button)
{
 bool result;
 result=false;
 if(this->check_button(current,button)==true)
 {
  if(this->check_button(preversion,button)==false) result=true;
 }
 return result;
}

bool IGF_Gamepad::check_button_release(const unsigned short int button)
{
 bool result;
 result=false;
 if(this->check_button(current,button)==false)
 {
  if(this->check_button(preversion,button)==true) result=true;
 }
 return result;
}

bool IGF_Gamepad::check_trigger_hold(const unsigned char trigger)
{
 return this->check_trigger(current,trigger);
}

bool IGF_Gamepad::check_trigger_press(const unsigned char trigger)
{
 bool result;
 result=false;
 if(this->check_trigger(current,trigger)==true)
 {
  if(this->check_trigger(preversion,trigger)==false) result=true;
 }
 return result;
}

bool IGF_Gamepad::check_trigger_release(const unsigned char trigger)
{
 bool result;
 result=false;
 if(this->check_trigger(current,trigger)==false)
 {
  if(this->check_trigger(preversion,trigger)==true) result=true;
 }
 return result;
}

bool IGF_Gamepad::set_vibration(const unsigned short int left,const unsigned short int right)
{
 this->set_motor(left,right);
 return this->write_state();
}

char IGF_Gamepad::get_stick_x(const unsigned char stick)
{
 char result;
 short int control;
 result=0;
 if(stick==IGF_GAMEPAD_LEFT_STICK)
 {
  control=32767-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
  if(current.Gamepad.sThumbLX>=control) result=1;
  if(current.Gamepad.sThumbLX<=-1*control) result=-1;
 }
 if(stick==IGF_GAMEPAD_RIGHT_STICK)
 {
  control=32767-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
  if(current.Gamepad.sThumbRX>=control) result=1;
  if(current.Gamepad.sThumbRX<=-1*control) result=-1;
 }
 return result;
}

char IGF_Gamepad::get_stick_y(const unsigned char stick)
{
 char result;
 short int control;
 result=0;
 if(stick==IGF_GAMEPAD_LEFT_STICK)
 {
  control=32767-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
  if(current.Gamepad.sThumbLY>=control) result=1;
  if(current.Gamepad.sThumbLY<=-1*control) result=-1;
 }
 if(stick==IGF_GAMEPAD_RIGHT_STICK)
 {
  control=32767-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
  if(current.Gamepad.sThumbRY>=control) result=1;
  if(current.Gamepad.sThumbRY<=-1*control) result=-1;
 }
 return result;
}

IGF_Multimedia::IGF_Multimedia()
{
 loader=NULL;
 player=NULL;
 controler=NULL;
 video=NULL;
}

IGF_Multimedia::~IGF_Multimedia()
{
 if(player!=NULL) player->StopWhenReady();
 if(video!=NULL) video->Release();
 if(controler!=NULL) controler->Release();
 if(player!=NULL) player->Release();
 if(loader!=NULL) loader->Release();
}

wchar_t *IGF_Multimedia::convert_file_name(const char *target)
{
 wchar_t *name;
 unsigned long int index,length;
 length=strlen(target);
 name=(wchar_t*)calloc(length+1,sizeof(wchar_t));
 if(name==NULL)
 {
  puts("Can't allocate memory");
  exit(EXIT_FAILURE);
 }
 for(index=0;index<length;index++) name[index]=btowc(target[index]);
 return name;
}

void IGF_Multimedia::open(const wchar_t *target)
{
 player->StopWhenReady();
 if(loader->RenderFile(target,NULL)!=S_OK)
 {
  puts("Can't load a multimedia file");
  exit(EXIT_FAILURE);
 }
 video->put_FullScreenMode(OATRUE);
}

void IGF_Multimedia::rewind()
{
 long long position;
 position=0;
 if(controler->SetPositions(&position,AM_SEEKING_AbsolutePositioning,NULL,AM_SEEKING_NoPositioning)!=S_OK)
 {
  puts("Can't set start position");
  exit(EXIT_FAILURE);
 }

}

void IGF_Multimedia::initialize()
{
 if(CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,IID_IGraphBuilder,(void**)&loader)!=S_OK)
 {
  puts("Can't create a multimedia loader");
  exit(EXIT_FAILURE);
 }
 if(loader->QueryInterface(IID_IMediaControl,(void**)&player)!=S_OK)
 {
  puts("Can't create a multimedia player");
  exit(EXIT_FAILURE);
 }
 if(loader->QueryInterface(IID_IMediaSeeking,(void**)&controler)!=S_OK)
 {
  puts("Can't create a player controler");
  exit(EXIT_FAILURE);
 }
 if(loader->QueryInterface(IID_IVideoWindow,(void**)&video)!=S_OK)
 {
  puts("Can't create a video player");
  exit(EXIT_FAILURE);
 }

}

void IGF_Multimedia::load(const char *target)
{
 wchar_t *name;
 name=this->convert_file_name(target);
 this->open(name);
 free(name);
}

bool IGF_Multimedia::is_end()
{
 bool result;
 long long current,stop;
 result=false;
 if(controler->GetPositions(&current,&stop)==S_OK)
 {
  if(current>=stop) result=true;
 }
 else
 {
  puts("Can't get the current and the end position");
  exit(EXIT_FAILURE);
 }
 return result;
}

void IGF_Multimedia::stop()
{
 player->StopWhenReady();
}

void IGF_Multimedia::play()
{
 this->stop();
 this->rewind();
 player->Run();
}

IGF_Memory::IGF_Memory()
{
 memory.dwLength=sizeof(MEMORYSTATUSEX);
}

IGF_Memory::~IGF_Memory()
{

}

unsigned long long int IGF_Memory::get_total_memory()
{
 GlobalMemoryStatusEx(&memory);
 return memory.ullTotalPhys;
}

unsigned long long int IGF_Memory::get_free_memory()
{
 GlobalMemoryStatusEx(&memory);
 return memory.ullAvailPhys;
}

IGF_System::IGF_System()
{
 srand(time(NULL));
}

IGF_System::~IGF_System()
{

}

unsigned long int IGF_System::get_random(const unsigned long int number)
{
 return rand()%number;
}

void IGF_System::quit()
{
 exit(EXIT_SUCCESS);
}

void IGF_System::run(const char *command)
{
 system(command);
}

char* IGF_System::read_environment(const char *variable)
{
 return getenv(variable);
}

void IGF_System::enable_logging(const char *name)
{
 if(freopen(name,"wt",stdout)==NULL)
 {
  puts("Can't create log file");
  exit(EXIT_FAILURE);
 }

}

IGF_Timer::IGF_Timer()
{
 interval=0;
 start=time(NULL);
}

IGF_Timer::~IGF_Timer()
{

}

void IGF_Timer::set_timer(const unsigned long int seconds)
{
 interval=seconds;
 start=time(NULL);
}

bool IGF_Timer::check_timer()
{
 bool result;
 time_t stop;
 result=false;
 stop=time(NULL);
 if(difftime(stop,start)>=interval)
 {
  result=true;
  start=time(NULL);
 }
 return result;
}

IGF_Primitive::IGF_Primitive()
{
 surface=NULL;
}

IGF_Primitive::~IGF_Primitive()
{
 surface=NULL;
}

void IGF_Primitive::initialize(IGF_Screen *Screen)
{
 surface=Screen;
}

void IGF_Primitive::draw_line(const unsigned long int x1,const unsigned long int y1,const unsigned long int x2,const unsigned long int y2,const unsigned char red,const unsigned char green,const unsigned char blue)
{
 unsigned long int delta_x,delta_y,index,steps;
 float x,y,shift_x,shift_y;
 if (x1>x2)
 {
  delta_x=x1-x2;
 }
 else
 {
  delta_x=x2-x1;
 }
 if (y1>y2)
 {
  delta_y=y1-y2;
 }
 else
 {
  delta_y=y2-y1;
 }
 steps=delta_x;
 if (steps<delta_y) steps=delta_y;
 x=x1;
 y=y1;
 shift_x=(float)delta_x/(float)steps;
 shift_y=(float)delta_y/(float)steps;
 for (index=steps;index>0;index--)
 {
  x+=shift_x;
  y+=shift_y;
  surface->draw_pixel(x,y,red,green,blue);
 }

}

void IGF_Primitive::draw_rectangle(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height,const unsigned char red,const unsigned char green,const unsigned char blue)
{
 unsigned long int stop_x,stop_y;
 stop_x=x+width;
 stop_y=y+height;
 this->draw_line(x,y,stop_x,y,red,green,blue);
 this->draw_line(x,stop_y,stop_x,stop_y,red,green,blue);
 this->draw_line(x,y,x,stop_y,red,green,blue);
 this->draw_line(stop_x,y,stop_x,stop_y,red,green,blue);
}

void IGF_Primitive::draw_filled_rectangle(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height,const unsigned char red,const unsigned char green,const unsigned char blue)
{
 unsigned long int step_x,step_y,stop_x,stop_y;
 stop_x=x+width;
 stop_y=y+height;
 for(step_x=x;step_x<stop_x;step_x++)
 {
  for(step_y=y;step_y<stop_y;step_y++)
  {
   surface->draw_pixel(step_x,step_y,red,green,blue);
  }

 }

}

IGF_Image::IGF_Image()
{
 width=0;
 height=0;
 data=NULL;
}

IGF_Image::~IGF_Image()
{
 if(data!=NULL) free(data);
}

void IGF_Image::load_tga(const char *name)
{
 FILE *target;
 unsigned long int index,position,amount,compressed_length,uncompressed_length;
 unsigned char *compressed;
 unsigned char *uncompressed;
 TGA_head head;
 TGA_map color_map;
 TGA_image image;
 target=fopen(name,"rb");
 if(target==NULL)
 {
  puts("Can't open a image file");
  exit(EXIT_FAILURE);
 }
 if(data!=NULL)
 {
  free(data);
  data=NULL;
 }
 fseek(target,0,SEEK_END);
 compressed_length=ftell(target)-18;
 rewind(target);
 fread(&head,3,1,target);
 fread(&color_map,5,1,target);
 fread(&image,10,1,target);
 if((head.color_map!=0)||(image.color!=24))
 {
  puts("Invalid image format");
  exit(EXIT_FAILURE);
 }
 if(head.type!=2)
 {
  if(head.type!=10)
  {
   puts("Invalid image format");
   exit(EXIT_FAILURE);
  }

 }
 index=0;
 position=0;
 uncompressed_length=3*(unsigned long int)image.width*(unsigned long int)image.height;
 uncompressed=(unsigned char*)calloc(uncompressed_length,1);
 if(uncompressed==NULL)
 {
  puts("Can't allocate memory for image buffer");
  exit(EXIT_FAILURE);
 }
 if(head.type==2)
 {
  fread(uncompressed,uncompressed_length,1,target);
 }
 if(head.type==10)
 {
  compressed=(unsigned char*)calloc(compressed_length,1);
  if(compressed==NULL)
  {
   puts("Can't allocate memory for image buffer");
   exit(EXIT_FAILURE);
  }
  fread(compressed,compressed_length,1,target);
  while(index<uncompressed_length)
  {
   if(compressed[position]<128)
   {
    amount=compressed[position]+1;
    amount*=3;
    memmove(uncompressed+index,compressed+(position+1),amount);
    index+=amount;
    position+=1+amount;
   }
   else
   {
    for(amount=compressed[position]-127;amount>0;amount--)
    {
     memmove(uncompressed+index,compressed+(position+1),3);
     index+=3;
    }
    position+=4;
   }

  }
  free(compressed);
 }
 fclose(target);
 width=image.width;
 height=image.height;
 data=uncompressed;
}

void IGF_Image::load_pcx(const char *name)
{
 FILE *target;
 unsigned long int x,y,index,position,line,row,length,uncompressed_length;
 unsigned char repeat;
 unsigned char *original;
 unsigned char *uncompressed;
 PCX_head head;
 target=fopen(name,"rb");
 if(target==NULL)
 {
  puts("Can't open a image file");
  exit(EXIT_FAILURE);
 }
 if(data!=NULL)
 {
  free(data);
  data=NULL;
 }
 fseek(target,0,SEEK_END);
 length=ftell(target)-128;
 rewind(target);
 fread(&head,128,1,target);
 if((head.color*head.planes!=24)&&(head.compress!=1))
 {
  puts("Incorrect image format");
  exit(EXIT_FAILURE);
 }
 width=head.max_x-head.min_x+1;
 height=head.max_y-head.min_y+1;
 row=3*width;
 line=head.planes*head.plane_length;
 uncompressed_length=row*height;
 index=0;
 position=0;
 original=(unsigned char*)calloc(length,1);
 if(original==NULL)
 {
  puts("Can't allocate memory for image buffer");
  exit(EXIT_FAILURE);
 }
 uncompressed=(unsigned char*)calloc(uncompressed_length,1);
 if(uncompressed==NULL)
 {
  puts("Can't allocate memory for image buffer");
  exit(EXIT_FAILURE);
 }
 fread(original,length,1,target);
 fclose(target);
 while (index<length)
 {
  if (original[index]<192)
  {
   uncompressed[position]=original[index];
   position++;
   index++;
  }
  else
  {
   for (repeat=original[index]-192;repeat>0;repeat--)
   {
    uncompressed[position]=original[index+1];
    position++;
   }
   index+=2;
  }

 }
 free(original);
 original=(unsigned char*)calloc(uncompressed_length,1);
 if(original==NULL)
 {
  puts("Can't allocate memory for image buffer");
  exit(EXIT_FAILURE);
 }
 for(x=0;x<width;x++)
 {
  for(y=0;y<height;y++)
  {
   index=x*3+y*row;
   position=x+y*line;
   original[index]=uncompressed[position+2*head.plane_length];
   original[index+1]=uncompressed[position+head.plane_length];
   original[index+2]=uncompressed[position];
  }

 }
 free(uncompressed);
 data=original;
}

unsigned long int IGF_Image::get_width()
{
 return width;
}

unsigned long int IGF_Image::get_height()
{
 return height;
}

unsigned long int IGF_Image::get_data_length()
{
 return width*height*3;
}

unsigned char *IGF_Image::get_data()
{
 return data;
}

void IGF_Image::destroy_image()
{
 if(data!=NULL)
 {
  width=0;
  height=0;
  free(data);
  data=NULL;
 }

}

IGF_Canvas::IGF_Canvas()
{
 image=NULL;
 surface=NULL;
 width=0;
 height=0;
 frames=1;
}

IGF_Canvas::~IGF_Canvas()
{
 surface=NULL;
 if(image!=NULL) free(image);
}

IGF_Color *IGF_Canvas::get_image()
{
 return image;
}

unsigned long int IGF_Canvas::get_width()
{
 return width;
}

unsigned long int IGF_Canvas::get_height()
{
 return height;
}

void IGF_Canvas::set_frames(const unsigned long int amount)
{
 if(amount>1) frames=amount;
}

unsigned long int IGF_Canvas::get_frames()
{
 return frames;
}

void IGF_Canvas::initialize(IGF_Screen *Screen)
{
 surface=Screen;
}

void IGF_Canvas::load_image(IGF_Image &buffer)
{
 unsigned long int length;
 width=buffer.get_width();
 height=buffer.get_height();
 length=buffer.get_data_length();
 if(image!=NULL) free(image);
 image=(IGF_Color*)calloc(length,1);
 if (image==NULL)
 {
  puts("Can't allocate memory for image buffer");
  exit(EXIT_FAILURE);
 }
 memmove(image,buffer.get_data(),length);
 buffer.destroy_image();
}

void IGF_Canvas::mirror_image(const unsigned char kind)
{
 unsigned long int x,y,index,index2;
 IGF_Color *mirrored_image;
 mirrored_image=(IGF_Color*)calloc(width*height,3);
 if (mirrored_image==NULL)
 {
  puts("Can't allocate memory for image buffer");
  exit(EXIT_FAILURE);
 }
 if (kind==0)
 {
  for (x=0;x<width;x++)
  {
   for (y=0;y<height;y++)
   {
    index=x+(y*width);
    index2=(width-x-1)+(y*width);
    mirrored_image[index]=image[index2];
   }

  }

 }
 else
 {
   for (x=0;x<width;x++)
  {
   for (y=0;y<height;y++)
   {
    index=x+(y*width);
    index2=x+(height-y-1)*width;
    mirrored_image[index]=image[index2];
   }

  }

 }
 free(image);
 image=mirrored_image;
}

void IGF_Canvas::resize_image(const unsigned long int new_width,const unsigned long int new_height)
{
 float x_ratio,y_ratio;
 unsigned long int x,y,index,index2;
 IGF_Color *scaled_image;
 scaled_image=(IGF_Color*)calloc(new_width*new_height,3);
 if (scaled_image==NULL)
 {
  puts("Can't allocate memory for image buffer");
  exit(EXIT_FAILURE);
 }
 x_ratio=(float)width/(float)new_width;
 y_ratio=(float)height/(float)new_height;
 for (x=0;x<new_width;x++)
 {
  for (y=0;y<new_height;y++)
  {
   index=x+(y*new_width);
   index2=(unsigned long int)(x_ratio*(float)x)+width*(unsigned long int)(y_ratio*(float)y);
   scaled_image[index]=image[index2];
  }

 }
 free(image);
 image=scaled_image;
 width=new_width;
 height=new_height;
}

void IGF_Background::draw_background()
{
 unsigned long int x,y,offset;
 for (x=0;x<width;x++)
 {
  for (y=0;y<height;y++)
  {
   offset=x+(width*y);
   surface->draw_pixel(x,y,image[offset].red,image[offset].green,image[offset].blue);
  }

 }

}

void IGF_Background::draw_horizontal_background(const unsigned long int frame)
{
 unsigned long int x,y,offset,start,frame_width;
 frame_width=width/frames;
 start=(frame-1)*frame_width;
 for (x=0;x<frame_width;x++)
 {
  for (y=0;y<height;y++)
  {
   offset=start+x+(width*y);
   surface->draw_pixel(x,y,image[offset].red,image[offset].green,image[offset].blue);
  }

 }

}

void IGF_Background::draw_vertical_background(const unsigned long int frame)
{
 unsigned long int x,y,offset,start,frame_height;
 frame_height=height/frames;
 start=(frame-1)*frame_height;
 for (x=0;x<width;x++)
 {
  for (y=0;y<frame_height;y++)
  {
   offset=start+x+(width*y);
   surface->draw_pixel(x,y,image[offset].red,image[offset].green,image[offset].blue);
  }

 }

}

IGF_Sprite::IGF_Sprite()
{
 current_x=0;
 current_y=0;
}

IGF_Sprite::~IGF_Sprite()
{

}

void IGF_Sprite::clone(IGF_Sprite &target)
{
 unsigned long int length;
 frames=target.get_frames();
 width=target.get_sprite_width();
 height=target.get_sprite_height();
 length=width*height*3;
 image=(IGF_Color*)calloc(length,1);
 if(image==NULL)
 {
  puts("Can't allocate memory for image buffer");
  exit(EXIT_FAILURE);
 }
 memmove(image,target.get_image(),length);
}

void IGF_Sprite::draw_sprite_frame(const unsigned long int x,const unsigned long int y,const unsigned long int frame)
{
 unsigned long int sprite_x,sprite_y,offset,start,frame_width;
 current_x=x;
 current_y=y;
 frame_width=width/frames;
 start=(frame-1)*frame_width;
 for(sprite_x=0;sprite_x<frame_width;sprite_x++)
 {
  for(sprite_y=0;sprite_y<height;sprite_y++)
  {
   offset=start+sprite_x+(sprite_y*width);
   if(memcmp(&image[0],&image[offset],3)!=0) surface->draw_pixel(x+sprite_x,y+sprite_y,image[offset].red,image[offset].green,image[offset].blue);
  }

 }

}

void IGF_Sprite::draw_sprite(const unsigned long int x,const unsigned long int y)
{
 current_x=x;
 current_y=y;
 this->draw_sprite_frame(x,y,1);
}

unsigned long int IGF_Sprite::get_x()
{
 return current_x;
}

unsigned long int IGF_Sprite::get_y()
{
 return current_y;
}

unsigned long int IGF_Sprite::get_sprite_width()
{
 return width/frames;
}

unsigned long int IGF_Sprite::get_sprite_height()
{
 return height;
}

IGF_Sprite* IGF_Sprite::get_handle()
{
 return this;
}

IGF_Box IGF_Sprite::get_box()
{
 IGF_Box target;
 target.x=current_x;
 target.y=current_y;
 target.width=width/frames;
 target.height=height;
 return target;
}

IGF_Text::IGF_Text()
{
 current_x=0;
 current_y=0;
 sprite=NULL;
}

IGF_Text::~IGF_Text()
{
 sprite=NULL;
}

void IGF_Text::set_position(const unsigned long int x,const unsigned long int y)
{
 current_x=x;
 current_y=y;
}

void IGF_Text::load_font(IGF_Sprite *font)
{
 sprite=font;
 sprite->set_frames(256);
}

void IGF_Text::draw_text(const char *text)
{
 unsigned long int index,length,step_x,step_y;
 length=strlen(text);
 step_x=current_x;
 step_y=current_y;
 for (index=0;index<length;index++)
 {
  if ((text[index]>31)||(text[index]<0))
  {
   sprite->draw_sprite_frame(step_x,step_y,(unsigned char)text[index]+1);
   step_x+=sprite->get_sprite_width();
  }

 }

}

bool IGF_Collision::check_horizontal_collision(IGF_Box first,IGF_Box second)
{
 bool result;
 result=false;
 if((first.x+first.width)>=second.x)
 {
  if(first.x<=(second.x+second.width)) result=true;
 }
 return result;
}

bool IGF_Collision::check_vertical_collision(IGF_Box first,IGF_Box second)
{
 bool result;
 result=false;
 if((first.y+first.height)>=second.y)
 {
  if(first.y<=(second.y+second.height)) result=true;
 }
 return result;
}

bool IGF_Collision::check_collision(IGF_Box first,IGF_Box second)
{
 bool result;
 result=false;
 if((this->check_horizontal_collision(first,second)==true)||(this->check_vertical_collision(first,second)==true)) result=true;
 return result;
}
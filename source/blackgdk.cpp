/*
Copyright (C) 2017-2021 Popov Evgeniy Alekseyevich

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include "blackgdk.h"

const size_t KEYBOARD=256;
const unsigned char KEY_RELEASE=0;
const unsigned char KEY_PRESS=1;

unsigned char Keys[KEYBOARD];
unsigned char Buttons[MOUSE];

LRESULT CALLBACK Process_Message(HWND window,UINT Message,WPARAM wParam,LPARAM lParam)
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
  memset(Keys,KEY_RELEASE,KEYBOARD);
  memset(Buttons,KEY_RELEASE,MOUSE);
  break;
  case WM_LBUTTONDOWN:
  Buttons[MOUSE_LEFT]=KEY_PRESS;
  break;
  case WM_LBUTTONUP:
  Buttons[MOUSE_LEFT]=KEY_RELEASE;
  break;
  case WM_RBUTTONDOWN:
  Buttons[MOUSE_RIGHT]=KEY_PRESS;
  break;
  case WM_RBUTTONUP:
  Buttons[MOUSE_RIGHT]=KEY_RELEASE;
  break;
  case WM_MBUTTONDOWN:
  Buttons[MOUSE_MIDDLE]=KEY_PRESS;
  break;
  case WM_MBUTTONUP:
  Buttons[MOUSE_MIDDLE]=KEY_RELEASE;
  break;
  case WM_KEYDOWN:
  Keys[GETSCANCODE(lParam)]=KEY_PRESS;
  break;
  case WM_KEYUP:
  Keys[GETSCANCODE(lParam)]=KEY_RELEASE;
  break;
 }
 return DefWindowProc(window,Message,wParam,lParam);
}

namespace BLACKGDK
{

void Halt(const char *message)
{
 puts(message);
 exit(EXIT_FAILURE);
}

 COM_Base::COM_Base()
{
 HRESULT status;
 status=CoInitializeEx(NULL,COINIT_APARTMENTTHREADED);
 if (status!=S_OK)
 {
  if (status!=S_FALSE)
  {
   Halt("Can't initialize COM");
  }

 }

}

COM_Base::~COM_Base()
{
 CoUninitialize();
}

Synchronization::Synchronization()
{
 timer=NULL;
}

Synchronization::~Synchronization()
{
 if (timer!=NULL)
 {
  CancelWaitableTimer(timer);
  CloseHandle(timer);
 }

}

void Synchronization::create_timer()
{
 timer=CreateWaitableTimer(NULL,FALSE,NULL);
 if (timer==NULL)
 {
  Halt("Can't create synchronization timer");
 }

}

void Synchronization::set_timer(const unsigned long int interval)
{
 LARGE_INTEGER start;
 start.QuadPart=0;
 if (SetWaitableTimer(timer,&start,interval,NULL,NULL,FALSE)==FALSE)
 {
  Halt("Can't set timer");
 }

}

void Synchronization::wait_timer()
{
 WaitForSingleObjectEx(timer,INFINITE,TRUE);
}

Engine::Engine()
{
 window_class.lpszClassName=TEXT("BLACKGDK");
 window_class.style=CS_HREDRAW|CS_VREDRAW;
 window_class.cbSize=sizeof(WNDCLASSEX);
 window_class.lpfnWndProc=Process_Message;
 window_class.hInstance=NULL;
 window_class.hbrBackground=NULL;
 window_class.hIcon=NULL;
 window_class.hCursor=NULL;
 window_class.hIconSm=NULL;
 window_class.cbClsExtra=0;
 window_class.cbWndExtra=0;
 window=NULL;
}

Engine::~Engine()
{
 if (window!=NULL) CloseWindow(window);
 UnregisterClass(window_class.lpszClassName,window_class.hInstance);
}

void Engine::get_instance()
{
 if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,NULL,&window_class.hInstance)==FALSE)
 {
  Halt("Can't get the application instance");
 }

}

void Engine::load_icon()
{
 window_class.hIcon=LoadIcon(NULL,IDI_APPLICATION);
 if (window_class.hIcon==NULL)
 {
  Halt("Can't load the standart program icon");
 }

}

void Engine::load_cursor()
{
 window_class.hCursor=LoadCursor(NULL,IDC_ARROW);
 if (window_class.hCursor==NULL)
 {
  Halt("Can't load the standart cursor");
 }

}

void Engine::register_window_class()
{
 if (!RegisterClassEx(&window_class))
 {
  Halt("Can't register window class");
 }

}

HWND Engine::get_window()
{
 return window;
}

void Engine::prepare_engine()
{
 this->get_instance();
 this->load_icon();
 this->load_cursor();
 this->register_window_class();
}

void Engine::create_window()
{
 window=CreateWindowEx(WS_EX_APPWINDOW,window_class.lpszClassName,NULL,WS_VISIBLE|WS_POPUP,0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),NULL,NULL,window_class.hInstance,NULL);
 if (window==NULL)
 {
  Halt("Can't create window");
 }
 SetFocus(window);
}

void Engine::capture_mouse()
{
 RECT border;
 if (GetClientRect(window,&border)==FALSE)
 {
  Halt("Can't capture window");
 }
 if (ClipCursor(&border)==FALSE)
 {
  Halt("Can't capture cursor");
 }

}

bool Engine::process_message()
{
 bool run;
 MSG Message;
 run=true;
 while(PeekMessage(&Message,window,0,0,PM_NOREMOVE)==TRUE)
 {
  if (GetMessage(&Message,window,0,0)==TRUE)
  {
   TranslateMessage(&Message);
   DispatchMessage(&Message);
  }
  else
  {
   run=false;
   break;
  }

 }
 return run;
}

unsigned long int Engine::get_width()
{
 return GetSystemMetrics(SM_CXSCREEN);
}

unsigned long int Engine::get_height()
{
 return GetSystemMetrics(SM_CYSCREEN);
}

Frame::Frame()
{
 frame_width=512;
 frame_height=512;
 frame_line=0;
 pixels=0;
 buffer=NULL;
 shadow=NULL;
}

Frame::~Frame()
{
 if (buffer!=NULL)
 {
  delete[] buffer;
  buffer=NULL;
 }
 if (shadow!=NULL)
 {
  delete[] shadow;
  shadow=NULL;
 }

}

void Frame::calculate_buffer_length()
{
 pixels=static_cast<size_t>(frame_width)*static_cast<size_t>(frame_height);
 frame_line=frame_width*static_cast<unsigned long int>(sizeof(unsigned int));
}

unsigned int *Frame::get_memory(const char *error)
{
 unsigned int *target;
 target=NULL;
 try
 {
  target=new unsigned int[pixels];
 }
 catch (...)
 {
  Halt(error);
 }
 return target;
}

void Frame::clear_buffer(unsigned int *target)
{
 size_t index;
 for (index=0;index<pixels;++index)
 {
  target[index]=0;
 }

}

unsigned int *Frame::create_buffer(const char *error)
{
 unsigned int *target;
 target=NULL;
 this->calculate_buffer_length();
 target=this->get_memory(error);
 this->clear_buffer(target);
 return target;
}

size_t Frame::get_offset(const unsigned long int x,const unsigned long int y,const unsigned long int target_width)
{
 return static_cast<size_t>(x)+static_cast<size_t>(y)*static_cast<size_t>(target_width);
}

size_t Frame::get_offset(const unsigned long int x,const unsigned long int y) const
{
 return static_cast<size_t>(x)+static_cast<size_t>(y)*static_cast<size_t>(frame_width);
}

void Frame::set_size(const unsigned long int surface_width,const unsigned long int surface_height)
{
 frame_width=surface_width;
 frame_height=surface_height;
}

void Frame::set_size(const SURFACE surface)
{
 if (surface==SURFACE_SMALL) this->set_size(256,256);
 if (surface==SURFACE_LARGE) this->set_size(512,512);
}

void Frame::create_buffers()
{
 buffer=this->create_buffer("Can't allocate memory for render buffer");
 shadow=this->create_buffer("Can't allocate memory for shadow buffer");
}

unsigned long int Frame::get_frame_line() const
{
 return frame_line;
}

unsigned int *Frame::get_buffer()
{
 return buffer;
}

size_t Frame::get_pixels() const
{
 return pixels;
}

bool Frame::draw_pixel(const unsigned long int x,const unsigned long int y,const unsigned long int red,const unsigned long int green,const unsigned long int blue)
{
 bool result;
 size_t offset;
 result=false;
 offset=static_cast<size_t>(x)+static_cast<size_t>(y)*static_cast<size_t>(frame_width);
 if (offset<pixels)
 {
  buffer[offset]=blue+(green<<8)+(red<<16);
  result=true;
 }
 return result;
}

void Frame::clear_screen()
{
 this->clear_buffer(buffer);
}

void Frame::save()
{
 size_t index;
 for (index=0;index<pixels;++index)
 {
  shadow[index]=buffer[index];
 }

}

void Frame::restore()
{
 size_t index;
 for (index=0;index<pixels;++index)
 {
  buffer[index]=shadow[index];
 }

}

void Frame::restore(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height)
{
 unsigned long int target_x,target_y,stop_x,stop_y;
 size_t position;
 stop_x=x+width;
 stop_y=y+height;
 if ((x<frame_width)&&(y<frame_height))
 {
  if ((stop_x<=frame_width)&&(stop_y<=frame_height))
  {
   for (target_x=x;target_x<stop_x;++target_x)
   {
    for (target_y=y;target_y<stop_y;++target_y)
    {
     position=this->get_offset(target_x,target_y);
     buffer[position]=shadow[position];
    }

   }

  }

 }

}

unsigned long int Frame::get_frame_width() const
{
 return frame_width;
}

unsigned long int Frame::get_frame_height() const
{
 return frame_height;
}

Plane::Plane()
{
 target=NULL;
 plane=NULL;
 target_width=0;
 target_height=0;
 x_ratio=0;
 y_ratio=0;
}

Plane::~Plane()
{

}

void Plane::create_plane(const unsigned long int width,const unsigned long int height,const unsigned long int surface_width,const unsigned long int surface_height,unsigned int *surface_buffer)
{
 this->set_size(width,height);
 this->create_buffers();
 plane=this->get_buffer();
 target=surface_buffer;
 target_width=surface_width;
 target_height=surface_height;
 x_ratio=static_cast<float>(width)/static_cast<float>(surface_width);
 y_ratio=static_cast<float>(height)/static_cast<float>(surface_height);
}

void Plane::transfer()
{
 unsigned long int x,y,width;
 size_t index,position;
 width=this->get_frame_width();
 for (x=0;x<target_width;++x)
 {
  for (y=0;y<target_height;++y)
  {
   index=this->get_offset(x,y,target_width);
   position=this->get_offset((x_ratio*static_cast<float>(x)),(y_ratio*static_cast<float>(y)),width);
   target[index]=plane[position];
  }

 }

}

Plane* Plane::get_handle()
{
 return this;
}

Timer::Timer()
{
 interval=0;
 start=time(NULL);
}

Timer::~Timer()
{

}

void Timer::set_timer(const unsigned long int seconds)
{
 interval=seconds;
 start=time(NULL);
}

bool Timer::check_timer()
{
 bool result;
 result=false;
 if (difftime(time(NULL),start)>=interval)
 {
  result=true;
  start=time(NULL);
 }
 return result;
}

FPS::FPS()
{
 timer.set_timer(1);
 current=0;
 fps=0;
}

FPS::~FPS()
{

}

void FPS::update_counter()
{
 ++current;
 if (timer.check_timer()==true)
 {
  fps=current;
  current=0;
 }

}

unsigned long int FPS::get_fps() const
{
 return fps;
}

Unicode_Convertor::Unicode_Convertor()
{
 target=NULL;
}

Unicode_Convertor::~Unicode_Convertor()
{
 if (target!=NULL) delete[] target;
}

void Unicode_Convertor::get_memory(const size_t length)
{
 try
 {
  target=new wchar_t[length+1];
 }
 catch (...)
 {
  Halt("Can't allocate memory");
 }

}

void Unicode_Convertor::clear_buffer(const size_t length)
{
 size_t index,stop;
 stop=length+1;
 for (index=0;index<stop;++index)
 {
  target[index]=0;
 }

}

void Unicode_Convertor::create_buffer(const size_t length)
{
 this->get_memory(length);
 this->clear_buffer(length);
}

void Unicode_Convertor::convert_string(const char *source)
{
 size_t index,length;
 length=strlen(source);
 for (index=0;index<length;++index)
 {
  target[index]=btowc(source[index]);
 }

}

wchar_t *Unicode_Convertor::convert(const char *source)
{
 this->create_buffer(strlen(source));
 this->convert_string(source);
 return target;
}

Render::Render()
{
 render=NULL;
 target=NULL;
 surface=NULL;
 source=D2D1::RectU(0,0,0,0);
 destanation=D2D1::RectF(0,0,0,0);
 texture=D2D1::RectF(0,0,0,0);
 setting.dpiX=0;
 setting.dpiY=0;
 setting.type=D2D1_RENDER_TARGET_TYPE_HARDWARE;
 setting.pixelFormat=D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_IGNORE);
 setting.usage=D2D1_RENDER_TARGET_USAGE_NONE;
 setting.minLevel=D2D1_FEATURE_LEVEL_9;
 configuration.hwnd=NULL;
 configuration.pixelSize=D2D1::SizeU(0,0);
 configuration.presentOptions=D2D1_PRESENT_OPTIONS_IMMEDIATELY;
}

Render::~Render()
{
 if (surface!=NULL) surface->Release();
 if (target!=NULL) target->Release();
 if (render!=NULL) render->Release();
}

void Render::create_factory()
{
 if (D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,&render)!=S_OK)
 {
  Halt("Can't create render");
 }

}

void Render::create_target()
{
 if (render->CreateHwndRenderTarget(setting,configuration,&target)!=S_OK)
 {
  Halt("Can't create render target");
 }
 target->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
}

void Render::create_surface()
{
 if (target->CreateBitmap(D2D1::SizeU(this->get_frame_width(),this->get_frame_height()),this->get_buffer(),this->get_frame_line(),D2D1::BitmapProperties(setting.pixelFormat,96.0,96.0),&surface)!=S_OK)
 {
  Halt("Can't create render surface");
 }

}

void Render::set_render_setting()
{
 configuration.hwnd=this->get_window();
 configuration.pixelSize=D2D1::SizeU(this->get_width(),this->get_height());
}

void Render::set_render()
{
 this->set_render_setting();
 this->create_factory();
 this->create_target();
 this->create_surface();
}

void Render::destroy_resource()
{
 if (surface!=NULL)
 {
  surface->Release();
  surface=NULL;
 }
 if (target!=NULL)
 {
  target->Release();
  target=NULL;
 }

}

void Render::recreate_render()
{
 this->destroy_resource();
 this->create_target();
 this->create_surface();
}

void Render::prepare_surface()
{
 source=D2D1::RectU(0,0,this->get_frame_width(),this->get_frame_height());
 destanation=D2D1::RectF(0,0,this->get_width(),this->get_height());
 texture=D2D1::RectF(0,0,this->get_frame_width(),this->get_frame_height());
}

void Render::create_render()
{
 this->set_render_setting();
 this->set_render();
 this->prepare_surface();
 this->create_buffers();
}

void Render::refresh()
{
 surface->CopyFromMemory(&source,this->get_buffer(),this->get_frame_line());
 target->BeginDraw();
 target->DrawBitmap(surface,destanation,1.0,D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,texture);
 if (target->EndDraw()==(HRESULT)D2DERR_RECREATE_TARGET) this->recreate_render();
}

Screen::Screen()
{

}

Screen::~Screen()
{

}

void Screen::initialize()
{
 this->prepare_engine();
 this->create_buffers();
 this->create_timer();
 this->create_window();
 this->capture_mouse();
 this->create_render();
 this->set_timer(17);
}

void Screen::initialize(const SURFACE surface)
{
 this->set_size(surface);
 this->initialize();
}

bool Screen::update()
{
 this->refresh();
 this->update_counter();
 return this->process_message();
}

bool Screen::sync()
{
 bool run;
 run=this->update();
 this->wait_timer();
 return run;
}

Screen* Screen::get_handle()
{
 return this;
}

Keyboard::Keyboard()
{
 preversion=NULL;
}

Keyboard::~Keyboard()
{
 if (preversion!=NULL)
 {
  delete[] preversion;
  preversion=NULL;
 }

}

void Keyboard::create_buffer()
{
 try
 {
  preversion=new unsigned char[KEYBOARD];
 }
 catch (...)
 {
  Halt("Can't allocate memory for keyboard state buffer");
 }

}

void Keyboard::clear_buffer()
{
 size_t index;
 for (index=0;index<KEYBOARD;++index)
 {
  preversion[index]=KEY_RELEASE;
 }

}

bool Keyboard::check_state(const unsigned char code,const unsigned char state)
{
 bool result;
 result=false;
 if (Keys[code]==state)
 {
  if (preversion[code]!=state) result=true;
 }
 preversion[code]=Keys[code];
 return result;
}

void Keyboard::initialize()
{
 this->create_buffer();
 this->clear_buffer();
}

bool Keyboard::check_hold(const unsigned char code)
{
 bool result;
 result=false;
 if (Keys[code]==KEY_PRESS) result=true;
 preversion[code]=Keys[code];
 return result;
}

bool Keyboard::check_press(const unsigned char code)
{
 return this->check_state(code,KEY_PRESS);
}

bool Keyboard::check_release(const unsigned char code)
{
 return this->check_state(code,KEY_RELEASE);
}

Mouse::Mouse()
{
 preversion[MOUSE_LEFT]=KEY_RELEASE;
 preversion[MOUSE_RIGHT]=KEY_RELEASE;
 preversion[MOUSE_MIDDLE]=KEY_RELEASE;
 position.x=0;
 position.y=0;
}

Mouse::~Mouse()
{
 while(ShowCursor(TRUE)<1) ;
}

void Mouse::get_position()
{
 if (GetCursorPos(&position)==FALSE)
 {
  Halt("Can't get the mouse cursor position");
 }

}

bool Mouse::check_state(const MOUSE_BUTTON button,const unsigned char state)
{
 bool result;
 result=false;
 if (Buttons[button]==state)
 {
  if (preversion[button]!=state) result=true;
 }
 preversion[button]=Buttons[button];
 return result;
}

void Mouse::show()
{
 while(ShowCursor(TRUE)<1) ;
}

void Mouse::hide()
{
 while(ShowCursor(FALSE)>-2) ;
}

void Mouse::set_position(const unsigned long int x,const unsigned long int y)
{
 if (SetCursorPos(x,y)==FALSE)
 {
  Halt("Can't set the mouse cursor position");
 }

}

unsigned long int Mouse::get_x()
{
 this->get_position();
 return position.x;
}

unsigned long int Mouse::get_y()
{
 this->get_position();
 return position.y;
}

bool Mouse::check_hold(const MOUSE_BUTTON button)
{
 bool result;
 result=false;
 if (Buttons[button]==KEY_PRESS)
 {
  result=true;
 }
 preversion[button]=Buttons[button];
 return result;
}

bool Mouse::check_press(const MOUSE_BUTTON button)
{
 return this->check_state(button,KEY_PRESS);
}

bool Mouse::check_release(const MOUSE_BUTTON button)
{
 return this->check_state(button,KEY_RELEASE);
}

Gamepad::Gamepad()
{
 length=sizeof(XINPUT_STATE);
 XInputEnable(TRUE);
 memset(&current,0,length);
 memset(&preversion,0,length);
 memset(&vibration,0,sizeof(XINPUT_VIBRATION));
 memset(&battery,0,sizeof(XINPUT_BATTERY_INFORMATION));
 active=0;
}

Gamepad::~Gamepad()
{
 XInputEnable(FALSE);
}

bool Gamepad::read_battery_status()
{
 return XInputGetBatteryInformation(active,BATTERY_DEVTYPE_GAMEPAD,&battery)==ERROR_SUCCESS;
}

void Gamepad::clear_state()
{
 memset(&current,0,length);
 memset(&preversion,0,length);
}

bool Gamepad::read_state()
{
 return XInputGetState(active,&current)==ERROR_SUCCESS;
}

bool Gamepad::write_state()
{
 return XInputSetState(active,&vibration)==ERROR_SUCCESS;
}

void Gamepad::set_motor(const unsigned short int left,const unsigned short int right)
{
 vibration.wLeftMotorSpeed=left;
 vibration.wRightMotorSpeed=right;
}

bool Gamepad::check_button(XINPUT_STATE &target,const GAMEPAD_BUTTONS button)
{
 bool result;
 result=false;
 if (target.Gamepad.wButtons&button) result=true;
 return result;
}

bool Gamepad::check_trigger(XINPUT_STATE &target,const GAMEPAD_TRIGGERS trigger)
{
 bool result;
 result=false;
 if (trigger==GAMEPAD_LEFT_TRIGGER)
 {
  if (target.Gamepad.bLeftTrigger>=XINPUT_GAMEPAD_TRIGGER_THRESHOLD) result=true;
 }
 else
 {
  if (target.Gamepad.bRightTrigger>=XINPUT_GAMEPAD_TRIGGER_THRESHOLD) result=true;
 }
 return result;
}

void Gamepad::set_active(const unsigned long int gamepad)
{
 if (active<XUSER_MAX_COUNT)
 {
  this->clear_state();
  active=gamepad;
 }

}

unsigned long int Gamepad::get_active() const
{
 return active;
}

unsigned long int Gamepad::get_maximum_amount()
{
 return XUSER_MAX_COUNT;
}

unsigned long int Gamepad::get_amount()
{
 unsigned long int old,result;
 result=0;
 old=active;
 for (active=0;active<XUSER_MAX_COUNT;++active)
 {
  if (this->read_state()==true)
  {
   this->clear_state();
   result=active+1;
  }

 }
 active=old;
 return result;
}

unsigned long int Gamepad::get_last_index()
{
 unsigned long int last_index;
 last_index=this->get_amount();
 if (last_index>0) --last_index;
 return last_index;
}

bool Gamepad::check_connection()
{
 return this->read_state();
}

bool Gamepad::is_wireless()
{
 bool result;
 result=false;
 if (this->read_battery_status()==true)
 {
  if (battery.BatteryType!=BATTERY_TYPE_WIRED) result=true;
 }
 return result;
}

GAMEPAD_BATTERY_TYPE Gamepad::get_battery_type()
{
 GAMEPAD_BATTERY_TYPE result;
 result=GAMEPAD_BATTERY_TYPE_ERROR;
 if (this->read_battery_status()==true)
 {
  switch (battery.BatteryType)
  {
   case BATTERY_TYPE_ALKALINE:
   result=GAMEPAD_BATTERY_ALKAINE;
   break;
   case BATTERY_TYPE_NIMH:
   result=GAMEPAD_BATTERY_NIMH;
   break;
   case BATTERY_TYPE_UNKNOWN:
   result=GAMEPAD_BATTERY_UNKNOW;
   break;
  }

 }
 return result;
}

GAMEPAD_BATTERY_LEVEL Gamepad::get_battery_level()
{
 GAMEPAD_BATTERY_LEVEL result;
 result=GAMEPAD_BATTERY_LEVEL_ERROR;
 if (this->read_battery_status()==true)
 {
  switch (battery.BatteryLevel)
  {
   case BATTERY_LEVEL_EMPTY:
   result=GAMEPAD_BATTERY_EMPTY;
   break;
   case BATTERY_LEVEL_LOW:
   result=GAMEPAD_BATTERY_LOW;
   break;
   case BATTERY_LEVEL_MEDIUM:
   result=GAMEPAD_BATTERY_MEDIUM;
   break;
   case BATTERY_LEVEL_FULL:
   result=GAMEPAD_BATTERY_FULL;
   break;
  }
  if ((battery.BatteryType==BATTERY_TYPE_WIRED)||(battery.BatteryType==BATTERY_TYPE_DISCONNECTED)) result=GAMEPAD_BATTERY_LEVEL_ERROR;
 }
 return result;
}

void Gamepad::update()
{
 preversion=current;
 if (this->read_state()==false) this->clear_state();
}

bool Gamepad::check_button_hold(const GAMEPAD_BUTTONS button)
{
 return this->check_button(current,button);
}

bool Gamepad::check_button_press(const GAMEPAD_BUTTONS button)
{
 bool result;
 result=false;
 if (this->check_button(current,button)==true)
 {
  if (this->check_button(preversion,button)==false) result=true;
 }
 return result;
}

bool Gamepad::check_button_release(const GAMEPAD_BUTTONS button)
{
 bool result;
 result=false;
 if (this->check_button(current,button)==false)
 {
  if (this->check_button(preversion,button)==true) result=true;
 }
 return result;
}

bool Gamepad::check_trigger_hold(const GAMEPAD_TRIGGERS trigger)
{
 return this->check_trigger(current,trigger);
}

bool Gamepad::check_trigger_press(const GAMEPAD_TRIGGERS trigger)
{
 bool result;
 result=false;
 if (this->check_trigger(current,trigger)==true)
 {
  if (this->check_trigger(preversion,trigger)==false) result=true;
 }
 return result;
}

bool Gamepad::check_trigger_release(const GAMEPAD_TRIGGERS trigger)
{
 bool result;
 result=false;
 if (this->check_trigger(current,trigger)==false)
 {
  if (this->check_trigger(preversion,trigger)==true) result=true;
 }
 return result;
}

unsigned char Gamepad::get_trigger(const GAMEPAD_TRIGGERS trigger) const
{
 unsigned char result;
 result=0;
 if (trigger==GAMEPAD_LEFT_TRIGGER) result=current.Gamepad.bLeftTrigger;
 if (trigger==GAMEPAD_RIGHT_TRIGGER) result=current.Gamepad.bRightTrigger;
 return result;
}

unsigned char Gamepad::get_left_trigger() const
{
 return current.Gamepad.bLeftTrigger;
}

unsigned char Gamepad::get_right_trigger() const
{
 return current.Gamepad.bRightTrigger;
}

bool Gamepad::set_vibration(const unsigned short int left,const unsigned short int right)
{
 this->set_motor(left,right);
 return this->write_state();
}

bool Gamepad::disable_vibration()
{
 return this->set_vibration(0,0);
}

GAMEPAD_DIRECTION Gamepad::get_stick_x(const GAMEPAD_STICKS stick) const
{
 GAMEPAD_DIRECTION result;
 short int control;
 result=GAMEPAD_NEUTRAL_DIRECTION;
 if (stick==GAMEPAD_LEFT_STICK)
 {
  control=SHRT_MAX-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
  if (current.Gamepad.sThumbLX>=control) result=GAMEPAD_POSITIVE_DIRECTION;
  if (current.Gamepad.sThumbLX<=-1*control) result=GAMEPAD_NEGATIVE_DIRECTION;
 }
 if (stick==GAMEPAD_RIGHT_STICK)
 {
  control=SHRT_MAX-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
  if (current.Gamepad.sThumbRX>=control) result=GAMEPAD_POSITIVE_DIRECTION;
  if (current.Gamepad.sThumbRX<=-1*control) result=GAMEPAD_NEGATIVE_DIRECTION;
 }
 return result;
}

GAMEPAD_DIRECTION Gamepad::get_stick_y(const GAMEPAD_STICKS stick) const
{
 GAMEPAD_DIRECTION result;
 short int control;
 result=GAMEPAD_NEUTRAL_DIRECTION;
 if (stick==GAMEPAD_LEFT_STICK)
 {
  control=SHRT_MAX-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
  if (current.Gamepad.sThumbLY>=control) result=GAMEPAD_POSITIVE_DIRECTION;
  if (current.Gamepad.sThumbLY<=-1*control) result=GAMEPAD_NEGATIVE_DIRECTION;
 }
 if (stick==GAMEPAD_RIGHT_STICK)
 {
  control=SHRT_MAX-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
  if (current.Gamepad.sThumbRY>=control) result=GAMEPAD_POSITIVE_DIRECTION;
  if (current.Gamepad.sThumbRY<=-1*control) result=GAMEPAD_NEGATIVE_DIRECTION;
 }
 return result;
}

Multimedia::Multimedia()
{
 loader=NULL;
 player=NULL;
 controler=NULL;
 video=NULL;
}

Multimedia::~Multimedia()
{
 if (player!=NULL) player->StopWhenReady();
 if (video!=NULL) video->Release();
 if (controler!=NULL) controler->Release();
 if (player!=NULL) player->Release();
 if (loader!=NULL) loader->Release();
}

void Multimedia::open(const wchar_t *target)
{
 player->StopWhenReady();
 if (loader->RenderFile(target,NULL)!=S_OK)
 {
  Halt("Can't load a multimedia file");
 }
 video->put_FullScreenMode(OATRUE);
}

bool Multimedia::is_play()
{
 bool result;
 long long current,total;
 result=false;
 if (controler->GetPositions(&current,&total)==S_OK)
 {
  if (current<total) result=true;
 }
 return result;
}

void Multimedia::rewind()
{
 long long position;
 position=0;
 if (controler->SetPositions(&position,AM_SEEKING_AbsolutePositioning,NULL,AM_SEEKING_NoPositioning)!=S_OK)
 {
  Halt("Can't set start position");
 }

}

void Multimedia::create_loader()
{
 if (CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,IID_IGraphBuilder,reinterpret_cast<void**>(&loader))!=S_OK)
 {
  Halt("Can't create a multimedia loader");
 }

}

void Multimedia::create_player()
{
 if (loader->QueryInterface(IID_IMediaControl,reinterpret_cast<void**>(&player))!=S_OK)
 {
  Halt("Can't create a multimedia player");
 }

}

void Multimedia::create_controler()
{
 if (loader->QueryInterface(IID_IMediaSeeking,reinterpret_cast<void**>(&controler))!=S_OK)
 {
  Halt("Can't create a player controler");
 }

}

void Multimedia::create_video_player()
{
 if (loader->QueryInterface(IID_IVideoWindow,reinterpret_cast<void**>(&video))!=S_OK)
 {
  Halt("Can't create a video player");
 }

}

void Multimedia::initialize()
{
 this->create_loader();
 this->create_player();
 this->create_controler();
 this->create_video_player();
}

void Multimedia::load(const char *target)
{
 Unicode_Convertor convertor;
 this->open(convertor.convert(target));
}

bool Multimedia::check_playing()
{
 OAFilterState state;
 bool result;
 result=false;
 if (player->GetState(INFINITE,&state)!=E_FAIL)
 {
  if (state==State_Running) result=this->is_play();
 }
 return result;
}

void Multimedia::stop()
{
 player->StopWhenReady();
}

void Multimedia::play()
{
 this->stop();
 this->rewind();
 player->Run();
}

Memory::Memory()
{
 memset(&memory,0,sizeof(MEMORYSTATUSEX));
 memory.dwLength=sizeof(MEMORYSTATUSEX);
}

Memory::~Memory()
{

}

void Memory::get_status()
{
 if (GlobalMemoryStatusEx(&memory)==FALSE)
 {
  Halt("Can't get the memory status");
 }

}

unsigned long long int Memory::get_total_physical()
{
 this->get_status();
 return memory.ullTotalPhys;
}

unsigned long long int Memory::get_free_physical()
{
 this->get_status();
 return memory.ullAvailPhys;
}

unsigned long long int Memory::get_total_virtual()
{
 this->get_status();
 return memory.ullTotalVirtual;
}

unsigned long long int Memory::get_free_virtual()
{
 this->get_status();
 return memory.ullAvailVirtual;
}

unsigned long int Memory::get_usage()
{
 this->get_status();
 return memory.dwMemoryLoad;
}

System::System()
{
 srand(UINT_MAX);
}

System::~System()
{

}

unsigned long int System::get_random(const unsigned long int number)
{
 return rand()%number;
}

void System::quit()
{
 exit(EXIT_SUCCESS);
}

void System::run(const char *command)
{
 system(command);
}

char* System::read_environment(const char *variable)
{
 return getenv(variable);
}

void System::enable_logging(const char *name)
{
 if (freopen(name,"wt",stdout)==NULL)
 {
  Halt("Can't create log file");
 }

}

Filesystem::Filesystem()
{
 status=false;
}

Filesystem::~Filesystem()
{

}

void Filesystem::file_exist(const char *name)
{
 FILE *target;
 status=false;
 target=fopen(name,"rb");
 if (target!=NULL)
 {
  status=true;
  fclose(target);
 }

}

void Filesystem::delete_file(const char *name)
{
 status=(remove(name)==0);
}

bool Filesystem::get_status() const
{
 return status;
}

Binary_File::Binary_File()
{
 target=NULL;
}

Binary_File::~Binary_File()
{
 if (target!=NULL)
 {
  fclose(target);
  target=NULL;
 }

}

void Binary_File::open_file(const char *name,const char *mode)
{
 target=fopen(name,mode);
 if (target==NULL)
 {
  Halt("Can't open the binary file");
 }

}

void Binary_File::close()
{
 if (target!=NULL)
 {
  fclose(target);
  target=NULL;
 }

}

void Binary_File::set_position(const long int offset)
{
 fseek(target,offset,SEEK_SET);
}

long int Binary_File::get_position()
{
 return ftell(target);
}

long int Binary_File::get_length()
{
 long int result;
 fseek(target,0,SEEK_END);
 result=ftell(target);
 rewind(target);
 return result;
}

bool Binary_File::check_error()
{
 return ferror(target)!=0;
}

Input_File::Input_File()
{

}

Input_File::~Input_File()
{

}

void Input_File::open(const char *name)
{
 this->close();
 this->open_file(name,"rb");
}

void Input_File::read(void *buffer,const size_t length)
{
 fread(buffer,sizeof(char),length,target);
}

Output_File::Output_File()
{

}

Output_File::~Output_File()
{

}

void Output_File::open(const char *name)
{
 this->close();
 this->open_file(name,"wb");
}

void Output_File::create_temp()
{
 this->close();
 target=tmpfile();
 if (target==NULL)
 {
  Halt("Can't create a temporary file");
 }

}

void Output_File::write(void *buffer,const size_t length)
{
 fwrite(buffer,sizeof(char),length,target);
}

void Output_File::flush()
{
 fflush(target);
}

Primitive::Primitive()
{
 color.red=0;
 color.green=0;
 color.blue=0;
 surface=NULL;
}

Primitive::~Primitive()
{

}

void Primitive::initialize(Screen *screen)
{
 surface=screen;
}

void Primitive::set_color(const unsigned char red,const unsigned char green,const unsigned char blue)
{
 color.red=red;
 color.green=green;
 color.blue=blue;
}

void Primitive::draw_line(const unsigned long int x1,const unsigned long int y1,const unsigned long int x2,const unsigned long int y2)
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
 shift_x=static_cast<float>(delta_x)/static_cast<float>(steps);
 shift_y=static_cast<float>(delta_y)/static_cast<float>(steps);
 for (index=steps;index>0;--index)
 {
  x+=shift_x;
  y+=shift_y;
  surface->draw_pixel(x,y,color.red,color.green,color.blue);
 }

}

void Primitive::draw_rectangle(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height)
{
 unsigned long int stop_x,stop_y;
 stop_x=x+width;
 stop_y=y+height;
 this->draw_line(x,y,stop_x,y);
 this->draw_line(x,stop_y,stop_x,stop_y);
 this->draw_line(x,y,x,stop_y);
 this->draw_line(stop_x,y,stop_x,stop_y);
}

void Primitive::draw_filled_rectangle(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height)
{
 unsigned long int step_x,step_y,stop_x,stop_y;
 stop_x=x+width;
 stop_y=y+height;
 for (step_x=x;step_x<stop_x;++step_x)
 {
  for (step_y=y;step_y<stop_y;++step_y)
  {
   surface->draw_pixel(step_x,step_y,color.red,color.green,color.blue);
  }

 }

}

Image::Image()
{
 width=0;
 height=0;
 data=NULL;
}

Image::~Image()
{
 if (data!=NULL)
 {
  delete[] data;
  data=NULL;
 }

}

unsigned char *Image::create_buffer(const size_t length)
{
 unsigned char *result;
 result=NULL;
 try
 {
  result=new unsigned char[length];
 }
 catch (...)
 {
  Halt("Can't allocate memory for image buffer");
 }
 return result;
}

void Image::clear_buffer()
{
 if (data!=NULL)
 {
  delete[] data;
  data=NULL;
 }

}

void Image::load_tga(const char *name)
{
 Input_File target;
 size_t index,position,amount,compressed_length,uncompressed_length;
 unsigned char *compressed;
 unsigned char *uncompressed;
 TGA_head head;
 TGA_map color_map;
 TGA_image image;
 this->clear_buffer();
 target.open(name);
 compressed_length=static_cast<size_t>(target.get_length()-18);
 target.read(&head,3);
 target.read(&color_map,5);
 target.read(&image,10);
 if ((head.color_map!=0)||(image.color!=24))
 {
  Halt("Invalid image format");
 }
 if (head.type!=2)
 {
  if (head.type!=10)
  {
   Halt("Invalid image format");
  }

 }
 index=0;
 position=0;
 width=image.width;
 height=image.height;
 uncompressed_length=this->get_length();
 uncompressed=this->create_buffer(uncompressed_length);
 if (head.type==2)
 {
  target.read(uncompressed,uncompressed_length);
 }
 if (head.type==10)
 {
  compressed=this->create_buffer(compressed_length);
  target.read(compressed,compressed_length);
  while(index<uncompressed_length)
  {
   if (compressed[position]<128)
   {
    amount=compressed[position]+1;
    amount*=3;
    memmove(uncompressed+index,compressed+(position+1),amount);
    index+=amount;
    position+=1+amount;
   }
   else
   {
    for (amount=compressed[position]-127;amount>0;--amount)
    {
     memmove(uncompressed+index,compressed+(position+1),3);
     index+=3;
    }
    position+=4;
   }

  }
  delete[] compressed;
 }
 target.close();
 data=uncompressed;
}

void Image::load_pcx(const char *name)
{
 Input_File target;
 unsigned long int x,y;
 size_t index,position,line,row,length,uncompressed_length;
 unsigned char repeat;
 unsigned char *original;
 unsigned char *uncompressed;
 PCX_head head;
 this->clear_buffer();
 target.open(name);
 length=static_cast<size_t>(target.get_length()-128);
 target.read(&head,128);
 if ((head.color*head.planes!=24)&&(head.compress!=1))
 {
  Halt("Incorrect image format");
 }
 width=head.max_x-head.min_x+1;
 height=head.max_y-head.min_y+1;
 row=static_cast<size_t>(width)*3;
 line=static_cast<size_t>(head.planes)*static_cast<size_t>(head.plane_length);
 uncompressed_length=row*height;
 index=0;
 position=0;
 original=this->create_buffer(length);
 uncompressed=this->create_buffer(uncompressed_length);
 target.read(original,length);
 target.close();
 while (index<length)
 {
  if (original[index]<192)
  {
   uncompressed[position]=original[index];
   ++position;
   ++index;
  }
  else
  {
   for (repeat=original[index]-192;repeat>0;--repeat)
   {
    uncompressed[position]=original[index+1];
    ++position;
   }
   index+=2;
  }

 }
 delete[] original;
 original=this->create_buffer(uncompressed_length);
 for (x=0;x<width;++x)
 {
  for (y=0;y<height;++y)
  {
   index=static_cast<size_t>(x)*3+static_cast<size_t>(y)*row;
   position=static_cast<size_t>(x)+static_cast<size_t>(y)*line;
   original[index]=uncompressed[position+2*static_cast<size_t>(head.plane_length)];
   original[index+1]=uncompressed[position+static_cast<size_t>(head.plane_length)];
   original[index+2]=uncompressed[position];
  }

 }
 delete[] uncompressed;
 data=original;
}

unsigned long int Image::get_width() const
{
 return width;
}

unsigned long int Image::get_height() const
{
 return height;
}

size_t Image::get_length() const
{
 return static_cast<size_t>(width)*static_cast<size_t>(height)*3;
}

unsigned char *Image::get_data()
{
 return data;
}

void Image::destroy_image()
{
 width=0;
 height=0;
 this->clear_buffer();
}

Surface::Surface()
{
 width=0;
 height=0;
 image=NULL;
 surface=NULL;
}

Surface::~Surface()
{
 surface=NULL;
 if (image!=NULL) free(image);
}

IMG_Pixel *Surface::create_buffer(const unsigned long int image_width,const unsigned long int image_height)
{
 IMG_Pixel *result;
 size_t length;
 length=static_cast<size_t>(image_width)*static_cast<size_t>(image_height);
 result=reinterpret_cast<IMG_Pixel*>(calloc(length,3));
 if (result==NULL)
 {
  Halt("Can't allocate memory for image buffer");
 }
 return result;
}

void Surface::save()
{
 surface->save();
}

void Surface::restore()
{
 surface->restore();
}

void Surface::clear_buffer()
{
 if (image!=NULL)
 {
  free(image);
  image=NULL;
 }

}

void Surface::load_from_buffer(Image &buffer)
{
 width=buffer.get_width();
 height=buffer.get_height();
 this->clear_buffer();
 image=this->create_buffer(width,height);
 memmove(image,buffer.get_data(),buffer.get_length());
}

void Surface::set_width(const unsigned long int image_width)
{
 width=image_width;
}

void Surface::set_height(const unsigned long int image_height)
{
 height=image_height;
}

void Surface::set_buffer(IMG_Pixel *buffer)
{
 image=buffer;
}

size_t Surface::get_offset(const unsigned long int start,const unsigned long int x,const unsigned long int y,const unsigned long int target_width)
{
 return static_cast<size_t>(start)+static_cast<size_t>(x)+static_cast<size_t>(y)*static_cast<size_t>(target_width);
}

size_t Surface::get_offset(const unsigned long int start,const unsigned long int x,const unsigned long int y) const
{
 return static_cast<size_t>(start)+static_cast<size_t>(x)+static_cast<size_t>(y)*static_cast<size_t>(width);
}

void Surface::draw_image_pixel(const size_t offset,const unsigned long int x,const unsigned long int y)
{
 surface->draw_pixel(x,y,image[offset].red,image[offset].green,image[offset].blue);
}

bool Surface::compare_pixels(const size_t first,const size_t second) const
{
 bool result;
 result=false;
 if (image[first].red!=image[second].red)
 {
  result=true;
  goto finish;
 }
 if (image[first].green!=image[second].green)
 {
  result=true;
  goto finish;
 }
 if (image[first].blue!=image[second].blue)
 {
  result=true;
  goto finish;
 }
 finish: ;
 return result;
}

unsigned long int Surface::get_surface_width() const
{
 return surface->get_frame_width();
}

unsigned long int Surface::get_surface_height() const
{
 return surface->get_frame_width();
}

void Surface::initialize(Screen *screen)
{
 surface=screen;
}

size_t Surface::get_length() const
{
 return static_cast<size_t>(width)*static_cast<size_t>(height)*3;
}

IMG_Pixel *Surface::get_image()
{
 return image;
}

unsigned long int Surface::get_image_width() const
{
 return width;
}

unsigned long int Surface::get_image_height() const
{
 return height;
}

void Surface::mirror_image(const MIRROR_TYPE kind)
{
 unsigned long int x,y;
 IMG_Pixel *mirrored_image;
 mirrored_image=this->create_buffer(width,height);
 if (kind==MIRROR_HORIZONTAL)
 {
  for (x=0;x<width;++x)
  {
   for (y=0;y<height;++y)
   {
    mirrored_image[this->get_offset(0,x,y)]=image[this->get_offset(0,(width-x-1),y)];
   }

  }

 }
 if (kind==MIRROR_VERTICAL)
 {
   for (x=0;x<width;++x)
  {
   for (y=0;y<height;++y)
   {
    mirrored_image[this->get_offset(0,x,y)]=image[this->get_offset(0,x,(height-y-1))];
   }

  }

 }
 free(image);
 image=mirrored_image;
}

void Surface::resize_image(const unsigned long int new_width,const unsigned long int new_height)
{
 float x_ratio,y_ratio;
 unsigned long int x,y;
 size_t index,position;
 IMG_Pixel *scaled_image;
 scaled_image=this->create_buffer(new_width,new_height);
 x_ratio=static_cast<float>(width)/static_cast<float>(new_width);
 y_ratio=static_cast<float>(height)/static_cast<float>(new_height);
 for (x=0;x<new_width;++x)
 {
  for (y=0;y<new_height;++y)
  {
   index=this->get_offset(0,x,y,new_width);
   position=this->get_offset(0,(x_ratio*static_cast<float>(x)),(y_ratio*static_cast<float>(y)),width);
   scaled_image[index]=image[position];
  }

 }
 free(image);
 image=scaled_image;
 width=new_width;
 height=new_height;
}

void Surface::horizontal_mirror()
{
 this->mirror_image(MIRROR_HORIZONTAL);
}

void Surface::vertical_mirror()
{
 this->mirror_image(MIRROR_VERTICAL);
}

Canvas::Canvas()
{
 start=0;
 frame=1;
 frames=1;
}

Canvas::~Canvas()
{

}

void Canvas::set_frame(const unsigned long int target)
{
 if (target>0)
 {
  if (target<=frames) frame=target;
 }

}

void Canvas::increase_frame()
{
 ++frame;
 if (frame>frames)
 {
  frame=1;
 }

}

void Canvas::set_frames(const unsigned long int amount)
{
 if (amount>1) frames=amount;
}

unsigned long int Canvas::get_frames() const
{
 return frames;
}

unsigned long int Canvas::get_frame() const
{
 return frame;
}

void Canvas::load_image(Image &buffer)
{
 this->load_from_buffer(buffer);
}

Background::Background()
{
 background_width=0;
 background_height=0;
 maximum_width=0;
 maximum_height=0;
 current_kind=NORMAL_BACKGROUND;
}

Background::~Background()
{

}

void Background::get_maximum_width()
{
 maximum_width=background_width;
 if (maximum_width>this->get_surface_width())
 {
  maximum_width=this->get_surface_width();
 }

}

void Background::get_maximum_height()
{
 maximum_height=background_height;
 if (maximum_height>this->get_surface_height())
 {
  maximum_height=this->get_surface_height();
 }

}

void Background::configure_background()
{
 switch(current_kind)
 {
  case NORMAL_BACKGROUND:
  background_width=this->get_image_width();
  background_height=this->get_image_height();
  start=0;
  break;
  case HORIZONTAL_BACKGROUND:
  background_width=this->get_image_width()/this->get_frames();
  background_height=this->get_image_height();
  start=(this->get_frame()-1)*background_width;
  break;
  case VERTICAL_BACKGROUND:
  background_width=this->get_image_width();
  background_height=this->get_image_height()/this->get_frames();
  start=(this->get_frame()-1)*background_width*background_height;
  break;
 }

}

unsigned long int Background::get_width() const
{
 return background_width;
}

unsigned long int Background::get_height() const
{
 return background_height;
}

void Background::set_kind(const BACKGROUND_TYPE kind)
{
 current_kind=kind;
 this->configure_background();
 this->get_maximum_width();
 this->get_maximum_height();
}

void Background::set_setting(const BACKGROUND_TYPE kind,const unsigned long int frames)
{
 if (kind!=NORMAL_BACKGROUND) this->set_frames(frames);
 this->set_kind(kind);
}

void Background::set_target(const unsigned long int target)
{
 this->set_frame(target);
 this->set_kind(current_kind);
}

void Background::step()
{
 this->increase_frame();
 this->set_kind(current_kind);
}

void Background::draw_background()
{
 unsigned long int x,y,index;
 x=0;
 y=0;
 for (index=maximum_width*maximum_height;index>0;--index)
 {
  this->draw_image_pixel(this->get_offset(start,x,y),x,y);
  if (x==maximum_width)
  {
   x=0;
   ++y;
  }
  ++x;
 }

}

Sprite::Sprite()
{
 transparent=true;
 current_x=0;
 current_y=0;
 sprite_width=0;
 sprite_height=0;
 current_kind=SINGLE_SPRITE;
}

Sprite::~Sprite()
{

}

void Sprite::draw_transparent_sprite()
{
 unsigned long int x,y,index;
 x=0;
 y=0;
 for (index=sprite_width*sprite_height;index>0;--index)
 {
  if (this->compare_pixels(0,this->get_offset(start,x,y))==true)
  {
   this->draw_image_pixel(this->get_offset(start,x,y),x+current_x,y+current_y);
  }
  if (x==sprite_width)
  {
   x=0;
   ++y;
  }
  ++x;
 }

}

void Sprite::draw_normal_sprite()
{
 unsigned long int x,y,index;
 x=0;
 y=0;
 for (index=sprite_width*sprite_height;index>0;--index)
 {
  this->draw_image_pixel(this->get_offset(start,x,y),x+current_x,y+current_y);
  if (x==sprite_width)
  {
   x=0;
   ++y;
  }
  ++x;
 }

}

void Sprite::load_sprite(Image &buffer,const SPRITE_TYPE kind,const unsigned long int frames)
{
 this->load_image(buffer);
 if (kind!=SINGLE_SPRITE) this->set_frames(frames);
 this->set_kind(kind);
}

void Sprite::set_transparent(const bool enabled)
{
 transparent=enabled;
}

bool Sprite::get_transparent() const
{
 return transparent;
}

void Sprite::set_x(const unsigned long int x)
{
 current_x=x;
}

void Sprite::set_y(const unsigned long int y)
{
 current_y=y;
}

void Sprite::increase_x()
{
 ++current_x;
}

void Sprite::decrease_x()
{
 --current_x;
}

void Sprite::increase_y()
{
 ++current_y;
}

void Sprite::decrease_y()
{
 --current_y;
}

void Sprite::increase_x(const unsigned long int increment)
{
 current_x+=increment;
}

void Sprite::decrease_x(const unsigned long int decrement)
{
 current_x-=decrement;
}

void Sprite::increase_y(const unsigned long int increment)
{
 current_y+=increment;
}

void Sprite::decrease_y(const unsigned long int decrement)
{
 current_y-=decrement;
}

unsigned long int Sprite::get_x() const
{
 return current_x;
}

unsigned long int Sprite::get_y() const
{
 return current_y;
}

unsigned long int Sprite::get_width() const
{
 return sprite_width;
}

unsigned long int Sprite::get_height() const
{
 return sprite_height;
}

Sprite* Sprite::get_handle()
{
 return this;
}

Collision_Box Sprite::get_box() const
{
 Collision_Box target;
 target.x=current_x;
 target.y=current_y;
 target.width=sprite_width;
 target.height=sprite_height;
 return target;
}

void Sprite::set_kind(const SPRITE_TYPE kind)
{
 switch(kind)
 {
  case SINGLE_SPRITE:
  sprite_width=this->get_image_width();
  sprite_height=this->get_image_height();
  start=0;
  break;
  case HORIZONTAL_STRIP:
  sprite_width=this->get_image_width()/this->get_frames();
  sprite_height=this->get_image_height();
  start=(this->get_frame()-1)*sprite_width;
  break;
  case VERTICAL_STRIP:
  sprite_width=this->get_image_width();
  sprite_height=this->get_image_height()/this->get_frames();
  start=(this->get_frame()-1)*sprite_width*sprite_height;
  break;
 }
 current_kind=kind;
}

SPRITE_TYPE Sprite::get_kind() const
{
 return current_kind;
}

void Sprite::set_target(const unsigned long int target)
{
 this->set_frame(target);
 this->set_kind(current_kind);
}

void Sprite::step()
{
 this->increase_frame();
 this->set_kind(current_kind);
}

void Sprite::set_position(const unsigned long int x,const unsigned long int y)
{
 current_x=x;
 current_y=y;
}

void Sprite::clone(Sprite &target)
{
 this->set_width(target.get_image_width());
 this->set_height(target.get_image_height());
 this->set_frames(target.get_frames());
 this->set_kind(target.get_kind());
 this->set_transparent(target.get_transparent());
 this->set_buffer(this->create_buffer(target.get_image_width(),target.get_image_width()));
 memmove(this->get_image(),target.get_image(),target.get_length());
}

void Sprite::draw_sprite()
{
 if (transparent==true)
 {
  this->draw_transparent_sprite();
 }
 else
 {
  this->draw_normal_sprite();
 }

}

void Sprite::draw_sprite(const unsigned long int x,const unsigned long int y)
{
 this->set_position(x,y);
 this->draw_sprite();
}

void Sprite::draw_sprite(const bool transparency)
{
 this->set_transparent(transparency);
 this->draw_sprite();
}

void Sprite::draw_sprite(const bool transparency,const unsigned long int x,const unsigned long int y)
{
 this->set_transparent(transparency);
 this->draw_sprite(x,y);
}

Tileset::Tileset()
{
 offset=0;
 rows=0;
 columns=0;
 tile_width=0;
 tile_height=0;
}

Tileset::~Tileset()
{

}

unsigned long int Tileset::get_tile_width() const
{
 return tile_width;
}

unsigned long int Tileset::get_tile_height() const
{
 return tile_height;
}

unsigned long int Tileset::get_rows() const
{
 return rows;
}

unsigned long int Tileset::get_columns() const
{
 return columns;
}

void Tileset::select_tile(const unsigned long int row,const unsigned long int column)
{
 if ((row<rows)&&(column<columns))
 {
  offset=this->get_offset(0,row*tile_width,column*tile_height);
 }

}

void Tileset::draw_tile(const unsigned long int x,const unsigned long int y)
{
 unsigned long int tile_x,tile_y,index;
 tile_x=0;
 tile_y=0;
 for (index=tile_width*tile_height;index>0;--index)
 {
  this->draw_image_pixel(offset+this->get_offset(0,tile_x,tile_y),x+tile_x,y+tile_y);
  if (tile_x==tile_width)
  {
   tile_x=0;
   ++tile_y;
  }
  ++tile_x;
 }

}

void Tileset::draw_tile(const unsigned long int row,const unsigned long int column,const unsigned long int x,const unsigned long int y)
{
 this->select_tile(row,column);
 this->draw_tile(x,y);
}

void Tileset::load_tileset(Image &buffer,const unsigned long int row_amount,const unsigned long int column_amount)
{
 if ((row_amount>0)&&(column_amount>0))
 {
  this->load_from_buffer(buffer);
  rows=row_amount;
  columns=column_amount;
  tile_width=this->get_image_width()/rows;
  tile_height=this->get_image_height()/columns;
 }

}

Text::Text()
{
 current_x=0;
 current_y=0;
 font=NULL;
}

Text::~Text()
{

}

void Text::increase_position()
{
 font->increase_x(font->get_width());
}

void Text::restore_position()
{
 font->set_position(current_x,current_y);
}

void Text::set_position(const unsigned long int x,const unsigned long int y)
{
 font->set_position(x,y);
 current_x=x;
 current_y=y;
}

void Text::load_font(Sprite *target)
{
 font=target;
 font->set_frames(256);
 font->set_kind(HORIZONTAL_STRIP);
}

void Text::draw_character(const char target)
{
 font->set_target(static_cast<unsigned char>(target)+1);
 font->draw_sprite();
}

void Text::draw_text(const char *text)
{
 size_t index,length;
 length=strlen(text);
 this->restore_position();
 for (index=0;index<length;++index)
 {
  this->draw_character(text[index]);
  this->increase_position();
 }
 this->restore_position();
}

void Text::draw_character(const unsigned long int x,const unsigned long int y,const char target)
{
 this->set_position(x,y);
 this->draw_character(target);
}

void Text::draw_text(const unsigned long int x,const unsigned long int y,const char *text)
{
 this->set_position(x,y);
 this->draw_text(text);
}

Transformation::Transformation()
{
 screen_x_factor=0;
 screen_y_factor=0;
 surface_x_factor=0;
 surface_y_factor=0;
}

Transformation::~Transformation()
{

}

void Transformation::initialize(const float screen_width,const float screen_height,const float surface_width,const float surface_height)
{
 screen_x_factor=screen_width/surface_width;
 screen_y_factor=screen_height/surface_height;
 surface_x_factor=surface_width/screen_width;
 surface_y_factor=surface_height/screen_height;
}

float Transformation::get_screen_x(const float surface_x) const
{
 return screen_x_factor*surface_x;
}

float Transformation::get_screen_y(const float surface_y) const
{
 return screen_y_factor*surface_y;
}

float Transformation::get_surface_x(const float screen_x) const
{
 return surface_x_factor*screen_x;
}

float Transformation::get_surface_y(const float screen_y) const
{
 return surface_y_factor*screen_y;
}

Collision::Collision()
{
 first.x=0;
 first.y=0;
 first.width=0;
 first.height=0;
 second.x=0;
 second.y=0;
 second.width=0;
 second.height=0;
}

Collision::~Collision()
{

}

void Collision::set_target(const Collision_Box &first_target,const Collision_Box &second_target)
{
 first=first_target;
 second=second_target;
}

bool Collision::check_horizontal_collision() const
{
 bool result;
 result=false;
 if ((first.x+first.width)>=second.x)
 {
  if (first.x<=(second.x+second.width)) result=true;
 }
 return result;
}

bool Collision::check_vertical_collision() const
{
 bool result;
 result=false;
 if ((first.y+first.height)>=second.y)
 {
  if (first.y<=(second.y+second.height)) result=true;
 }
 return result;
}

bool Collision::check_collision() const
{
 return this->check_horizontal_collision() || this->check_vertical_collision();
}

bool Collision::check_horizontal_collision(const Collision_Box &first_target,const Collision_Box &second_target)
{
 this->set_target(first_target,second_target);
 return this->check_horizontal_collision();
}

bool Collision::check_vertical_collision(const Collision_Box &first_target,const Collision_Box &second_target)
{
 this->set_target(first_target,second_target);
 return this->check_vertical_collision();
}

bool Collision::check_collision(const Collision_Box &first_target,const Collision_Box &second_target)
{
 this->set_target(first_target,second_target);
 return this->check_collision();
}

Collision_Box Collision::generate_box(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height) const
{
 Collision_Box result;
 result.x=x;
 result.y=y;
 result.width=width;
 result.height=height;
 return result;
}

}
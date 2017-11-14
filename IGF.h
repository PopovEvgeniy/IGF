/*
Indie framework was create by Popov Evgeniy Alekseyevich
Some code bases on code from SVGALib(http://www.svgalib.org/).

Indie game framework license

Copyright � 2017, Popov Evgeniy Alekseyevich

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <windows.h>
#include <unknwn.h>
#include <d2d1.h>
#include <dshow.h>
#include <xinput.h>

#define IGF_KEYBOARD 256
#define IGF_MOUSE 3

#define IGFKEY_NONE 0
#define IGFKEY_PRESS 1
#define IGFKEY_RELEASE 2

#define IGF_MOUSE_LEFT 0
#define IGF_MOUSE_RIGHT 1
#define IGF_MOUSE_MIDDLE 2

#define IGF_GAMEPAD_BATTERY_ERROR 0
#define IGF_GAMEPAD_BATTERY_WIRED 1
#define IGF_GAMEPAD_BATTERY_ALKAINE 2
#define IGF_GAMEPAD_BATTERY_NIMH 3
#define IGF_GAMEPAD_BATTERY_UNKNOW 4
#define IGF_GAMEPAD_BATTERY_EMPTY 5
#define IGF_GAMEPAD_BATTERY_LOW 6
#define IGF_GAMEPAD_BATTERY_MEDIUM 7
#define IGF_GAMEPAD_BATTERY_FULL 8
#define IGF_GAMEPAD_UP XINPUT_GAMEPAD_DPAD_UP
#define IGF_GAMEPAD_DOWN XINPUT_GAMEPAD_DPAD_DOWN
#define IGF_GAMEPAD_LEFT XINPUT_GAMEPAD_DPAD_LEFT
#define IGF_GAMEPAD_RIGHT XINPUT_GAMEPAD_DPAD_RIGHT
#define IGF_GAMEPAD_A XINPUT_GAMEPAD_A
#define IGF_GAMEPAD_B XINPUT_GAMEPAD_B
#define IGF_GAMEPAD_X XINPUT_GAMEPAD_X
#define IGF_GAMEPAD_Y XINPUT_GAMEPAD_Y
#define IGF_GAMEPAD_LEFT_BUMPER XINPUT_GAMEPAD_LEFT_SHOULDER
#define IGF_GAMEPAD_RIGHT_BUMPER XINPUT_GAMEPAD_RIGHT_SHOULDER
#define IGF_GAMEPAD_START XINPUT_GAMEPAD_START
#define IGF_GAMEPAD_BACK XINPUT_GAMEPAD_BACK
#define IGF_GAMEPAD_LEFT_TRIGGER 0
#define IGF_GAMEPAD_RIGHT_TRIGGER 1
#define IGF_GAMEPAD_LEFT_STICK 2
#define IGF_GAMEPAD_RIGHT_STICK 3

struct IGF_Color
{
 unsigned char blue:8;
 unsigned char green:8;
 unsigned char red:8;
};

struct TGA_head
{
 unsigned char id:8;
 unsigned char color_map:8;
 unsigned char type:8;
};

struct TGA_map
{
 unsigned short int index:16;
 unsigned short int length:16;
 unsigned char map_size:8;
};

struct TGA_image
{
 unsigned short int x:16;
 unsigned short int y:16;
 unsigned short int width:16;
 unsigned short int height:16;
 unsigned char color:8;
 unsigned char alpha:3;
 unsigned char direction:5;
};

struct PCX_head
{
 unsigned char vendor:8;
 unsigned char version:8;
 unsigned char compress:8;
 unsigned char color:8;
 unsigned short int min_x:16;
 unsigned short int min_y:16;
 unsigned short int max_x:16;
 unsigned short int max_y:16;
 unsigned short int vertical_dpi:16;
 unsigned short int horizontal_dpi:16;
 unsigned char palette[48];
 unsigned char reversed:8;
 unsigned char planes:8;
 unsigned short int plane_length:16;
 unsigned short int palette_type:16;
 unsigned short int screen_width:16;
 unsigned short int screen_height:16;
 unsigned char filled[54];
};

struct IGF_Box
{
 unsigned long int x:32;
 unsigned long int y:32;
 unsigned long int width:32;
 unsigned long int height:32;
};

LRESULT CALLBACK IGF_Process_Message(HWND window,UINT Message,WPARAM wParam,LPARAM lParam);

class IGF_Base
{
 public:
 IGF_Base();
 ~IGF_Base();
};

class IGF_Synchronization
{
 private:
 HANDLE timer;
 protected:
 void set_timer(unsigned long int interval);
 void wait_timer();
 public:
 IGF_Synchronization();
 ~IGF_Synchronization();
};

class IGF_Engine
{
 protected:
 HWND window;
 WNDCLASS window_class;
 unsigned long int width;
 unsigned long int height;
 void create_window();
 void capture_mouse();
 bool process_message();
 public:
 IGF_Engine();
 ~IGF_Engine();
 unsigned long int get_width();
 unsigned long int get_height();
};

class IGF_Frame
{
 protected:
 unsigned long int frame_width;
 unsigned long int frame_height;
 unsigned long int frame_line;
 unsigned long int length;
 unsigned long int *buffer;
 void create_render_buffer();
 public:
 IGF_Frame();
 ~IGF_Frame();
 void draw_pixel(const unsigned long int x,const unsigned long int y,const unsigned long int red,const unsigned long int green,const unsigned long int blue);
 void clear_screen();
 unsigned long int get_frame_width();
 unsigned long int get_frame_height();
};

class IGF_Render:public IGF_Base, public IGF_Engine, public IGF_Frame
{
 protected:
 ID2D1Factory *render;
 ID2D1HwndRenderTarget *target;
 ID2D1Bitmap *surface;
 D2D1_RENDER_TARGET_PROPERTIES setting;
 D2D1_HWND_RENDER_TARGET_PROPERTIES configuration;
 D2D1_RECT_U source;
 D2D1_RECT_F destanation;
 D2D1_RECT_F texture;
 void create_factory();
 void create_target();
 void create_surface();
 void set_render_setting();
 void set_render();
 void destroy_resource();
 void recreate_render();
 void prepare_surface();
 void create_render();
 void refresh();
 public:
 IGF_Render();
 ~IGF_Render();
};

class IGF_Screen:public IGF_Synchronization, public IGF_Render
{
 public:
 void initialize();
 bool sync();
 IGF_Screen* get_handle();
};

class IGF_Keyboard
{
 private:
 unsigned char *preversion;
 public:
 IGF_Keyboard();
 ~IGF_Keyboard();
 bool check_hold(const unsigned char code);
 bool check_press(const unsigned char code);
 bool check_release(const unsigned char code);
};

class IGF_Mouse
{
 private:
 unsigned char preversion[IGF_MOUSE];
 public:
 IGF_Mouse();
 ~IGF_Mouse();
 void show();
 void hide();
 void set_position(const unsigned long int x,const unsigned long int y);
 unsigned long int get_x();
 unsigned long int get_y();
 bool check_hold(const unsigned char button);
 bool check_press(const unsigned char button);
 bool check_release(const unsigned char button);
};

class IGF_Gamepad
{
 private:
 XINPUT_BATTERY_INFORMATION battery;
 XINPUT_STATE current;
 XINPUT_STATE preversion;
 XINPUT_VIBRATION vibration;
 unsigned long int length;
 unsigned long int active;
 bool read_battery_status();
 void clear_state();
 bool read_state();
 bool write_state();
 void set_motor(const unsigned short int left,const unsigned short int right);
 bool check_button(XINPUT_STATE &target,const unsigned short int button);
 bool check_trigger(XINPUT_STATE &target,const unsigned char trigger);
 public:
 IGF_Gamepad();
 ~IGF_Gamepad();
 void set_active(const unsigned long int gamepad);
 unsigned long int get_active();
 unsigned long int get_amount();
 bool check_connection();
 unsigned char get_battery_type();
 unsigned char get_battery_level();
 void update();
 bool check_button_hold(const unsigned short int button);
 bool check_button_press(const unsigned short int button);
 bool check_button_release(const unsigned short int button);
 bool check_trigger_hold(const unsigned char trigger);
 bool check_trigger_press(const unsigned char trigger);
 bool check_trigger_release(const unsigned char trigger);
 bool set_vibration(const unsigned short int left,const unsigned short int right);
 char get_stick_x(const unsigned char stick);
 char get_stick_y(const unsigned char stick);
};

class IGF_Multimedia: public IGF_Base
{
 private:
 IGraphBuilder *loader;
 IMediaControl *player;
 IMediaSeeking *controler;
 IVideoWindow *video;
 wchar_t *convert_file_name(const char *target);
 void open(const wchar_t *target);
 public:
 IGF_Multimedia();
 ~IGF_Multimedia();
 void initialize();
 void load(const char *target);
 void play();
 void stop();
 bool check_playing();
};

class IGF_Memory
{
 private:
 MEMORYSTATUSEX memory;
 public:
 IGF_Memory();
 ~IGF_Memory();
 unsigned long long int get_total_memory();
 unsigned long int long get_free_memory();
};

class IGF_System
{
 public:
 IGF_System();
 ~IGF_System();
 unsigned long int get_random(const unsigned long int number);
 void quit();
 void run(const char *command);
 char* read_environment(const char *variable);
 void enable_logging(const char *name);
};

class IGF_Timer
{
 private:
 unsigned long int interval;
 time_t start;
 public:
 IGF_Timer();
 ~IGF_Timer();
 void set_timer(const unsigned long int seconds);
 bool check_timer();
};

class IGF_Primitive
{
 private:
 IGF_Screen *surface;
 public:
 IGF_Primitive();
 ~IGF_Primitive();
 void initialize(IGF_Screen *Screen);
 void draw_line(const unsigned long int x1,const unsigned long int y1,const unsigned long int x2,const unsigned long int y2,const unsigned char red,const unsigned char green,const unsigned char blue);
 void draw_rectangle(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height,const unsigned char red,const unsigned char green,const unsigned char blue);
 void draw_filled_rectangle(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height,const unsigned char red,const unsigned char green,const unsigned char blue);
};

class IGF_Image
{
 private:
 unsigned long int width;
 unsigned long int height;
 unsigned char *data;
 public:
 IGF_Image();
 ~IGF_Image();
 void load_tga(const char *name);
 void load_pcx(const char *name);
 unsigned long int get_width();
 unsigned long int get_height();
 unsigned long int get_data_length();
 unsigned char *get_data();
 void destroy_image();
};

class IGF_Canvas
{
 protected:
 unsigned long int width;
 unsigned long int height;
 unsigned long int frames;
 IGF_Screen *surface;
 IGF_Color *image;
 public:
 IGF_Canvas();
 ~IGF_Canvas();
 unsigned long int get_width();
 unsigned long int get_height();
 void set_frames(const unsigned long int amount);
 unsigned long int get_frames();
 void initialize(IGF_Screen *Screen);
 void load_image(IGF_Image &buffer);
 void mirror_image(const unsigned char kind);
 void resize_image(const unsigned long int new_width,const unsigned long int new_height);
};

class IGF_Background:public IGF_Canvas
{
 public:
 void draw_background();
 void draw_horizontal_background(const unsigned long int frame);
 void draw_vertical_background(const unsigned long int frame);
};

class IGF_Sprite:public IGF_Canvas
{
 private:
 unsigned long int current_x;
 unsigned long int current_y;
 public:
 IGF_Sprite();
 ~IGF_Sprite();
 void draw_sprite_frame(const unsigned long int x,const unsigned long int y,const unsigned long int frame);
 void draw_sprite(const unsigned long int x,const unsigned long int y);
 unsigned long int get_x();
 unsigned long int get_y();
 unsigned long int get_sprite_width();
 unsigned long int get_sprite_height();
 IGF_Sprite* get_handle();
 IGF_Box get_box();
};

class IGF_Text
{
 private:
 unsigned long int current_x;
 unsigned long int current_y;
 IGF_Sprite *sprite;
 public:
 IGF_Text();
 ~IGF_Text();
 void set_position(const unsigned long int x,const unsigned long int y);
 void load_font(IGF_Sprite *font);
 void draw_text(const char *text);
};

class IGF_Collision
{
 public:
 bool check_horizontal_collision(IGF_Box first,IGF_Box second);
 bool check_vertical_collision(IGF_Box first,IGF_Box second);
 bool check_collision(IGF_Box first,IGF_Box second);
};
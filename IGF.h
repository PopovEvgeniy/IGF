/*
Copyright (C) 2017-2018 Popov Evgeniy Alekseyevich

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

#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"strmiids.lib")
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"xinput.lib")

//Uncomment follow lines if you will compile the code under Visual C++ 2017
/*
#pragma warning(disable : 4995)
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
*/

#include <stddef.h>
#include <limits.h>
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

#define IGF_GETSCANCODE(argument) ((argument >> 16)&0x7f)

#define IGF_KEYBOARD 256
#define IGF_MOUSE 3

#define IGFKEY_RELEASE 0
#define IGFKEY_PRESS 1

#define IGF_MOUSE_LEFT 0
#define IGF_MOUSE_RIGHT 1
#define IGF_MOUSE_MIDDLE 2

enum IGF_MIRROR_TYPE {IGF_MIRROR_HORIZONTAL=0,IGF_MIRROR_VERTICAL=1};
enum IGF_BACKGROUND_TYPE {IGF_NORMAL_BACKGROUND=0,IGF_HORIZONTAL_BACKGROUND=1,IGF_VERTICAL_BACKGROUND=2};
enum IGF_SPRITE_TYPE {IGF_SINGE_SPRITE=0,IGF_HORIZONTAL_STRIP=1,IGF_VERTICAL_STRIP=2};
enum IGF_SURFACE {IGF_SURFACE_SMALL=0,IGF_SURFACE_LARGE=1};
enum IGF_GAMEPAD_DIRECTION {IGF_NEUTRAL_DIRECTION=0,IGF_NEGATIVE_DIRECTION=-1,IGF_POSITIVE_DIRECTION=1};
enum IGF_GAMEPAD_BATTERY_TYPE {IGF_GAMEPAD_BATTERY_TYPE_ERROR=0,IGF_GAMEPAD_BATTERY_ALKAINE=1,IGF_GAMEPAD_BATTERY_NIMH=2,IGF_GAMEPAD_BATTERY_UNKNOW=3};
enum IGF_GAMEPAD_BATTERY_LEVEL {IGF_GAMEPAD_BATTERY_LEVEL_ERROR=0,IGF_GAMEPAD_BATTERY_EMPTY=1,IGF_GAMEPAD_BATTERY_LOW=2,IGF_GAMEPAD_BATTERY_MEDIUM=3,IGF_GAMEPAD_BATTERY_FULL=4};
enum IGF_GAMEPAD_BUTTONS {IGF_GAMEPAD_UP=XINPUT_GAMEPAD_DPAD_UP,IGF_GAMEPAD_DOWN=XINPUT_GAMEPAD_DPAD_DOWN,IGF_GAMEPAD_LEFT=XINPUT_GAMEPAD_DPAD_LEFT,IGF_GAMEPAD_RIGHT=XINPUT_GAMEPAD_DPAD_RIGHT,IGF_GAMEPAD_A=XINPUT_GAMEPAD_A,IGF_GAMEPAD_B=XINPUT_GAMEPAD_B,IGF_GAMEPAD_X=XINPUT_GAMEPAD_X,IGF_GAMEPAD_Y=XINPUT_GAMEPAD_Y,IGF_GAMEPAD_LEFT_BUMPER=XINPUT_GAMEPAD_LEFT_SHOULDER,IGF_GAMEPAD_RIGHT_BUMPER=XINPUT_GAMEPAD_RIGHT_SHOULDER,IGF_GAMEPAD_START=XINPUT_GAMEPAD_START,IGF_GAMEPAD_BACK=XINPUT_GAMEPAD_BACK};
enum IGF_GAMEPAD_TRIGGERS {IGF_GAMEPAD_LEFT_TRIGGER=0,IGF_GAMEPAD_RIGHT_TRIGGER=1};
enum IGF_GAMEPAD_STICKS {IGF_GAMEPAD_LEFT_STICK=0,IGF_GAMEPAD_RIGHT_STICK=1};

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
void IGF_Show_Error(const char *message);

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
 void create_timer();
 void set_timer(const unsigned long int interval);
 void wait_timer();
 public:
 IGF_Synchronization();
 ~IGF_Synchronization();
};

class IGF_Engine
{
 private:
 WNDCLASS window_class;
 HWND window;
 unsigned long int width;
 unsigned long int height;
 protected:
 HWND get_window();
 void prepare_engine();
 void create_window();
 void destroy_window();
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
 private:
 size_t buffer_length;
 unsigned long int frame_width;
 unsigned long int frame_height;
 unsigned long int frame_line;
 unsigned int *buffer;
 protected:
 void set_size(const IGF_SURFACE surface);
 void create_render_buffer();
 unsigned int get_rgb(const unsigned int red,const unsigned int green,const unsigned int blue);
 size_t get_offset(const unsigned long int x,const unsigned long int y);
 unsigned long int get_frame_line();
 unsigned int *get_buffer();
 public:
 IGF_Frame();
 ~IGF_Frame();
 void draw_pixel(const unsigned long int x,const unsigned long int y,const unsigned char red,const unsigned char green,const unsigned char blue);
 void clear_screen();
 unsigned long int get_frame_width();
 unsigned long int get_frame_height();
};

class IGF_Render:public IGF_Base, public IGF_Engine, public IGF_Frame
{
 private:
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
 protected:
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
 void initialize(const IGF_SURFACE surface);
 bool update();
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
 void initialize();
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
 unsigned long int active;
 size_t length;
 bool read_battery_status();
 void clear_state();
 bool read_state();
 bool write_state();
 void set_motor(const unsigned short int left,const unsigned short int right);
 bool check_button(XINPUT_STATE &target,const IGF_GAMEPAD_BUTTONS button);
 bool check_trigger(XINPUT_STATE &target,const IGF_GAMEPAD_TRIGGERS trigger);
 public:
 IGF_Gamepad();
 ~IGF_Gamepad();
 void set_active(const unsigned long int gamepad);
 unsigned long int get_active();
 unsigned long int get_last_index();
 unsigned long int get_maximum_amount();
 unsigned long int get_amount();
 bool check_connection();
 bool is_wireless();
 IGF_GAMEPAD_BATTERY_TYPE get_battery_type();
 IGF_GAMEPAD_BATTERY_LEVEL get_battery_level();
 void update();
 bool check_button_hold(const IGF_GAMEPAD_BUTTONS button);
 bool check_button_press(const IGF_GAMEPAD_BUTTONS button);
 bool check_button_release(const IGF_GAMEPAD_BUTTONS button);
 bool check_trigger_hold(const IGF_GAMEPAD_TRIGGERS trigger);
 bool check_trigger_press(const IGF_GAMEPAD_TRIGGERS trigger);
 bool check_trigger_release(const IGF_GAMEPAD_TRIGGERS trigger);
 unsigned char get_trigger(const IGF_GAMEPAD_TRIGGERS trigger);
 bool set_vibration(const unsigned short int left,const unsigned short int right);
 IGF_GAMEPAD_DIRECTION get_stick_x(const IGF_GAMEPAD_STICKS stick);
 IGF_GAMEPAD_DIRECTION get_stick_y(const IGF_GAMEPAD_STICKS stick);
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
 bool is_end();
 void rewind();
 public:
 IGF_Multimedia();
 ~IGF_Multimedia();
 void initialize();
 void load(const char *target);
 bool check_playing();
 void stop();
 void play();
};

class IGF_Memory
{
 private:
 MEMORYSTATUSEX memory;
 void get_status();
 public:
 IGF_Memory();
 ~IGF_Memory();
 unsigned long long int get_total_memory();
 unsigned long long int get_free_memory();
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

class IGF_File
{
 private:
 FILE *target;
 public:
 IGF_File();
 ~IGF_File();
 void open(const char *name);
 void close();
 void set_position(const off_t offset);
 long int get_position();
 long int get_length();
 void read(void *buffer,const size_t length);
 void write(void *buffer,const size_t length);
 bool check_error();
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
 IGF_Color color;
 IGF_Screen *surface;
 public:
 IGF_Primitive();
 ~IGF_Primitive();
 void initialize(IGF_Screen *Screen);
 void set_color(const unsigned char red,const unsigned char green,const unsigned char blue);
 void draw_line(const unsigned long int x1,const unsigned long int y1,const unsigned long int x2,const unsigned long int y2);
 void draw_rectangle(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height);
 void draw_filled_rectangle(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height);
};

class IGF_Image
{
 private:
 unsigned long int width;
 unsigned long int height;
 unsigned char *data;
 unsigned char *create_buffer(const size_t length);
 void clear_buffer();
 FILE *open_image(const char *name);
 unsigned long int get_file_size(FILE *target);
 public:
 IGF_Image();
 ~IGF_Image();
 void load_tga(const char *name);
 void load_pcx(const char *name);
 unsigned long int get_width();
 unsigned long int get_height();
 size_t get_data_length();
 unsigned char *get_data();
 void destroy_image();
};

class IGF_Canvas
{
 private:
 unsigned long int width;
 unsigned long int height;
 unsigned long int frames;
 IGF_Screen *surface;
 void clear_buffer();
 protected:
 IGF_Color *image;
 void set_width(const unsigned long int image_width);
 void set_height(const unsigned long int image_height);
 IGF_Color *create_buffer(const unsigned long int image_width,const unsigned long int image_height);
 void draw_image_pixel(const size_t offset,const unsigned long int x,const unsigned long int y);
 size_t get_offset(const unsigned long int start,const unsigned long int x,const unsigned long int y);
 public:
 IGF_Canvas();
 ~IGF_Canvas();
 IGF_Color *get_image();
 size_t get_length();
 unsigned long int get_image_width();
 unsigned long int get_image_height();
 void set_frames(const unsigned long int amount);
 unsigned long int get_frames();
 void initialize(IGF_Screen *Screen);
 void load_image(IGF_Image &buffer);
 void mirror_image(const IGF_MIRROR_TYPE kind);
 void resize_image(const unsigned long int new_width,const unsigned long int new_height);
};

class IGF_Background:public IGF_Canvas
{
 private:
 unsigned long int start;
 unsigned long int background_width;
 unsigned long int background_height;
 unsigned long int frame;
 IGF_BACKGROUND_TYPE current_kind;
 public:
 IGF_Background();
 ~IGF_Background();
 void set_kind(IGF_BACKGROUND_TYPE kind);
 void set_target(const unsigned long int target);
 void draw_background();
};

class IGF_Sprite:public IGF_Canvas
{
 private:
 unsigned long int current_x;
 unsigned long int current_y;
 unsigned long int sprite_width;
 unsigned long int sprite_height;
 unsigned long int frame;
 unsigned long int start;
 IGF_SPRITE_TYPE current_kind;
 bool compare_pixels(const IGF_Color &first,const IGF_Color &second);
 void draw_sprite_pixel(const size_t offset,const unsigned long int x,const unsigned long int y);
 public:
 IGF_Sprite();
 ~IGF_Sprite();
 unsigned long int get_x();
 unsigned long int get_y();
 unsigned long int get_width();
 unsigned long int get_height();
 IGF_Sprite* get_handle();
 IGF_Box get_box();
 void set_kind(const IGF_SPRITE_TYPE kind);
 IGF_SPRITE_TYPE get_kind();
 void set_target(const unsigned long int target);
 void set_position(const unsigned long int x,const unsigned long int y);
 void clone(IGF_Sprite &target);
 void draw_sprite();
};

class IGF_Text
{
 private:
 unsigned long int current_x;
 unsigned long int current_y;
 unsigned long int step_x;
 IGF_Sprite *sprite;
 void draw_character(const char target);
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
 bool check_horizontal_collision(const IGF_Box &first,const IGF_Box &second);
 bool check_vertical_collision(const IGF_Box &first,const IGF_Box &second);
 bool check_collision(const IGF_Box &first,const IGF_Box &second);
};
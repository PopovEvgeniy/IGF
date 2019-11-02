/*
Copyright (C) 2017-2019 Popov Evgeniy Alekseyevich

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

//Uncomment follow lines if you will compile the code under Visual C++ 2017 or higher
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

#define GETSCANCODE(argument) ((argument >> 16)&0x7f)

#define KEYBOARD 256
#define MOUSE 3

#define KEY_RELEASE 0
#define KEY_PRESS 1

#define MOUSE_LEFT 0
#define MOUSE_RIGHT 1
#define MOUSE_MIDDLE 2

enum MIRROR_TYPE {MIRROR_HORIZONTAL=0,MIRROR_VERTICAL=1};
enum BACKGROUND_TYPE {NORMAL_BACKGROUND=0,HORIZONTAL_BACKGROUND=1,VERTICAL_BACKGROUND=2};
enum SPRITE_TYPE {SINGLE_SPRITE=0,HORIZONTAL_STRIP=1,VERTICAL_STRIP=2};
enum SURFACE {SURFACE_SMALL=0,SURFACE_LARGE=1};
enum GAMEPAD_DIRECTION {GAMEPAD_NEUTRAL_DIRECTION=0,GAMEPAD_NEGATIVE_DIRECTION=-1,GAMEPAD_POSITIVE_DIRECTION=1};
enum GAMEPAD_BATTERY_TYPE {GAMEPAD_BATTERY_TYPE_ERROR=0,GAMEPAD_BATTERY_ALKAINE=1,GAMEPAD_BATTERY_NIMH=2,GAMEPAD_BATTERY_UNKNOW=3};
enum GAMEPAD_BATTERY_LEVEL {GAMEPAD_BATTERY_LEVEL_ERROR=0,GAMEPAD_BATTERY_EMPTY=1,GAMEPAD_BATTERY_LOW=2,GAMEPAD_BATTERY_MEDIUM=3,GAMEPAD_BATTERY_FULL=4};
enum GAMEPAD_BUTTONS {GAMEPAD_UP=XINPUT_GAMEPAD_DPAD_UP,GAMEPAD_DOWN=XINPUT_GAMEPAD_DPAD_DOWN,GAMEPAD_LEFT=XINPUT_GAMEPAD_DPAD_LEFT,GAMEPAD_RIGHT=XINPUT_GAMEPAD_DPAD_RIGHT,GAMEPAD_A=XINPUT_GAMEPAD_A,GAMEPAD_B=XINPUT_GAMEPAD_B,GAMEPAD_X=XINPUT_GAMEPAD_X,GAMEPAD_Y=XINPUT_GAMEPAD_Y,GAMEPAD_LEFT_BUMPER=XINPUT_GAMEPAD_LEFT_SHOULDER,GAMEPAD_RIGHT_BUMPER=XINPUT_GAMEPAD_RIGHT_SHOULDER,GAMEPAD_START=XINPUT_GAMEPAD_START,GAMEPAD_BACK=XINPUT_GAMEPAD_BACK};
enum GAMEPAD_TRIGGERS {GAMEPAD_LEFT_TRIGGER=0,GAMEPAD_RIGHT_TRIGGER=1};
enum GAMEPAD_STICKS {GAMEPAD_LEFT_STICK=0,GAMEPAD_RIGHT_STICK=1};

struct IMG_Pixel
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

struct Collision_Box
{
 unsigned long int x:32;
 unsigned long int y:32;
 unsigned long int width:32;
 unsigned long int height:32;
};

LRESULT CALLBACK Process_Message(HWND window,UINT Message,WPARAM wParam,LPARAM lParam);

namespace IGF
{

void Halt(const char *message);

class COM_Base
{
 public:
 COM_Base();
 ~COM_Base();
};

class Synchronization
{
 private:
 HANDLE timer;
 protected:
 void create_timer();
 void set_timer(const unsigned long int interval);
 void wait_timer();
 public:
 Synchronization();
 ~Synchronization();
};

class Engine
{
 private:
 WNDCLASS window_class;
 HWND window;
 unsigned long int width;
 unsigned long int height;
 void get_instance();
 void load_icon();
 void load_cursor();
 void register_window_class();
 protected:
 HWND get_window();
 void prepare_engine();
 void create_window();
 void destroy_window();
 void capture_mouse();
 bool process_message();
 public:
 Engine();
 ~Engine();
 unsigned long int get_width();
 unsigned long int get_height();
};

class Frame
{
 private:
 size_t pixels;
 unsigned long int frame_width;
 unsigned long int frame_height;
 unsigned long int frame_line;
 unsigned int *buffer;
 unsigned int *shadow;
 protected:
 void set_size(const SURFACE surface);
 unsigned int *create_buffer(const char *error);
 void create_buffers();
 unsigned int get_rgb(const unsigned int red,const unsigned int green,const unsigned int blue);
 size_t get_offset(const unsigned long int x,const unsigned long int y);
 unsigned long int get_frame_line();
 unsigned int *get_buffer();
 public:
 Frame();
 ~Frame();
 void draw_pixel(const unsigned long int x,const unsigned long int y,const unsigned char red,const unsigned char green,const unsigned char blue);
 void clear_screen();
 void save();
 void restore();
 unsigned long int get_frame_width();
 unsigned long int get_frame_height();
};

class FPS
{
 private:
 time_t start;
 unsigned long int current;
 unsigned long int fps;
 protected:
 void update_counter();
 public:
 FPS();
 ~FPS();
 unsigned long int get_fps();
};

class Render:public COM_Base, public Engine, public Frame
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
 Render();
 ~Render();
};

class Screen:public FPS, public Synchronization, public Render
{
 public:
 void initialize();
 void initialize(const SURFACE surface);
 bool update();
 bool sync();
 Screen* get_handle();
};

class Keyboard
{
 private:
 unsigned char *preversion;
 unsigned char *create_buffer(const char *error);
 public:
 Keyboard();
 ~Keyboard();
 void initialize();
 bool check_hold(const unsigned char code);
 bool check_press(const unsigned char code);
 bool check_release(const unsigned char code);
};

class Mouse
{
 private:
 unsigned char preversion[MOUSE];
 public:
 Mouse();
 ~Mouse();
 void show();
 void hide();
 void set_position(const unsigned long int x,const unsigned long int y);
 unsigned long int get_x();
 unsigned long int get_y();
 bool check_hold(const unsigned char button);
 bool check_press(const unsigned char button);
 bool check_release(const unsigned char button);
};

class Gamepad
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
 bool check_button(XINPUT_STATE &target,const GAMEPAD_BUTTONS button);
 bool check_trigger(XINPUT_STATE &target,const GAMEPAD_TRIGGERS trigger);
 public:
 Gamepad();
 ~Gamepad();
 void set_active(const unsigned long int gamepad);
 unsigned long int get_active();
 unsigned long int get_maximum_amount();
 unsigned long int get_amount();
 unsigned long int get_last_index();
 bool check_connection();
 bool is_wireless();
 GAMEPAD_BATTERY_TYPE get_battery_type();
 GAMEPAD_BATTERY_LEVEL get_battery_level();
 void update();
 bool check_button_hold(const GAMEPAD_BUTTONS button);
 bool check_button_press(const GAMEPAD_BUTTONS button);
 bool check_button_release(const GAMEPAD_BUTTONS button);
 bool check_trigger_hold(const GAMEPAD_TRIGGERS trigger);
 bool check_trigger_press(const GAMEPAD_TRIGGERS trigger);
 bool check_trigger_release(const GAMEPAD_TRIGGERS trigger);
 unsigned char get_trigger(const GAMEPAD_TRIGGERS trigger);
 bool set_vibration(const unsigned short int left,const unsigned short int right);
 GAMEPAD_DIRECTION get_stick_x(const GAMEPAD_STICKS stick);
 GAMEPAD_DIRECTION get_stick_y(const GAMEPAD_STICKS stick);
};

class Multimedia: public COM_Base
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
 void create_loader();
 void create_player();
 void create_controler();
 void create_video_player();
 public:
 Multimedia();
 ~Multimedia();
 void initialize();
 void load(const char *target);
 bool check_playing();
 void stop();
 void play();
};

class Memory
{
 private:
 MEMORYSTATUSEX memory;
 void get_status();
 public:
 Memory();
 ~Memory();
 unsigned long long int get_total_physical();
 unsigned long long int get_free_physical();
 unsigned long long int get_total_virtual();
 unsigned long long int get_free_virtual();
 unsigned long int get_usage();
};

class System
{
 public:
 System();
 ~System();
 unsigned long int get_random(const unsigned long int number);
 void quit();
 void run(const char *command);
 char* read_environment(const char *variable);
 bool file_exist(const char *name);
 bool delete_file(const char *name);
 void enable_logging(const char *name);
};

class Binary_File
{
 private:
 FILE *target;
 void open(const char *name,const char *mode);
 public:
 Binary_File();
 ~Binary_File();
 void open_read(const char *name);
 void open_write(const char *name);
 void close();
 void set_position(const long int offset);
 long int get_position();
 long int get_length();
 void read(void *buffer,const size_t length);
 void write(void *buffer,const size_t length);
 bool check_error();
};

class Timer
{
 private:
 unsigned long int interval;
 time_t start;
 public:
 Timer();
 ~Timer();
 void set_timer(const unsigned long int seconds);
 bool check_timer();
};

class Primitive
{
 private:
 IMG_Pixel color;
 Screen *surface;
 public:
 Primitive();
 ~Primitive();
 void initialize(Screen *Screen);
 void set_color(const unsigned char red,const unsigned char green,const unsigned char blue);
 void draw_line(const unsigned long int x1,const unsigned long int y1,const unsigned long int x2,const unsigned long int y2);
 void draw_rectangle(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height);
 void draw_filled_rectangle(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height);
};

class Image
{
 private:
 unsigned long int width;
 unsigned long int height;
 unsigned char *data;
 unsigned char *create_buffer(const size_t length);
 void clear_buffer();
 public:
 Image();
 ~Image();
 void load_tga(const char *name);
 void load_pcx(const char *name);
 unsigned long int get_width();
 unsigned long int get_height();
 size_t get_data_length();
 unsigned char *get_data();
 void destroy_image();
};

class Surface
{
 private:
 Screen *surface;
 IMG_Pixel *image;
 unsigned long int width;
 unsigned long int height;
 protected:
 void save();
 void restore();
 void clear_buffer();
 IMG_Pixel *create_buffer(const unsigned long int image_width,const unsigned long int image_height);
 void load_from_buffer(Image &buffer);
 void set_width(const unsigned long int image_width);
 void set_height(const unsigned long int image_height);
 void set_buffer(IMG_Pixel *buffer);
 size_t get_offset(const unsigned long int start,const unsigned long int x,const unsigned long int y);
 void draw_image_pixel(const size_t offset,const unsigned long int x,const unsigned long int y);
 bool compare_pixels(const size_t first,const size_t second);
 public:
 Surface();
 ~Surface();
 void initialize(Screen *screen);
 size_t get_length();
 IMG_Pixel *get_image();
 unsigned long int get_image_width();
 unsigned long int get_image_height();
 void mirror_image(const MIRROR_TYPE kind);
 void resize_image(const unsigned long int new_width,const unsigned long int new_height);
};

class Canvas:public Surface
{
 private:
 unsigned long int frames;
 unsigned long int frame;
 protected:
 unsigned long int start;
 void set_frame(const unsigned long int target);
 void increase_frame();
 unsigned long int get_frame();
 public:
 Canvas();
 ~Canvas();
 void set_frames(const unsigned long int amount);
 unsigned long int get_frames();
 void load_image(Image &buffer);
};

class Background:public Canvas
{
 private:
 unsigned long int background_width;
 unsigned long int background_height;
 unsigned long int current;
 BACKGROUND_TYPE current_kind;
 void draw_background_pixel(const unsigned long int x,const unsigned long int y);
 void slow_draw_background();
 public:
 Background();
 ~Background();
 unsigned long int get_background_width();
 unsigned long int get_background_height();
 void set_kind(const BACKGROUND_TYPE kind);
 void set_target(const unsigned long int target);
 void step();
 void draw_background();
};

class Sprite:public Canvas
{
 private:
 bool transparent;
 unsigned long int current_x;
 unsigned long int current_y;
 unsigned long int sprite_width;
 unsigned long int sprite_height;
 SPRITE_TYPE current_kind;
 void draw_sprite_pixel(const size_t offset,const unsigned long int x,const unsigned long int y);
 public:
 Sprite();
 ~Sprite();
 void set_transparent(const bool enabled);
 bool get_transparent();
 void set_x(const unsigned long int x);
 void set_y(const unsigned long int y);
 unsigned long int get_x();
 unsigned long int get_y();
 unsigned long int get_width();
 unsigned long int get_height();
 Sprite* get_handle();
 Collision_Box get_box();
 void set_kind(const SPRITE_TYPE kind);
 SPRITE_TYPE get_kind();
 void set_target(const unsigned long int target);
 void step();
 void set_position(const unsigned long int x,const unsigned long int y);
 void clone(Sprite &target);
 void draw_sprite();
 void draw_sprite(const unsigned long int x,const unsigned long int y);
};

class Tileset:public Surface
{
 private:
 size_t offset;
 unsigned long int tile_width;
 unsigned long int tile_height;
 unsigned long int rows;
 unsigned long int columns;
 public:
 Tileset();
 ~Tileset();
 unsigned long int get_tile_width();
 unsigned long int get_tile_height();
 unsigned long int get_rows();
 unsigned long int get_columns();
 void select_tile(const unsigned long int row,const unsigned long int column);
 void draw_tile(const unsigned long int x,const unsigned long int y);
 void load_tileset(Image &buffer,const unsigned long int row_amount,const unsigned long int column_amount);
};

class Text
{
 private:
 unsigned long int current_x;
 unsigned long int current_y;
 Sprite *font;
 void increase_position();
 public:
 Text();
 ~Text();
 void set_position(const unsigned long int x,const unsigned long int y);
 void load_font(Sprite *target);
 void draw_character(const char target);
 void draw_text(const char *text);
};

class Transformation
{
 private:
 float screen_x_factor;
 float screen_y_factor;
 float surface_x_factor;
 float surface_y_factor;
 public:
 Transformation();
 ~Transformation();
 void initialize(const unsigned long int screen_width,const unsigned long int screen_height,const unsigned long int surface_width,const unsigned long int surface_height);
 unsigned long int get_screen_x(const unsigned long int surface_x);
 unsigned long int get_screen_y(const unsigned long int surface_y);
 unsigned long int get_surface_x(const unsigned long int screen_x);
 unsigned long int get_surface_y(const unsigned long int screen_y);
};

class Collision
{
 public:
 Collision_Box generate_box(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height);
 bool check_horizontal_collision(const Collision_Box &first,const Collision_Box &second);
 bool check_vertical_collision(const Collision_Box &first,const Collision_Box &second);
 bool check_collision(const Collision_Box &first,const Collision_Box &second);
};

}
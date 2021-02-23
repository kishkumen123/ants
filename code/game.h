#if !defined(GAME_H)
//#include <stdbool.h> //FUTURE: Use this later once your more comfortable with bool
#include <stdint.h>

#define assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#define array_count(value) (sizeof(value)/sizeof(value[0]))
#define array_length(value) (sizeof(value)/sizeof(value[0]))
#define array_size(value) (sizeof(value)/sizeof(value[0]))
#define ABS(n) ((n)<0 ? -(n) : (n))
#define PI 3.14159265f
#define RAD2DEG(n) ((180.0f/PI) * n);
#define DEG2RAD(n) ((PI/180.0f) * n);

#define Kilobytes(Value) ((Value) * 1024LL)
#define Megabytes(Value) (Kilobytes(Value) * 1024LL)
#define Gigabytes(Value) (Megabytes(Value) * 1024LL)
#define Terabytes(Value) (Gigabytes(Value) * 1024LL)

#define local_static static
#define global static

#include <windows.h>
#include <stdio.h>
static void
print(char *format, ...) {
    char buffer[4096] = {0};
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    printf("%s", buffer);
    OutputDebugStringA(buffer);
}

//typedef float __attribute__((ext_vector_type(2))) v2;
//typedef size_t size;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t ui8;
typedef uint16_t ui16;
typedef uint32_t ui32;
typedef uint64_t ui64;

typedef float f32;
typedef double f64;

typedef int32_t bool;
enum{false, true};

typedef enum EventMouse{MOUSE_NONE, MOUSE_LBUTTON, MOUSE_RBUTTON, MOUSE_MBUTTON, MOUSE_XBUTTON1, MOUSE_XBUTTON2,MOUSE_WHEEL} EventMouse;
typedef enum EventPad{PAD_NONE, PAD_UP, PAD_DOWN, PAD_LEFT, PAD_RIGHT, PAD_BACK} EventPad;
typedef enum EventKey{KEY_NONE, KEY_W, KEY_A, KEY_S, KEY_D, KEY_L, KEY_P, KEY_ESCAPE, KEY_1, KEY_2, KEY_3} EventKey;
typedef enum EventType{EVENT_NONE, EVENT_KEYDOWN, EVENT_KEYUP, EVENT_MOUSEWHEEL, EVENT_MOUSEDOWN, EVENT_MOUSEUP, EVENT_MOUSEMOTION, EVENT_TEXT, EVENT_PADDOWN, EVENT_PADUP} EventType;

typedef struct Event{
    EventType type;
    EventKey key;
    EventPad pad;
    EventMouse mouse;
    i32 mouse_x;
    i32 mouse_y;
    i8 wheel_y;
    i8 wheel_x;
    // QUESTION: ask about ui16 vs WCHAR
    ui16 text;
} Event;

typedef struct Events{
    Event event[256];
    ui32 size;
    ui32 index;
} Events;

typedef struct RenderBuffer{
    void *memory;
    int memory_size;

    int bytes_per_pixel;
    int width;
    int height;
    int pitch;
} RenderBuffer;

typedef struct FileData{
    ui32 size;
    void* content;
} FileData;

typedef struct Controller{
    bool up;
    bool down;
    bool left;
    bool right;
} Controller;

typedef struct Clock{
    f32 dt;
} Clock;

#pragma pack(push, 1)
typedef struct BitmapHeader {
    ui16 file_type;
	ui32 file_size;
	ui16 reserved1;
	ui16 reserved2;
	ui32 bitmap_offset;
	ui32 header_size;
	i32  width;
	i32  height;
	ui16 planes;
	ui16 bits_per_pixel;
    ui32 Compression;
	ui32 SizeOfBitmap;
	i32  HorzResolution;
	i32  VertResolution;
	ui32 ColorsUsed;
	ui32 ColorsImportant;
//    ui32 RedMask;
//    ui32 GreenMask;
//    ui32 BlueMask;
} BitmapHeader;
#pragma pack(pop)

typedef struct Bitmap{
    //BitmapHeader *header; maybe i dont want this here?
	i32  width;
	i32  height;
    ui32 *pixels;
} Bitmap;

#define READ_ENTIRE_FILE(name) FileData name(char *filename)
typedef READ_ENTIRE_FILE(ReadEntireFile);

#define WRITE_ENTIRE_FILE(name) bool name(char *filename, void *memory, ui32 memory_size)
typedef WRITE_ENTIRE_FILE(WriteEntireFile);

#define FREE_FILE_MEMORY(name) void name(void *memory)
typedef FREE_FILE_MEMORY(FreeFileMemory);

#include "vectors.h"
typedef struct GameMemory{
	bool running;
    bool initialized;

    void *total_storage; // IMPORTANT: REQUIRED to be cleared to zero at startup
    void *permanent_storage; // IMPORTANT: REQUIRED to be cleared to zero at startup
    void *transient_storage; // IMPORTANT: REQUIRED to be cleared to zero at startup

    ui64 total_size;
    ui64 permanent_storage_size;
    ui64 transient_storage_size;

    char root_dir[256];
    char data_dir[256];

    ReadEntireFile *read_entire_file;
    WriteEntireFile *write_entire_file;
    FreeFileMemory *free_file_memory;
} GameMemory;

typedef struct MemoryArena {
    ui8 *base;
    size_t size;
    size_t used;
} MemoryArena; 

static void
initialize_arena(MemoryArena *arena, ui8 *base, size_t size){
    arena->size = size;
    arena->base = base;
    arena->used = 0;
}

#define push_array(arena, count, type) (type *)push_size_(arena, count * sizeof(type));
#define push_struct(arena, type) (type *)push_size_(arena, sizeof(type));
#define push_size(arena, size) push_size_(arena, size);
static void*
push_size_(MemoryArena *arena, size_t size){
    assert((arena->used + size) < arena->size);
    void *result = arena->base + arena->used;
    arena->used += size;

    return(result);
}

#define MAIN_GAME_LOOP(name) void name(GameMemory *memory, RenderBuffer *render_buffer, Events *events, Clock *clock)
typedef MAIN_GAME_LOOP(MainGameLoop);

static int
string_length(char* s){
    int count = 0;
    while(*s++){
        ++count;
    }

    return(count);
}

static char *
string_point_at_last(char s[], char t, int count){
    char *result = NULL;
    char *found[10];
    ui32 i = 0;

    while(*s++){
        if(*s == t){
            found[i] = s + 1;
            i++;
        }
    }
    result = found[i - count];

    return(result);
}

static void
cat_strings(char *left, char *right, char *dest){
    int left_size = string_length(left);
    int right_size = string_length(right);

    for(int i=0; i < left_size; ++i){
        *dest++ = *left++;
    }
    for(int i=0; i < right_size; ++i){
        *dest++ = *right++;
    }

    *dest++ = 0;
}

static char*
end_of_string(char *str){
    while(*str++){
    }
    return(str);
}

static void
get_root_dir(char *left, i64 length, char *full_path){
    int i=0;
    for(;i < length; ++i){
        *left++ = *full_path++;
    }
    *left++ = 0;
}

static void
copy_string(char *src, char *dst, size_t size){
    for(ui32 i=0; i < size; ++i){
        *dst++ = *src++;
    }
}

typedef struct GeometryType{
    bool PIXEL;
    bool LINE;
    bool RAY;
    bool SEGMENT;
    bool TRIANGLE;
    bool RECTANGLE;
    bool QUAD;
    bool BOX;
    bool CIRCLE;
} GeometryType;

typedef struct Pixel{
    int temp;
} Pixel;

typedef struct Quad{
    int temp;
} Quad;

typedef struct Line{
    int temp;
} Line;

typedef struct Ray{
    int temp;
} Ray;

typedef struct Segment{
    int temp;
} Segment;

typedef union Triangle{
    v2 e[3];
    struct{
        v2 p0, p1, p2;
    };

} Triangle;

static Triangle
triangle(v2 p0, v2 p1, v2 p2){
    Triangle result = {0};
    result.p0 = p0;
    result.p1 = p1;
    result.p2 = p2;
    return(result);
}

typedef struct Box{
    f32 x, y, w, h, width, height;
} Box;

static Box
box(v2 pos, v2 dim){
    Box result = {0};
    result.x = pos.x;
    result.y = pos.y;
    result.w = dim.w;
    result.h = dim.h;
    result.width = result.w;
    result.height = result.h;
}

typedef enum EntityFlags {EntityFlag_Movable} EntityFlags;
typedef enum EntityType {EntityType_None, EntityType_Player, EntityType_Object, EntityType_Pixel, EntityType_Line, EntityType_Ray, EntityType_Segment, EntityType_Triangle, EntityType_Rect, EntityType_Quad, EntityType_Box, EntityType_Circle} EntityType;

typedef struct Entity{
    int id;
    int index;
    ui32 flags;
    EntityType type;
    v2 position;
    f32 x;
    f32 y;
    bool fill;
    v2 dimension;
    v2 direction;
    v4 color;
    v4 outline_color;
    v2 p0;
    v2 p1;
    v2 p2;
    v2 p3;
    ui8 rad;
} Entity;


static bool
has_flags(Entity *e, ui32 flags){
    return(e->flags & flags);
}

static void
set_flags(Entity *e, ui32 flags){
    e->flags |= flags;
}

static void
clear_flags(Entity *e, ui32 flags){
    e->flags &= ~flags; 
}

typedef struct GameState{
    MemoryArena permanent_arena;
    Entity entities[256];
    ui32 entity_count;
    ui32 player_index;
    MemoryArena a;
    Controller controller;
    Bitmap test;
    Bitmap circle;
    Bitmap image;
} GameState;

#include "renderer.h"
typedef struct TranState{
    MemoryArena transient_arena;
    RenderCommands *render_commands;
} TranState;

static Entity*
add_entity(GameState *game_state, EntityType type){
    Entity *result = game_state->entities + game_state->entity_count;
    result->index = game_state->entity_count;
    result->type = type;
    game_state->entity_count++;

    return(result);
}

static Entity*
add_pixel(GameState* game_state, f32 x, f32 y, v4 color){
    Entity *e = add_entity(game_state, EntityType_Pixel);
    e->x = x;
    e->y = y;
    e->color = color;
    return(e);
}

static Entity*
add_segment(GameState* game_state, v2 p0, v2 p1, v4 color){
    Entity *e = add_entity(game_state, EntityType_Segment);
    e->color = color;
    e->p0 = p0;
    e->p1 = p1;
    return(e);
}

static Entity*
add_ray(GameState* game_state, v2 position, v2 direction, v4 color){
    Entity *e = add_entity(game_state, EntityType_Ray);
    e->color = color;
    e->position = position;
    e->direction = direction;
    return(e);
}

static Entity*
add_line(GameState* game_state, v2 position, v2 direction, v4 color){
    Entity *e = add_entity(game_state, EntityType_Line);
    e->color = color;
    e->position = position;
    e->direction = direction;
    return(e);
}

//static Entity*
//add_rect(GameState* game_state, Rect rect, v4 color){
//}

//static Entity*
//add_box(GameState* game_state, Rect rect, v4 color){
//}

static Entity*
add_quad(GameState* game_state, v2 p0, v2 p1, v2 p2, v2 p3, v4 color, bool fill){
    Entity *e = add_entity(game_state, EntityType_Quad);
    e->color = color;
    e->p0 = p0;
    e->p1 = p1;
    e->p2 = p2;
    e->p3 = p3;
    e->fill = fill;
    return(e);
}

static Entity*
add_triangle(GameState *game_state, v2 p0, v2 p1, v2 p2, v4 color, bool fill){
    Entity *e = add_entity(game_state, EntityType_Triangle);
    e->p0 = p0;
    e->p1 = p1;
    e->p2 = p2;
    e->color = color;
    e->fill = fill;
    return(e);
}

static Entity*
add_circle(GameState *game_state, v2 pos, ui8 rad, v4 color, bool fill){
    Entity *e = add_entity(game_state, EntityType_Circle);
    e->position = pos;
    e->color = color;
    e->fill = fill;
    e->rad = rad;
    return(e);
}

//static Entity*
//add_bitmap(GameState* game_state, v2 position, Bitmap image){
//    Entity *e = add_entity(game_state, EntityType_Bitmap);
//    e->position = position
//    e->color = color;
//    return(e);
//}


static Entity*
add_player(GameState *game_state, v2 position, v4 color){
    Entity *e = add_entity(game_state, EntityType_Player);
    e->position = position;
    e->color = color;
    return(e);
}

static v4
convert_ui32_v4_normalized(ui32 value){
	f32 alpha = ((f32)((value >> 24) & 0xFF) / 255.0f);
	f32 red =   ((f32)((value >> 16) & 0xFF) / 255.0f);
	f32 green = ((f32)((value >> 8) & 0xFF) / 255.0f);
	f32 blue =  ((f32)((value >> 0) & 0xFF) / 255.0f);
    v4 result = {red, green, blue, alpha};
    return result;
}

#define GAME_H
#endif

#if !defined(GAME_H)


#include "game_platform.h"
#include "random.h"
#include <time.h>
#include "vectors.h"
#include "linked_list.h"

// get rid of this shit
#include <stdio.h>
#include <windows.h>
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

#pragma pack(push, 1)
typedef struct BitmapHeader {
    u16 file_type;
	u32 file_size;
	u16 reserved1;
	u16 reserved2;
	u32 bitmap_offset;
	u32 header_size;
	i32  width;
	i32  height;
	u16 planes;
	u16 bits_per_pixel;
    u32 Compression;
	u32 SizeOfBitmap;
	i32  HorzResolution;
	i32  VertResolution;
	u32 ColorsUsed;
	u32 ColorsImportant;
//    u32 RedMask;
//    u32 GreenMask;
//    u32 BlueMask;
} BitmapHeader;
#pragma pack(pop)

typedef struct Bitmap{
    BitmapHeader *header; //maybe i dont want this here?
	u32  width;
	u32  height;
    u32 *pixels;
} Bitmap;


typedef struct MemoryArena {
    u8 *base;
    size_t size;
    size_t used;
} MemoryArena; 

static void
initialize_arena(MemoryArena *arena, u8 *base, size_t size){
    arena->base = base;
    arena->size = size;
    arena->used = 0;
}

#define allocate_array(arena, count, type) (type *)allocate_size_(arena, count * sizeof(type));
#define allocate_struct(arena, type) (type *)allocate_size_(arena, sizeof(type));
#define allocate_size(arena, size) allocate_size_(arena, size);
static void*
allocate_size_(MemoryArena *arena, size_t size){
    assert((arena->used + size) < arena->size);
    void *result = arena->base + arena->used;
    arena->used += size;

    return(result);
}

typedef enum EntityFlags {EntityFlag_Movable} EntityFlags;
typedef enum EntityType {EntityType_None, EntityType_Player, EntityType_Object, EntityType_Pixel, EntityType_Line, EntityType_Ray, EntityType_Segment, EntityType_Triangle, EntityType_Rect, EntityType_Quad, EntityType_Box, EntityType_Circle, EntityType_Bitmap, EntityType_Food, EntityType_Ant, EntityType_Colony, EntityType_ToHomePheromone, EntityType_ToFoodPheromone} EntityType;
typedef enum AntState {AntState_Wondering, AntState_Collecting, AntState_Depositing} AntState;

typedef struct Entity{
    u32 index;
    u32 id;
    EntityType type;
    struct Entity* first_child;
    struct Entity* next_child;
    int children_count;
    u32 flags;
    v2 position;
    v2 dimension;
    v2 direction;
    v4 color;
    f32 x;
    f32 y;
    f32 w;
    f32 h;
    bool fill;
    bool draw_bounding_box;
    v4 outline_color;
    v2 p0;
    v2 p1;
    v2 p2;
    v2 p3;
    u8 rad;
    f32 speed;

    AntState ant_state;
    struct Entity *ant_food;
    v2 random_vector;
    bool changing_state;
    bool change_direction;
    u64 t;
    f32 t_max;
    f32 timer;
    f32 max_timer;
	f32 rot_percent;
    bool targeted;
    v2 right_sensor;
    v2 left_sensor;
    v2 mid_sensor;
    u8 sensor_radius;
    f32 sensor_angle;
    f32 sensor_distance;
	f32 pheromone_timer;
	f32 pheromone_timer_max;
    u64 tp;
    f32 tp_max;
    struct Entity *pher_target;
    bool pher_targeted;
    f32 right_sensor_density;
    f32 middle_sensor_density;
    f32 left_sensor_density;
    bool right;
    bool middle;
    bool left;
    v2 target_direction;

    f32 pher_home_decay_rate;
    f32 pher_food_decay_rate;
    u64 food_decay_timer;
    u64 home_decay_timer;

    f32 rotate_speed;

    Bitmap image;
    bool render;
} Entity;

static bool
has_flags(Entity *e, u32 flags){
    return(e->flags & flags);
}

static void
set_flags(Entity *e, u32 flags){
    e->flags |= flags;
}

static void
clear_flags(Entity *e, u32 flags){
    e->flags &= ~flags; 
}


static void
reset_LL_sentinel(LinkedList *sentinel){
    sentinel->count = 0;
    sentinel->next = sentinel;
    sentinel->prev = sentinel;
    sentinel->data = NULL;
}


#include "renderer.h"

typedef struct TranState{
    MemoryArena transient_arena;
    RenderCommandBuffer *render_commands_buffer;
    LinkedListBuffer *LL_buffer;
} TranState;

typedef struct GameState{
    MemoryArena permanent_arena;

    u32 c;
    u64 start;
    bool added;
    u32 fc; 
    u32 wc; 
    u32 cc; 
    u32 dc; 
    u32 tc; 
    u32 ntc;
    u32 ptc;

    LinkedList regions[4][4];
    LinkedList ants;
    LinkedList foods;
    LinkedList phers;
    LinkedList misc;

    u32 region_row_count;
    u32 region_width;
    u32 region_height;

    f32 screen_width;
    f32 screen_height;
    u32 colony_index;
    
    u32 ants_count;

    u32 free_entities[110];
    u32 free_entities_size;
    u32 free_entities_at;

    Entity entities[110];
    u32 entities_size;
    u32 entity_at;

    f32 ant_speed;

    u32 clip_region_index;
    Controller controller;
    Bitmap test;
    Bitmap circle;
    Bitmap image;

} GameState;

static void
add_child(GameState *game_state, u32 parent_index, u32 child_index){
    Entity *parent = game_state->entities + parent_index;
    Entity *child = game_state->entities + child_index;
    child->next_child = parent->first_child;
    parent->first_child = child;
}

static u32
add_entity(GameState *game_state, EntityType type){
    if(game_state->free_entities_at >= 0){
        i32 entity_index = game_state->free_entities[game_state->free_entities_at--];
        Entity *result = game_state->entities + entity_index;
        result->index = game_state->entity_at;
        result->id = game_state->entity_at;
        result->type = type;
        game_state->entity_at++;

        return(result->index);
    }
    //if(game_state->entity_at < game_state->entities_size){
    //    Entity *result = game_state->entities + game_state->entity_at;
    //    result->index = game_state->entity_at;
    //    result->id = game_state->entity_at;
    //    result->type = type;
    //    game_state->entity_at++;

    //    return(result->index);
    //}
    return(0);
}

//static u32
//add_entity_pher(GameState *game_state, v2 quad_coord, EntityType type){
//    u32 quad_size = game_state->pher_size / (game_state->quad_row_count * game_state->quad_row_count);
//    u32 quad_stride = quad_size * game_state->quad_row_count;
//
//    u32 offset_index = (quad_stride * (quad_coord.y - 1)) + (quad_size * (quad_coord.x - 1));
//    u32 quad_index = ((quad_coord.y - 1) * game_state->quad_row_count) + (quad_coord.x - 1);
//    u32 relative_index = game_state->pher_ats[quad_index];
//    u32 index = relative_index + offset_index;
//
//    if(relative_index >= quad_size){
//        game_state->pher_ats[quad_index] = 0;
//        relative_index = 0;
//        index = offset_index;
//    }
//    if(index <= game_state->pher_size){
//        Entity *result = game_state->pheromones + index;
//        result->index = index;
//        result->id = index;
//        result->type = type;
//        game_state->pher_ats[quad_index]++;
//
//        return(result->index);
//    }
//    return(0);
//}

static u32
add_pixel(GameState* game_state, f32 x, f32 y, v4 color){
    u32 e_index = add_entity(game_state, EntityType_Pixel);
    Entity *e = game_state->entities + e_index;
    e->x = x;
    e->y = y;
    e->color = color;
    return(e->index);
}

static u32
add_segment(GameState* game_state, v2 p0, v2 p1, v4 color){
    u32 e_index = add_entity(game_state, EntityType_Segment);
    Entity *e = game_state->entities + e_index;
    e->color = color;
    e->p0 = p0;
    e->p1 = p1;
    return(e->index);
}

static u32
add_ray(GameState* game_state, v2 position, v2 direction, v4 color){
    u32 e_index = add_entity(game_state, EntityType_Ray);
    Entity *e = game_state->entities + e_index;
    e->color = color;
    e->position = position;
    e->direction = direction;
    return(e->index);
}

static u32
add_line(GameState* game_state, v2 position, v2 direction, v4 color){
    u32 e_index = add_entity(game_state, EntityType_Line);
    Entity *e = game_state->entities + e_index;
    e->color = color;
    e->position = position;
    e->direction = direction;
    return(e->index);
}

static u32
add_rect(GameState* game_state, v2 position, v2 dimension, v4 color){
    u32 e_index = add_entity(game_state, EntityType_Rect);
    Entity *e = game_state->entities + e_index;
    e->position = position;
    e->dimension = dimension;
    e->w = dimension.w;
    e->h = dimension.h;
    e->color = color;
    return(e->index);
}

static u32
add_box(GameState* game_state, v2 position, v2 dimension, v4 color){
    u32 e_index = add_entity(game_state, EntityType_Box);
    Entity *e = game_state->entities + e_index;
    e->position = position;
    e->dimension = dimension;
    e->w = dimension.w;
    e->h = dimension.h;
    e->color = color;
    return(e->index);
}

static u32
add_quad(GameState* game_state, v2 p0, v2 p1, v2 p2, v2 p3, v4 color, bool fill){
    u32 e_index = add_entity(game_state, EntityType_Quad);
    Entity *e = game_state->entities + e_index;
    e->color = color;
    e->p0 = p0;
    e->p1 = p1;
    e->p2 = p2;
    e->p3 = p3;
    e->fill = fill;
    return(e->index);
}

static u32
add_triangle(GameState *game_state, v2 p0, v2 p1, v2 p2, v4 color, bool fill){
    u32 e_index = add_entity(game_state, EntityType_Triangle);
    Entity *e = game_state->entities + e_index;
    e->p0 = p0;
    e->p1 = p1;
    e->p2 = p2;
    e->color = color;
    e->fill = fill;
    return(e->index);
}

static u32
add_circle(GameState *game_state, v2 pos, u8 rad, v4 color, bool fill){
    u32 e_index = add_entity(game_state, EntityType_Circle);
    Entity *e = game_state->entities + e_index;
    e->position = pos;
    e->color = color;
    e->fill = fill;
    e->rad = rad;
    return(e->index);
}

static u32
add_bitmap(GameState* game_state, v2 position, Bitmap image){
    u32 e_index = add_entity(game_state, EntityType_Bitmap);
    Entity *e = game_state->entities + e_index;
    e->position = position;
    e->x = position.x;
    e->y = position.y;
    e->image = image;
    return(e->index);
}

static u32
add_player(GameState *game_state, v2 position, v2 dimension, v4 color, Bitmap image){
    u32 e_index = add_entity(game_state, EntityType_Player);
    Entity *e = game_state->entities + e_index;
    e->position = position;
    e->dimension = dimension;
    e->color = color;
    e->image = image;
    return(e->index);
}

static u32
add_food(GameState *game_state, v2 pos, u8 rad, v4 color, bool fill){
    u32 e_index = add_entity(game_state, EntityType_Food);
    Entity *e = game_state->entities + e_index;

    e->position = pos;
    e->color = color;
    e->targeted = false;
    //e->dimension = dimension;
    e->draw_bounding_box = true;
    e->rad = rad;
    e->fill = true;
    return(e->index);
}

static u32
add_ant(GameState *game_state, v2 pos, u8 rad, v4 color, bool fill){
    u32 e_index = add_entity(game_state, EntityType_Ant);
    Entity *e = game_state->entities + e_index;

    e->ant_state = AntState_Wondering;
    e->position = pos;
    e->color = color;
    e->fill = fill;
    e->rad = rad;
    e->direction.x = (((i32)(random_range((2 * 100) + 1)) - 100)/100.0f);
    e->direction.y = (((i32)(random_range((2 * 100) + 1)) - 100)/100.0f);
    e->random_vector.x = e->direction.x;
    e->random_vector.y = e->direction.y;
    e->target_direction.x = e->direction.x;
    e->target_direction.y = e->direction.y;
    e->timer = 0.0f;
    e->rot_percent = 0.0f;
    e->change_direction = false;
    e->changing_state = false;
    e->max_timer = (random_range(10) + 1);
    e->t_max = (random_range(3) + 1);
    e->tp_max = 0.25;
    e->speed = 250.0f;
    e->draw_bounding_box = true;
    e->sensor_radius = 10;
    e->sensor_angle = 60;
    e->sensor_distance = 20;
    e->pheromone_timer = 0;
    e->pheromone_timer_max = 1.0f;
    e->pher_target = NULL;
    e->pher_targeted = false;
    e->right_sensor_density = 0.0f;
    e->middle_sensor_density = 0.0f;
    e->left_sensor_density = 0.0f;
    e->rotate_speed = 0.05f;
    
    return(e->index);
}

static u32
add_colony(GameState *game_state, v2 pos, u8 rad, v4 color, bool fill){
    u32 e_index = add_entity(game_state, EntityType_Colony);
    Entity *e = game_state->entities + e_index;
    e->position = pos;
    e->color = color;
    e->fill = fill;
    e->rad = rad;
    e->draw_bounding_box = true;
    e->speed = 250.0f;
    return(e->index);
}

static u32
add_to_home_pheromone(GameState *game_state, v2 pos, u8 rad, v4 color){
    u32 e_index = add_entity(game_state, EntityType_ToHomePheromone);
    Entity *e = game_state->entities + e_index;
    e->position = pos;
    e->color = color;
    e->color.w = 1;
    e->rad = rad;
    e->pher_home_decay_rate = 8.0f;
    return(e->index);
}

static u32
add_to_food_pheromone(GameState *game_state, v2 pos, u8 rad, v4 color){
    u32 e_index = add_entity(game_state, EntityType_ToFoodPheromone);
    Entity *e = game_state->entities + e_index;
    e->position = pos;
    e->color = color;
    e->color.w = 1;
    e->rad = rad;
    e->pher_food_decay_rate = 8.0f;
    return(e->index);
}

static v4
convert_u32_v4_normalized(u32 value){
	f32 alpha = ((f32)((value >> 24) & 0xFF) / 255.0f);
	f32 red =   ((f32)((value >> 16) & 0xFF) / 255.0f);
	f32 green = ((f32)((value >> 8) & 0xFF) / 255.0f);
	f32 blue =  ((f32)((value >> 0) & 0xFF) / 255.0f);
    v4 result = {red, green, blue, alpha};
    return result;
}

#define GAME_H
#endif

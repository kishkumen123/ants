#if !defined(BASE_H)
#define BASE_H

#include <stdint.h>
#include <stdbool.h>

///////////////////////////////
// NOTE: Helper Macros
///////////////////////////////

#define ENABLE_ASSERT 1

#if ENABLE_ASSERT
# define Assert(x) if(!(x)) __debugbreak()
#else
# define Assert(x)
#endif

#define AlignUpPow2(x,p) (((x) + (p) - 1)&~((p) - 1))
#define AlignDownPow2(x,p) ((x)&~((p) - 1))

#define ArrayCount(x) (sizeof(x)/sizeof(*(x)))

#define Min(a,b) (((a)<=(b))?(a):(b))
#define Max(a,b) (((a)>=(b))?(a):(b))
#define Clamp(a,x,b) (((x)<(a))?(a):((x)>(b))?(b):(x))
#define ABS(x) ((x)<0?-(x):(x))

#define STR_(x) #x
#define STR(x) STR_(x)
#define Glue(a,b) a##b

#define KB(x) (x * 1024LL)
#define MB(x) (KB(x) * 1024LL)
#define GB(x) (MB(x) * 1024LL)
#define TB(x) (GB(x) * 1024LL)

#define Thousand(x) ((x) * 1000)
#define Million(x) ((x) * 1000000llu)
#define Billion(x) ((x) * 1000000000llu)
#define Trillion(x) ((x) * 1000000000000llu)

#define global static
#define local static
//#define function static

// TODO: memory zero (struct, array)
// TODO: memory copy (struct, array)

///////////////////////////////
// NOTE: Basic Types
///////////////////////////////

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

///////////////////////////////
// NOTE: Basic Constants
///////////////////////////////

global s8  s8_min  = (s8) 0x80;
global s16 s16_min = (s16)0x8000;
global s32 s32_min = (s32)0x80000000;
global s64 s64_min = (s64)0x8000000000000000llu;

global s8  s8_max  = (s8) 0x7f;
global s16 s16_max = (s16)0x7fff;
global s32 s32_max = (s32)0x7fffffff;
global s64 s64_max = (s64)0x7fffffffffffffffllu;

global u8  u8_max  = (u8) 0xff;
global u16 u16_max = (u16)0xffff;
global u32 u32_max = (u32)0xffffffff;
global u64 u64_max = (u64)0xffffffffffffffffllu;

global f32 f32_pi = 3.14159265359f;
global f64 f64_pi = 3.14159265359;
global f32 RAD = 0.0174533f;

///////////////////////////////
// NOTE: Math Functions
///////////////////////////////

#include <math.h>
static f32 abs_f32(f32 x){
    union{ f32 f; u32 u; } r;
    r.f = x;
    r.u &= 0x7fffffff;
    return(r.f);
}

static f64 abs_f64(f64 x){
    union{ f64 f; u64 u; } r;
    r.f = x;
    r.u &= 0x7fffffffffffffff;
    return(r.f);
}

static s32 abs_s32(s32 x){
    s32 result = ((x<0)?(-x):(x));
    return(result);
}

static s64 abs_s64(s64 x){
    s64 result = ((x<0)?(-x):(x));
    return(result);
}

#define PI 3.14159265f
#define RAD 0.0174533f
#define RAD2DEG(n) ((180.0f/PI) * (n))
#define DEG2RAD(n) ((PI/180.0f) * (n))

static f32 sqrt_f32(f32 x){ return(sqrt(x)); }
static f32 sin_f32(f32 x){  return(sin(x)); }
static f32 cos_f32(f32 x){  return(cos(x)); }
static f32 tan_f32(f32 x){  return(tan(x)); }

static f64 sqrt_f64(f64 x){ return(sqrt(x)); }
static f64 sin_f64(f64 x){  return(sin(x)); }
static f64 cos_f64(f64 x){  return(cos(x)); }
static f64 tan_f64(f64 x){  return(tan(x)); }

static f32 lerp(f32 a, f32 t, f32 b){
    f32 x = ((a + ((b - a) * t)));
    return(x);
}

static f32 unlerp(f32 a, f32 x, f32 b){
    f32 t = 0.0f;
    if(a != b){
        t = (x - a) / (b - a);
    }
    return(t);
}

///////////////////////////////
// NOTE: Compound Types
///////////////////////////////

typedef union v2{
    struct{ f32 x; f32 y; };
    struct{ f32 w; f32 h; };
    f32 v[2];
} v2;

typedef union v3{
    struct{ f32 x; f32 y; f32 z; };
    struct{ f32 r; f32 g; f32 b; };
    f32 v[3];
} v3;

typedef union v4{
    struct{ f32 x; f32 y; f32 z; f32 w; };
    struct{ f32 r; f32 g; f32 b; f32 a; };
    f32 v[4];
} v4;

typedef union v2s32{
    struct{ s32 x; s32 y; };
    struct{ s32 w; s32 h; };
    s32 v[2];
} v2s32;

///////////////////////////////
// NOTE: Compound Types Constructors
///////////////////////////////

static v2 vec2(f32 x, f32 y){
    v2 result = {x, y};
    return(result);
}

static v3 vec3(f32 x, f32 y, f32 z){
    v3 result = {x, y, z};
    return(result);
}

static v4 vec4(f32 x, f32 y, f32 w, f32 h){
    v4 result = {x, y, w, h};
    return(result);
}

static v2s32 vec2s32(s32 x, s32 y){
    v2s32 result = {x, y};
    return(result);
}


///////////////////////////////
// NOTE: Compound Types Operators
///////////////////////////////

static v2s32 operator+(const v2s32& a, const v2s32& b){
    v2s32 result = {a.x + b.x, a.y + b.y};
    return(result);
}

static v2 operator+(const v2& a, const v2& b){
    v2 result = {a.x + b.x, a.y + b.y};
    return(result);
}

static v3 operator+(const v3& a, const v3& b){
    v3 result = {a.x + b.x, a.y + b.y, a.z + b.z};
    return(result);
}

static v4 operator+(const v4& a, const v4& b){
    v4 result = {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
    return(result);
}

static v2s32 operator-(const v2s32& a, const v2s32& b){
    v2s32 result = {a.x - b.x, a.y - b.y};
    return(result);
}

static v2 operator-(const v2& a, const v2& b){
    v2 result = {a.x - b.x, a.y - b.y};
    return(result);
}

static v3 operator-(const v3& a, const v3& b){
    v3 result = {a.x - b.x, a.y - b.y, a.z - b.z};
    return(result);
}

static v4 operator-(const v4& a, const v4& b){
    v4 result = {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
    return(result);
}

static v2s32 operator*(const v2s32& b, const s32 a){
    v2s32 result = {b.x * a, b.y * a};
    return(result);
}

static v2 operator*(const v2& b, const f32& a){
    v2 result = {b.x * a, b.y * a};
    return(result);
}

static v3 operator*(const v3& b, const f32& a){
    v3 result = {b.x * a, b.y * a, b.z * a};
    return(result);
}

static v4 operator*(const v4& b, const f32& a){
    v4 result = {b.x * a, b.y * a, b.z * a, b.w * a};
    return(result);
}

static v2s32 operator*(const s32& a, const v2s32& b){
    v2s32 result = {b.x * a, b.y * a};
    return(result);
}

static v2 operator*(const f32& a, const v2& b){
    v2 result = {b.x * a, b.y * a};
    return(result);
}

static v3 operator*(const f32& a, const v3& b){
    v3 result = {b.x * a, b.y * a, b.z * a};
    return(result);
}

static v4 operator*(const f32& a, const v4& b){
    v4 result = {b.x * a, b.y * a, b.z * a, b.w * a};
    return(result);
}

static bool operator==(const v2s32& a, const v2s32& b){
    if((a.x == b.x) && (a.y == b.y)){
        return(true);
    }
    return(false);
}

static bool operator==(const v2& a, const v2& b){
    if((a.x == b.x) && (a.y == b.y)){
        return(true);
    }
    return(false);
}

static bool operator==(const v3& a, const v3& b){
    if((a.x == b.x) && (a.y == b.y) && (a.z == b.z)){
        return(true);
    }
    return(false);
}

static bool operator==(const v4& a, const v4& b){
    if((a.x == b.x) && (a.y == b.y && (a.z == b.z) && (a.w == b.w))){
        return(true);
    }
    return(false);
}

///////////////////////////////
// NOTE: Arena Functions
///////////////////////////////

typedef struct Arena{
    void* base;
    size_t size;
    size_t used;
} Arena;

typedef struct ScratchArena{
    Arena* arena;
    size_t used;
    ScratchArena() {};
    ScratchArena(Arena* a) { arena = a; used = a->used; }
    //ScratchArena() { arena = &transient_memory->arena; used = transient_memory->arena.used; }
    //CONSIDER: This might not be good for a base, since some scratch you dont want to deallocate at end of scope maybe?
    ~ScratchArena() { arena->used = used; }
} ScratchArena;

#define allocate_array(arena, type, count) (type*)allocate_size_aligned(arena, count * sizeof(type), _Alignof(type))
#define allocate_struct(arena, type) (type*)allocate_size_aligned(arena, sizeof(type), _Alignof(type))
#define allocate_size(arena, size) allocate_size_aligned(arena, size, _Alignof(s32))
static void* allocate_size_aligned(Arena* arena, size_t size, size_t align){
    size_t used_aligned = AlignUpPow2(arena->used, align);
    Assert(used_aligned + size <= arena->size);
    void* result = (u8*)arena->base + used_aligned;
    arena->used = used_aligned + size;
    return(result);
}

static Arena* allocate_arena(Arena *arena, size_t size){
    Arena* result = allocate_struct(arena, Arena);
    result->base = allocate_size(arena, size);
    result->size = size;
    result->used = 0;
    return(result);
}

static void arena_init(Arena* arena, void* base, size_t size){
    arena->base = base;
    arena->size = size;
    arena->used = 0;
}

static void arena_free(Arena* arena){
    arena->used = 0;
}

static ScratchArena allocate_scratch(Arena* arena){
    ScratchArena result;
    result.arena = arena;
    result.used = arena->used;
    return(result);
}

static void free_scratch(ScratchArena scratch){
    scratch.arena->used = scratch.used;
}

//TODO: arena_resize_align

///////////////////////////////
// NOTE: Doubly Linked List
///////////////////////////////
// NOTE: DLL here can be treated as DLL, SLL, Queue, Stack
// out of convenience, although not optimal. Each should
// have its own implementation

typedef struct Node{
    struct Node* next;
    struct Node* prev;
    void* data;
    u32 count;
} SLL, DLL, Node;

static Node* allocate_node(Arena* arena){
    Node* result = allocate_struct(arena, Node);
    return(result);
}

static void dll_push_front(Node* sentinel, Node* node){
    node->prev = sentinel;
    node->next = sentinel->next;

    node->prev->next = node;
    node->next->prev = node;
    sentinel->count++;
}

static void dll_push_back(Node* sentinel, Node* node){
    node->prev = sentinel->prev;
    node->next = sentinel;

    node->prev->next = node;
    node->next->prev = node;
    sentinel->count++;
}

static Node* dll_pop_front(Node* sentinel){
    Node* node = sentinel->next;
    sentinel->next = sentinel->next->next;
    node->next = node; // QUESTION: gaurd against node referncing nodes in linked list? is this even necessay?
    node->prev = node; // QUESTION: gaurd against node referncing nodes in linked list? is this even necessay?
    sentinel->count--;
    return(node);
}

static Node* dll_pop_back(Node* sentinel){
    Node* node = sentinel->next;
    sentinel->prev = sentinel->prev->prev;
    node->next = node; // QUESTION: gaurd against node referncing nodes in linked list? is this even necessay?
    node->prev = node; // QUESTION: gaurd against node referncing nodes in linked list? is this even necessay?
    sentinel->count--;
    return(node);
}

static void dll_remove(Node* sentinel, Node* node){
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next = node; // QUESTION: gaurd against node referncing nodes in linked list? is this even necessay?
    node->prev = node; // QUESTION: gaurd against node referncing nodes in linked list? is this even necessay?
    sentinel->count--;
}

static void reset_sentinel(DLL *sentinel){
    sentinel->count = 0;
    sentinel->next = sentinel;
    sentinel->prev = sentinel;
    sentinel->data = NULL;
}

///////////////////////////////
// NOTE: Symbolic Constants
///////////////////////////////

typedef enum Axis{
    Axis_X,
    Axis_Y,
    Axis_Z,
    Axis_W,
}Axis;

typedef enum Direction{
    Direction_Up,
    Direction_Down,
    Direction_Reft,
    Direction_Right,
} Direction;

typedef enum OperatingSystem{
    OperatingSystem_Null,
    OperatingSystem_Windows,
    OperatingSystem_Linux,
    OperatingSystem_Mac,
    OperatingSystem_Count,
} OperatingSysem;

typedef enum Architecture{
    Architecture_Null,
    Architecture_X64,
    Architecture_X86,
    Architecture_Arm,
    Architecture_Arm64,
    Architecture_Count,
} Architecture;

typedef enum Month{
    Month_Jan,
    Month_Feb,
    Month_Mar,
    Month_Apr,
    Month_May,
    Month_Jun,
    Month_Jul,
    Month_Sep,
    Month_Aug,
    Month_Oct,
    Month_Nov,
    Month_Dec,
} Month;

typedef enum DayOfWeek{
    DayOfWeek_Sun,
    DayOfWeek_Mon,
    DayOfWeek_Tue,
    DayOfWeek_Wed,
    DayOfWeek_Thu,
    DayOfWeek_Fri,
    DayOfWeek_Sat,
} DayOfWeek;

///////////////////////////////
// INCOMPLETE: NOTE: String8
///////////////////////////////
// Just write it as you need it.... bitch

typedef struct String8{
    u8* str;
    u32 size;
} String8;

typedef struct String16{
    u16* str;
    u32 size;
} String16;

#define str8_litteral(x) str8_create_((u8*)x, (sizeof(x) - 1))
#define str8(x, size) str8_create_((u8*)x, size)
static String8 str8_create_(u8* str, u32 size){
    String8 result = {str, size};
    return(result);
}

#define str16_litteral(x) str16_((u16*)x, (sizeof(x) - 1))
#define str16(x, size) str16_((u16*)x, size)
static String16 str16_(u16* str, u32 size){
    String16 result = {str, size};
    return(result);
}
// NOTE: make a String32 and experiment with it please


// copy str8
// copy str16
// str_length:
// str_in:
// char_in:
// char_replace:
// str_index:
// str_upper:
// str_is_upper:
// str_lower:
// str_is_lower:
// str_strop:
// str_split:
// str_starts_with:
// str_ends_with:
// str_range:
// append:

#endif
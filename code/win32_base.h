#if !defined(WIN32_BASE_H)
#define WIN32_BASE_H

#include <windows.h>
#include <stdio.h>
#include "base.h"

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


///////////////////////////////
// NOTE: Win32 Memory
///////////////////////////////

static void* os_virtual_alloc(u64 size){
    // NOTE: VirtualAlloc() initializes its allocation to 0, malloc() does not
    void* result = VirtualAlloc(0, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    return(result);
}

static bool os_virtual_free(void* base){
    bool result = VirtualFree(base, 0, MEM_RELEASE);
    return(result);
}

static Arena* os_allocate_arena(size_t size){
    Arena* result = {0};
    result->base = os_virtual_alloc(size);
    result->size = size;
    result->used = 0;
    return(result);
}

///////////////////////////////
// NOTE: Win32 File I/O
///////////////////////////////

typedef struct FileData{
    void* base;
    u32 size;
} FileData;

static FileData
os_file_read(Arena* arena, char* file_name){
    FileData result;

    HANDLE file_handle = CreateFileA(file_name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(file_handle != INVALID_HANDLE_VALUE){
        LARGE_INTEGER LARGE_file_size;
        if(GetFileSizeEx(file_handle, &LARGE_file_size)){
            u32 file_size = (u32)LARGE_file_size.QuadPart;

            result.base = allocate_size(arena, file_size);
            if(result.base){
                DWORD bytes_read;
                if(ReadFile(file_handle, result.base, file_size, &bytes_read, 0)){
                    if(bytes_read == file_size){
                        result.size = file_size;
                    }
                    else{
                        print("os_read_entire_file: bytes_read != file_size");
                    }
                }
                else{
                    os_virtual_free(result.base);
                    print("os_read_entire_file: failed to read file");
                }
            }
            else{
                print("os_read_entire_file: failed to allocate memory");
            }
        }
        else{
            print("os_read_entire_file: failed to get file size");
        }
        CloseHandle(file_handle);
    }
    else{
        print("os_read_entire_file: failed to create file handle");
    }
    return(result);
}

static bool
os_file_write(char* file_name, FileData file){
    bool result = false;

    HANDLE file_handle = CreateFileA(file_name, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if(file_handle != INVALID_HANDLE_VALUE){
        DWORD bytes_written;
        if(WriteFile(file_handle, file.base, file.size, &bytes_written, 0)){
            result = (file.size == bytes_written);
        }
        CloseHandle(file_handle);
    }
    else{
        print("os_read_entire_file: failed to create file handle");
    }
    return(result);
}

///////////////////////////////
// NOTE: Win32 File Operations
///////////////////////////////

static bool
os_file_delete(char* file_name){
    bool result = DeleteFileA(file_name);
    return(result);
}

static bool
os_file_move(char* source_file_name, char* dest_file_name){
    bool result = MoveFileA(source_file_name, dest_file_name);
    return(result);
}

static bool
os_directory_create(char* dir_name){
    bool result = CreateDirectoryA(dir_name, 0);
    return(result);
}

static bool
os_directory_delete(char* dir_name){
    bool result = RemoveDirectoryA(dir_name);
    return(result);
}

// list out the files in a directory

///////////////////////////////
// NOTE: Win32 File Paths
///////////////////////////////

static String16
os_get_cwd(Arena* arena){
    String16 result;

    DWORD size = GetCurrentDirectoryW(0, 0);
    ScratchArena scratch(arena);
    u16* buffer = allocate_array(arena, u16, size+1);
    GetCurrentDirectoryW(size+1, (WCHAR*)buffer);
    result = str16(buffer, size);

    return(result);
}

// path to binary/.exe
// path for user config/data
// path where temporary files can go


#endif

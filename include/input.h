#ifndef INPUT_H
#define INPUT_H

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys\stat.h>

#ifdef INPUT_DEBUG
    #ifndef DPRINTF
        #define DPRINTF(...) fprintf(stderr, __VA_ARGS__);
    #endif
#else
    #ifndef DPRINTF
        #define DPRINTF(...) ;
    #endif
#endif

typedef struct BufferData {
    char* buffer;
    int lines_count;
} BufferData_t;

typedef struct PtrDataParams {
    char** ptrdata;
    int lines_count;
} PtrDataParams_t;

typedef struct FileInfo {
    FILE* stream;
    const char* filepath;
    size_t size;
} FileInfo_t;

typedef struct InputCtx {
    FileInfo_t input_file_info;
    BufferData_t buffer_data;
    PtrDataParams_t ptrdata_params;
    FileInfo_t output_file_info;
} InputCtx_t;

int ReadAndParseFile(InputCtx_t* InputCtx);

int OpenFile(FileInfo_t* file_info, const char* mode);

int CountSize(InputCtx_t* InputCtx);

int ReadText(InputCtx_t* InputCtx);

int AllocateBuffer(InputCtx_t* InputCtx);

int FillBuffer(InputCtx_t* InputCtx);

int ParseText(InputCtx_t* InputCtx);

int CountLines(InputCtx_t* InputCtx);

int AllocatePtrdata(InputCtx_t* InputCtx);

int FillPtrdata(InputCtx_t* InputCtx);

void AddNullTerminators(char* ptr);

#endif /* INPUT_H */

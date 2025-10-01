#ifndef INPUT_H
#define INPUT_H

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys\stat.h>

#ifdef DEBUG
    #define DPRINTF(...) fprintf(stderr, __VA_ARGS__);
#else
    #define DPRINTF(...) ;
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

typedef struct Context {
    FileInfo_t input_file_info;
    BufferData_t buffer_data;
    PtrDataParams_t ptrdata_params;
    FileInfo_t output_file_info;
} Context_t;

int ReadAndParseFile(Context_t* context);

int OpenFile(FileInfo_t* file_info, const char* mode);

int CountSize(Context_t* context);

int ReadText(Context_t* context);

int AllocateBuffer(Context_t* context);

int FillBuffer(Context_t* context);

int ParseText(Context_t* context);

int CountLines(Context_t* context);

int AllocatePtrdata(Context_t* context);

int FillPtrdata(Context_t* context);

void AddNullTerminators(char* ptr);

#endif /* INPUT_H */

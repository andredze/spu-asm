#include <stdio.h>
#include "input.h"
#include "colors.h"

const int MAX_FILENAME_LEN = 30;
const int FRAMES_COUNT = 6572; // 6572
const int STRING_LEN = 96;

int ConvertFrame(int i, FILE* stream);

int main()
{
    printf(BLUE "Converting started...\n" RESET_CLR);

    FILE* stream = fopen("badapple/badapple.txt", "w");

    for (int i = 1; i <= FRAMES_COUNT; i++)
    {
        if (ConvertFrame(i, stream))
        {
            fclose(stream);
            printf("Frame %d conversion failed\n", i);
            return 1;
        }
        if (i % 1000 == 0)
        {
            printf(LIGHT_YELLOW "Converted %d files..\n" RESET_CLR, i);
        }
    }

    fclose(stream);
    printf(GREEN "Success: converted %d frames\n" RESET_CLR, FRAMES_COUNT);

    return 0;
}

int ConvertFrame(int i, FILE* output_stream)
{
    char input_filename[MAX_FILENAME_LEN] = "";
    sprintf(input_filename, "badapple/frames/out%04d.jpg.txt", i);

    InputCtx_t input_ctx = {.input_file_info =  {.filepath = input_filename}};

    if (ReadAndParseFile(&input_ctx))
    {
        printf("Input %d frame failed\n", i);
        return 1;
    }

    char* line = NULL;
    int ram_index = 0;

    fprintf(output_stream, "PUSH 0\nPOPR RAX\n");

    // int flag = 1;

    for (int i = 0; i < input_ctx.ptrdata_params.lines_count; i++)
    {
        // if (flag)
        // {
        //     int len = strlen(input_ctx.ptrdata_params.ptrdata[i]);
        //     printf("len = %d\n", len);
        //     flag = 0;
        // }
        line = input_ctx.ptrdata_params.ptrdata[i];
        for (int j = 0; j < STRING_LEN; j++)
        {
            fprintf(output_stream, "PUSH %d\nPOPM [RAX]\nPUSHR RAX\nPUSH 1\nADD\nPOPR RAX\n", line[j], ram_index);
            ram_index++;
        }
    }

    fprintf(output_stream, "DRAW 100\n");

    return 0;
}
// TODO: POPM [] может принимать просто число - номер элемента в ram
// RAM = (len = STRING_LEN) x (str_count = ...)

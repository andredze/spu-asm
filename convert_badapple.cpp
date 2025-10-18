#include <stdio.h>
#include "input.h"

const int MAX_FILENAME_LEN = 30;

int ConvertFrame(int i, FILE* stream);

int main()
{
    FILE* stream = fopen("badapple.txt", "w");

    for (int i = 1; i < 6573; i++)
    {
        ConvertFrame(i, stream);
    }
}

int ConvertFrame(int i, FILE* output_stream)
{
    char input_filename[MAX_FILENAME_LEN] = "";
    sprintf(input_filename, "output_%04d.txt", i)
    FILE* input_stream = fopen(input_filename, "rb");
    if (input_stream == NULL)
    {
        printf("Opening frame %d failed\n", i);
        return 1;
    }

    InputCtx_t input_ctx = {.input_file_info =  {.filepath = input_filename}};

    if (ReadAndParseFile(&input_ctx))
    {
        printf("Oneg error\n");
        return 1;
    }

    char* line = NULL;
    int len = 0;

    for (int i = 0; i < input_ctx.ptrdata_params.lines_count; i++)
    {
        line = input_ctx.ptrdata_params.ptrdata[i];
        len = strlen(line);
        for (int j = 0; j < len; j++)
        {
            fprintf(output_stream, "PUSH %c\nPOPM [%d]", line[j], ram_index);
        }
    }

    fprintf(output_stream, "PUSH %c", )

}
// TODO: POPM [] может принимать просто число - номер элемента в ram

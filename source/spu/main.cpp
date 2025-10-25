#include "processor.h"
#include "config.h"

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>

#pragma comment(lib, "winmm.lib")

// Боря и Захар просмотр кода
// NOTE: atexit - выволнит если return 0 in main
// Можно возвращать ошибку с внутренних функций до main - SAFE_CALL()
// do while(0) - вызывать destroy при ошибках
// Printerr -> __LINE__ __FUNC__ и т.д.
// bsearch() - есть встроенный bin search ()

int main(int argc, char* argv[])
{
    char music_filepath[MAX_MUSIC_PATH_LEN] = "open \"";
    const char* code_filename = NULL;
    int play_music = 0;

    if (SpuSetFilenames(&code_filename, argc, argv, &play_music, music_filepath))
    {
        return EXIT_FAILURE;
    }
    DPRINTF("filename = %s\n", code_filename);

    if (play_music)
    {
        strcat(music_filepath, "\" type mpegvideo alias mymusic");
        mciSendString(music_filepath, NULL, 0, NULL); // D:\\Coding\\spu-asm\\badapple\\music.wav
        mciSendString("play mymusic", NULL, 0, NULL);
    }

    Proc_t proc_data = {};

    if (ProcCtor(&proc_data) != PROC_SUCCESS)
    {
        return EXIT_FAILURE;
    }
    if (ProcLoadCode(&proc_data, code_filename) != PROC_SUCCESS)
    {
        return EXIT_FAILURE;
    }

#ifdef PROC_DEBUG
    if (ProcConsoleDump(&proc_data) != PROC_SUCCESS)
    {
        return EXIT_FAILURE;
    }
    if (ProcVerify(&proc_data) != PROC_SUCCESS)
    {
        DPRINTF("Proc verify failed\n");
        return EXIT_FAILURE;
    }
    ProcDump(&proc_data, PROC_SUCCESS);
#endif /* PROC_DEBUG */

    if (ProcExecuteCommands(&proc_data) != PROC_SUCCESS)
    {
        return EXIT_FAILURE;
    }
    if (ProcDtor(&proc_data) != PROC_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    if (play_music)
    {
        mciSendString("stop mymusic",  NULL, 0, NULL);
        mciSendString("close mymusic", NULL, 0, NULL);
    }

    return EXIT_SUCCESS;
}

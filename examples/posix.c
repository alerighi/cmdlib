#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#include "../cmdlib.h"

Cmdlib_Command_t table[] = {
    { NULL, NULL, NULL },
};


void prompt()
{
    printf("> ");
}


int main()
{
    char ch;
    struct termios termios;
    Cmdlib_Context_t ctx;

    /* unbuffered terminal */
    tcgetattr(STDIN_FILENO, &termios);
    termios.c_lflag &= ~ICANON;
    termios.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &termios);

    memset(&ctx, 0, sizeof(ctx));
    ctx.cmdTable = table;
    ctx.printf = printf;
    ctx.prompt = prompt;

    Cmdlib_Init(&ctx);

    while (1)
    {
        read(STDERR_FILENO, &ch, 1);
        Cmdlib_ProvideCharacter(&ctx, ch);
        fflush(stdout);
    }

    return EXIT_SUCCESS;
}
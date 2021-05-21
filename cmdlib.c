#include <stdio.h>
#include <string.h>

#include "cmdlib.h"

static int Cmdlib_Help(Cmdlib_Context_t *ctx)
{
    Cmdlib_Command_t *entry;

    ctx->printf("Available commands:\n");
    for (entry = ctx->cmdTable; entry->name != NULL; entry++)
    {
        ctx->printf("%s\n", entry->name);
    }

    return CMDLIB_EXIT_SUCCESS;
}

static Cmdlib_Command_t *Cmdlib_FindCommand(Cmdlib_Context_t *ctx, const char *cmd)
{
    Cmdlib_Command_t *current = ctx->cmdTable;

    while (current->name != NULL)
    {
        if (strcmp(current->name, cmd) == 0)
        {
            return current;
        }

        current++;
    }

    return NULL;
}

static void Cmdlib_Autocomplete(Cmdlib_Context_t *ctx)
{
}

static int Cmdlib_Execute(Cmdlib_Context_t *ctx)
{
    int status = 0;
    int i;

    ctx->argc = 0;
    for (i = 0; i < ctx->lineLength; i++)
    {
        if (ctx->line[i] == ' ')
        {
            ctx->line[i] = '\0';
        }
        if (i == 0 || (ctx->line[i] != '\0' && ctx->line[i - 1] == '\0'))
        {
            ctx->argv[ctx->argc++] = &ctx->line[i];
        }
    }

    if (strcmp(ctx->argv[0], "help") == 0 || strcmp(ctx->argv[0], "?") == 0)
    {
        status = Cmdlib_Help(ctx);
    }
    else
    {
        ctx->currentCommand = Cmdlib_FindCommand(ctx, ctx->argv[0]);

        if (ctx->currentCommand == NULL)
        {
            status = CMDLIB_EXIT_COMMAND_NOT_FOUND;
        }
        else if (Cmdlib_HasArgument(ctx, "-h") || Cmdlib_HasArgument(ctx, "--help") || Cmdlib_HasArgument(ctx, "help"))
        {
            ctx->printf("%s\n", ctx->currentCommand->help);
        }
        else
        {
            status = ctx->currentCommand->handler(ctx);
        }
    }

    switch (status)
    {
        case CMDLIB_EXIT_SUCCESS:
            ctx->printf("+OK\n");
            break;
        case CMDLIB_EXIT_INVALID_ARGS:
            ctx->printf("-ERROR #%d: invalid argument\n", status);
            break;
        case CMDLIB_EXIT_COMMAND_NOT_FOUND:
            ctx->printf("-ERROR #%d: command %s not found\n", status, ctx->argv[0]);
            break;
        case CMDLIB_EXIT_NOT_IMPLEMENTED:
            ctx->printf("-ERROR #%d: feature not implemented\n", status);
            break;
        default:
            ctx->printf("-ERROR #%d\n", status);
    }

    return status;
}

void Cmdlib_Init(Cmdlib_Context_t *ctx)
{
    ctx->prompt(ctx);
}

void Cmdlib_ProvideCharacter(Cmdlib_Context_t *ctx, char ch)
{
    if (ctx->lastEscape)
    {
        ctx->lastEscape = 0;
        switch (ch)
        {
            case '[':
                /* thre is another character to check */
                ctx->lastEscape = 1;
                break;
            /* history up */
            case 'A':
                /* check if I'm really going back in the history */
                if ((ctx->historyPosition - 1) % CMDLIB_HISTORY_SIZE != ctx->historyEnd % CMDLIB_HISTORY_SIZE
                    && ctx->history[(ctx->historyPosition - 1) % CMDLIB_HISTORY_SIZE][0] != '\0')
                {
                    ctx->historyPosition--;
                    strcpy(ctx->line, ctx->history[ctx->historyPosition % CMDLIB_HISTORY_SIZE]);
                    ctx->lineLength = strlen(ctx->line);

                    /* print line from history */
                    ctx->printf("\033[2K\r");
                    ctx->prompt(ctx);
                    ctx->printf("%s", ctx->line);
                }

                break;
            /* history down */
            case 'B':
                /* check if I'm really going forward in the history */
                if ((ctx->historyPosition + 1) % CMDLIB_HISTORY_SIZE != (ctx->historyEnd + 1) % CMDLIB_HISTORY_SIZE
                    && ctx->history[(ctx->historyPosition + 1) % CMDLIB_HISTORY_SIZE][0] != '\0')
                {
                    ctx->historyPosition++;
                    strcpy(ctx->line, ctx->history[ctx->historyPosition % CMDLIB_HISTORY_SIZE]);
                    ctx->lineLength = strlen(ctx->line);

                    /* print line from history */
                    ctx->printf("\033[2K\r");
                    ctx->prompt(ctx);
                    ctx->printf("%s", ctx->line);
                }
                break;
        }
    }
    else
    {
        switch (ch)
        {
            case '\n':
            case '\r':
                ctx->printf("\n");
                if (ctx->lineLength > 0)
                {
                    strcpy(ctx->history[ctx->historyEnd % CMDLIB_HISTORY_SIZE], ctx->line);
                    Cmdlib_Execute(ctx);
                    ctx->historyEnd++;
                    ctx->historyPosition = ctx->historyEnd;
                }
                ctx->prompt(ctx);
                ctx->lineLength = 0;
                break;
            case '\t':
                Cmdlib_Autocomplete(ctx);
                break;
            case '\b':
                if (ctx->lineLength > 0)
                {
                    ctx->line[ctx->lineLength--] = '\0';
                    ctx->printf("%c", ch);
                }
                break;
            case '\033':
                ctx->lastEscape = 1;
                break;
            default:
                if (ctx->lineLength < CMDLIB_MAX_COMMAND_LINE_LENGTH - 2)
                {
                    ctx->line[ctx->lineLength++] = ch;
                    ctx->line[ctx->lineLength] = '\0';
                    ctx->printf("%c", ch);
                }
        }
    }
}

int Cmdlib_HasArgument(Cmdlib_Context_t *ctx, const char *arg)
{
    int i;

    for (i = 0; i < ctx->argc; i++)
    {
        if (strcmp(ctx->argv[i], arg) == 0)
        {
            return 1;
        }
    }

    return 0;
}

const char *Cmdlib_ArgumentValue(Cmdlib_Context_t *ctx, const char *arg)
{
    int i;

    for (i = 0; i < ctx->argc - 1; i++)
    {
        if (strcmp(ctx->argv[i], arg) == 0)
        {
            return ctx->argv[i + 1];
        }
    }

    return NULL;
}
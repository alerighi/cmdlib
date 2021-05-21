#ifndef CMDLIB_H
#define CMDLIB_H

/**
 * Defines the number of entries that should be saved in the command history.
 */
#define CMDLIB_HISTORY_SIZE 10

/**
 * Defines the maximum length of a command line.
 */
#define CMDLIB_MAX_COMMAND_LINE_LENGTH 128

/**
 * Maximum arguments per command
 */
#define CMDLIB_MAX_ARGUMENTS 10

/**
 * Maximum cwd length
 */
#define CMDLIB_MAX_CWD_LENGTH 128

/**
 * Exit code of a command.
 */
enum Cmdlib_ExitCode
{
    /**
     * command exited succesfully
     */
    CMDLIB_EXIT_SUCCESS,

    /**
     * indicate a generic faulure
     */
    CMDLIB_EXIT_FAILURE,

    /**
     * invalid command line arguments. Causes usage print
     */
    CMDLIB_EXIT_INVALID_ARGS = -1,

    /**
     * command not found
     */
    CMDLIB_EXIT_COMMAND_NOT_FOUND = -2,

    /**
     * command not implemented
     */
    CMDLIB_EXIT_NOT_IMPLEMENTED = -3,

    /**
     * filesystem error
     */
    CMDLIB_EXIT_FILESYSTEM_ERROR = -4,

};

struct Cmdlib_Context;

/**
 * Handler of for a command, C main like.
 *
 * @param ctx context of the command parsing library
 * @return CMDLIB_EXIT_SUCCESS in case of success, or a failure code
 */
typedef int Cmdlib_Handler_t(struct Cmdlib_Context *ctx);

/**
 * A function like printf.
 *
 * @param fmt format to print
 * @param ... argumetns
 * @return number of characters written
 */
typedef int Cmdlib_Printf_t(const char *fmt, ...);

/**
 * A function to print a custom prompt
 *
 * @param ctx command parser context
 */
typedef void Cmdlib_Prompt_t(struct Cmdlib_Context *ctx);

/**
 * A command descriptor
 */
typedef struct Cmdlib_Command
{
    /**
     * name of the command, as typed on the command line.
     * NULL indicated the last entry of the command table.
     */
    const char *name;

    /**
     * help string for this command. Printed when asking for help,
     * with cmd --help or cmd -help or help cmd.
     */
    const char *help;

    /**
     * pointer to the command handler function
     */
    Cmdlib_Handler_t *handler;
} Cmdlib_Command_t;


/**
 * The context of the command parsing library.
 */
typedef struct Cmdlib_Context
{
    /**
     * function to write to standard output.
     */
    Cmdlib_Printf_t *printf;

    /**
     * pointer to an array of commands.
     * The last command must have the name set to NULL.
     */
    Cmdlib_Command_t *cmdTable;

    /**
     * function print the prompt.
     * Should not print a newline at the end.
     */
    Cmdlib_Prompt_t *prompt;

    /**
     * history of the commands
     */
    char history[CMDLIB_MAX_COMMAND_LINE_LENGTH][CMDLIB_HISTORY_SIZE];

    /**
     * current line
     */
    char line[CMDLIB_MAX_COMMAND_LINE_LENGTH];

    /**
     * line length
     */
    int lineLength;

    /**
     * current position in the command history.
     * Can never be higher than historyEnd.
     */
    int historyPosition;

    /**
     * end of the history.
     * This is the last entry put into it.
     * It will increment at each command execution
     */
    int historyEnd;

    /**
     * 1 if last character was an escape character
     */
    int lastEscape;

    /**
     * pointer to the current command
     */
    Cmdlib_Command_t *currentCommand;

    /**
     * current command argv, null terminated
     */
    const char *argv[CMDLIB_MAX_ARGUMENTS + 1];

    /**
     * current command argc
     */
    int argc;
} Cmdlib_Context_t;

/**
 * Initializes the command parsing library
 *
 * @param ctx the context of the command parsing library
 */
void Cmdlib_Init(Cmdlib_Context_t *ctx);

/**
 * Provied a character to the command parsing library.
 * Typically invoked when reading a character from the serial port.
 *
 * @param ctx the context of the command parsing library
 * @param ch the characters read from the user
 */
void Cmdlib_ProvideCharacter(Cmdlib_Context_t *ctx, char ch);

/**
 * Check whatever the command was invoked with or without a particular argument.
 *
 * @param ctx context of the command parser
 * @param arg name of the argument to check
 * @return 1 if the argument is set, 0 if not
 */
int Cmdlib_HasArgument(Cmdlib_Context_t *ctx, const char *arg);

/**
 * Get the value of an argument.
 *
 * @param ctx context of the command parser
 * @param arg name of the argument to check
 * @return the value of the argument if set, NULL if argument not set.
 */
const char *Cmdlib_ArgumentValue(Cmdlib_Context_t *ctx, const char *arg);

#endif
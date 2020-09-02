#ifndef CONSTANTS
#define CONSTANTS

#define DISK_FILENAME          "disk.json"
#define MAX_FILE_NAME           24
#define MAX_INPUT_SIZE          512
#define BUFFER_SIZE             10
#define MAX_REDIRECTION_SIZE    4096
#define SHELL_BUFFER_SIZE       64
#define ASCII_OFFSET            48
#define DISK_BYTES_LIMIT        1024000

#define MAX_FILES_DIR           5
#define MAX_DATA_BLOCKS         5
#define MAX_DIR_DIR             5

#define TEXT                    1
#define BINARY                  2
#define DIRECTORY               3

#define RESET       "\033[0m"               /* Resets the style */
#define BLACK       "\033[30m"              /* Black */
#define RED         "\033[31m"              /* Red */
#define GREEN       "\033[32m"              /* Green */
#define YELLOW      "\033[33m"              /* Yellow */
#define BLUE        "\033[34m"              /* Blue */
#define MAGENTA     "\033[35m"              /* Magenta */
#define CYAN        "\033[36m"              /* Cyan */
#define WHITE       "\033[37m"              /* White */
#define BOLDBLACK   "\033[1m\033[30m"       /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"       /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"       /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"       /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"       /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"       /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"       /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"       /* Bold White */
#define HIGHTLIGHT  "\033[7m"               /* Highlight (Inverse) */

#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <limits.h>

#include "directory.h"

#define MAX_INPUT_SIZE 1024
#define PATH_MAX 4096
#define HOST_NAME_MAX 64

// ANSI escape codes for text color
#define COLOR_DARK_PURPLE "\033[38;5;53m"
#define COLOR_TEAL "\033[38;5;30m"
#define COLOR_RESET "\033[0m"

// function to replace home directory with ~ in the path
void replaceHomeWithPath(char *path)
{
    struct passwd *pw = getpwuid(getuid());
    const char *homeDir = pw->pw_dir;
    size_t homeDirLen = strlen(homeDir);

    if (strncmp(path, homeDir, homeDirLen) == 0)
    {
        memmove(path, "~", 1);
        memmove(path + 1, path + homeDirLen, strlen(path) - homeDirLen + 1);
    }
}

int main()
{
    char input[MAX_INPUT_SIZE];
    char currentDir[PATH_MAX];
    char hostName[HOST_NAME_MAX];
    struct passwd *pw = getpwuid(getuid());

    while (1)
    {
        if (gethostname(hostName, HOST_NAME_MAX) != 0)
        {
            perror("Error: gethostname");
            break;
        }

        if (getcwd(currentDir, PATH_MAX) == NULL)
        {
            perror("Error: getcwd");
            break;
        }

        replaceHomeWithPath(currentDir);

        printf("%s%s@%s:%s%s$ %s", COLOR_DARK_PURPLE, pw->pw_name, hostName, COLOR_TEAL, currentDir, COLOR_RESET);

        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL)
        {
            break;
        }

        // remove the newline character at the end of the input
        input[strcspn(input, "\n")] = '\0';

        // declare an array to store tokens
        char *tokens[MAX_INPUT_SIZE];

        // tokenize input
        char *token = strtok(input, " ");
        int tokenCount = 0; // to keep track of the number of tokens

        while (token != NULL)
        {
            // store the token in the tokens array
            tokens[tokenCount] = token;
            tokenCount++;

            token = strtok(NULL, " ");
        }

        // handle commands

        // quit shell on "exit" command
        if (tokenCount == 1 && strcmp(tokens[0], "exit") == 0)
        {
            printf("Exiting Shell..\n");
            exit(0);
        }

        if (tokenCount > 0)
        {
            if (strcmp(tokens[0], "cd") == 0)
            {
                if (tokenCount < 2)
                {
                    if (changeDirectory(getenv("HOME")) != 0)
                    {
                        perror("Error: cd");
                    }
                }
                else
                {
                    if (changeDirectory(tokens[1]) != 0)
                    {
                        perror("Error: cd");
                    }
                }
            }
            else if (strcmp(tokens[0], "mkdir") == 0)
            {
                if (tokenCount < 2)
                {
                    // Handle error, not enough arguments
                    printf("Usage: mkdir <directory_name>\n");
                }
                else
                {
                    if (createDirectory(tokens[1]) != 0)
                    {
                        perror("Error: mkdir");
                    }
                }
            }
            else if (strcmp(tokens[0], "rmdir") == 0)
            {
                if (tokenCount < 2)
                {
                    // Handle error, not enough arguments
                    printf("Usage: rmdir <directory_name>\n");
                }
                else
                {
                    if (deleteDirectory(tokens[1]) != 0)
                    {
                        perror("Error: rmdir");
                    }
                }
            }
            else if (strcmp(tokens[0], "exit") == 0)
            {
                printf("Exiting Shell..\n");
                exit(0);
            }
            else
            {
                // Handle other commands using execvp
                // Create a child process
                pid_t child_pid = fork();

                if (child_pid == -1)
                {
                    perror("Error: Fork Failed");
                    exit(1);
                }

                if (child_pid == 0)
                {
                    // Child process

                    // Null-terminate the tokens array
                    tokens[tokenCount] = NULL;

                    if (execvp(tokens[0], tokens) == -1)
                    {
                        perror("Error: execvp");
                        exit(1);
                    }
                }
                else
                {
                    // Parent process

                    // Wait for the child to finish
                    int status;
                    waitpid(child_pid, &status, 0);
                }
            }
        }
    }

    return 0;
}

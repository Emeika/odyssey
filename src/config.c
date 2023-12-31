#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

bool show_ascii_art = false;

void createConfigFolder()
{
    char homeDir[1024];

    // get the home directory path
    const char *homeEnv = getenv("HOME");

    if (homeEnv == NULL)
    {
        fprintf(stderr, "Error: HOME environment variable not set.\n");
        exit(EXIT_FAILURE);
    }

    strcpy(homeDir, homeEnv);

    // create the full path for the config folder
    char configFolderPath[strlen(homeDir) + 1 + strlen(CONFIG_FOLDER_NAME) + 1];
    snprintf(configFolderPath, sizeof(configFolderPath), "%s/%s", homeDir, CONFIG_FOLDER_NAME);

    // check if the folder already exists, create it if not
    struct stat st = {0};
    if (stat(configFolderPath, &st) == -1)
    {
        if (mkdir(configFolderPath, 0700) != 0)
        {
            perror("Error creating config folder");
            exit(EXIT_FAILURE);
        }

        // create the full path for the config file
        char configFilePath[strlen(configFolderPath) + 1 + strlen(CONFIG_FILE_NAME) + 1];
        snprintf(configFilePath, sizeof(configFilePath), "%s/%s", configFolderPath, CONFIG_FILE_NAME);

        // create and open the config file
        FILE *configFile = fopen(configFilePath, "w");
        if (configFile == NULL)
        {
            perror("Error creating config file");
            exit(EXIT_FAILURE);
        }

        // --------WRITE DEFAULT CONFIG TO FILE-------
        fprintf(configFile, "show_ascii_art=true\n");

        // close the file
        fclose(configFile);
    }
    else
    {
        printf("Config folder already exists.\n");
    }
}

void loadConfig()
{
    char *home_dir = getenv("HOME");
    char config_path[strlen(home_dir) + 1 + strlen(CONFIG_FOLDER_NAME) + 1 + strlen(CONFIG_FILE_NAME) + 1];

    snprintf(config_path, sizeof(config_path), "%s/%s/%s", home_dir, CONFIG_FOLDER_NAME, CONFIG_FILE_NAME);

    FILE *fp = fopen(config_path, "r");

    if (fp == NULL)
    {
        createConfigFolder();
        fp = fopen(config_path, "r");
    }

    // ---------LOAD CONFIG FROM FILE----------
    char setting[64];
    fscanf(fp, "show_ascii_art=%s", setting);

    if (strcmp(setting, "true") == 0)
    {
        show_ascii_art = true;
    }
    else
    {
        show_ascii_art = false;
    }

    fclose(fp);
}

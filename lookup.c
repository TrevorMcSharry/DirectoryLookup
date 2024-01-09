// Trevor McSharry
// lookup.c
// A program that parses a directory and looks for readable .txt files and searches for the keyword
// 12/13/2023

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <stdint.h>

int main(int argc, char *argv[]){

    if(argc < 3){
        printf("Usage: %s <directory> <keyword>\n", argv[0]);
        return 1;
    }
    char *directory = argv[1];
    char *keyword = argv[2];
    char **lines = malloc(4096 * sizeof(char*));
    
    // Open the directory
    DIR *dir = opendir(directory);
    struct dirent *entry;

    if(dir == NULL){
        printf("Error opening directory\n");
        return 1;
    }

    // Iterate through the directory
    while((entry = readdir(dir))){   
        char line[4096];
        char line2[4096];

        int occurences = 0;
        char* path = entry->d_name;
        char *fullPath;
        if(strcmp(path, ".") == 0 || strcmp(path, "..") == 0) continue;

        size_t fullPathLength = strlen(directory) + strlen(path) + 2;
        fullPath = (char *)malloc(fullPathLength * sizeof(char));
        snprintf(fullPath, fullPathLength, "%s/%s", directory, path);
        FILE* file = fopen(fullPath, "r");
        if(file == NULL) continue;

        int lineNum = 1;
        int lineNums[4096];

        // Read the file line by line
        while(fgets(line, 4096, file) != NULL){
        
            sprintf(line2, "%s", line);

            // Removes newline characters
            line[strcspn(line, "\n")] = 0;

            char *token = strtok(line, " ");
            char *words[4096];
            int i = 0;

            while(token != NULL){
                words[i] = token;
                i++;
                token = strtok(NULL, " ");
            }
            
            // Check words for keyword
            for(int j = 0; j < i; j++){

                // printf("%s\n", words[j]);

                if(strncmp(words[j], keyword, strlen(keyword)) == 0){
                    lineNums[occurences] = lineNum;
                    lines[occurences] = strdup(line2);
                    occurences++;
                    break;
                }
            }
            lineNum++;
        }
        if(occurences == 1){
            printf("%s (1 line)\n", fullPath);
            printf("    line %-5d: %s", lineNums[0], lines[0]);
        }
        else if(occurences > 1){
            printf("%s (%d lines)\n", fullPath, occurences);
            for(int i = 0; i < occurences; i++){
                printf("    line %-5d: %s", lineNums[i], lines[i]);
            }
        }
        fclose(file);
        free(fullPath);
    }

    closedir(dir);

    return 0;
}

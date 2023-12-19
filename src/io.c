#include "io.h"

#include "parser.h"
#include "display.h"
#include "format.h"

int compare_integers(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

int sort(const struct dirent **a, const struct dirent **b) {
    int number_a = strtol((*a)->d_name, NULL, 10);
    int number_b = strtol((*b)->d_name, NULL, 10);
    return compare_integers(&number_a, &number_b);
}

void processFolder(int argc, char **argv){
    char* inputFolder;
    char* outputFolder;
    int displayOutput = 0;

    inputFolder = malloc(sizeof(char) * 1000);
    outputFolder = malloc(sizeof(char) * 1000);
    sprintf(outputFolder, "images/");

    parseArguments(argc, argv, inputFolder, outputFolder, &displayOutput);

    if (access(outputFolder, F_OK) == -1) {
        mkdir(outputFolder, 0777);
    }

    DIR *dir = opendir(inputFolder);

    if (dir) {
        struct dirent **namelist;
        int num_entries = scandir(inputFolder, &namelist, 0, sort);

        if (num_entries < 0) {
            perror("scandir");
            exit(EXIT_FAILURE);
        }
        
        if (displayOutput) {
            display(inputFolder, outputFolder, num_entries, namelist);
        }
        else
        {
            for (int i = 0; i < num_entries; ++i) {
                if (namelist[i]->d_type == DT_REG) {
                    char *inputFilename = malloc(sizeof(char) * 1000);
                    char *outputFilename = malloc(sizeof(char) * 1000);
                    sprintf(inputFilename, "%s/%s", inputFolder, namelist[i]->d_name);
                    sprintf(outputFilename, "%s%s", outputFolder, namelist[i]->d_name);
                    outputFilename[strlen(outputFilename) - 2] = 'p';
                    pgmToPpm(inputFilename, outputFilename);
                    free(inputFilename);
                    free(outputFilename);
                }
                free(namelist[i]);
            }
        }

        free(namelist);
        closedir(dir);
    } else {
        fprintf(stderr, "Le dossier %s n'existe pas.\n", inputFolder);
        exit(EXIT_FAILURE);
    }
}
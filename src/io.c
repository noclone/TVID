#include "io.h"

#include "parser.h"
#include "display.h"
#include "format.h"
#include "header.h"
#include "bob.h"

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
    char* headerFileName;
    int displayOutput = 0;
    int frame_rate = 0;

    inputFolder = malloc(sizeof(char) * 1000);
    outputFolder = malloc(sizeof(char) * 1000);
    headerFileName = malloc(sizeof(char) * 1000);
    sprintf(outputFolder, "images/");

    parseArguments(argc, argv, inputFolder, outputFolder, &displayOutput, &frame_rate, headerFileName);

    int forceFrameRate = 1;
    if (frame_rate == 0){
        forceFrameRate = 0;
        frame_rate = 25;
    }

    if (access(outputFolder, F_OK) == -1) {
        mkdir(outputFolder, 0777);
    }

    DIR *dir = opendir(inputFolder);

    if (dir) {
        struct dirent **namelist;

        int num_entries = scandir(inputFolder, &namelist, 0, sort);

        Header* header = malloc(sizeof(Header));
        parseHeaderFile(headerFileName, header);

        if (num_entries < 0) {
            perror("scandir");
            exit(EXIT_FAILURE);
        }
        
        if (displayOutput) {
            printf("frame_rate: %d\n", frame_rate);
            display(inputFolder, num_entries, namelist, frame_rate, header, forceFrameRate);
        }
        else
        {
            int frame_number = 0;
            char *subFolder = "bob";
            char *subFolderName = malloc(sizeof(char) * 1000);
            sprintf(subFolderName, "%s/%s", outputFolder, subFolder);
            if (access(subFolderName, F_OK) == -1) {
                mkdir(subFolderName, 0777);
            }

            for (int i = 0; i < num_entries; ++i) {
                if (namelist[i]->d_type == DT_REG) {
                    char *inputFilename = malloc(sizeof(char) * 1000);
                    sprintf(inputFilename, "%s/%s", inputFolder, namelist[i]->d_name);

                    if (header->progs[frame_number] == 1) {
                        char *fileNameSub = malloc(sizeof(char) * 1000);
                        sprintf(fileNameSub, "%s/%s/%d.ppm", outputFolder, subFolder, frame_number);
                        pgmToPpm(inputFilename, fileNameSub);
                        free(fileNameSub);
                    }
                    else {
                        char *outputFilenameA = malloc(sizeof(char) * 1000);
                        sprintf(outputFilenameA, "%s/%s/%d_A.ppm", outputFolder, subFolder, frame_number);
                        char *outputFilenameB = malloc(sizeof(char) * 1000);
                        sprintf(outputFilenameB, "%s/%s/%d_B.ppm", outputFolder, subFolder, frame_number);

                        deinterlaceBob(inputFilename, header, frame_number, outputFilenameA, outputFilenameB);

                        free(outputFilenameA);
                        free(outputFilenameB);
                    }

                    free(inputFilename);
                    frame_number++;
                }
                free(namelist[i]);
            }
            free(subFolderName);
        }

        free(namelist);
        closedir(dir);

        free(header->progs);
        free(header->tffs);
        free(header->rffs);
        free(header->period_changes_indices);
        free(header->period_changes_values);
        free(header);
    } else {
        fprintf(stderr, "Le dossier %s n'existe pas.\n", inputFolder);
        exit(EXIT_FAILURE);
    }

    free(inputFolder);
    free(outputFolder);
    free(headerFileName);
}
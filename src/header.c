#include "header.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int getHeaderSize(FILE *headerFile){
    int count;
    char line[100];
    while (fgets(line, sizeof(line), headerFile) != NULL) {
        count++;
    }
    return count;
}

void parseHeaderFile(char* headerFileName, header* header){

    FILE* headerFile = fopen(headerFileName, "r");
    if (headerFile == NULL){
        exit(EXIT_FAILURE);
    }

    int size = getHeaderSize(headerFile) / 3;
    header->size = 0;
    header->progs = malloc(sizeof(int) * size);
    header->tffs = malloc(sizeof(int) * size);
    header->rffs = malloc(sizeof(int) * size);
    header->period_changes_indices = calloc(sizeof(int), size);
    header->period_changes_values = calloc(sizeof(int), size);

    rewind(headerFile);

    

    char line[50];

    int i = 0;
    int frame_period_idx = 0;

    while (fgets(line, sizeof(line), headerFile) != NULL) {
        char *name = strtok(line, ":");
        char *valueStr = strtok(NULL, ":");
        
        if (name != NULL && valueStr != NULL) {
            char *endptr;
            int value = strtol(valueStr, &endptr, 10);

            if (*endptr == '\0' || *endptr == '\n') {
                if (strcmp(name, "PROG") == 0) {
                    header->progs[i] = value;
                } else if (strcmp(name, "TFF") == 0) {
                    header->tffs[i] = value;
                } else if (strcmp(name, "RFF") == 0) {
                    header->size++;
                    header->rffs[i++] = value;
                } else if (strcmp(name, "frame_period") == 0) {
                    header->period_changes_indices[frame_period_idx] = i;
                    header->period_changes_values[frame_period_idx++] = value;
                }
            } else {
                fprintf(stderr, "Error converting value to integer: %s\n", valueStr);
            }
        } else {
            fprintf(stderr, "Error parsing line: %s\n", line);
        }
    }

    fclose(headerFile);
    
    /*printf("size: %d\n", header->size);
    int idx = 0;
    for (int i = 0; i < header->size; ++i) {
        if (i == header->period_changes_indices[idx]){
            printf("changing frame_period to: %d\n", header->period_changes_values[idx++]);
        }
        printf("prog: %d, tff: %d, rff: %d\n", header->progs[i], header->tffs[i], header->rffs[i]);
    }*/

}
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

void parseArguments(int argc, char *argv[], char *inputFolder, char *outputFolder, int *displayOutput, int *frame_rate, char *headerFileName)
{
    int opt;
    int headerFileSet = 0;

    while ((opt = getopt(argc, argv, "hdo:m:")) != -1) {
        switch (opt) {
            case 'h':
                printf("Help:\nUsage: %s INPUT_FOLDER -m HEADER_FILE [-do]\n-h: Display this help message.\n-d: Display the output images without saving them. (Optional) Specify the frame rate.\n-o: Specify the output folder.\n", argv[0]);
                exit(EXIT_SUCCESS);
            case 'd':
                *displayOutput = 1;
                if (optind < argc && argv[optind][0] != '-') {
                    *frame_rate = atoi(argv[optind]);
                    optind++;
                }
                break;
            case 'o':
                strcpy(outputFolder, optarg);
                break;
            case 'm':
                headerFileSet = 1;
                strcpy(headerFileName, optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s INPUT_FOLDER -m HEADER_FILE [-do]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind < argc && headerFileSet) {
        strcpy(inputFolder, argv[optind]);
    } else {
        fprintf(stderr, "Usage: %s INPUT_FOLDER -m HEADER_FILE [-do]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}

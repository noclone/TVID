#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>

void parseArguments(int argc, char *argv[], char *inputFilename, char *outputFilename, int *displayOutput, int *frame_rate, char *headerFileName) 
{
    int opt;

    while ((opt = getopt(argc, argv, "hdom:")) != -1) {
        switch (opt) {
            case 'h':
                printf("Help:\nUsage: %s INPUT_FILE [-hdo]\n-h: Display this help message.\n-d: Display the output image without saving it.\n-o: Specify the output file.\n", argv[0]);
                exit(EXIT_SUCCESS);
            case 'd':
                *displayOutput = 1;
                if (optind < argc) {
                    *frame_rate = atoi(argv[optind]);
                    optind++;
                }
                break;
            case 'o':
                strcpy(outputFilename, optarg);
                break;
            case 'm':
                strcpy(headerFileName, optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s INPUT_FILE [-hdo]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind < argc) {
        strcpy(inputFilename, argv[optind]);
    } else {
        fprintf(stderr, "Usage: %s INPUT_FILE [-hdo]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}

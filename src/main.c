#include "format.h"
#include "parser.h"
#include "display.h"

int main(int argc, char *argv[]) {


    char* inputFilename;
    char* outputFilename;
    int displayOutput = 0;

    inputFilename = malloc(sizeof(char) * 100);
    outputFilename = malloc(sizeof(char) * 100);
    sprintf(outputFilename, "output.ppm");

    parseArguments(argc, argv, inputFilename, outputFilename, &displayOutput);

    pgmToPpm(inputFilename, outputFilename);

    printf("Conversion terminée.\n");

    if (displayOutput) {
        display(outputFilename);
        remove(outputFilename);
    }

    return 0;
}

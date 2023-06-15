#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"

///////////////////////////////////////////////////////////////////////////////
// Functions Implementation

int main(int argc, char *argv[]) {

	// gets file name
	char name[strlen(argv[1]) + 3];
	strcpy(name, argv[1]);
	name[strlen(name) - 1] = '\0';

	// open source file for reading
	FILE* input = fopen(argv[1], "r");

	// checks for success
	if (input == NULL) {
		// can't open input file
	    fprintf(stderr, "Can't open source file");
	    return EXIT_FAILURE;
	}

	// opens source file for writing
	FILE* output = fopen(strcat(name, "asm"), "w");

	// checks for success
	if (output == NULL) {
		// can't open input file
	    fprintf(stderr, "Can't open destination file");
	    return EXIT_FAILURE;
	}

	// declares token and counter
	token curr;
	
	// calls next token
	while (next_token(input, &curr)) {
		generate_asm(output, input, curr);
	}

	fclose(input);

	// // writes into the new file
	// FILE* output = fopen(argv[1], "w");
	// write_out(output, instr);

	return 0;
}
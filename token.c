#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#include "token.h"

///////////////////////////////////////////////////////////////////////////////
// Token Implementation

int counter = 0;
int if_counter = 0;
int while_counter = 0;

// line_number : You can use this to keep track of the current line number you
// are at in the J source file which can be useful in reporting syntax errors.
// Be sure to include a line that says 'int line_number;' in token.c outside
// of any functions if you would like to make use of this.
extern int line_number;

// gets the next J token from the specified input file.
// 
// Arguments:
// - j_file: the file stream to read from. Should be a valid file and have read permissions.
// - output: the output parameter to "return" the read token. The type must always be set.
//           If the token is of type IDENT, the name of the identifier should
//           stored in the field `str` as a null-terminated string.
//           If the token is of type LITERAL, then the field literal_value
//           should contain the equivalent integer value of the literal token.
//           If the token is of type ARG, then the arg number 'n' should be
//           stored in the field 'arg_no'. For the remaining cases, the
//           field can be set to whatever is desired.
// 
// Return Value:
// - true if a token was read successfully
// - false if a token could not be read due to either hitting the end of file
//   or some unrecoverable error

bool next_token(FILE *j_file, token *output) {
	char read_token[MAX_TOKEN_LENGTH + 1];
	int num_read = fscanf(j_file, "%s", read_token);
	// printf("Token: %s\n", read_token);

	if (num_read == EOF) {
		return false;
	}

	// creates truncated string for HEX
	char two_trunc[MAX_TOKEN_LENGTH + 1];
	strcpy(two_trunc, read_token);
	two_trunc[2] = '\0';

	if (strcmp(two_trunc, "0x") == 0) {
		output->type = LITERAL;
		sscanf(read_token, "%x", &(output->literal_value));
	} else if (sscanf(read_token, "%d", &(output->literal_value)) == 1) {
		output->type = LITERAL;
	} else {
		if (strcmp(read_token, "defun") == 0) {
			output->type = DEFUN;
		} else if (strcmp(read_token, "return") == 0) {
			output->type = RETURN;
		} else if (strcmp(read_token, "+") == 0) {
			output->type = PLUS;
		} else if (strcmp(read_token, "-") == 0) {
			output->type = MINUS;
		} else if (strcmp(read_token, "*") == 0) {
			output->type = MUL;
		} else if (strcmp(read_token, "/") == 0) {
			output->type = DIV;
		} else if (strcmp(read_token, "%") == 0) {
			output->type = MOD;
		} else if (strcmp(read_token, "and") == 0) {
			output->type = AND;
		} else if (strcmp(read_token, "or") == 0) {
			output->type = OR;
		} else if (strcmp(read_token, "not") == 0) {
			output->type = NOT;
		} else if (strcmp(read_token, "lt") == 0) {
			output->type = LT;
		} else if (strcmp(read_token, "le") == 0) {
			output->type = LE;
		} else if (strcmp(read_token, "eq") == 0) {
			output->type = EQ;
		} else if (strcmp(read_token, "ge") == 0) {
			output->type = GE;
		} else if (strcmp(read_token, "gt") == 0) {
			output->type = GT;
		} else if (strcmp(read_token, "if") == 0) {
			output->type = IF;
		} else if (strcmp(read_token, "else") == 0) {
			output->type = ELSE;
		} else if (strcmp(read_token, "endif") == 0) {
			output->type = ENDIF;
		} else if (strcmp(read_token, "while") == 0) {
			output->type = WHILE;
		} else if (strcmp(read_token, "endwhile") == 0) {
			output->type = ENDWHILE;
		} else if (strcmp(read_token, "drop") == 0) {
			output->type = DROP;
		} else if (strcmp(read_token, "dup") == 0) {
			output->type = DUP;
		} else if (strcmp(read_token, "swap") == 0) {
			output->type = SWAP;
		} else if (strcmp(read_token, "rot") == 0) {
			output->type = ROT;
 		} else { // cases for ARGS, HEX, and IDENT
 			// creates truncated string for args
			char three_trunc[MAX_TOKEN_LENGTH + 1];
			strcpy(three_trunc, read_token);
			three_trunc[3] = '\0';
			// creates truncated string for comments
			char one_trunc[MAX_TOKEN_LENGTH + 1];
			strcpy(one_trunc, read_token);
			one_trunc[1] = '\0';

			// checks for ARGS and HEX
			if (strcmp(three_trunc, "arg") == 0) {
				char *arg_value = &read_token[3];
				sscanf(arg_value, "%d", &(output->arg_no));
				output->type = ARG;
			} else if (strcmp(two_trunc, "0x") == 0) {
				output->type = LITERAL;
				sscanf(read_token, "%x", &(output->literal_value));
			} else if (strcmp(two_trunc, ";;") == 0) {
				char c;
				while ((c = getc(j_file)) != EOF) {
					if (c == '\n') {
						next_token(j_file, output);
						break;
					}
				}
			} else if (strcmp(one_trunc, ";") == 0) {
				char c;
				while ((c = getc(j_file)) != EOF) {
					if (c == '\n') {
						next_token(j_file, output);
						break;
					}
				}
			} else {
				output->type = IDENT;
				sscanf(read_token, "%s", output->str);
			}
		}
	}

	return true;
}

// Extra function which you may wish to define and use, but are not required to use

// Prints a token to the specified file
//
// Arguments:
// - f: a file stream that should be valid and have write permissions.
//      The token is printed to this file in whatever format you like.
//      To print to the terminal, pass in 'std_out'.
// - to_print: the token to be printed to the file
void print_token (FILE* f, token to_print) {

}

void generate_asm (FILE* f, FILE* input_f, token output) {
	if (output.type == LITERAL) {
		uint16_t upper_bits = (output.literal_value & 0xFF00) >> 8;
		uint16_t lower_bits = output.literal_value & 0xFF;

		fprintf(f, "ADD R6, R6, #-1\n");

		fprintf(f, "CONST R0, #%d\n", lower_bits);
		fprintf(f, "HICONST R0, #%d\n", upper_bits);
		fprintf(f, "STR R0, R6, #0\n");
	} else if (output.type == PLUS) {
		fprintf(f, "LDR R1, R6, #0\n");
		fprintf(f, "LDR R2, R6, #1\n");
		fprintf(f, "ADD R6, R6, #1\n");
		fprintf(f, "ADD R1, R1, R2\n");
		fprintf(f, "STR R1, R6, #0\n");
	} else if (output.type == MINUS) {
		fprintf(f, "LDR R1, R6, #0\n");
		fprintf(f, "LDR R2, R6, #1\n");  
		fprintf(f, "ADD R6, R6, #1\n");
		fprintf(f, "SUB R1, R1, R2\n");
		fprintf(f, "STR R1, R6, #0\n");
	} else if (output.type == MUL) {
		fprintf(f, "LDR R1, R6, #0\n");
		fprintf(f, "LDR R2, R6, #1\n");
		fprintf(f, "ADD R6, R6, #1\n");
		fprintf(f, "MUL R1, R1, R2\n");
		fprintf(f, "STR R1, R6, #0\n");
	} else if (output.type == DIV) {
		fprintf(f, "LDR R1, R6, #0\n");
		fprintf(f, "LDR R2, R6, #1\n");
		fprintf(f, "ADD R6, R6, #1\n");
		fprintf(f, "DIV R1, R1, R2\n");
		fprintf(f, "STR R1, R6, #0\n");
	} else if (output.type == MOD) {
		fprintf(f, "LDR R1, R6, #0\n");
		fprintf(f, "LDR R2, R6, #1\n");
		fprintf(f, "ADD R6, R6, #1\n");
		fprintf(f, "MOD R1, R1, R2\n");
		fprintf(f, "STR R1, R6, #0\n");
	} else if (output.type == LT) {
		counter++;

		// load values and updates R6
		fprintf(f, "LDR R1, R6, #0\n");
		fprintf(f, "LDR R2, R6, #1\n");
		fprintf(f, "ADD R6, R6, #1\n");

		// compares and jumps accordingly
		fprintf(f, "CMP R1, R2\n");

		// if a < b
		fprintf(f, "BRn SET_LT_%d\n", counter);
		fprintf(f, "CONST R1, #0\n");
		fprintf(f, "JMP AFTER_SET_LT_%d\n", counter);

		// if not the case
		fprintf(f, "SET_LT_%d\n", counter);
		fprintf(f, "CONST R1, #1\n");
		fprintf(f, "AFTER_SET_LT_%d\n", counter);

		// stores the value
		fprintf(f, "STR R1, R6, #0\n");
	} else if (output.type == LE) {
		counter++;

		// load values and updates R6
		fprintf(f, "LDR R1, R6, #0\n");
		fprintf(f, "LDR R2, R6, #1\n");
		fprintf(f, "ADD R6, R6, #1\n");

		// compares and jumps accordingly
		fprintf(f, "CMP R1, R2\n");

		// if a <= b
		fprintf(f, "BRnz SET_LE_%d\n", counter);
		fprintf(f, "CONST R1, #0\n");
		fprintf(f, "JMP AFTER_SET_LE_%d\n", counter);

		// if not the case
		fprintf(f, "SET_LE_%d\n", counter);
		fprintf(f, "CONST R1, #1\n");
		fprintf(f, "AFTER_SET_LE_%d\n", counter);

		// stores the value
		fprintf(f, "STR R1, R6, #0\n");
	} else if (output.type == EQ) {
		counter++;

		// load values and updates R6
		fprintf(f, "LDR R1, R6, #0\n");
		fprintf(f, "LDR R2, R6, #1\n");
		fprintf(f, "ADD R6, R6, #1\n");

		// compares and jumps accordingly
		fprintf(f, "CMP R1, R2\n");

		// if a = b
		fprintf(f, "BRz SET_EQ_%d\n", counter);
		fprintf(f, "CONST R1, #0\n");
		fprintf(f, "JMP AFTER_SET_EQ_%d\n", counter);

		// if not the case
		fprintf(f, "SET_EQ_%d\n", counter);
		fprintf(f, "CONST R1, #1\n");
		fprintf(f, "AFTER_SET_EQ_%d\n", counter);

		// stores the value
		fprintf(f, "STR R1, R6, #0\n");
	} else if (output.type == GE) {
		counter++;

		// load values and updates R6
		fprintf(f, "LDR R1, R6, #0\n");
		fprintf(f, "LDR R2, R6, #1\n");
		fprintf(f, "ADD R6, R6, #1\n");

		// compares and jumps accordingly
		fprintf(f, "CMP R1, R2\n");

		// if a = b
		fprintf(f, "BRzp SET_GE_%d\n", counter);
		fprintf(f, "CONST R1, #0\n");
		fprintf(f, "JMP AFTER_SET_GE_%d\n", counter);

		// if not the case
		fprintf(f, "SET_GE_%d\n", counter);
		fprintf(f, "CONST R1, #1\n");
		fprintf(f, "AFTER_SET_GE_%d\n", counter);

		// stores the value
		fprintf(f, "STR R1, R6, #0\n");
	} else if (output.type == GT) {
		counter++;

		// load values and updates R6
		fprintf(f, "LDR R1, R6, #0\n");
		fprintf(f, "LDR R2, R6, #1\n");
		fprintf(f, "ADD R6, R6, #1\n");

		// compares and jumps accordingly
		fprintf(f, "CMP R1, R2\n");

		// if a > b
		fprintf(f, "BRp SET_GT_%d\n", counter);
		fprintf(f, "CONST R1, #0\n");
		fprintf(f, "JMP AFTER_SET_GT_%d\n", counter);

		// if not the case
		fprintf(f, "SET_GT_%d\n", counter);
		fprintf(f, "CONST R1, #1\n");
		fprintf(f, "AFTER_SET_GT_%d\n", counter);

		// stores the value
		fprintf(f, "STR R1, R6, #0\n");
	} else if (output.type == AND) {
		// load values and updates R6
		fprintf(f, "LDR R1, R6, #0\n");
		fprintf(f, "LDR R2, R6, #1\n");
		fprintf(f, "ADD R6, R6, #1\n");

		// compares
		fprintf(f, "AND R1, R1, R2\n");

		// stores the value
		fprintf(f, "STR R1, R6, #0\n");
	} else if (output.type == OR) {
		// load values and updates R6
		fprintf(f, "LDR R1, R6, #0\n");
		fprintf(f, "LDR R2, R6, #1\n");
		fprintf(f, "ADD R6, R6, #1\n");

		// compares
		fprintf(f, "OR R1, R1, R2\n");

		// stores the value
		fprintf(f, "STR R1, R6, #0\n");
	} else if (output.type == NOT) {
		// load value
		fprintf(f, "LDR R1, R6, #0\n");

		// NOTs the value
		fprintf(f, "NOT R1, R1\n");

		// stores the value
		fprintf(f, "STR R1, R6, #0\n");
	} else if (output.type == DROP) {
		fprintf(f, "ADD R6, R6, #1\n");
	} else if (output.type == DUP) {
		// load value and decrements R6
		fprintf(f, "LDR R1, R6, #0\n");
		fprintf(f, "ADD R6, R6, #-1\n");

		// DUPS the value
		fprintf(f, "STR R1, R6, #0\n");
	} else if (output.type == SWAP) {
		// load values
		fprintf(f, "LDR R1, R6, #0\n");
		fprintf(f, "LDR R2, R6, #1\n");

		// swaps the value
		fprintf(f, "STR R1, R6, #1\n");
		fprintf(f, "STR R2, R6, #0\n");
	} else if (output.type == ROT) {
		// load values
		fprintf(f, "LDR R1, R6, #0\n");
		fprintf(f, "LDR R2, R6, #1\n");
		fprintf(f, "LDR R3, R6, #2\n");

		// swaps the value
		fprintf(f, "STR R1, R6, #1\n");
		fprintf(f, "STR R2, R6, #2\n");
		fprintf(f, "STR R3, R6, #0\n");
	} else if (output.type == IF) {
		bool seen_else = false;
		if_counter++;
		int curr_counter = if_counter;

		// stores value and sets R6
		fprintf(f, "LDR R1, R6, #0\n");
		fprintf(f, "ADD R6, R6, #1\n");

		// compares and jumps to else portion if is 0
		fprintf(f, "CMPI R1, #0\n");
		fprintf(f, "BRz ELSE_%d\n", curr_counter);

		// recursively reads future token
		while (next_token(input_f, &output)) {
			// ends if hits endif
			if (output.type == ENDIF) {
				if (!seen_else) {
					fprintf(f, "ELSE_%d\n", curr_counter);
				}
				fprintf(f, "ENDIF_%d\n", curr_counter);
				break;
			} else if (output.type == ELSE) {
				seen_else = true;
				fprintf(f, "JMP ENDIF_%d\n", curr_counter);
				fprintf(f, "ELSE_%d\n", curr_counter);
			} else {
				generate_asm(f, input_f, output);
			}
		}
	} else if (output.type == ELSE) {
	} else if (output.type == ENDIF) {
	} else if (output.type == WHILE) {
		while_counter++;
		int curr_counter = while_counter;
		// lables top of while
		fprintf(f, "WHILE_%d\n", curr_counter);

		// stores value and sets R6
		fprintf(f, "LDR R1, R6, #0\n");
		fprintf(f, "ADD R6, R6, #1\n");

		// compares and jumps to end if is 0
		fprintf(f, "CMPI R1, #0\n");
		fprintf(f, "BRz END_WHILE_%d\n", curr_counter);

		// recursively reads future token
		while (next_token(input_f, &output)) {
			// ends if hits endif
			if (output.type == ENDWHILE) {
				fprintf(f, "JMP WHILE_%d\n", curr_counter);
				fprintf(f, "END_WHILE_%d\n", curr_counter);
				break;
			} else {
				generate_asm(f, input_f, output);
			}
		}
	} else if (output.type == ENDWHILE) {
	} else if (output.type == DEFUN) {
		next_token(input_f, &output);
		if (output.type == IDENT) {
			// prologue
			fprintf(f, ".CODE\n.FALIGN\n");
			fprintf(f, "%s\n", output.str);
			fprintf(f, "ADD R6, R6, #-3\n");
			fprintf(f, "STR R5, R6, #0\n");
			fprintf(f, "STR R7, R6, #1\n");
			fprintf(f, "ADD R5, R6, #0\n");
		} else {
			printf("IDENT not following DEFUN\n");
			exit(EXIT_FAILURE);
		}
	} else if (output.type == IDENT) {
		fprintf(f, "JSR %s\n", output.str);
		fprintf(f, "ADD R6, R6, #-1\n");
	} else if (output.type == RETURN) {
		// epilogue
		fprintf(f, "LDR R7, R6, #0\n");
		fprintf(f, "STR R7, R5, #2\n");
		fprintf(f, "LDR R7, R5, #1\n");
		fprintf(f, "ADD R6, R5, #3\n");
		fprintf(f, "LDR R5, R5, #0\n");
		fprintf(f, "RET\n");
	} else if (output.type == ARG) {
		fprintf(f, "LDR R0, R5, #%d\n", output.arg_no + 2);
		fprintf(f, "ADD R6, R6, #-1\n");
		fprintf(f, "STR R0, R6, #0\n");
	}
}
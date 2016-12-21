/*
 * yad3Service_app.c
 *
 *  Created on: Apr 10, 2016
 *
 *  This is main source file, which is called first and gets
 *  input from user/file.
 *
 *  IMPORTANT: System assumes that all commands are correct, and number of
 *  parameters is also correct. Parameters values may be invalid, but they must
 *  be of correct type (e.g you can't enter a string, where number is expected
 *  and so on).
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

#include "mtm_ex2.h"
#include "yad3Service.h"
#include "utilities.h"
#include "map.h"

#define PRINT_AND_BREAK(mtm_error) mtmPrintErrorMessage(stderr,mtm_error);break;

#define PRINT_AND_RETURN(mtm_error,return_value) mtmPrintErrorMessage(stderr, \
	mtm_error); return return_value;

//====Helper types and their functions==========================================
/* Fuction pointer container. Used because casting from void* to
 * function pointer is prohibited */
typedef bool (*Yad3Function)(FILE*, FILE*, Yad3Service);
typedef struct command_t* Command;
typedef struct command_t {
	Yad3Function function;
} command_t;

/* Allocates and creates a copy of command
 * @Return
 * NULL in case of memory allocation error */
static MapDataElement commandCopy(MapDataElement command) {
	assert(command);
	Command copy = malloc(sizeof(*copy));
	NULL_PTR_CHECK(copy,NULL);
	copy->function = ((Command) command)->function;
	return copy;
}

/* Frees all memory, allocated for command */
static void commandDestroy(MapDataElement command) {
	free(command);
}

/* Adds a command to the system.
 * @param map - used to store commands
 * @param label - command text. Exactly two words with a single space character
 * 		between them.
 * @param function - function, that reads and checks parameters, and calls
 * 		yad3 function. You need to write one, if you want to add a command to
 * 		the system.
 * 	@Return:
 * 	false - in case of malloc error or if not succeed adding a command to map
 * 	true - otherwise
 * 		*/
static bool commandAdd(Map map, const char* label, Yad3Function function) {
	assert(map && label && function);
	Command new_command = malloc(sizeof(*new_command));
	NULL_PTR_CHECK(new_command, false);
	new_command->function = function;
	MapResult result = mapPut(map, label, new_command);
	free(new_command);
	return result == MAP_SUCCESS;
}

static Map createMapOfCommands() {
	return mapCreate((copyMapDataElements) commandCopy,
		(copyMapKeyElements) emailCopy, (freeMapDataElements) commandDestroy,
		(freeMapKeyElements) emailDestroy, (compareMapKeyElements)emailCompare);
}

//====Errors conversion=========================================================
/* Checks Yad3Result errors and prints them (via mtmPrintErrorMessage) if needed
 * Returns false if there's a need to exit program, because of critical error */
static bool testYad3Result(Yad3Result result, FILE* output) {
	assert(result != YAD3_NULL_ARG); //Since we call yad3 functions
	//with non-NULL parameters this error should never happen
	switch (result) {
	case YAD3_OUT_OF_MEMORY:
		mtmPrintErrorMessage(stderr, MTM_OUT_OF_MEMORY);
		return false;
	case YAD3_INVALID_PARAMETERS:
		PRINT_AND_BREAK(MTM_INVALID_PARAMETERS);
	case YAD3_EMAIL_ALREADY_EXISTS:
		PRINT_AND_BREAK(MTM_EMAIL_ALREADY_EXISTS);
	case YAD3_EMAIL_DOES_NOT_EXIST:
		PRINT_AND_BREAK(MTM_EMAIL_DOES_NOT_EXIST);
	case YAD3_EMAIL_WRONG_ACCOUNT_TYPE:
		PRINT_AND_BREAK(MTM_EMAIL_WRONG_ACCOUNT_TYPE);
	case YAD3_ALREADY_REQUESTED:
		PRINT_AND_BREAK(MTM_ALREADY_REQUESTED);
	case YAD3_NOT_REQUESTED:
		PRINT_AND_BREAK(MTM_NOT_REQUESTED);
	case YAD3_APARTMENT_SERVICE_ALREADY_EXISTS:
		PRINT_AND_BREAK(MTM_APARTMENT_SERVICE_ALREADY_EXISTS);
	case YAD3_APARTMENT_SERVICE_DOES_NOT_EXIST:
		PRINT_AND_BREAK(MTM_APARTMENT_SERVICE_DOES_NOT_EXIST);
	case YAD3_APARTMENT_SERVICE_FULL:
		PRINT_AND_BREAK(MTM_APARTMENT_SERVICE_FULL);
	case YAD3_APARTMENT_ALREADY_EXISTS:
		PRINT_AND_BREAK(MTM_APARTMENT_ALREADY_EXISTS);
	case YAD3_APARTMENT_DOES_NOT_EXIST:
		PRINT_AND_BREAK(MTM_APARTMENT_DOES_NOT_EXIST);
	case YAD3_PURCHASE_WRONG_PROPERTIES:
		PRINT_AND_BREAK(MTM_PURCHASE_WRONG_PROPERTIES);
	case YAD3_REQUEST_WRONG_PROPERTIES:
		PRINT_AND_BREAK(MTM_REQUEST_WRONG_PROPERTIES);
	case YAD3_REQUEST_ILLOGICAL_PRICE:
		PRINT_AND_BREAK(MTM_REQUEST_ILLOGICAL_PRICE);
	default:
		break;
	}
	return true;
}
//==============================================================================
/* Frees any number of strings (char*). Can receive NULLs.
 * You need to provide the number of strings as first parameter. */
static void freeStrings(int num, ...) {
	va_list arg;
	va_start(arg, num);
	for (int i = 0; i < num; i++) {
		char* tmp = va_arg(arg, char*);
		free(tmp);
	}
	va_end(arg);
}

/* Jumps to the next line of input. Useful for skipping comments */
static void skipLine(FILE* input) {
	assert(input);
	char command_line[MAX_LEN + 1];
	fgets(command_line, MAX_LEN, input);
}

/* Allocates memory and copies one first non-comment word from input.
 * Stores the word at out_word
 * @Return:
 * false in case of malloc error or if EOF encountered
 * true otherwise */
static bool getWord(FILE* input, char** out_word) {
	assert(input && out_word);
	char tmp[MAX_LEN + 1];
	int write_index = 0;
	char current;
	while (1) {
		if (fscanf(input, " %c", &current) == EOF) {
			return false;
		} else if (current == '#') {
			skipLine(input);
		} else {
			tmp[write_index++] = current;
			break;
		}
	}
	while (fscanf(input, "%c", &current) != EOF) {
		if (current == ' ' || current == '	'/*tab*/|| current == '\n'
			|| current == '\r') {
			break;
		}
		tmp[write_index++] = current;
	}
	tmp[write_index] = '\0';
	*out_word = malloc(strlen(tmp) + 1);
	NULL_PTR_CHECK(*out_word, false);
	strcpy(*out_word, tmp);
	return true;
}

/* Reads two words from command_line, skips unnecessary spaces and tabs
 * returns false if got EOF or malloc error. Stores the words (a copy) at
 * out_command. */
static bool getCommand(FILE* input, char** out_command) {
	assert(input && out_command);
	char *command, *subcommand;
	if (!getWord(input, &command)) {
		return false;
	}
	if (!getWord(input, &subcommand)) {
		free(command);
		return false;
	}
	*out_command = malloc(strlen(command) + strlen(subcommand) + 2);
	// +2 is for space between words and '\0'
	if(!out_command){
		freeStrings(2, command, subcommand);
		return false;
	}
	strcpy(*out_command, command);
	*(*out_command + strlen(command)) = ' ';
	strcpy((*out_command + strlen(command) + 1), subcommand);
	freeStrings(2, command, subcommand);
	return true;
}

/* Almost the same as getWord, but with simplified syntax. Returns pointer to
 * a new string, read from input
 * This function asserts EOF or malloc errors don't happen, while reading input
 * */
static char* readString(FILE* input) {
	assert(input);
	char* tmp;
	getWord(input, &tmp);
	assert(tmp);
	return tmp;
}

/* Reads a number from input and returns read value.
 * This function asserts EOF doesn't happen, while reading input */
static int readNumber(FILE* input) {
	assert(input);
	int tmp;
#ifndef NDEBUG
	assert(fscanf(input, " %d", &tmp) != EOF);
#else
	fscanf(input, " %d", &tmp);
#endif
	return tmp;
}
//====Yad3 functions wrappers===================================================
/*
 * For all runYad3* functions:
 * all those functions assume that number of parameters for commands is correct
 * @Return:
 * false - if critical error occurred, and there's need to exit program
 * true - otherwise
 */
bool runYad3RealtorAdd(FILE* input, FILE* output, Yad3Service yad3) {
	char *email = readString(input), *company_name = readString(input);
	int tax_percentage = readNumber(input);
	Yad3Result result = yad3RealtorAdd(yad3, email, company_name,
		tax_percentage);
	freeStrings(2, email, company_name);
	return testYad3Result(result, output);
}
bool runYad3RealtorRemove(FILE* input, FILE* output, Yad3Service yad3) {
	char *email = readString(input);
	Yad3Result result = yad3RealtorRemove(yad3, email);
	free(email);
	return testYad3Result(result, output);
}
bool runYad3RealtorAddApartment(FILE* input, FILE* output, Yad3Service yad3) {
	char *email = readString(input), *service_name = readString(input);
	int id = readNumber(input), price = readNumber(input), width = readNumber(
		input), height = readNumber(input);
	char *matrix = readString(input);
	Yad3Result result = yad3RealtorAddApartment(yad3, email, service_name, id,
		width, height, price, matrix);
	freeStrings(3, email, service_name, matrix);
	return testYad3Result(result, output);
}
bool runYad3RealtorRemoveApartment(FILE* input, FILE* output, Yad3Service yad3){
	char *email = readString(input), *service_name = readString(input);
	int id = readNumber(input);
	Yad3Result result = yad3RealtorRemoveApartment(yad3, email, service_name,
		id);
	freeStrings(2, email, service_name);
	return testYad3Result(result, output);
}
bool runYad3RealtorAddApartmentService(FILE* input, FILE* output,
	Yad3Service yad3) {
	char *email = readString(input), *service_name = readString(input);
	int max_apartments = readNumber(input);
	Yad3Result result = yad3RealtorAddApartmentService(yad3, email,
		service_name, max_apartments);
	freeStrings(2, email, service_name);
	return testYad3Result(result, output);
}
bool runYad3RealtorRemoveApartmentService(FILE* input, FILE* output,
	Yad3Service yad3) {
	char *email = readString(input), *service_name = readString(input);
	Yad3Result result = yad3RealtorRemoveApartmentService(yad3, email,
		service_name);
	freeStrings(2, email, service_name);
	return testYad3Result(result, output);
}
bool runYad3CustomerAdd(FILE* input, FILE* output, Yad3Service yad3) {
	char *email = readString(input);
	int min_area = readNumber(input), min_rooms = readNumber(input), max_price =
		readNumber(input);
	Yad3Result result = yad3CustomerAdd(yad3, email, min_area, min_rooms,
		max_price);
	free(email);
	return testYad3Result(result, output);
}
bool runYad3CustomerRemove(FILE* input, FILE* output, Yad3Service yad3) {
	char *email = readString(input);
	Yad3Result result = yad3CustomerRemove(yad3, email);
	free(email);
	return testYad3Result(result, output);
}
bool runYad3CustomerPurchase(FILE* input, FILE* output, Yad3Service yad3) {
	char *customer_email = readString(input), *realtor_email = readString(
		input), *service_name = readString(input);
	int id = readNumber(input);
	Yad3Result result = yad3CustomerPurchase(yad3, customer_email,
		realtor_email, service_name, id);
	freeStrings(3, realtor_email, customer_email, service_name);
	return testYad3Result(result, output);
}
bool runYad3CustomerMakeOffer(FILE* input, FILE* output, Yad3Service yad3) {
	char *customer_email = readString(input), *realtor_email = readString(
		input), *service_name = readString(input);
	int id = readNumber(input), new_price = readNumber(input);
	Yad3Result result = yad3CustomerMakeOffer(yad3, customer_email,
		realtor_email, service_name, id, new_price);
	freeStrings(3, realtor_email, customer_email, service_name);
	return testYad3Result(result, output);
}
bool runYad3RealtorRespondToOffer(FILE* input, FILE* output, Yad3Service yad3) {
	char *realtor_email = readString(input), *customer_email = readString(
		input), *choice = readString(input);
	Response response;
	if (!strcmp(choice, "accept")) {
		response = ACCEPT;
	} else if (!strcmp(choice, "decline")) {
		response = DECLINE;
	} else {
		freeStrings(3, realtor_email, customer_email, choice);
		PRINT_AND_RETURN(MTM_INVALID_PARAMETERS, true);
	}
	Yad3Result result = yad3RealtorRespondToOffer(yad3, customer_email,
		realtor_email, response);
	freeStrings(3, realtor_email, customer_email, choice);
	return testYad3Result(result, output);
}
bool runYad3ReportRelevantRealtors(FILE* input, FILE* output, Yad3Service yad3) {
	char *email = readString(input);
	Yad3Result result = yad3ReportRelevantRealtors(yad3, email, output);
	free(email);
	return testYad3Result(result, output);
}
bool runYad3ReportMostPayingCustomers(FILE* input, FILE* output,
	Yad3Service yad3) {
	int count = readNumber(input);
	Yad3Result result = yad3ReportMostPayingCustomers(yad3, count, output);
	return testYad3Result(result, output);
}
bool runYad3ReportSignificantRealtors(FILE* input, FILE* output,
	Yad3Service yad3) {
	int count = readNumber(input);
	Yad3Result result = yad3ReportSignificantRealtors(yad3, count, output);
	return testYad3Result(result, output);
}

//====Functions to use by main()================================================
/* Reads command line parameters, and if they are valid - opens files for
 * input and output (if requested).
 * Valid parameters are -i <input file name>, -o <output file name>.
 * It's allowed to omit one of parameters (if so - both flag and filename should
 * be omitted), and to swap -i and -o flags.
 * 		@param argc, *argv[] - parameters read by main()
 * 		@param input, output - input and output file pointers to set up
 * @Return
 * false - if user entered wrong command line parameters (extra/less parameters,
 * 		file names omitted, wrong flags) or failed to open input file (if output
 * 		file doesn't exist, it will be created). Error message will be printed
 * 		to output file (if any) or stdout.
 * true otherwise
 *  */
bool setUpInputOutput(int argc, char *argv[], FILE** input, FILE** output) {
	FILE *tmpInput = stdin, *tmpOutput = stdout;
	if (argc == 1) {
		return true;
	} else if (argc == 3) {
		if (!strcmp(argv[1], "-i")) {
			tmpInput = fopen(argv[2], "r");
		} else if (!strcmp(argv[1], "-o")) {
			tmpOutput = fopen(argv[2], "w");
		} else {
			PRINT_AND_RETURN(MTM_INVALID_COMMAND_LINE_PARAMETERS, false);
		}
	} else if (argc == 5) {
		if (!strcmp(argv[1], "-i") && !strcmp(argv[3], "-o")) {
			tmpInput = fopen(argv[2], "r");
			tmpOutput = fopen(argv[4], "w");
		} else if (!strcmp(argv[1], "-o") && !strcmp(argv[3], "-i")) {
			tmpInput = fopen(argv[4], "r");
			tmpOutput = fopen(argv[2], "w");
		} else {
			PRINT_AND_RETURN(MTM_INVALID_COMMAND_LINE_PARAMETERS, false);
		}
	} else { //wrong number of command line parameters
		PRINT_AND_RETURN(MTM_INVALID_COMMAND_LINE_PARAMETERS, false);
	}
	if (!tmpInput || !tmpOutput) {
		PRINT_AND_RETURN(MTM_CANNOT_OPEN_FILE, false);
	}
	*input = tmpInput;
	*output = tmpOutput;
	return true;
}

/* Returns false at the end of file, or if while running some function
 * there was an error, that requires to exit program. */
bool runCommand(FILE* input, FILE* output, Map all_commands, Yad3Service yad_3){
	char* label;
	if (!getCommand(input, &label)) {
		return false;
	}
	Command command = mapGet(all_commands, label);
	assert(command); //that's assertion that all commands are valid
	free(label);
	Yad3Function function = command->function;
	return function(input, output, yad_3);
}

/* Initializes the system with all known commands.
 * For each text command there's a function that reads parameters and calls
 * to yad3 functions.
 * If you need to add an extra command - add a line to this function.
 * For extra info - look registerCommand description.
 * @Return:
 * false - if at least one command wasn't registered successfully
 * true - otherwise */
bool registerAllCommands(Map map) {
	bool result = true;
	result *= commandAdd(map, "realtor add", runYad3RealtorAdd);
	result *= commandAdd(map, "realtor remove", runYad3RealtorRemove);
	result *= commandAdd(map, "realtor add_apartment_service",
		runYad3RealtorAddApartmentService);
	result *= commandAdd(map, "realtor remove_apartment_service",
		runYad3RealtorRemoveApartmentService);
	result *= commandAdd(map, "realtor add_apartment",
		runYad3RealtorAddApartment);
	result *= commandAdd(map, "realtor remove_apartment",
		runYad3RealtorRemoveApartment);
	result *= commandAdd(map, "customer add", runYad3CustomerAdd);
	result *= commandAdd(map, "customer remove", runYad3CustomerRemove);
	result *= commandAdd(map, "customer purchase", runYad3CustomerPurchase);
	result *= commandAdd(map, "customer make_offer", runYad3CustomerMakeOffer);
	result *= commandAdd(map, "realtor respond_to_offer",
		runYad3RealtorRespondToOffer);
	result *= commandAdd(map, "report relevant_realtors",
		runYad3ReportRelevantRealtors);
	result *= commandAdd(map, "report most_paying_customers",
		runYad3ReportMostPayingCustomers);
	result *= commandAdd(map, "report significant_realtors",
		runYad3ReportSignificantRealtors);
	return result;
}

//==============================================================================
int main(int argc, char *argv[]) {
	FILE *input = stdin, *output = stdout;
	if (!setUpInputOutput(argc, argv, &input, &output)) {
		return 0;
	}
	Map all_commands = createMapOfCommands();
	Yad3Service yad_3 = yad3ServiceCreate();
	if (!all_commands || !yad_3 || !registerAllCommands(all_commands)) {
		yad3ServiceDestroy(yad_3);
		mapDestroy(all_commands);
		mtmPrintErrorMessage(stderr, MTM_OUT_OF_MEMORY);
		return 0;
	}
	while (runCommand(input, output, all_commands, yad_3)) {
		; //stops at EOF or critical errors
	}
	yad3ServiceDestroy(yad_3);
	mapDestroy(all_commands);
	fclose(input);
#ifndef NDEBUG
	assert(fclose(output)!=EOF);
#else
	fclose(output);
#endif
	return 0;
}

/******************************************************************
 * FILE:des_input.c
 * AUTHOR: SARFRAJ SINGH - ERNESTE IRADUKUNDA SENGA
 * STUDENT NUMBER: 041026598 - 040973461
 ******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <unistd.h>

#include "../../des_controller/src/des-mva.h"
/*function declaration*/
void input_event(char *input, Person *person);
void input_id(Person *person);
void input_weight(Person *person);
/*************************************************
 * Function: main
 * Parameters: int, char*
 * Return: int
 * Description: The main function of the des_inputs project. des_inputs acts as a client and communicates 
 * with the controller of the Door Entry System (DES).
 * The main function initializes the necessary variables and establishes a connection with the controller using 
 * IPC message passing. It sets the initial state of the person to GUARD_RIGHT_LOCK_ST.
 * Within the main loop, the function prompts the user to enter the event type corresponding to the person's actions
 * in the DES system. It uses the input_event function to update the person's state based on the input.
 * The updated person state is then sent to the controller using IPC message passing, and the controller's response 
 * is received and stored in the controller_response variable.
 * If there is an error in sending or receiving messages, the program exits with a failure status.
 * If the person's state is EXIT_ST, indicating that the person has exited the building, the loop breaks and the program waits 
 * for 5 seconds before detaching from the controller and exiting successfully.
**************************************************/
int main(int argc, char *argv[]) {
	pid_t cpid;
	int coid;
	Person person;
	CONTROLLER_RESPONSE controller_response;

	if (argc != 2) {
		exit(EXIT_FAILURE);
	}

	cpid = atoi(argv[1]);

	if ((coid = ConnectAttach(ND_LOCAL_NODE, cpid, 1, _NTO_SIDE_CHANNEL, 0)) == -1) {
		exit(EXIT_FAILURE);
	}

	person.state = GUARD_RIGHT_LOCK_ST;
	if (MsgSend(coid, &person, sizeof(person), &controller_response, sizeof(controller_response)) == -1) {
		exit(EXIT_FAILURE);
	}

	while (RUNNING) {
		char input[5];
		printf(
			"Enter the event type (ls = left scan, rs = right scan, ws = weight scale, lo = left open, \n"
			"ro = right open, lc = left closed, rc = right closed, gru = guard right unlock, grl = guard right lock, \n"
			"gll = guard left lock, glu = guard left unlock, exit = exit programs) \n"
		);

		scanf(" %s", input);

		input_event(input, &person);

		if (MsgSend(coid, &person, sizeof(person), &controller_response, sizeof(controller_response)) == -1) {
			exit(EXIT_FAILURE);
		}

		if (sizeof(controller_response) == 0) {
			exit(EXIT_FAILURE);
		}

		if (controller_response.statusCode != EOK) {

		}

		if (person.state == EXIT_ST)
			break;
	}
	sleep(5);
	ConnectDetach(coid);
	return EXIT_SUCCESS;
}

/**************************************
 * Function: input_event
 * Parameters: char *, Person *
 * Return: void
 * Description:
 * This function is used to update the state of the person based on user input.
 * The input parameter represents the user input, and the person parameter is a pointer
 * to the Person struct whose state will be updated.
 * The function compares the input with predefined input messages and updates the person's state accordingly.
 * If the input matches any of the messages, the corresponding state is assigned to the person.
 * Possible inputs and their corresponding states:
 * "ls" (left scan): Person state is updated to LEFT_SCAN_ST.
 * "rs" (right scan): Person state is updated to RIGHT_SCAN_ST.
 * "ws" (weight scale): Person state is updated to WEIGHT_SCALE_ST.
 * "lo" (left open): Person state is updated to LEFT_OPEN_ST.
 * "ro" (right open): Person state is updated to RIGHT_OPEN_ST.
 * "lc" (left close): Person state is updated to LEFT_CLOSE_ST.
 * "rc" (right close): Person state is updated to RIGHT_CLOSE_ST.
 * "grl" (guard right lock): Person state is updated to GUARD_RIGHT_LOCK_ST.
 * "gru" (guard right unlock): Person state is updated to GUARD_RIGHT_UNLOCK_ST.
 * "gll" (guard left lock): Person state is updated to GUARD_LEFT_LOCK_ST.
 * "glu" (guard left unlock): Person state is updated to GUARD_LEFT_UNLOCK_ST.
 * "exit": Person state is updated to EXIT_ST.
**************************************/
void input_event(char *input, Person *person) {
	if (strncmp(input, inMessage[LEFT_SCAN_IN], strlen(inMessage[LEFT_SCAN_IN])) == 0) {
		person->state = LEFT_SCAN_ST;
		input_id(person);
	} else if (strncmp(input, inMessage[RIGHT_SCAN_IN], strlen(inMessage[RIGHT_SCAN_IN])) == 0) {
		person->state = RIGHT_SCAN_ST;
		input_id(person);
	} else if (strncmp(input, inMessage[WEIGHT_SCALE_IN], strlen(inMessage[WEIGHT_SCALE_IN])) == 0) {
		person->state = WEIGHT_SCALE_ST;
		input_weight(person);
	} else if (strncmp(input, inMessage[LEFT_OPEN_IN], strlen(inMessage[LEFT_OPEN_IN])) == 0)
		person->state = LEFT_OPEN_ST;
	else if (strncmp(input, inMessage[RIGHT_OPEN_IN], strlen(inMessage[RIGHT_OPEN_IN])) == 0)
		person->state = RIGHT_OPEN_ST;
	else if (strncmp(input, inMessage[LEFT_CLOSE_IN], strlen(inMessage[LEFT_CLOSE_IN])) == 0)
		person->state = LEFT_CLOSE_ST;
	else if (strncmp(input, inMessage[RIGHT_CLOSE_IN], strlen(inMessage[RIGHT_CLOSE_IN])) == 0)
		person->state = RIGHT_CLOSE_ST;
	else if (strncmp(input, inMessage[GUARD_RIGHT_LOCK_IN], strlen(inMessage[GUARD_RIGHT_LOCK_IN])) == 0)
		person->state = GUARD_RIGHT_LOCK_ST;
	else if (strncmp(input, inMessage[GUARD_RIGHT_UNLOCK_IN], strlen(inMessage[GUARD_RIGHT_UNLOCK_IN])) == 0)
		person->state = GUARD_RIGHT_UNLOCK_ST;
	else if (strncmp(input, inMessage[GUARD_LEFT_LOCK_IN], strlen(inMessage[GUARD_LEFT_LOCK_IN])) == 0)
		person->state = GUARD_LEFT_LOCK_ST;
	else if (strncmp(input, inMessage[GUARD_LEFT_UNLOCK_IN], strlen(inMessage[GUARD_LEFT_UNLOCK_IN])) == 0)
		person->state = GUARD_LEFT_UNLOCK_ST;
	else if (strncmp(input, inMessage[EXIT_IN], strlen(inMessage[EXIT_IN])) == 0)
		person->state = EXIT_ST;
}
/********************
 * Function: input_id
 * Parameters: Person *
 * Description:
 * This function prompts the user to enter an ID and updates the person's ID property accordingly.
 * The function displays a message asking the user to enter an ID, and then reads the input from the user
 * and assigns it to the person's ID property.
********************/
void input_id(Person *person) {
	printf("Enter ID:\n");
	scanf("%d", &person->id);
}

/********************
 * Function: input_weight
 * Parameters: Person *
 * Description:
 * This function prompts the user to enter an weight and updates the person's weight property accordingly.
 * The function displays a message asking the user to enter an weight, and then reads the input from the user
 * and assigns it to the person's weight property.
********************/
void input_weight(Person *person) {
	printf("Please Enter your Weight\n");
	scanf("%d", &person->weight);
}

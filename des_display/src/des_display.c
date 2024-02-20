/******************************************************************
 * FILE:des_display.c
 * AUTHOR: SARFRAJ SINGH - ERNESTE IRADUKUNDA SENGA
 * STUDENT NUMBER: 041026598 - 040973461
 ******************************************************************/

/*including libraries*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <unistd.h>

#include "../../des_controller/src/des-mva.h"

Person person; // Declare a global variable of type Person
CONTROLLER_RESPONSE controller_response; // Declare a global variable of type CONTROLLER_RESPONSE

void display(Person *person); // Function declaration
/**********************************
 * Function: main
 * Return: Int
 * Description: Main function for the display server
 * of the door entry system. The function runs an
 * infinite loop using IPC message passing to communicate
 * with the controller. Its purpose is to receive person_t
 * struct data, which provides information about the current
 * state of the person who has entered the building. The function
 * then displays this information on the console.
*********************************/
int main(void) {
	int chid, rcvid;

	if ((chid = ChannelCreate(0)) == -1) {
		exit(EXIT_FAILURE);
	}

	printf("Display PID = %d\n", getpid()); // Print the Process ID of the server

	while (RUNNING) {
		if ((rcvid = MsgReceive(chid, &person, sizeof(person), NULL)) == -1) {
			exit(EXIT_FAILURE);
		}
		display(&person); // Display the current state of the person

		if (MsgReply(rcvid, EOK, &controller_response,
				sizeof(controller_response)) == -1) {
			return EXIT_FAILURE;
		}

		if (person.state == EXIT_ST)
			break;
	}

	ChannelDestroy(chid);
	return EXIT_SUCCESS;
}

/**********************************
 * Function: display
 * Parameters: Person
 * Description: This function is responsible for displaying the current state of a person who is attempting to enter or has already entered the building. 
 *  It takes a person struct as a parameter and utilizes the predefined output messages defined in des.h to generate the appropriate display output.
 * The function examines the state of the person and prints the corresponding message based on the state. For example, if the person is in the LEFT_SCAN_ST
 *  state, it will display the "LEFT SCAN" message along with the person's ID card number. Similarly, it displays the appropriate messages for other states 
 * such as RIGHT_SCAN_ST, WEIGHT_SCALE_ST, LEFT_OPEN_ST, RIGHT_OPEN_ST, LEFT_CLOSE_ST, RIGHT_CLOSE_ST, GUARD_RIGHT_LOCK_ST, GUARD_RIGHT_UNLOCK_ST, GUARD_LEFT_LOCK_ST, GUARD_LEFT_UNLOCK_ST, and EXIT_ST.
 * If the input state is not recognized or valid, an error message is displayed indicating an invalid input.
*********************************/
void display(Person *person) {
	switch (person->state) {
	case LEFT_SCAN_ST:
		printf("%s %d \n", outMessage[LEFT_SCAN_OUT], person->id); // Display left scan message with person ID card number
		break;
	case RIGHT_SCAN_ST:
		printf("%s %d \n", outMessage[RIGHT_SCAN_OUT], person->id); // Display right scan message with person ID card number
		break;
	case WEIGHT_SCALE_ST:
		printf("%s %d \n", outMessage[WEIGHT_SCALE_OUT], person->weight); // Display weight scale message with person weight
		break;
	case LEFT_OPEN_ST:
		printf("%s\n", outMessage[LEFT_OPEN_OUT]); // Display left open message
		break;
	case RIGHT_OPEN_ST:
		printf("%s\n", outMessage[RIGHT_OPEN_OUT]); // Display right open message
		break;
	case LEFT_CLOSE_ST:
		printf("%s\n", outMessage[LEFT_CLOSE_OUT]); // Display left close message
		break;
	case RIGHT_CLOSE_ST:
		printf("%s\n", outMessage[RIGHT_CLOSE_OUT]); // Display right close message
		break;
	case GUARD_RIGHT_LOCK_ST:
		printf("%s\n", outMessage[GUARD_RIGHT_LOCK_OUT]); // Display guard right lock message
		break;
	case GUARD_RIGHT_UNLOCK_ST:
		printf("%s\n", outMessage[GUARD_RIGHT_UNLOCK_OUT]); // Display guard right unlock message
		break;
	case GUARD_LEFT_LOCK_ST:
		printf("%s\n", outMessage[GUARD_LEFT_LOCK_OUT]); // Display guard left lock message
		break;
	case GUARD_LEFT_UNLOCK_ST:
		printf("%s\n", outMessage[GUARD_LEFT_UNLOCK_OUT]); // Display guard left unlock message
		break;
	case EXIT_ST:
		printf("%s\n", outMessage[EXIT_OUT]); // Display exit message
		break;
	default:
		printf("ERROR: Invalid input\n");
		break;
	}
}

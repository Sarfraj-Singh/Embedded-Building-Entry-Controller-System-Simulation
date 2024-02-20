/******************************************************************
 * FILE:des_controller.c
 * AUTHOR: SARFRAJ SINGH - ERNESTE IRADUKUNDA SENGA
 * STUDENT NUMBER: 041026598 - 040973461
 ******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <errno.h>

#include "des-mva.h"
/*
*Function Declarations
*/
void* left_scan_st();
void* left_open_st();
void* left_close_st();
void* right_scan_st();
void* right_open_st();
void* right_close_st();
void* guard_right_lock_st();
void* guard_right_unlock_st();
void* guard_left_lock_st();
void* guard_left_unlock_st();
void* weight_scale_st();
void* start_st();
void* exit_st();

void reset();

/*
*global variables
*/
CONTROLLER_RESPONSE controller_response;
Person person;
int coid, chid, rcvid;
FUNCTION_STATE function_state = start_st;
int direction = DEFAULT;

/**********************************
 * Function: main
 * Parameters: int, char*
 * Return: int
 * Description: The main function serves as the entry point for the controller of the
 * Door Entry System (DES). The controller communicates with the `./des-inputs` project
 * using IPC message passing to receive information about the current state of an entry (person).
 * Based on the received state, the controller sends messages to the `./des_display` project
 * to display the corresponding state of the entry.
 *********************************/
int main(int argc, char *argv[]) {
	pid_t dpid; /*for displaying pid*/

	if (argc != 2) {
		exit(EXIT_FAILURE);
	}

	dpid = atoi(argv[1]);

	if ((chid = ChannelCreate(0)) == -1) {
		exit(EXIT_FAILURE);
	}

	if ((coid = ConnectAttach(ND_LOCAL_NODE, dpid, 1, _NTO_SIDE_CHANNEL, 0)) == -1) {
		exit(EXIT_FAILURE);
	}

	printf("%s %d\n", outMessage[START_OUT], getpid());
	reset();

	while (RUNNING) {
		if ((rcvid = MsgReceive(chid, &person, sizeof(person), NULL)) == -1) {
			exit(EXIT_FAILURE);
		}

		if (person.state == EXIT_ST)
			function_state = (*exit_st)();
		else
			function_state = (FUNCTION_STATE) (*function_state)();

		MsgReply(rcvid, EOK, &controller_response, sizeof(controller_response));

		if (person.state == EXIT_ST)
			break;
	}
	ConnectDetach(coid);
	ChannelDestroy(chid);
	return EXIT_SUCCESS;
}
/**************************************************
 * Function: start_st
 * Description: Function pointer used when the person is in the starting state of the door entry system.
 *              The function determines the next state based on the input.
 *
 * NEXT POSSIBLE STATES:
 * - GRL (Guard Right Lock)
 *
 * The start_st function is responsible for initializing the system and transitioning it to the idling state. It
 * receives the first message from the client input, which triggers the start of the system. By default, the
 * next state is set to GRL. At the end of the DES process, the idling state can be either GLL (Guard Left Lock)
 * if the person entered from the right or GRL if the person entered from the left.
 *
 *
 *************************************************/
void* start_st() {
	return guard_right_lock_st;/*returns the default state of the person waiting to scan*/
}

/**************************************************
 * Function: left_scan_st
 * Description: Function pointer used when the person is in the left scan state of the door entry system.
 *              The function determines the next state based on the input.
 *
 * NEXT POSSIBLE STATES:
 * - GLU (Guard Left Unlock)
 *
 * The left_scan_st function is responsible for handling the left scan state of the door entry system. It determines
 * the next state based on the input received. The next possible state is GLU, which stands for Guard Left Unlock.
 *
 * If an error occurs, the function returns the current state without transitioning to a new state.
 *************************************************/

void* left_scan_st() {
	if (person.state == GUARD_LEFT_UNLOCK_ST) {
		if (MsgSend(coid, &person, sizeof(person), &controller_response, sizeof(controller_response)) == -1) {
			exit(EXIT_FAILURE);
		}
		return guard_left_unlock_st;/*if scan or input were successful, returns next state */
	}
	return left_scan_st;
}


/**************************************************
 * Function: guard_left_unlock_st
 * Description: Function pointer used when the person is in the Guard Left Unlock state of the door entry system.
 *              The function determines the next state based on the input.
 *
 * NEXT POSSIBLE STATES:
 * - LO (Left Open)
 *
 * The guard_left_unlock_st function handles the Guard Left Unlock state of the door entry system. It determines the next state
 * based on the input received. The next possible state is LO (Left Open).
 *
 * If an error occurs, the function returns the current state without transitioning to a new state.
 *************************************************/
void* guard_left_unlock_st() {
	if (person.state == LEFT_OPEN_ST) {
		if (MsgSend(coid, &person, sizeof(person), &controller_response, sizeof(controller_response)) == -1) {
			exit(EXIT_FAILURE);
		}
		return left_open_st;
	}
	return guard_left_unlock_st;
}

/**************************************************
 * Function: left_open_st
 * Description: Function pointer used when the person is in the left open state of the door entry system.
 *              The function determines the next state based on the input.
 *
 * NEXT POSSIBLE STATES:
 * - WS (Weigh Scale)
 * - LC (Left Close)
 *
 * The left_open_st function handles the left open state of the door entry system. It determines the next state
 * based on the input received. The next possible states are WS (Weigh Scale) and LC (Left Close).
 *
 * If an error occurs, the function returns the current state without transitioning to a new state.
 *************************************************/
void* left_open_st() {
	if (person.state == WEIGHT_SCALE_ST) {
		if (MsgSend(coid, &person, sizeof(person), &controller_response, sizeof(controller_response)) == -1) {
			exit(EXIT_FAILURE);
		}
		return weight_scale_st;
	} else if (person.state == LEFT_CLOSE_ST) {
		if (MsgSend(coid, &person, sizeof(person), &controller_response, sizeof(controller_response)) == -1) {
			exit(EXIT_FAILURE);
		}
		return left_close_st;
	}
	return left_open_st;
}

/**************************************************
 * Function: right_scan_st
 * Description: Function pointer used when the person is in the Right Scan state of the door entry system.
 *              The function determines the next state based on the input.
 *
 * NEXT POSSIBLE STATES:
 * - GRU (Guard Right Unlock)
 *
 * The right_scan_st function handles the Right Scan state of the door entry system. It determines the next state
 * based on the input received. The next possible state is GRU (Guard Right Unlock).
 *
 * If an error occurs, the function returns the current state without transitioning to a new state.
 *************************************************/
void* right_scan_st() {
	if (person.state == GUARD_RIGHT_UNLOCK_ST) {
		if (MsgSend(coid, &person, sizeof(person), &controller_response, sizeof(controller_response)) == -1) {
			exit(EXIT_FAILURE);
		}
		return guard_right_unlock_st;
	}
	return right_scan_st;
}

/**************************************************
 * Function: guard_right_unlock_st
 * Description: Function pointer used when the person is in the Guard Right Unlock state of the door entry system.
 *              The function determines the next state based on the input.
 *
 * NEXT POSSIBLE STATES:
 * - RO (Right Open)
 *
 * The guard_right_unlock_st function handles the Guard Right Unlock state of the door entry system. It determines the next state
 * based on the input received. The next possible state is RO (Right Open).
 *
 * If an error occurs, the function returns the current state without transitioning to a new state.
 *************************************************/
void* guard_right_unlock_st() {
	if (person.state == RIGHT_OPEN_ST) {
		if (MsgSend(coid, &person, sizeof(person), &controller_response, sizeof(controller_response)) == -1) {
			exit(EXIT_FAILURE);
		}
		return right_open_st;
	}
	return guard_right_unlock_st;
}

/**************************************************
 * Function: right_open_st
 * Description: Function pointer used when the person is in the Right Open state of the door entry system.
 *              The function determines the next state based on the input.
 *
 * NEXT POSSIBLE STATES:
 * - WS (Weigh Scale)
 * - RC (Right Close)
 *
 * The right_open_st function is responsible for handling the Right Open state of the door entry system. It checks the
 * input received to determine the next state. If the input is weight_scale_st (Weigh Scale), it means that the person
 * entered from the right side and needs to proceed to the weigh scale. The function transitions to the weight_scale_st
 * function pointer. If the input is right_close_st (Right Close), it means that the person entered from the left side,
 * and the right door needs to be closed. The function transitions to the right_close_st function pointer. If an error
 * occurs during message sending, an error message is displayed, and the program exits with failure.
 *
 * If the input does not match any of the expected values, the function remains in the current state.
 *************************************************/
void* right_open_st() {
	if (person.state == WEIGHT_SCALE_ST) {
		if (MsgSend(coid, &person, sizeof(person), &controller_response, sizeof(controller_response)) == -1) {
			exit(EXIT_FAILURE);
		}
		return weight_scale_st;
	} else if (person.state == RIGHT_CLOSE_ST) {
		if (MsgSend(coid, &person, sizeof(person), &controller_response, sizeof(controller_response)) == -1) {
			exit(EXIT_FAILURE);
		}
		return right_close_st;
	}
	return right_open_st;
}

/**************************************************
 * Function: weight_scale_st
 * Description: Function pointer used when the person is in the Weigh Scale state of the door entry system.
 *              The function determines the next state based on the input.
 *
 * NEXT POSSIBLE STATES:
 * - LC (Left Close)
 * - RC (Right Close)
 *
 * The weight_scale_st function is responsible for handling the Weigh Scale state of the door entry system. It checks the
 * input received to determine the next state. If the input is left_close_st (Left Close), it means that the person
 * entered from the left side, and the left door needs to be closed. The function transitions to the left_close_st
 * function pointer. If the input is right_close_st (Right Close), it means that the person entered from the right side,
 * and the right door needs to be closed. The function transitions to the right_close_st function pointer. If an error
 * occurs during message sending, an error message is displayed, and the program exits with failure.
 *
 * If the input does not match any of the expected values, the function remains in the current state.
 *************************************************/
void* weight_scale_st() {
	if (person.state == LEFT_CLOSE_ST) {
		if (MsgSend(coid, &person, sizeof(person), &controller_response, sizeof(controller_response)) == -1) {
			exit(EXIT_FAILURE);
		}
		return left_close_st;
	}
	if (person.state == RIGHT_CLOSE_ST) {
		if (MsgSend(coid, &person, sizeof(person), &controller_response, sizeof(controller_response)) == -1) {
			exit(EXIT_FAILURE);
		}
		return right_close_st;
	}
	return weight_scale_st;
}

/**************************************************
 * Function: left_close_st
 * Description: Function pointer used when the person is in the Left Close state of the door entry system.
 *              The function determines the next state based on the input.
 *
 * NEXT POSSIBLE STATES:
 * - GLL (Guard Left Lock)
 *
 * The left_close_st function handles the Left Close state of the door entry system. It checks the input received
 * to determine the next state. If the input is guard_left_lock_st (Guard Left Lock), it means that the person has left
 * through the left doors, and the left doors need to be locked. The function transitions to the guard_left_lock_st
 * function pointer. If an error occurs during message sending, an error message is displayed, and the program
 * exits with failure.
 *
 * If the input does not match the expected value, the function remains in the current state.
 *************************************************/
void* left_close_st() {
	if (person.state == GUARD_LEFT_LOCK_ST) {
		if (MsgSend(coid, &person, sizeof(person), &controller_response, sizeof(controller_response)) == -1) {
			exit(EXIT_FAILURE);
		}
		return guard_left_lock_st;
	}
	return left_close_st;
}
/**************************************************
 * Function: right_close_st
 * Description: Function pointer used when the person is in the Right Close state of the door entry system.
 *              The function determines the next state based on the input.
 *
 * NEXT POSSIBLE STATES:
 * - GRL (Guard Right Lock)
 *
 * The right_close_st function handles the Right Close state of the door entry system. It checks the input received
 * to determine the next state. If the input is guard_right_lock_st (Guard Right Lock), it means that the person has left
 * through the right doors, and the right doors need to be locked. The function transitions to the guard_right_lock_st
 * function pointer. If an error occurs during message sending, an error message is displayed, and the program
 * exits with failure.
 *
 * If the input does not match the expected value, the function remains in the current state.
 *************************************************/
void* right_close_st() {
	if (person.state == GUARD_RIGHT_LOCK_ST) {
		if (MsgSend(coid, &person, sizeof(person), &controller_response, sizeof(controller_response)) == -1) {
			exit(EXIT_FAILURE);
		}
		return guard_right_lock_st;
	}
	return right_close_st;
}

/**************************************************
 * Function: guard_right_lock_st
 * Description: Function pointer used when the person is in the Guard Right Lock state of the door entry system.
 *              The function determines the next state based on the input.
 *
 * 
 * If the person entered from the right and is now exiting from the left: Next state is GLU (Guard Left Unlock). If the person
 * entered from the left and has now left the building: Wait for a new person to enter.
 *
 * NEXT POSSIBLE STATES:
 * - GLU (Guard Left Unlock)
 * - RS (Right Scan)
 * - LS (Left Scan)
 *
 * If the input does not match the expected value, the function remains in the current state.
 *************************************************/
void* guard_right_lock_st() {
	if (direction == RIGHT && person.state == GUARD_LEFT_UNLOCK_ST) {
		if (MsgSend(coid, &person, sizeof(person), &controller_response, sizeof(controller_response)) == -1) {
			exit(EXIT_FAILURE);
		}
direction = DEFAULT;
		return guard_left_unlock_st;
	}
	if (direction == DEFAULT && person.state == RIGHT_SCAN_ST) {
		if (MsgSend(coid, &person, sizeof(person), &controller_response, sizeof(controller_response)) == -1) {
			exit(EXIT_FAILURE);
		}
		direction = RIGHT;
		return right_scan_st;
	}
	if (direction == DEFAULT && person.state == LEFT_SCAN_ST) {
		if (MsgSend(coid, &person, sizeof(person), &controller_response, sizeof(controller_response)) == -1) {
			exit(EXIT_FAILURE);
		}
		direction = LEFT;
		return left_scan_st;
	}
	return guard_right_lock_st;
}

/**************************************************
 * Function: guard_left_lock_st
 * Description: Function pointer used when the person is in the Guard Left Lock state of the door entry system.
 *              The function determines the next state based on the input.
 *
 * 
 * If the person entered from the left and is now exiting from the right: Next state is GRU (Guard Right Unlock). If the person
 * entered from the right and has now left the building: Wait for a new person to enter.
 *
 * NEXT POSSIBLE STATES:
 * - GRU (Guard Right Unlock)
 * - RS (Right Scan)
 * - LS (Left Scan)
 *
 * If the input does not match the expected value, the function remains in the current state.
 *************************************************/
void* guard_left_lock_st() {
	if (direction == LEFT && person.state == GUARD_RIGHT_UNLOCK_ST) {
		if (MsgSend(coid, &person, sizeof(person), &controller_response, sizeof(controller_response)) == -1) {
			exit(EXIT_FAILURE);
		}
		direction = DEFAULT;
		return guard_right_unlock_st;
	} else if (direction == DEFAULT && (person.state == RIGHT_SCAN_ST || person.state == LEFT_SCAN_ST)) {
		if (MsgSend(coid, &person, sizeof(person), &controller_response, sizeof(controller_response)) == -1) {
			exit(EXIT_FAILURE);
		}
		if (person.state == LEFT_SCAN_ST) {
			if (MsgSend(coid, &person, sizeof(person), &controller_response, sizeof(controller_response)) == -1) {
				exit(EXIT_FAILURE);
			}
			direction = LEFT;
			return left_scan_st;
		}
		direction = RIGHT;
		return right_scan_st;
	}
	return guard_left_lock_st;
}
/**************************************************
 * Function exit_st
 * Description: Function pointer used when the person is in the EXIT state of the door entry system.
 *              The function determines the next state based on the input. The EXIT state has no next state
 *              and will terminate the door entry system program.
 *
 * NEXT POSSIBLE STATES:
 * - NONE: Termination
 *
 *************************************************/
void* exit_st() {
	if (MsgSend(coid, &person, sizeof(person), &controller_response, sizeof(controller_response)) == -1) {
		exit(EXIT_FAILURE);
	}
	return exit_st;
}
/**************************************************
 * Function: reset
 * Description: Resets all properties to their initial values in case of corrupted data.
*************************************************/
void reset() {
	person.id = 0;
	person.weight = 0;
	person.state = START_ST;
}

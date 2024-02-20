/******************************************************************
 * FILE:des.h
 * AUTHOR: SARFRAJ SINGH - ERNESTE IRADUKUNDA SENGA
 * STUDENT NUMBER: 041026598 - 040973461
 * Header file, defining the different states
 * encountered in the Door Entry System program.
 *
 * Input/Output possibilities mapped to input/output messages
 * 
 ******************************************************************/

#ifndef DOOR_ENTRY_SYSTEM_H_
#define DOOR_ENTRY_SYSTEM_H_

#define RUNNING 1

#define RIGHT 0
#define LEFT 1
#define DEFAULT 2

#define NUM_STATES 13
typedef enum
{
	START_ST = 0,                   // Starting state of the door entry system.
	LEFT_SCAN_ST = 1,               // State for left scanning.
	RIGHT_SCAN_ST = 2,              // State for right scanning.
	WEIGHT_SCALE_ST = 3,            // State for weight scaling.
    LEFT_OPEN_ST = 4,               // State for left door open.
	RIGHT_OPEN_ST = 5,              // State for right door open.
	LEFT_CLOSE_ST = 6,              // State for left door close.
	RIGHT_CLOSE_ST = 7,             // State for right door close.
	GUARD_RIGHT_LOCK_ST = 8,        // State for right door locked by guard.
	GUARD_RIGHT_UNLOCK_ST = 9,      // State for right door unlocked by guard.
	GUARD_LEFT_LOCK_ST = 10,        // State for left door locked by guard.
	GUARD_LEFT_UNLOCK_ST = 11,      // State for left door unlocked by guard.
	EXIT_ST = 12                    // Exit state of the door entry system.
} State;
#define NUM_INPUTS 12	// Number of input commands that drive the FSM.
typedef enum {
	LEFT_SCAN_IN = 0,               // Input command for left scanning.
	RIGHT_SCAN_IN = 1,              // Input command for right scanning.
	WEIGHT_SCALE_IN = 2,            // Input command for weight scaling.
	LEFT_OPEN_IN = 3,               // Input command for left door open.
	RIGHT_OPEN_IN = 4,              // Input command for right door open.
	LEFT_CLOSE_IN = 5,              // Input command for left door close.
	RIGHT_CLOSE_IN = 6,             // Input command for right door close.
	GUARD_RIGHT_LOCK_IN = 7,        // Input command for right door lock by guard.
	GUARD_RIGHT_UNLOCK_IN = 8,      // Input command for right door unlock by guard.
	GUARD_LEFT_LOCK_IN = 9,         // Input command for left door lock by guard.
	GUARD_LEFT_UNLOCK_IN = 10,      // Input command for left door unlock by guard.
	EXIT_IN = 11                    // Input command for system exit.
} Input;


const char *inMessage[NUM_INPUTS] = {
	"ls",  /* IN_LS    */             // Input message for left scanning.
	"rs",  /* IN_RS    */             // Input message for right scanning.
	"ws",  /* IN_WS    */             // Input message for weight scaling.
	"lo",  /* IN_LO    */             // Input message for left door open.
	"ro",  /* IN_RO    */             // Input message for right door open.
	"lc",  /* IN_LC    */             // Input message for left door close.
	"rc",  /* IN_RC    */             // Input message for right door close.
	"grl", /* IN_GRL   */             // Input message for right door lock by guard.
	"gru", /* IN_GRU   */             // Input message for right door unlock by guard.
	"gll", /* IN_GLL   */             // Input message for left door lock by guard.
	"glu", /* IN_GLU   */             // Input message for left door unlock by guard.
	"exit" /* IN_EXIT  */             // Input message for system exit.
};

#define NUM_OUTPUTS 13	//Number of output messages from the FSM.
typedef enum {
	START_OUT = 0,                  // Output message for system start.
	LEFT_SCAN_OUT = 1,              // Output message for left scanning.
	RIGHT_SCAN_OUT = 2,             // Output message for right scanning.
	WEIGHT_SCALE_OUT = 3,           // Output message for weight scaling.
	LEFT_OPEN_OUT = 4,              // Output message for left door open.
	RIGHT_OPEN_OUT = 5,             // Output message for right door open.
	LEFT_CLOSE_OUT = 6,             // Output message for left door close.
	RIGHT_CLOSE_OUT = 7,            // Output message for right door close.
	GUARD_RIGHT_LOCK_OUT = 8,       // Output message for right door locked by guard.
	GUARD_RIGHT_UNLOCK_OUT = 9,     // Output message for right door unlocked by guard.
	GUARD_LEFT_LOCK_OUT = 10,       // Output message for left door locked by guard.
	GUARD_LEFT_UNLOCK_OUT = 11,     // Output message for left door unlocked by guard.
	EXIT_OUT = 12                   // Output message for system exit.
} Output;

const char *outMessage[NUM_OUTPUTS] = {
	"Controller PID: ",             // Output message for controller PID.
    "Person scanned(left) ID. = ",  // Output message for person scanned on the left.
	"Person scanned(right) ID. = ", // Output message for person scanned on the right.
	"Person's weight = ",    // Output message for person's weight.
	"Left door Open.",              // Output message for left door open.
	"Right door Open.",             // Output message for right door open.
	"Left door Closed.",            // Output message for left door close.
	"Right door Closed.",           // Output message for right door close.
	"Guard Right door locked. ", // Output message for right door locked by guard.
	"Guard Right door unlocked. ",// Output message for right door unlocked by guard.
	"Guard Left door locked. ",  // Output message for left door locked by guard.
	"Guard Left door unlocked. ",// Output message for left door unlocked by guard.
	"Exiting Door Entry System..."  // Output message for system exit.
};

typedef struct {
	int id;
	int weight;
	int state;
} Person;

/* Controller response structure (REPLY MSG) */
typedef struct {
	Person person;                  // Response structure from the server, includes person information.
	int statusCode;                 // Status code of the response.
} CONTROLLER_RESPONSE;

typedef void *(*FUNCTION_STATE)();

#endif /* DOOR_ENTRY_SYSTEM_H_ */

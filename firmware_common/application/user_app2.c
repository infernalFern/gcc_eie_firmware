/*!*********************************************************************************************************************
@file user_app2.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app2 as a template:
 1. Copy both user_app2.c and user_app2.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app2" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp2" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void UserApp2Initialize(void)
- void UserApp2RunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>UserApp2"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp2Flags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */

extern u8 G_au8DebugScanfBuffer[];
extern u8 G_u8DebugScanfCharCount;
/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp2_<type>" and be declared as static.
***********************************************************************************************************************/
static u8 au8UserInputBuffer[USER_INPUT_BUFFER_SIZE];
static u8 au8ConsumedCharacterArray[USER_INPUT_BUFFER_SIZE];

static fnCode_type UserApp2_pfStateMachine;               /*!< @brief The state machine function pointer */
//static u32 UserApp2_u32Timeout;                           /*!< @brief Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void UserApp2Initialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void UserApp2Initialize(void)
{
#if 0
	memset(au8UserInputBuffer, 0, sizeof(u8) * USER_INPUT_BUFFER_SIZE);
	memset(au8ConsumedCharacterArray, 0, sizeof(u8) * USER_INPUT_BUFFER_SIZE);
#endif
	/* If good initialization, set state to Idle */
	if( 1 )
	{
		UserApp2_pfStateMachine = UserApp2SM_Idle;
	}
	else
	{
		/* The task isn't properly initialized, so shut it down and don't run */
		UserApp2_pfStateMachine = UserApp2SM_Error;
	}

} /* end UserApp2Initialize() */

	
/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserApp2RunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UserApp2RunActiveState(void)
{
	UserApp2_pfStateMachine();

} /* end UserApp2RunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */
// u8 IsNamePresent(const char *arrToCompare, const char *arrActualName); //TODO: move to .h file.

// u8 IsNamePresent(const char *arrToCompare, const char *arrActualName) {
// 	for (u8 i = 0; arrToCompare[i] != '\0'; i++) {

// 	}
// }

static void UserApp2SM_Idle(void)
{
#if 0
	//Part 1 of Module Exercise.
	static u8 au8NameConstant[] = "Some Name";
	static u8 u8NameCount = 0;
	static u8 u8NumberNameMatches;
	
	DebugSetPassthrough();

	u8 u8CharacterInputCount = DebugScanf(au8UserInputBuffer);

	if (u8CharacterInputCount > 0) {  //could consider using G_u8DebugScanfCharCount
		// append to second array for further processing and clear input buffer.
		// au8UserInputBuffer[u8CharacterInputCount] = '\0';
		strcat(au8ConsumedCharacterArray, au8UserInputBuffer);
		memset(au8UserInputBuffer, 0, sizeof(u8) * USER_INPUT_BUFFER_SIZE);
	}

	u8 u8ConsumedCharactersLength = strlen(au8ConsumedCharacterArray);
	u8 u8NameConstantLength = strlen(au8NameConstant);

	if (u8ConsumedCharactersLength >= USER_INPUT_BUFFER_SIZE - (u8NameConstantLength + 1)) {
		// shift the last few potential matching characters to the beginning of the array and clear the rest with '\0'.
		u8 i;
		
		// shift the last few potential matches to the beginning.
		for (i = 0; i <= u8NameConstantLength; i++) {
			au8ConsumedCharacterArray[i] = au8ConsumedCharacterArray[i + USER_INPUT_BUFFER_SIZE - (u8NameConstantLength + 1)];
		}

		// clear the rest.
		for ( ; i <= USER_INPUT_BUFFER_SIZE; i++) {
			au8ConsumedCharacterArray[i + USER_INPUT_BUFFER_SIZE - (u8NameConstantLength + 1)] = '\0';
		}
	}

	u8 *pu8SubstringReturnValue = strstr(au8ConsumedCharacterArray, au8NameConstant);

	if (pu8SubstringReturnValue == NULL) { /*do nothing.*/ }
	else {  // An exact match was found within the string.

		//!@c perform pointer arithmetic to obtain index      ---- MAYBE NOT NECESSARY
		// u8 u8IndexOfOcurrence = pu8SubstringReturnValue - au8ConsumedCharacterArray; 
		
		u8NameCount++;

		// remember to clear the array of all occurences so as not to double count.
		memset(au8ConsumedCharacterArray, 0, sizeof(u8) * USER_INPUT_BUFFER_SIZE); 

		
		
		
		/*! @p Part 2. Print current name count with surrounding * characters each time name count is increased. */
		
		u8 u8CopyOfNameCount = u8NameCount;
		u8 u8NumberOfDigits = 0;
		
		while (u8CopyOfNameCount /= 10 != 0) {
			u8NumberOfDigits++;
		}

		// create the printed pattern.
		for (u8 i = 0; i <= u8NumberOfDigits + 2; i++) { // may wish to change to +1
			DebugPrintf("*");
		}
		
		DebugLineFeed();
		DebugPrintf("*");
		DebugPrintNumber((u32)u8NameCount);
		DebugPrintf("*");
		DebugLineFeed();
		
		for (u8 i = 0; i <= u8NumberOfDigits + 2; i++) { // may wish to change to +1
			DebugPrintf("*");
		}
		DebugLineFeed();
	}

#endif
} /* end UserApp2SM_Idle() */
		 

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp2SM_Error(void)          
{
	
} /* end UserApp2SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/

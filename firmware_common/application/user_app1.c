/*!*********************************************************************************************************************
@file user_app1.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
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
- void UserApp1Initialize(void)
- void UserApp1RunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */

// extern u8 G_au8MessageOK[];                            /* From utilities.c */
// extern u8 G_au8MessageFAIL[];                          /* From utilities.c */

// extern u32 G_u32AntFlags;                              /* From ant.c */

extern u32 G_u32AntApiCurrentMessageTimeStamp;                           /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;           /* From ant_api.c */
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES]; /* From ant_api.c */
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;               /* From ant_api.c  */

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_pfStateMachine;               /*!< @brief The state machine function pointer */
static u32 UserApp1_u32Timeout;                           /*!< @brief Timeout counter used across states */

static  AntAssignChannelInfoType sChannelInfo; /* ANT channel configuration */

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
@fn void UserApp1Initialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void UserApp1Initialize(void)
{
  AntAssignChannelInfoType sChannelInfo; /* ANT channel configuration */

	if (AntRadioStatusChannel(ANT_CHANNEL_0) == ANT_UNCONFIGURED) {
		/* Configure the ANT radio */
  		sChannelInfo.AntChannel          = U8_ANT_CHANNEL_USERAPP;
  		sChannelInfo.AntChannelType      = CHANNEL_TYPE_MASTER;
  		sChannelInfo.AntChannelPeriodLo  = U8_ANT_CHANNEL_PERIOD_HI_DEFAULT;
  		sChannelInfo.AntChannelPeriodHi  = U8_ANT_CHANNEL_PERIOD_LO_DEFAULT;

  		sChannelInfo.AntDeviceIdHi       = U8_ANT_DEVICE_HI_USERAPP;
  		sChannelInfo.AntDeviceIdLo       = U8_ANT_DEVICE_LO_USERAPP;
  		sChannelInfo.AntDeviceType       = U8_ANT_DEVICE_TYPE_DEFAULT;
  		sChannelInfo.AntTransmissionType = U8_ANT_TRANSMISSION_TYPE_DEFAULT;

  		sChannelInfo.AntFrequency        = U8_ANT_FREQUENCY_DEFAULT;
  		sChannelInfo.AntTxPower          = U8_ANT_TX_POWER_DEFAULT;

  		sChannelInfo.AntNetwork = U8_ANT_NETWORK_DEFAULT;
  		
		for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++) {
  			sChannelInfo.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
  		}

  		/* Queue the channel assignment and go to wait state */
  		AntAssignChannel(&sChannelInfo);
	}

	// test for decent initialization, otherwise set state to idle.
	if ( 1 ) {
		UserApp1_pfStateMachine = UserApp1SM_WaitAntReady;
	} else {
		UserApp1_pfStateMachine = UserApp1SM_Error;
	}
  

} /* end UserApp1Initialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserApp1RunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UserApp1RunActiveState(void)
{
  UserApp1_pfStateMachine();

} /* end UserApp1RunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
//wait for ANT channel to be configured.
static void UserApp1SM_WaitAntReady(void)
{
	if (AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_CONFIGURED) {
		if (AntOpenChannelNumber(U8_ANT_CHANNEL_USERAPP)) {
			UserApp1_pfStateMachine = UserApp1SM_WaitChannelOpen;
		} else {
			UserApp1_pfStateMachine = UserApp1SM_Error;
		}
	}
} // end UserApp1SM_WaitAntReady()

static void UserApp1SM_WaitChannelOpen(void) {
	if (AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_OPEN) {
		UserApp1_pfStateMachine = UserApp1SM_ChannelOpen;
	}
}


//TODO: Add Add ChannelOpen() State


#if 0
static void UserApp1SM_SetupAnt(void)
{
  /* Check to see if the channel assignment is successful */
  if(AntRadioStatusChannel(U8_ANT_CHANNEL_DEFAULT) == ANT_CONFIGURED)
  {
    DebugPrintf("Board test ANT Master ready\n\r");
    DebugPrintf("Device ID: ");
    DebugPrintNumber(U32_ANT_DEVICEID_DEC_BOARDTEST);
    DebugPrintf(", Device Type 96, Trans Type 1, Frequency 50\n\r");

    UserApp1_pfStateMachine = UserApp1SM_Idle;
  }

  /* Watch for timeout */
  if(IsTimeUp(&UserApp1_u32Timeout, 3000))
  {
    /* Init failed */
    DebugPrintf("Board test cannot assign ANT channel\n\r");
    UserApp1_pfStateMachine = UserApp1SM_Idle;
  }
} /* end UserApp1SM_SetupAnt() */


static void UserApp1SM_Idle(void)
{
  static bool bButton0Test = FALSE;
  static u8 u8Button2Test = 0;
  static u8 u8Button3Test = 0;
  static u8 au8TestMessage[] = {0, 0, 0, 0, 0, 0, 0, 0};
  static u8 au8DataMessage[] = "ANT data: ";
  u8 au8DataContent[26];
  AntChannelStatusType eAntCurrentState;

                          /* "01234567890123456789" */
  static const u8 au8Eng[] = "ENGENUICS RAZOR     ";
  static const u8 au8MPG[] = "ASCII DEV BOARD     ";
  u8 au8Temp1[21];
  u8 au8Temp2[21];
  
  static u8 u8ResetIndex = 0;
  static u8 u8Index = 0;
  static u32 u32LcdTimer;

  /* BUTTON0 toggles LEDs */
  if( WasButtonPressed(BUTTON0) )
  {
    ButtonAcknowledge(BUTTON0);
    
    /* If test is active, deactivate it, put all LEDs back on */
    if(bButton0Test)
    {
      bButton0Test = FALSE;

      LedOn(WHITE);
      LedOn(PURPLE);
      LedOn(BLUE);
      LedOn(CYAN);
      LedOn(GREEN);
      LedOn(YELLOW);
      LedOn(ORANGE);
      LedOn(RED);
    }
    /* Else activate it: turn all LEDs off */
    else
    {
      bButton0Test = TRUE;

      LedOff(WHITE);
      LedOff(PURPLE);
      LedOff(BLUE);
      LedOff(CYAN);
      LedOff(GREEN);
      LedOff(YELLOW);
      LedOff(ORANGE);
      LedOff(RED);
    }
  } /* End of BUTTON 0 test */

/* BUTTON1 toggles the radio and buzzer test.  When the button is pressed,
  an open channel request is made.  The system monitors _ANT_FLAGS_CHANNEL_OPEN
  to control wether or not the buzzer is on. */
  
  /* Toggle the beeper and ANT radio on BUTTON1 */
  if( WasButtonPressed(BUTTON1) )
  {
    ButtonAcknowledge(BUTTON1);
    eAntCurrentState = AntRadioStatusChannel(U8_ANT_CHANNEL_BOARDTEST);

    if(eAntCurrentState == ANT_CLOSED )
    {
       AntOpenChannelNumber(U8_ANT_CHANNEL_BOARDTEST);
    }

    if(eAntCurrentState == ANT_OPEN)
    {
       AntCloseChannelNumber(U8_ANT_CHANNEL_BOARDTEST);
    }
  }
 
 
#if 0
  /* Monitor the CHANNEL_OPEN flag to decide whether or not audio should be on */
  if( (AntRadioStatusChannel(U8_ANT_CHANNEL_BOARDTEST) == ANT_OPEN ) && 
     !(BoardTest_u32Flags & _AUDIO_ANT_ON) )
  {
    PWMAudioOn(BUZZER1);
    BoardTest_u32Flags |= _AUDIO_ANT_ON;
  }
  
  if( AntRadioStatusChannel(U8_ANT_CHANNEL_BOARDTEST) == ANT_CLOSED )
  {
    PWMAudioOff(BUZZER1);
    BoardTest_u32Flags &= ~_AUDIO_ANT_ON;
  }
#endif
  
  /* Process ANT Application messages */  
        
  if( AntReadAppMessageBuffer() )
  {
     /* New data message: check what it is */
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      /* We got some data: print it */
      for(u8 i = 0; i < ANT_DATA_BYTES; i++)
      {
        au8DataContent[3 * i]     = HexToASCIICharUpper(G_au8AntApiCurrentMessageBytes[i] / 16);
        au8DataContent[3 * i + 1] = HexToASCIICharUpper(G_au8AntApiCurrentMessageBytes[i] % 16);
        au8DataContent[3 * i + 2] = '-';
      }
      au8DataContent[23] = '\n';
      au8DataContent[24] = '\r';
      au8DataContent[25] = '\0';
      
      DebugPrintf(au8DataMessage);
      DebugPrintf(au8DataContent);
    }
    else if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {

     /* Update and queue the new message data */
      au8TestMessage[7]++;
      if(au8TestMessage[7] == 0)
      {
        au8TestMessage[6]++;
        if(au8TestMessage[6] == 0)
        {
          au8TestMessage[5]++;
        }
      }
      AntQueueBroadcastMessage(U8_ANT_CHANNEL_BOARDTEST, au8TestMessage);
    }
  }

  /* BUTTON2 toggles LCD backlights */
  if( WasButtonPressed(BUTTON2) )
  {
    ButtonAcknowledge(BUTTON2);
    
    /* If test is active, deactivate it, put all LEDs back on */
    switch(u8Button2Test)
    {
      case 0:
        u8Button2Test = 1;

        LedOff(LCD_RED);
        LedOff(LCD_GREEN);
        LedOff(LCD_BLUE);
        break;

      case 1:
        u8Button2Test = 2;

        LedOn(LCD_RED);
        LedOff(LCD_GREEN);
        LedOff(LCD_BLUE);
        break;

      case 2:
        u8Button2Test = 3;

        LedOff(LCD_RED);
        LedOn(LCD_GREEN);
        LedOff(LCD_BLUE);
        break;

      case 3:
        u8Button2Test = 4;

        LedOff(LCD_RED);
        LedOff(LCD_GREEN);
        LedOn(LCD_BLUE);
        break;

      case 4:
        u8Button2Test = 0;

        LedOn(LCD_RED);
        LedOn(LCD_GREEN);
        LedOn(LCD_BLUE);
        break;
        
      default:
        break;
    }
  } /* End of BUTTON 2 test */

  /* BUTTON3 toggles buzzer test */
  if( WasButtonPressed(BUTTON3) )
  {
    ButtonAcknowledge(BUTTON3);
    
    /* If test is active, deactivate it, put all LEDs back on */
    switch(u8Button3Test)
    {
      case 0:
        u8Button3Test = 1;

        PWMAudioSetFrequency(BUZZER2, 1000);
        PWMAudioOff(BUZZER1);
        PWMAudioOn(BUZZER2);
        break;

      case 1:
        u8Button3Test = 2;

        PWMAudioSetFrequency(BUZZER1, 500);
        PWMAudioOn(BUZZER1);
        PWMAudioOff(BUZZER2);
        break;

      case 2:
        u8Button3Test = 0;

        PWMAudioOff(BUZZER1);
        PWMAudioOff(BUZZER2);
        break;
       
      default:
        break;
    }
  } /* End of BUTTON 3 test */

  
  /* LCD scrolling message */
  if(IsTimeUp(&u32LcdTimer, 200))
  {
    u32LcdTimer = G_u32SystemTime1ms;
    au8Temp1[20] = NULL;
    au8Temp2[20] = NULL;
    u8Index = u8ResetIndex;
    for(u8 i = 0; i < 20; i++)
    { 
      if( u8Index == 20)
      {
        u8Index = 0; 
      }
      au8Temp1[u8Index] = au8Eng[i];
      au8Temp2[u8Index] = au8MPG[i];
      u8Index++;
    }
    
    LcdMessage(LINE1_START_ADDR, au8Temp1);
    LcdMessage(LINE2_START_ADDR, au8Temp2);
    
    if(u8ResetIndex == 0)
    {
      u8ResetIndex = 21;
    }

    u8ResetIndex--;
  }

} /* end UserApp1SM_Idle() */
#endif

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  static bool bErrorStateEntered = FALSE;
  
  /* Print error state entry message once; application hangs here */
  if(!bErrorStateEntered)
  {
   DebugPrintf("\n\r***BOARDTEST ERROR STATE***\n\n\r");
   bErrorStateEntered = TRUE;
  }

} /* end UserApp1SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/

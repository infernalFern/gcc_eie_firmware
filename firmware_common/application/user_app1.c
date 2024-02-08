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
static u32 UserApp1_u32DataMsgCount = 0; //ANT_DATA packets received
static u32 UserApp1_u32TickMsgCount = 0; //ANT_TICK packets received

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

  //clear screen; place starting messages
  LcdCommand(LCD_CLEAR_CMD);
  LcdMessage(LINE1_START_ADDR, "ANT SLAVE DEMO");
  LcdMessage(LINE2_START_ADDR, "B0 Toggles Radio");

  //initialize with LED0 in red state (i.e. channel not configured)
  LedOn(RED);

  //start ANT channel information setup
	if (AntRadioStatusChannel(ANT_CHANNEL_0) == ANT_UNCONFIGURED) {
		/* Configure the ANT radio */
  	sChannelInfo.AntChannel          = U8_ANT_CHANNEL_USERAPP;
  	sChannelInfo.AntChannelType      = CHANNEL_TYPE_SLAVE;
  	sChannelInfo.AntChannelPeriodHi  = U8_ANT_CHANNEL_PERIOD_HI_USERAPP;
  	sChannelInfo.AntChannelPeriodLo  = U8_ANT_CHANNEL_PERIOD_LO_USERAPP;

  	sChannelInfo.AntDeviceIdHi       = U8_ANT_DEVICE_HI_USERAPP;
  	sChannelInfo.AntDeviceIdLo       = U8_ANT_DEVICE_LO_USERAPP;
  	sChannelInfo.AntDeviceType       = U8_ANT_DEVICE_TYPE_USERAPP;
  	sChannelInfo.AntTransmissionType = U8_ANT_TRANSMISSION_TYPE_USERAPP;

  	sChannelInfo.AntFrequency        = U8_ANT_FREQUENCY_USERAPP;
  	sChannelInfo.AntTxPower          = U8_ANT_TX_POWER_USERAPP;

  	sChannelInfo.AntNetwork = U8_ANT_NETWORK_DEFAULT;
  		
		for (u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++) {
  		sChannelInfo.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
  	}
	}

	// test for decent initialization and proceed to wait state for channel assignment, otherwise set state to idle.
	if ( AntAssignChannel(&sChannelInfo) ) {
    //channel configuration is queued, so advance to the next step
		UserApp1_pfStateMachine = UserApp1SM_WaitAntReady;
	} else {
    //the task is not initialized properly; shut down and don't run;
		LedBlink(RED, LED_4HZ);
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
		/* Update status LEDs and advance to Idle State*/
    LedOff(RED);
    LedOn(YELLOW);

    UserApp1_pfStateMachine = UserApp1SM_Idle;
	}
} // end UserApp1SM_WaitAntReady()

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for BUTTON0 to be pressed to open channel (LED is yellow) */
static void UserApp1SM_Idle(void) {
  /* Look for BUTTON 0 to open channel */
  if ( WasButtonPressed(BUTTON0) ) {
    /* Got BUTTON0, so complete one-time actions (i.e. button acknowledgement) before next state */
    ButtonAcknowledge(BUTTON0);
    
    /* Queue open channel and change LED0 from yellow to blinking green to indicate channel is opening */
    AntOpenChannelNumber(U8_ANT_CHANNEL_USERAPP);

    LedOff(YELLOW);
    LedBlink(GREEN, LED_2HZ);
    
    /* Set timer and advance states */
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_pfStateMachine = UserApp1SM_WaitChannelOpen;
  }
} /* end UserApp1SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for channel to be open.  (LED status: green blinking at 2Hz) */
static void UserApp1SM_WaitChannelOpen(void) {
	if (AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_OPEN) {
    //Channel opened: proceed to ChannelOpen State with LED at solid green
    LedOn(GREEN);
		UserApp1_pfStateMachine = UserApp1SM_ChannelOpen;
	}

  /* Check for timeout during the opening of the ANT channel */
  if ( IsTimeUp(&UserApp1_u32Timeout, U32_TIMEOUT_OPEN_CHANNEL) ) {
    
    //return to waiting for BUTTON0 to advance state
    AntCloseChannelNumber(U8_ANT_CHANNEL_USERAPP);
    LedOff(GREEN);
    LedOn(YELLOW);
    UserApp1_pfStateMachine = UserApp1SM_Idle;
  }
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* ANT channel open: process messages and update data */
static void UserApp1SM_ChannelOpen(void) {
  static u8 u8LastState = 0xff;
  static u8 au8TickMessage[] = "EVENT x\n\r";  /* "x" at index [6] will be replaced by the current code */
  static u8 au8DataContent[] = "xxxxxxxxxxxxxxxx";
  static u8 au8LastAntData[ANT_APPLICATION_MESSAGE_BYTES] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  static u8 au8TestMessage[] = {0, 0, 0, 0, 0xA5, 0, 0, 0};
  bool bGotNewData;


  /* Check for BUTTON0 to close channel */
  if (WasButtonPressed(BUTTON0)) {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON0);
    
    /* Queue close channel, initialize the u8LastState variable and change LED to blinking green to indicate channel is closing */
    AntCloseChannelNumber(U8_ANT_CHANNEL_USERAPP);
    u8LastState = 0xff;
    LedOff(BLUE);
    LedBlink(GREEN, LED_2HZ); //channel closing
    
    /* Set timer and advance states */
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_pfStateMachine = UserApp1SM_WaitChannelClose;
  } /* end if(WasButtonPressed(BUTTON0)) */


  /* A slave channel can close on its own, so explicitly check the channel's status */
  if (AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) != ANT_OPEN) {
    u8LastState = 0xff;
    LedBlink(GREEN, LED_2HZ);
    LedOff(BLUE);
    
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_pfStateMachine = UserApp1SM_WaitChannelClose;
  } /* if(AntRadioStatusChannel() != ANT_OPEN) */
  

  /* Check for new messages and process */
  if ( AntReadAppMessageBuffer() ) {
    /* New data message: check what it is */
    if (G_eAntApiCurrentMessageClass == ANT_DATA) {
      /* If data was received, the channel must therefore be synced with a Master */
      LedOff(GREEN);
      LedOn(BLUE);
      u8LastState = 0xff;
      UserApp1_u32DataMsgCount++;
      
      /* Check if the new data is the same as the old data and update as we go if this is not the case */
      bGotNewData = FALSE;
      for (u8 i = 0; i < ANT_APPLICATION_MESSAGE_BYTES; i++) {
        if (G_au8AntApiCurrentMessageBytes[i] != au8LastAntData[i]) {
          bGotNewData = TRUE; //message discrepancy present ==> we got new data
          au8LastAntData[i] = G_au8AntApiCurrentMessageBytes[i];

          au8DataContent[2 * i] = HexToASCIICharUpper(G_au8AntApiCurrentMessageBytes[i] / 16);
          au8DataContent[2 * i + 1] = HexToASCIICharUpper(G_au8AntApiCurrentMessageBytes[i] % 16); 
        }
      }
      
      if (bGotNewData) {
        /* We got new data: display this update on the LCD */
        LcdClearChars(LINE2_START_ADDR, 20); 
        LcdMessage(LINE2_START_ADDR, au8DataContent); 


        /* Update our local message counter and send the message back */
        au8TestMessage[7]++;
        if (au8TestMessage[7] == 0) {
          au8TestMessage[6]++;
          if (au8TestMessage[6] == 0) {
            au8TestMessage[5]++;
          }
        }

        //send out updated message from previous part above
        AntQueueBroadcastMessage(U8_ANT_CHANNEL_USERAPP, au8TestMessage);
        
        /* Check for a special packet and respond */
        if (G_au8AntApiCurrentMessageBytes[0] == 0xA5) {          
          if (G_au8AntApiCurrentMessageBytes[1] == 1) {
            LedOn(LCD_RED);
          } else {
            LedOff(LCD_RED);            
          }
          
          if (G_au8AntApiCurrentMessageBytes[2] == 1) {
            LedOn(LCD_GREEN);
          } else {
            LedOff(LCD_GREEN);            
          }

          if (G_au8AntApiCurrentMessageBytes[3] == 1) {
            LedOn(LCD_BLUE);
          } else {
            LedOff(LCD_BLUE);            
          }
        }

      } /* end if(bGotNewData) */

    } else if (G_eAntApiCurrentMessageClass == ANT_TICK) {
      /* A channel period has gone by */    
      /* Just increment a counter for now */
      UserApp1_u32TickMsgCount++;

      /* Look at the TICK contents to check the event code and respond only if it's different */
      if (u8LastState != G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX]) {
        /* The state changed so update u8LastState and queue a debug message to show EVENT CODE */
        u8LastState = G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX];
        au8TickMessage[6] = HexToASCIICharUpper(u8LastState);
        DebugPrintf(au8TickMessage);

        /* Parse u8LastState to update LED status */
        switch (u8LastState) {
          /* Handle "good response" code that can appear when other ANT commands are sent */
          case RESPONSE_NO_ERROR:
          {
            /* Don't do anything here for now */
            break;
          }

          /* If we are paired but missing messages, blue blinks */
          case EVENT_RX_FAIL:
          {
            LedOff(GREEN);
            LedBlink(BLUE, LED_2HZ);
            break;
          }

          /* If we drop to search, LED is green */
          case EVENT_RX_FAIL_GO_TO_SEARCH:
          {
            LedOff(BLUE);
            LedOn(GREEN);
            break;
          }

          /* If the search times out, the channel should automatically close */
          case EVENT_RX_SEARCH_TIMEOUT:
          {
            DebugPrintf("Search Timeout\r\n");
            break;
          }

          default:
          {
            DebugPrintf("Unexpected Event\r\n");
            break;
          }
        } /* end switch (G_au8AntApiCurrentData) */
      } /* end if (u8LastState ...) */

    } /* end ANT_TICK */

  } /* end AntReadAppMessageBuffer() */    

} /* end UserApp1SM_ChannelOpen() */



#if 0 //NOTE: from boardtest branch
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
/* Wait for channel to close.  LED status: green blink 2Hz */
static void UserApp1SM_WaitChannelClose(void) {
  /* Wait for the channel status to update */
  if (AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_CLOSED) {
    LedOff(GREEN);
    LedOn(YELLOW);

    UserApp1_pfStateMachine = UserApp1SM_Idle;
  }

  /* Check for timeout */
  if ( IsTimeUp(&UserApp1_u32Timeout, U32_TIMEOUT_CLOSE_CHANNEL) ) {
    LedOff(GREEN);
    LedBlink(RED, LED_4HZ);

    UserApp1_pfStateMachine = UserApp1SM_Error;
  }

} /* end UserApp1SM_WaitChannelClose() */

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void) {
  static bool bErrorStateEntered = FALSE;
  
  /* Print error state entry message once; application hangs here */
  if(!bErrorStateEntered) {
    DebugPrintf("\n\r***BOARDTEST ERROR STATE***\n\n\r");
    bErrorStateEntered = TRUE;
  }

} /* end UserApp1SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/

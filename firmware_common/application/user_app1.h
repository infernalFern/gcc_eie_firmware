/*!*********************************************************************************************************************
@file user_app1.h                                                                
@brief Header file for user_app1

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
1. Follow the instructions at the top of user_app1.c
2. Use ctrl-h to find and replace all instances of "user_app1" with "yournewtaskname"
3. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
4. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
5. Add #include yournewtaskname.h" to configuration.h
6. Add/update any special configurations required in configuration.h (e.g. peripheral assignment and setup values)
7. Delete this text (between the dashed lines)
----------------------------------------------------------------------------------------------------------------------

**********************************************************************************************************************/

#ifndef __USER_APP1_H
#define __USER_APP1_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/


/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void UserApp1Initialize(void);
void UserApp1RunActiveState(void);


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void UserApp1SM_SetupAnt(void);
static void UserApp1SM_Idle(void);    
static void UserApp1SM_Error(void);         



/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
/* BoardTest_u32Flags */

#define U8_ANT_CHANNEL_USERAPP (u8)ANT_CHANNEL_0       /* Channel 0 – 7 */
#define U8_ANT_DEVICE_LO_USERAPP (u8)0x7D              /* Low byte of two-byte Device # */
#define U8_ANT_DEVICE_HI_USERAPP (u8)0x04              /* High byte of two-byte Device # */
#define U8_ANT_DEVICE_TYPE_USERAPP (u8)1               /* 1 – 255 */
#define U8_ANT_TRANSMISSION_TYPE_USERAPP (u8)1         /* 1-127 (MSB is pairing bit) */
#define U8_ANT_CHANNEL_PERIOD_LO_USERAPP (u8)0x00      /* Low byte of two-byte channel period */
#define U8_ANT_CHANNEL_PERIOD_HI_USERAPP (u8)0x20      /* High byte of two-byte channel period */
#define U8_ANT_FREQUENCY_USERAPP (u8)50                /* 2400MHz + this number 0 – 99 */
#define U8_ANT_TX_POWER_USERAPP RADIO_TX_POWER_4DBM    /* RADIO_TX_POWER_xxx */

#if 0
#define _LEDS_ON                                  (u32)0x000000001  /* Set when audio and ANT test is active */
#define _AUDIO_ANT_ON                             (u32)0x000000008  /* Set when audio and ANT test is active */

#define U32_BOARDTEST_INIT_MSG_TIMEOUT            (u32)1000   /* Timeout for init display message to send */

/* BoardTest ANT radio settings */
/* MAKE SURE TO CHANGE THE u8 and u32 values for DEVICEID */
#define	U8_ANT_DEVICEID_LO_BOARDTEST			  (u8)0x7D
#define U8_ANT_DEVICEID_HI_BOARDTEST		          (u8)0x04
#define U32_ANT_DEVICEID_DEC_BOARDTEST	                  (u32)1149

/* IF YOU CHANGE DEVICE TYPE OR TRANSMISSION TYPE, YOU MUST CHANGE 
THE STARTUP TEXT MESSAGE IN BoardTestSM_SetupAnt */
#define	U8_ANT_DEVICE_TYPE_BOARDTEST					    U8_DEVICE_TYPE_BOARDTEST
#define	U8_ANT_TRANSMISSION_TYPE_BOARDTEST		    (u8)0x01

/* Default channel configuration parameters */
#define U8_ANT_CHANNEL_BOARDTEST                  ANT_CHANNEL_0
#define U8_ANT_CHANNEL_TYPE_BOARDTEST             CHANNEL_TYPE_MASTER
#define U8_ANT_NETWORK_BOARDTEST                  (u8)0

#define U16_ANT_CHANNEL_PERIOD_DEC_BOARDTEST      (u16)8192
#define U16_ANT_CHANNEL_PERIOD_HEX_BOARDTEST      (u16)0x2000
#define U8_ANT_CHANNEL_PERIOD_HI_BOARDTEST		    (u8)0x20
#define U8_ANT_CHANNEL_PERIOD_LO_BOARDTEST		    (u8)0x00

#define U8_ANT_FREQUENCY_BOARDTEST						        (u8)(50)
#define U8_ANT_TX_POWER_BOARDTEST					  	  RADIO_TX_POWER_0DBM

/* EiE Board Test specific Device Types */
#define	U8_DEVICE_TYPE_BOARDTEST		              (u8)0x60

#endif //endif 0

#endif /* __USER_APP1_H */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/

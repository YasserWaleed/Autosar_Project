/**********************************************************************************
 Module	     : Port
 File Name   : Port.h
 Author	     : Yasser Waleed
 Description : Header file declaring the function prototypes in the port driver
               and lists the module data types and the relevant macro definitions
***********************************************************************************/
#ifndef PORT_H
#define PORT_H
/**********************************************************************************
 INCLUDES
***********************************************************************************/
#include "Common_Macros.h"
#include "Std_Types.h"
#include "Port_Cfg.h"
/**********************************************************************************
 VERSIONS CHECK
***********************************************************************************/
/* Module Version 1.0.0 */
#define PORT_SW_MAJOR_VERSION           (1U)
#define PORT_SW_MINOR_VERSION           (0U)
#define PORT_SW_PATCH_VERSION           (0U)

/* AUTOSAR Version 4.0.3 */
#define PORT_AR_RELEASE_MAJOR_VERSION   (4U)
#define PORT_AR_RELEASE_MINOR_VERSION   (0U)
#define PORT_AR_RELEASE_PATCH_VERSION   (3U)

/* Macros for Port Status */
#define PORT_INITIALIZED                (1U)
#define PORT_NOT_INITIALIZED            (0U)


/* AUTOSAR version checking between Std Types and Port Modules */
#if ((STD_TYPES_AR_RELEASE_MAJOR_VERSION != PORT_AR_RELEASE_MAJOR_VERSION)\
 ||  (STD_TYPES_AR_RELEASE_MINOR_VERSION != PORT_AR_RELEASE_MINOR_VERSION)\
 ||  (STD_TYPES_AR_RELEASE_PATCH_VERSION != PORT_AR_RELEASE_PATCH_VERSION))
  #error "The AR version of Std_Types.h does not match the expected version"
#endif


 /*AUTOSAR version checking between Port_Cfg.h and Port.h files */
#if ((PORT_CFG_AR_RELEASE_MAJOR_VERSION != PORT_AR_RELEASE_MAJOR_VERSION)\
 ||  (PORT_CFG_AR_RELEASE_MINOR_VERSION != PORT_AR_RELEASE_MINOR_VERSION)\
 ||  (PORT_CFG_AR_RELEASE_PATCH_VERSION != PORT_AR_RELEASE_PATCH_VERSION))
  #error "The AR version of Port_Cfg.h does not match the expected version"
#endif

/* Software Version checking between Port_Cfg.h and Port.h files */
#if ((PORT_CFG_SW_MAJOR_VERSION != PORT_SW_MAJOR_VERSION)\
 ||  (PORT_CFG_SW_MINOR_VERSION != PORT_SW_MINOR_VERSION)\
 ||  (PORT_CFG_SW_PATCH_VERSION != PORT_SW_PATCH_VERSION))
  #error "The SW version of Port_Cfg.h does not match the expected version"
#endif 
/**********************************************************************************
 ID MACROS
***********************************************************************************/
#define PORT_VENDOR_ID    (1010U) 	/* ID for the vendor (company) in AUTOSAR, 1010 isn't an actual vendor ID */
#define PORT_MODULE_ID    (124U) 		/* Port Module Id */
#define PORT_INSTANCE_ID  (0U) 			/* Port Instance Id */
/******************************************************************************
 *      API SERVICE ID MACROS (from 8.3 Function Definitions)                 *
 ******************************************************************************/
#define PORT_INIT		    (uint8)0x00
#define PORT_SET_PIN_DIRECTION      (uint8)0x01
#define PORT_REFRESH_PORT_DIRECTION (uint8)0x02
#define PORT_GET_VERSION_INFO       (uint8)0x03
#define PORT_SET_PIN_MODE           (uint8)0x04
/******************************************************************************
 *      DET ERROR MACROS (from 7.2 Error Classification)                       *
 ******************************************************************************/
#define PORT_E_PARAM_PIN                 (uint8)0x0A /* Invalid Port Pin ID requested */
#define PORT_E_DIRECTION_UNCHANGEABLE    (uint8)0x0B /* Port Pin not configured as changeable */
#define PORT_E_PARAM_CONFIG              (uint8)0x0C /* API Port_Init service called with wrong parameter */
#define PORT_E_PARAM_INVALID_MODE        (uint8)0x0D /* Port Pin Mode passed not valid */
#define PORT_E_MODE_UNCHANGEABLE         (uint8)0x0E /* API Port_SetPinMode service called when the mode is unchangeable */
#define PORT_E_UNINIT                    (uint8)0x0F /* API service called without module initialization */
#define PORT_E_PARAM_POINTER             (uint8)0x10 /* API called with a NULL Pointer Parameter  */
/******************************************************************************
 *      DATA TYPES (from 8.2 Type definitions)                                *
 ******************************************************************************/
typedef uint8 Port_PinType;   	        /* 8.2.2 the symbolic name of a Port Pin */
typedef uint8 Port_PinModeType;		/* 8.2.4 configurable with the number of port pin modes */

/* Other Parameters: defining the port number of a port pin */
typedef enum 
{ 
  PORT_PORT_A,
  PORT_PORT_B,
  PORT_PORT_C,
  PORT_PORT_D,
  PORT_PORT_E,
  PORT_PORT_F	
}               Port_PortNumType;

typedef enum 
{ 
  PORT_PIN_0,
  PORT_PIN_1,
  PORT_PIN_2,
  PORT_PIN_3,
  PORT_PIN_4,
  PORT_PIN_5,
  PORT_PIN_6,
  PORT_PIN_7
}               Port_PinNumType;

/* 8.2.3 defining the direction of a port pin */
typedef enum  
{ 
  PORT_PIN_IN, 
  PORT_PIN_OUT 
}               Port_PinDirectionType ;

/* Defining the internal resistor of a port pin */
typedef enum 
{ 
  OFF, 
  PULL_UP, 
  PULL_DOWN 
}               Port_InternalResistorType;

/* Defining the pin level of a port pin */
typedef enum 
{ 
  LOW, 
  HIGH
}               Port_PinLevelInitValue;

/* Defining the pin direction changeability of a port pin */
typedef enum  
{ 
  DIR_NOT_CHANGEABLE, 
  DIR_IS_CHANGEABLE 
}               Port_PinDirChangeability;

/* Defining the pin mode changeability of a port pin */
typedef enum  
{ 
  MODE_NOT_CHANGEABLE, 
  MODE_IS_CHANGEABLE 
}               Port_PinModeChangeability;

/* Data Structure required for initializing the port Driver */
typedef struct 
{
  
  Port_PortNumType		PortNum;
  Port_PinNumType		PinNum;
  Port_PinDirectionType		PinDirection;
  Port_InternalResistorType 	InternalResistor;
  Port_PinLevelInitValue	PinLevelInitValue;
  Port_PinModeType 		PinInitMode;
  Port_PinDirChangeability 	DirChangeability;
  Port_PinModeChangeability	ModeChangeability;
  
} Port_ConfigPins;

/* An array carrying the initialization configurations */
typedef struct
{
  Port_ConfigPins PortPins [ PORT_NUMBER_OF_PORT_PINS ];     
} Port_ConfigType;

extern const Port_ConfigType PortConfigrations;  /* 8.2.1 external data structure containing all initialization data */

/******************************************************************************
 *      FUNCTION PROTOTYPES (from 8.3 Function Definitions)                   *
 ******************************************************************************/
/*******************************************************************************
* Service Name: Port_Init
* Service ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): ConfigPtr - Pointer to configuration set
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Function to Initialize the Port Driver module.
********************************************************************************/
void Port_Init( const Port_ConfigType* ConfigPtr );

/*******************************************************************************
* Service Name: Port_SetPinDirection
* Service ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: reentrant
* Parameters (in): Pin - Port Pin ID number , Direction - Port Pin Direction
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Function to Sets the port pin direction.
********************************************************************************/
#if (PortSetPinDirectionApi == TRUE)
void Port_SetPinDirection( Port_PinType Pin, Port_PinDirectionType Direction );
#endif

/*******************************************************************************
* Service Name: Port_RefreshPortDirection
* Service ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): None
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Refreshes port direction.
********************************************************************************/
void Port_RefreshPortDirection ( void );

/*******************************************************************************
* Service Name: Port_GetVersionInfo
* Service ID[hex]: 0x03
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): None
* Parameters (inout): None
* Parameters (out): versioninfo - Pointer to where to store the version information of this module.
* Return value: None
* Description: Returns the version information of this module.
********************************************************************************/
#if (PortVersionInfoApi == STD_ON)
void Port_GetVersionInfo ( Std_VersionInfoType* versioninfo );
#endif

/*******************************************************************************
* Service Name: Port_SetPinMode
* Service ID[hex]: 0x04
* Sync/Async: Synchronous
* Reentrancy: reentrant
* Parameters (in): Pin - Port Pin ID number, Mode - New Port Pin mode to be set on port pin
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Sets the port pin mode.
********************************************************************************/
void Port_SetPinMode ( Port_PinType Pin, Port_PinModeType Mode );

/*******************************************************************************
 END OF FILE                                     
********************************************************************************/
#endif
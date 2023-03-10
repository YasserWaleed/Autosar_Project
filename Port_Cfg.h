/**********************************************************************************
 Module		: Port
 File Name	: Port_Cfg.h
 Author		: Yasser Waleed
 Description    : Header file containing configurations. Enables/Disables certain APIs,
		  and defines number of pins.
***********************************************************************************/
#ifndef PORT_CFG_H
#define PORT_CFG_H
/**********************************************************************************
 VERSIONS CHECK
***********************************************************************************/
/* Module Version 1.0.0 */
#define PORT_CFG_SW_MAJOR_VERSION                      (1U)
#define PORT_CFG_SW_MINOR_VERSION                      (0U)
#define PORT_CFG_SW_PATCH_VERSION                      (0U)

/* AUTOSAR Version 4.0.3 */
#define PORT_CFG_AR_RELEASE_MAJOR_VERSION              (4U)
#define PORT_CFG_AR_RELEASE_MINOR_VERSION              (0U)
#define PORT_CFG_AR_RELEASE_PATCH_VERSION              (3U)
/**********************************************************************************
 CONFIGURATIONS
***********************************************************************************/
#define PORT_DEV_ERROR_DETECT	        (STD_ON)        /* Enable/Disable Development Error Detection */
  
#define PORT_VERSION_INFO_API	        (STD_ON)	/* Enable/Disable Get Version Info API */
  
#define PORT_SET_PIN_DIRECTION_API      (STD_ON)	/* Enable/Disable Port_SetPinDirection API */
  
#define PORT_SET_PIN_MODE_API	        (STD_ON)        /* Enable/Disable Port_SetPinMode API */
  
#define PORT_NUMBER_OF_PORT_PINS        (43U) 		/* Number of port pins in Tiva C */

/* Pin Modes */
#define PORT_PIN_MODE_ADC               0
#define PORT_PIN_MODE_ALT1              1
#define PORT_PIN_MODE_ALT2              2
#define PORT_PIN_MODE_ALT3              3
#define PORT_PIN_MODE_ALT4              4
#define PORT_PIN_MODE_ALT5              5
#define PORT_PIN_MODE_ALT6              6
#define PORT_PIN_MODE_ALT7              7
#define PORT_PIN_MODE_ALT8              8
#define PORT_PIN_MODE_ALT9              9
#define PORT_PIN_MODE_DIO               10
/*******************************************************************************
 END OF FILE                                     
********************************************************************************/
#endif
/********************************************************************************
 Module	        : Port
 File Name	: Port.c
 Author		: Yasser Waleed
 Description    : Source file containing the functions implementations
*********************************************************************************/
/********************************************************************************
 INCLUDES
*********************************************************************************/
#include "Port.h"
#include "tm4c123gh6pm_registers.h"
/********************************************************************************
 DEVELOPMENT ERRORS DETECTION
*********************************************************************************/
#if (PORT_DEV_ERROR_DETECT == STD_ON)

#include "Det.h"
/* AUTOSAR Version checking between Det and Port Modules */
#if ((DET_AR_MAJOR_VERSION != PORT_AR_RELEASE_MAJOR_VERSION)\
 || (DET_AR_MINOR_VERSION != PORT_AR_RELEASE_MINOR_VERSION)\
 || (DET_AR_PATCH_VERSION != PORT_AR_RELEASE_PATCH_VERSION))
  #error "The AR version of Det.h does not match the expected version"
#endif

#endif
/********************************************************************************
 LOCAL VARIABLES
*********************************************************************************/
static uint8 Port_Status = PORT_NOT_INITIALIZED;
static const Port_ConfigType* Port_ConfigPtr = NULL_PTR;
/************************************************************************************
* Service Name: Port_Init
* Service ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): ConfigPtr - Pointer to configuration set
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Function to Initialize the Port Driver module.
************************************************************************************/
void Port_Init(const Port_ConfigType* ConfigPtr)
{
  #if (PORT_DEV_ERROR_DETECT == STD_ON)
  /* check if the input configuration pointer is not a NULL_PTR */
  if(ConfigPtr == NULL_PTR)
  {
    Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_INIT, PORT_E_PARAM_CONFIG);
  }
  else
  {	/* Do Nothing */	}
  #endif

  Port_Status = PORT_INITIALIZED;
  Port_ConfigPtr = ConfigPtr;
  for (Port_PinType index = 0; index < PORT_NUMBER_OF_PORT_PINS; index++)
  {
    volatile uint32 * PortGpio_Ptr = NULL_PTR; /* point to the required Port Registers base address */
    volatile uint32 delay = 0;
    
    switch(ConfigPtr->PortPins[index].PortNum)
    {
      case  0: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; break;
      case  1: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; break;
      case  2: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; break;
      case  3: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; break;
      case  4: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; break;
      case  5: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; break;
    }
    
    /* Enable clock for PORT and allow time for clock to start*/
    SYSCTL_REGCGC2_REG |= (1<<Port_ConfigPtr->PortPins[index].PortNum);
    delay = SYSCTL_REGCGC2_REG;
    
    if(((Port_ConfigPtr->PortPins[index].PortNum == 3) && (Port_ConfigPtr->PortPins[index].PinNum == 7))\
    || ((Port_ConfigPtr->PortPins[index].PortNum == 5) && (Port_ConfigPtr->PortPins[index].PinNum == 0))) /* PD7 or PF0 */
    {
      *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_LOCK_REG_OFFSET) = 0x4C4F434B;                               /* Unlock the GPIOCR register */   
      SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_COMMIT_REG_OFFSET) , ConfigPtr->PortPins[index].PinNum);  /* Set the corresponding bit in GPIOCR register to allow changes on this pin */
    }
		
    else if((Port_ConfigPtr->PortPins[index].PortNum == 2) && (Port_ConfigPtr->PortPins[index].PinNum <= 3)) /* PC0 to PC3 */
    {
      /* Do Nothing ...  this is a JTAG pin */
    }
		
    else
    {
      /* Do Nothing ... No need to unlock the commit register for this pin */
    }
    
    if (Port_ConfigPtr->PortPins[index].PinInitMode == PORT_PIN_MODE_DIO)
    {
      /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
      CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , Port_ConfigPtr->PortPins[index].PinNum);

      /* Disable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */
      CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , Port_ConfigPtr->PortPins[index].PinNum);

      /* Clear the PMCx bits for this pin */
      *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (Port_ConfigPtr->PortPins[index].PinNum * 4));

      /* Set the corresponding bit in the GPIODEN register to enable digital functionality on this pin */
      SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , Port_ConfigPtr->PortPins[index].PinNum);
    }
    
    else if (Port_ConfigPtr->PortPins[index].PinInitMode == PORT_PIN_MODE_ADC)
    {
      /* Clear the corresponding bit in the GPIODEN register to disable digital functionality on this pin */
      CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , Port_ConfigPtr->PortPins[index].PinNum);

      /* Disable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */
      CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , Port_ConfigPtr->PortPins[index].PinNum);

      /* Clear the PMCx bits for this pin */
      *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (Port_ConfigPtr->PortPins[index].PinNum * 4));

      /* Set the corresponding bit in the GPIOAMSEL register to enable analog functionality on this pin */
      SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , Port_ConfigPtr->PortPins[index].PinNum);
    }
    
    else /* Another mode */
    {
      /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
      CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , Port_ConfigPtr->PortPins[index].PinNum);

      /* Enable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */
      SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , Port_ConfigPtr->PortPins[index].PinNum);

      /* Set the PMCx bits for this pin */
      *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) |= (Port_ConfigPtr->PortPins[index].PinInitMode & 0x0000000F << (Port_ConfigPtr->PortPins[index].PinNum * 4));

      /* Set the corresponding bit in the GPIODEN register to enable digital functionality on this pin */
      SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , Port_ConfigPtr->PortPins[index].PinNum);
    }

    if(Port_ConfigPtr->PortPins[index].PinDirection == PORT_PIN_OUT)
    {
      /* Set the corresponding bit in the GPIODIR register to configure it as output pin */
      SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , Port_ConfigPtr->PortPins[index].PinNum);

      if(Port_ConfigPtr->PortPins[index].PinLevelInitValue == STD_HIGH)
      {
        /* Set the corresponding bit in the GPIODATA register to provide initial value 1 */
        SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DATA_REG_OFFSET) , Port_ConfigPtr->PortPins[index].PinNum);
      }
      else
      {
        /* Clear the corresponding bit in the GPIODATA register to provide initial value 0 */
        CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DATA_REG_OFFSET) , Port_ConfigPtr->PortPins[index].PinNum);
      }
    }
    
    else if(Port_ConfigPtr->PortPins[index].PinDirection == PORT_PIN_IN)
    {
      /* Clear the corresponding bit in the GPIODIR register to configure it as input pin */
      CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , Port_ConfigPtr->PortPins[index].PinNum);

      if(Port_ConfigPtr->PortPins[index].InternalResistor == PULL_UP)
      {
        /* Set the corresponding bit in the GPIOPUR register to enable the internal pull up pin */
        SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_UP_REG_OFFSET) , Port_ConfigPtr->PortPins[index].PinNum);
      }
      
      else if(Port_ConfigPtr->PortPins[index].InternalResistor == PULL_DOWN)
      {
        /* Set the corresponding bit in the GPIOPDR register to enable the internal pull down pin */
        SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_DOWN_REG_OFFSET) , Port_ConfigPtr->PortPins[index].PinNum);
      }
      
      else
      {
        /* Clear the corresponding bit in the GPIOPUR register to disable the internal pull up pin */
        CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_UP_REG_OFFSET) , Port_ConfigPtr->PortPins[index].PinNum);
        
        /* Clear the corresponding bit in the GPIOPDR register to disable the internal pull down pin */
        CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_DOWN_REG_OFFSET) , Port_ConfigPtr->PortPins[index].PinNum);
      }
    }
    
    else
    {
      /* Do Nothing */
    }
  }
}

/************************************************************************************
* Service Name: Port_SetPinDirection
* Service ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: reentrant
* Parameters (in): Pin - Port Pin ID number , PinDirection - Port Pin PinDirection
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Function to Sets the port pin PinDirection.
************************************************************************************/
#if (PORT_SET_PIN_PinDirection_API == STD_ON)
void Port_SetPinPinDirection(Port_PinType Pin, Port_PinDirectionType PinDirection)
{
  #if (PORT_DEV_ERROR_DETECT == STD_ON)
  /* Check if the Driver is initialized before using this function */
  if(Port_Status == PORT_NOT_INITIALIZED)
  {
    Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_DIR, PORT_E_UNINIT);
  }
  else
  {	
    /* Do Nothing */	
  }

  /* check if incorrect Port Pin ID passed */
  if(Pin >= PORT_CONFIGURED_PINS)
  {
    Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_DIR, PORT_E_PARAM_PIN);
  }
  else
  {	
    /* Do Nothing */	
  }

  /* check if Port Pin not configured as changeable */
  if(Port_ConfigPtr->PortPins[Pin].DirChangeability == STD_OFF)
  {
    Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_DIR, PORT_E_PinDirection_UNCHANGEABLE);
  }
  else
  {	
    /* Do Nothing */	
  }
  #endif
  
  volatile uint32 * PortGpio_Ptr = NULL_PTR; /* point to the required Port Registers base address */

  switch(Port_ConfigPtr->PortPins[Pin].PortNum)
  {
    case  0: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; break;
    case  1: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; break;
    case  2: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; break;
    case  3: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; break;
    case  4: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; break;
    case  5: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; break;
  }

  if( (Port_ConfigPtr->PortPins[Pin].PortNum == 2) && (Port_ConfigPtr->PortPins[Pin].PinNum <= 3)) /* PC0 to PC3 */
  {
    /* Do Nothing ...  this is a JTAG pin */
    return;
  }

  if(PinDirection == PORT_PIN_OUT)
  {
    /* Set the corresponding bit in the GPIODIR register to configure it as output pin */
    SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , Port_ConfigPtr->PortPins[Pin].PinNum);
  }
  else if(PinDirection == PORT_PIN_IN)
  {
    /* Clear the corresponding bit in the GPIODIR register to configure it as input pin */
    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , Port_ConfigPtr->PortPins[Pin].PinNum);
  }
  else
  {	
    /* Do Nothing */	
  }
}
#endif

/************************************************************************************
* Service Name: Port_RefreshPortDirection
* Service ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): None
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Refreshes port PinDirection.
************************************************************************************/
void Port_RefreshPortPinDirection(void)
{
  #if (PORT_DEV_ERROR_DETECT == STD_ON)
  /* Check if the Driver is initialized before using this function */
  if(Port_Status == PORT_NOT_INITIALIZED)
  {
    Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_REFRESH_PORT_DIRECTION, PORT_E_UNINIT);
  }
  else
  {	
    /* Do Nothing */	
  }
  #endif
  
  for(Port_PinType index = 0; index < PORT_NUMBER_OF_PORT_PINS; index++)
  {
    volatile uint32 * PortGpio_Ptr = NULL_PTR; /* point to the required Port Registers base address */

    switch(Port_ConfigPtr->PortPins[index].PortNum)
    {
      case  0: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; break;
      case  1: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; break;
      case  2: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; break;
      case  3: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; break;
      case  4: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; break;
      case  5: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; break;
    }

    if( (Port_ConfigPtr->PortPins[index].PortNum == 2) && (Port_ConfigPtr->PortPins[index].PinNum <= 3)) /* PC0 to PC3 */
    {
      /* Do Nothing ...  this is a JTAG pin */
      continue;
    }

    /* PORT061: The function Port_RefreshPortPinDirection shall exclude those port pins from
      refreshing that are configured as "pin PinDirection changeable during runtime" */
    if (Port_ConfigPtr->PortPins[index].DirChangeability == STD_OFF)
    {
      if(Port_ConfigPtr->PortPins[index].PinDirection == PORT_PIN_OUT)
      {
              /* Set the corresponding bit in the GPIODIR register to configure it as output pin */
              SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , Port_ConfigPtr->PortPins[index].PinNum);
      }
      else if(Port_ConfigPtr->PortPins[index].PinDirection == PORT_PIN_IN)
      {
              /* Clear the corresponding bit in the GPIODIR register to configure it as input pin */
              CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , Port_ConfigPtr->PortPins[index].PinNum);
      }
      
      else
      {	
        /* Do Nothing */	
      }
    }
    else
    {	/* Do Nothing */	}
  }
}

#if (PORT_VERSION_INFO_API == STD_ON)
void Port_GetVersionInfo(Std_VersionInfoType* versioninfo)
{
  #if (PORT_DEV_ERROR_DETECT == STD_ON)
  /* check if the input configuration pointer is not a NULL_PTR */
  if(versioninfo == NULL_PTR)
  {
    Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_GET_VERSION_INFO, PORT_E_PARAM_POINTER);
  }
  else
  {
    /* Do Nothing */	
  }

  /* Check if the Driver is initialized before using this function */
  if(Port_Status == PORT_NOT_INITIALIZED)
  {
    Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_GET_VERSION_INFO, PORT_E_UNINIT);
  }
  else
   {	
     /* Do Nothing */	
   }
  #endif

  /* Copy the module Id */
  versioninfo->moduleID = (uint16)PORT_MODULE_ID;
  /* Copy the vendor Id */
  versioninfo->vendorID = (uint16)PORT_VENDOR_ID;
  /* Copy Software Major Version */
  versioninfo->sw_major_version = (uint8)PORT_SW_MAJOR_VERSION;
  /* Copy Software Minor Version */
  versioninfo->sw_minor_version = (uint8)PORT_SW_MINOR_VERSION;
  /* Copy Software Patch Version */
  versioninfo->sw_patch_version = (uint8)PORT_SW_PATCH_VERSION;
}
#endif

/************************************************************************************
* Service Name: Port_SetPinMode
* Service ID[hex]: 0x04
* Sync/Async: Synchronous
* Reentrancy: reentrant
* Parameters (in): Pin - Port Pin ID number, Mode - New Port Pin mode to be set on port pin
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Sets the port pin mode.
************************************************************************************/
#if (PORT_SET_PIN_MODE_API == STD_ON)
void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode)
{
  #if (PORT_DEV_ERROR_DETECT == STD_ON)
  /* Check if the Driver is initialized before using this function */
  if(Port_Status == PORT_NOT_INITIALIZED)
  {
    Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_MODE, PORT_E_UNINIT);
  }
  else
  {	
    /* Do Nothing */	
  }

  /* check if incorrect Port Pin ID passed */
  if(Pin >= PORT_NUMBER_OF_PORT_PINS)
  {
    Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_MODE, PORT_E_PARAM_PIN);
  }
  else
   {	
     /* Do Nothing */	
   }

  /* check if the Port Pin Mode passed not valid */
  if(Mode > PORT_PIN_MODE_DIO)
  {
    Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_MODE, PORT_E_PARAM_INVALID_MODE);
  }
  else
  {	
    /* Do Nothing */	
  }

  /* check if the API called when the mode is unchangeable */
  if(Port_ConfigPtr->PortPins[Pin].ModeChangeability == STD_OFF)
  {
    Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_MODE, PORT_E_MODE_UNCHANGEABLE);
  }
  else
  {	
    /* Do Nothing */	
  }
  #endif

  volatile uint32 * PortGpio_Ptr = NULL_PTR; /* point to the required Port Registers base address */

  switch(Port_ConfigPtr->PortPins[Pin].PortNum)
  {
    case  0: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; break;
    case  1: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; break;
    case  2: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; break;
    case  3: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; break;
    case  4: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; break;
    case  5: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; break;
  }

  if((Port_ConfigPtr->PortPins[Pin].PortNum == 2) && (Port_ConfigPtr->PortPins[Pin].PinNum <= 3)) /* PC0 to PC3 */
  {
    /* Do Nothing ...  this is a JTAG pin */
    return;
  }

  if (Mode == PORT_PIN_MODE_DIO)
  {
    /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , Port_ConfigPtr->PortPins[Pin].PinNum);

    /* Disable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */
    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , Port_ConfigPtr->PortPins[Pin].PinNum);

    /* Clear the PMCx bits for this pin */
    *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (Port_ConfigPtr->PortPins[Pin].PinNum * 4));

    /* Set the corresponding bit in the GPIODEN register to enable digital functionality on this pin */
    SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , Port_ConfigPtr->PortPins[Pin].PinNum);
  }
  else if (Mode == PORT_PIN_MODE_ADC)
  {
    /* Clear the corresponding bit in the GPIODEN register to disable digital functionality on this pin */
    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , Port_ConfigPtr->PortPins[Pin].PinNum);

    /* Disable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */
    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , Port_ConfigPtr->PortPins[Pin].PinNum);

    /* Clear the PMCx bits for this pin */
    *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (Port_ConfigPtr->PortPins[Pin].PinNum * 4));

    /* Set the corresponding bit in the GPIOAMSEL register to enable analog functionality on this pin */
    SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , Port_ConfigPtr->PortPins[Pin].PinNum);
  }
  else /* Another mode */
  {
    /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , Port_ConfigPtr->PortPins[Pin].PinNum);

    /* Enable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */
    SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , Port_ConfigPtr->PortPins[Pin].PinNum);

    /* Set the PMCx bits for this pin */
    *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) |= (Mode & 0x0000000F << (Port_ConfigPtr->PortPins[Pin].PinNum * 4));

    /* Set the corresponding bit in the GPIODEN register to enable digital functionality on this pin */
    SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , Port_ConfigPtr->PortPins[Pin].PinNum);
  }
}
/*******************************************************************************
 END OF FILE                                     
********************************************************************************/
#endif

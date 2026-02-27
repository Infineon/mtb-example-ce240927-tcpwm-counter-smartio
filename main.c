/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the TCPWM counter using Smart IO
*              Example for ModusToolbox.
*
* Related Document: See README.md
*
*******************************************************************************
 * (c) 2024-2026, Infineon Technologies AG, or an affiliate of Infineon
 * Technologies AG. All rights reserved.
 * This software, associated documentation and materials ("Software") is
 * owned by Infineon Technologies AG or one of its affiliates ("Infineon")
 * and is protected by and subject to worldwide patent protection, worldwide
 * copyright laws, and international treaty provisions. Therefore, you may use
 * this Software only as provided in the license agreement accompanying the
 * software package from which you obtained this Software. If no license
 * agreement applies, then any use, reproduction, modification, translation, or
 * compilation of this Software is prohibited without the express written
 * permission of Infineon.
 *
 * Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
 * IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
 * THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
 * SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
 * Infineon reserves the right to make changes to the Software without notice.
 * You are responsible for properly designing, programming, and testing the
 * functionality and safety of your intended application of the Software, as
 * well as complying with any legal requirements related to its use. Infineon
 * does not guarantee that the Software will be free from intrusion, data theft
 * or loss, or other breaches ("Security Breaches"), and Infineon shall have
 * no liability arising out of any Security Breaches. Unless otherwise
 * explicitly approved by Infineon, the Software may not be used in any
 * application where a failure of the Product or any consequences of the use
 * thereof can reasonably be expected to result in personal injury.
*******************************************************************************/

#include "cy_pdl.h"
#include "cybsp.h"

/*******************************************************************************
* Macros
********************************************************************************/

/*******************************************************************************
* Function Prototypes
********************************************************************************/

/*******************************************************************************
* Global Variables
********************************************************************************/

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This counter will be triggered to start by PWM output signal, and counter period is
* 60000 with 200Khz clock frequency. Enable the CC0 match interrupt, CC0 match value
* is 50000. It will generate the interrupt when counter up to CC0 match value, then
* toggle user LED1 with interrupt generated. The signal to User LED1, which switches
* depending on the level of the CC0 match interrupt of the TCPWM counter initiated by
* the PWM signal on the TRAVEOO&trade; T2G MCU, as the input port of Smart IO, and
* confirm that the output of Smart IO is reflected to User LED2.
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;
    uint32_t intrMask;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

    /*Initialize the User LED*/
    Cy_GPIO_Pin_Init(CYBSP_USER_LED1_PORT, CYBSP_USER_LED1_PIN, &CYBSP_USER_LED1_config );

    /* Then start the counter, the counter will be trigger by PWM signal */

    /*TCPWM PWM Mode initial*/
    if (CY_TCPWM_SUCCESS != Cy_TCPWM_PWM_Init(TCPWM_PWM_HW, TCPWM_PWM_NUM, &TCPWM_PWM_config))
    {
        CY_ASSERT(0);
    }

    /* Enable the initialized PWM */
    Cy_TCPWM_PWM_Enable(TCPWM_PWM_HW, TCPWM_PWM_NUM);

    /* Then start the PWM */
    Cy_TCPWM_TriggerReloadOrIndex_Single(TCPWM_PWM_HW, TCPWM_PWM_NUM);

    /*TCPWM Counter Mode initial*/
    if (CY_TCPWM_SUCCESS != Cy_TCPWM_Counter_Init(TCPWM_COUNTER_HW, TCPWM_COUNTER_NUM, &TCPWM_COUNTER_config))
    {
        CY_ASSERT(0);
    }

    /* Enable the initialized counter */
    Cy_TCPWM_Counter_Enable(TCPWM_COUNTER_HW, TCPWM_COUNTER_NUM);

    /* Initialize the SmartIO block. The Smart I/O connect the output (LED1) of TCPWM counter to
     * output pin to toggle an external LED2 */
    Cy_SmartIO_Init(SMART_IO_HW, &SMART_IO_config);

    /* Enable the Smart I/O */
    Cy_SmartIO_Enable(SMART_IO_HW);

    for (;;)
    {
        /*get the counter CC0 compare interrupt mask*/
        intrMask = Cy_TCPWM_GetInterruptStatusMasked(TCPWM_COUNTER_HW, TCPWM_COUNTER_NUM);
        if (intrMask == CY_TCPWM_INT_ON_CC0)
        {
            /*toggle user LED1*/
            Cy_GPIO_Inv(CYBSP_USER_LED1_PORT, CYBSP_USER_LED1_PIN);

            /*clear the counter CC0 compare interrupt*/
            Cy_TCPWM_ClearInterrupt(TCPWM_COUNTER_HW, TCPWM_COUNTER_NUM, CY_TCPWM_INT_ON_CC0);
        }
    }
}

/* [] END OF FILE */

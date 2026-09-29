/*******************************************************************************
  PMP Peripheral Library Template Implementation

  File Name:
    pmp_DualModeReadAddressControl_Default.h

  Summary:
    PMP PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : DualModeReadAddressControl
    and its Variant : Default
    For following APIs :
        PLIB_PMP_ExistsDualModeReadAddressControl
        PLIB_PMP_DualModeReadAddressSet
        PLIB_PMP_DualModeReadAddressGet

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/

//DOM-IGNORE-END

#ifndef _PMP_DUALMODEREADADDRESSCONTROL_DEFAULT_H
#define _PMP_DUALMODEREADADDRESSCONTROL_DEFAULT_H

//******************************************************************************
/* Routines available for accessing VREGS, MASKS, POS, LEN are 

  VREGs: 
    _PMP_DUAL_READ_ADDRESS_CONTROL_VREG(index)

  MASKs: 
    _PMP_DUAL_READ_ADDRESS_CONTROL_MASK(index)

  POSs: 
    _PMP_DUAL_READ_ADDRESS_CONTROL_POS(index)

  LENs: 
    _PMP_DUAL_READ_ADDRESS_CONTROL_LEN(index)

*/


//******************************************************************************
/* Function :  PMP_ExistsDualModeReadAddressControl_Default

  Summary:
    Implements Default variant of PLIB_PMP_ExistsDualModeReadAddressControl

  Description:
    This template implements the Default variant of the PLIB_PMP_ExistsDualModeReadAddressControl function.
*/

#define PLIB_PMP_ExistsDualModeReadAddressControl PLIB_PMP_ExistsDualModeReadAddressControl
PLIB_TEMPLATE bool PMP_ExistsDualModeReadAddressControl_Default( PMP_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  PMP_DualModeReadAddressSet_Default

  Summary:
    Implements Default variant of PLIB_PMP_DualModeReadAddressSet 

  Description:
    This template implements the Default variant of the PLIB_PMP_DualModeReadAddressSet function.
*/

PLIB_TEMPLATE void PMP_DualModeReadAddressSet_Default( PMP_MODULE_ID index , uint32_t readAddress )
{
    _SFR_WRITE(
        _PMP_DUAL_READ_ADDRESS_CONTROL_VREG(index),
         (readAddress)
    );
}


//******************************************************************************
/* Function :  PMP_DualModeReadAddressGet_Default

  Summary:
    Implements Default variant of PLIB_PMP_DualModeReadAddressGet 

  Description:
    This template implements the Default variant of the PLIB_PMP_DualModeReadAddressGet function.
*/

PLIB_TEMPLATE uint32_t PMP_DualModeReadAddressGet_Default( PMP_MODULE_ID index )
{
    return(_SFR_READ( _PMP_DUAL_READ_ADDRESS_CONTROL_VREG ( index ) ));
}


#endif /*_PMP_DUALMODEREADADDRESSCONTROL_DEFAULT_H*/

/******************************************************************************
 End of File
*/


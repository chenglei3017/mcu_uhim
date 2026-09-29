/*******************************************************************************
  PMP Peripheral Library Template Implementation

  File Name:
    pmp_OperationMode_Default.h

  Summary:
    PMP PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : OperationMode
    and its Variant : Default
    For following APIs :
        PLIB_PMP_ExistsOperationMode
        PLIB_PMP_OperationModeSelect
        PLIB_PMP_OperationModeGet

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2012 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _PMP_OPERATIONMODE_DEFAULT_H
#define _PMP_OPERATIONMODE_DEFAULT_H

//******************************************************************************
/* Routines available for accessing VREGS, MASKS, POS, LEN are 

  VREGs: 
    _PMP_OP_MODE_VREG(index)

  MASKs: 
    _PMP_OP_MODE_MASK(index)

  POSs: 
    _PMP_OP_MODE_POS(index)

  LENs: 
    _PMP_OP_MODE_LEN(index)

*/


//******************************************************************************
/* Function :  PMP_ExistsOperationMode_Default

  Summary:
    Implements Default variant of PLIB_PMP_ExistsOperationMode

  Description:
    This template implements the Default variant of the PLIB_PMP_ExistsOperationMode function.
*/

#define PLIB_PMP_ExistsOperationMode PLIB_PMP_ExistsOperationMode
PLIB_TEMPLATE bool PMP_ExistsOperationMode_Default( PMP_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  PMP_OperationModeSelect_Default

  Summary:
    Implements Default variant of PLIB_PMP_OperationModeSelect 

  Description:
    This template implements the Default variant of the PLIB_PMP_OperationModeSelect function.
*/

PLIB_TEMPLATE void PMP_OperationModeSelect_Default( PMP_MODULE_ID index , PMP_OPERATION_MODE operationMode )
{
    _SFR_FIELD_WRITE(	_PMP_OP_MODE_VREG(index),
						_PMP_OP_MODE_MASK(index),
						_PMP_OP_MODE_POS(index),
						operationMode);
}


//******************************************************************************
/* Function :  PMP_OperationModeGet_Default

  Summary:
    Implements Default variant of PLIB_PMP_OperationModeGet 

  Description:
    This template implements the Default variant of the PLIB_PMP_OperationModeGet function.
*/

PLIB_TEMPLATE PMP_OPERATION_MODE PMP_OperationModeGet_Default( PMP_MODULE_ID index )
{
    return (PMP_OPERATION_MODE)(_SFR_FIELD_READ(_PMP_OP_MODE_VREG(index),
						_PMP_OP_MODE_MASK(index),
						_PMP_OP_MODE_POS(index)));
}


#endif /*_PMP_OPERATIONMODE_DEFAULT_H*/

/******************************************************************************
 End of File
*/


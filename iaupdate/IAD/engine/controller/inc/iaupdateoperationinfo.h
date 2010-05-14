/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   This file contains the header file of CIAUpdateOperationInfo class 
*
*/



#ifndef IA_UPDATE_OPERATION_INFO_H
#define IA_UPDATE_OPERATION_INFO_H


#include <e32def.h>

class MNcdOperation;


/**
 * TIAUpdateOperationInfo
 *
 * Information container for elements in arrays describing on-going operations.
 */
class TIAUpdateOperationInfo
    {

public:

    /**
     * Type of the on-going operation.
     */
    enum TOperationType
        {
        EIdle,
        ELoadRoot,
        ELoadContainer,
        ELoadChildren,
        ELoadAllChildren
        };


    /**
     * Default constructor
     */
    TIAUpdateOperationInfo():
        iOperationType( EIdle ),
        iOperation( NULL )
        {
        }


    /**
     * Constructor
     */
    TIAUpdateOperationInfo( TOperationType aOperationType, 
                            MNcdOperation* aOperation ): 
        iOperationType( aOperationType ),
        iOperation( aOperation )
        {
        }


    TOperationType iOperationType;
    MNcdOperation* iOperation;
    
    };

#endif // IA_UPDATE_LOAD_INFO_H

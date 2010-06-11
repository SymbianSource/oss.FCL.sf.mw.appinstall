/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/



#ifndef IA_UPDATE_OPERATION_H
#define IA_UPDATE_OPERATION_H


class CIAUpdateNode;
class MIAUpdateContentOperationObserver;


/**
 * MIAUpdateOperation
 *
 * General interface for all the node operations.
 *
 * @see MIAUpdateContentOperationObserver
 */
class MIAUpdateOperation
    {
    
public:

    /**
     * Empty destructor defined for this interface.
     * Now, the object can be deleted through this interface.
     */
    virtual ~MIAUpdateOperation() { }


    /**
     * @return TBool ETrue if the operation was started and observer's  
     * MIAUpdateContentOperationObserver::ContentOperationComplete callback
     * function will be called when completed. EFalse if operation was not started.
     */
    virtual TBool StartOperationL() = 0;
    
    /**
     * Will result a call to observer ContentOperationComplete
     * function if operation is on.
     */
    virtual void CancelOperation() = 0;    
        
    };

#endif // IA_UPDATE_OPERATION_H


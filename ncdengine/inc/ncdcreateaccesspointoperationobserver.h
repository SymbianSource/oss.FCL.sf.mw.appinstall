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
* Description:   Observer interface definition
*
*/
	

#ifndef M_NCDCREATEACCESSPOINTOPERATIONOBSERVER_H
#define M_NCDCREATEACCESSPOINTOPERATIONOBSERVER_H

#include <e32cmn.h>

class MNcdCreateAccessPointOperation;

/**
 *  Observer interface for accesspoint creation operation
 *
 * 
 */
class MNcdCreateAccessPointOperationObserver
    {
    
public:

    /**
     * Called when an operation has been completed.
     *
     * 
     * @param aOperation The operation that sent the event.
     * @param aError Error code for operation completion.
     *               KErrNone for successful completion, otherwise a system
     *               wide error code.
     */
    virtual void OperationComplete( 
        MNcdCreateAccessPointOperation& aOperation,
        TInt aError ) = 0;
    
    };
    
#endif //  M_NCDCREATEACCESSPOINTOPERATIONOBSERVER_H

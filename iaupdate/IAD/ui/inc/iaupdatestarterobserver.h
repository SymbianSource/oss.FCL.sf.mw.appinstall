/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   MIAUpdateStarterObserver interface
*
*/



#ifndef IAUPDATESTARTEROBSERVER_H
#define IAUPDATESTARTEROBSERVER_H


/**
 * MIAUpdateStarterObserver interface is used to observe
 * when the requested operation has been completed.
 *
 */
class MIAUpdateStarterObserver
    {
    
public:

    /**
     * Called when an outstanding request is completed
     *
     * @param aError Error code that informs the result 
     * of the start operation. KErrNone if operation was
     * succesfull.
     */
    virtual void StartExecutableCompletedL( TInt aError ) = 0;
  
    };

#endif // IAUPDATESTARTEROBSERVER_H
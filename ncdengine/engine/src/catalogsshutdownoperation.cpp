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
* Description:   Implementation of CCatalogsShutdownOperation
*
*/


#include "catalogsshutdownoperation.h"

#include "catalogsdebug.h"

CCatalogsShutdownOperation::CCatalogsShutdownOperation( 
    const TUid& aFamilyUid ) :
    iFamilyUid( aFamilyUid )
    {
    }


CCatalogsShutdownOperation::~CCatalogsShutdownOperation()
    {
    }


void CCatalogsShutdownOperation::Execute()
    {
    DLTRACEIN((""));
    Cancel();
    TRAPD( err, DoExecuteL() );
    if ( err != KErrNone )
        {        
        NotifyObserver( err );
        }
    }


void CCatalogsShutdownOperation::SetObserver( 
    MCatalogsShutdownOperationObserver& aObserver )
    {
    iObserver = &aObserver;
    }


const TUid& CCatalogsShutdownOperation::FamilyUid() const
    {
    return iFamilyUid;
    }


void CCatalogsShutdownOperation::Cancel()
    {
    DoCancel();    
    }


void CCatalogsShutdownOperation::NotifyObserver( TInt aError )
    {
    DASSERT( iObserver );
    iObserver->ShutdownOperationComplete( this, aError );
    }

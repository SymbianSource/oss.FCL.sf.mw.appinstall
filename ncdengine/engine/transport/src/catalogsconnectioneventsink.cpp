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
* Description:   ?Description
*
*/


#include "catalogsconnectioneventsink.h"

#include <e32property.h>

#include "catalogsconstants.h"
#include "catalogsdebug.h"

CCatalogsConnectionEventSink* CCatalogsConnectionEventSink::NewL( const TUid& aClientSID )
    {
    CCatalogsConnectionEventSink* self = new ( ELeave ) CCatalogsConnectionEventSink;
    CleanupStack::PushL( self );
    self->ConstructL( aClientSID );
    CleanupStack::Pop( self );
    return self;
    }

CCatalogsConnectionEventSink::CCatalogsConnectionEventSink() 
    : iCategoryUid( RProcess().SecureId() )
    {	
    }    

void CCatalogsConnectionEventSink::ConstructL( const TUid& aClientSID )
    {
    DLTRACEIN((""));
    // security: allow client to read, allow only server to write
    _LIT_SECURITY_POLICY_S0( KReadPolicy, aClientSID.iUid );
        
    // Note that clients that read this property must know the SID of the process
    // that publishes this property
    _LIT_SECURITY_POLICY_S0( KWritePolicy, iCategoryUid.iUid ); 
    iKey = aClientSID.iUid;
    
    TInt rc = RProperty::Define( iCategoryUid, iKey, 
                                 RProperty::EInt, KReadPolicy, KWritePolicy );
    
    // Property might already exists, for example if server process had panic,
    // so that is not a problem.
    if ( rc != KErrAlreadyExists )
        {
        User::LeaveIfError( rc );
        }
    }    

CCatalogsConnectionEventSink::~CCatalogsConnectionEventSink()
    {
    RProperty::Delete( iCategoryUid, iKey );
    }
    
void CCatalogsConnectionEventSink::ReportConnectionStatus( TBool aActive )
    {
    DLTRACEIN((""));
    TBool wasActive = iActiveConnectionCount > 0;
    
    if ( aActive )
        {
        iActiveConnectionCount += 1;
        }
    else
        {
        iActiveConnectionCount -= 1;
        DASSERT( iActiveConnectionCount >= 0 );
        }
    
    TBool isActive = iActiveConnectionCount > 0;
    
    if ( wasActive != isActive )
        {
        TInt rc = RProperty::Set( iCategoryUid, iKey, isActive );
        DASSERT( rc == KErrNone );
        }
    }

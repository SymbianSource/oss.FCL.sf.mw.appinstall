/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    
*
*/



#include <SyncMLTransportProperties.h>
#include "iaupdatefwsyncprofile.h"
#include "iaupdatefwdebug.h"
#include "iaupdatefwsyncutil.h"
#include "iaupdateuids.h"

#include "iaupdatefwconst.h" //EAspBearer, KBufSize


// ============================ MEMBER FUNCTIONS ==============================
//

// -----------------------------------------------------------------------------
// NewLC
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateFWSyncProfile* CIAUpdateFWSyncProfile::NewLC( 
                                               RSyncMLSession* aSyncSession )
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::NewLC()" );
    
    CIAUpdateFWSyncProfile* self = new( ELeave ) CIAUpdateFWSyncProfile( 
                                                                 aSyncSession );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
    }

// -----------------------------------------------------------------------------
// NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
 CIAUpdateFWSyncProfile* CIAUpdateFWSyncProfile::NewL( 
                                              RSyncMLSession* aSyncSession )
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::NewL:" );
    
    CIAUpdateFWSyncProfile* self = new( ELeave ) CIAUpdateFWSyncProfile( 
                                                                 aSyncSession );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CIAUpdateFWSyncProfile::~CIAUpdateFWSyncProfile()
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::~CIAUpdateFWSyncProfile:" );
    
    iConnection.Close();
    iProfile.Close();
    }

// -----------------------------------------------------------------------------
// ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncProfile::ConstructL()
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::ConstructL:" );
    }

// -----------------------------------------------------------------------------
// Constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateFWSyncProfile::CIAUpdateFWSyncProfile( 
                                        RSyncMLSession* aSyncSession )
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::CIAUpdateFWSyncProfile:" );
	
	iSyncSession = aSyncSession;
	
    iConnectionOpen = EFalse;
	}

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::OpenL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::OpenL( TInt aProfileId, TInt aOpenMode )
    {
    FTRACE( FPrint(
        _L("[IAUPDATEFW] CIAUpdateFWSyncProfile::OpenL(), aProfileId = %d, aOpenMode = %d"),
        aProfileId, aOpenMode ) );

    if ( aOpenMode == EOpenRead )
    	{
    	iProfile.OpenL( *iSyncSession, aProfileId, ESmlOpenRead );
    	}
    else
    	{
    	iProfile.OpenL( *iSyncSession, aProfileId, ESmlOpenReadWrite );
    	}
    if ( !iConnectionOpen )
		{
    	OpenConnection();
		}
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::OpenL() completed" );
    }



// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::SaveL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::SaveL()
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::SaveL()" );
    
    iProfile.UpdateL();
    if ( iConnectionOpen )
    	{
	    FLOG( "[IAUPDATEFW] Connection open" );
    	iConnection.UpdateL(); // IPC call
    	}
	else
	{
	    FLOG( "[IAUPDATEFW] Connection is not open" );		
	}
    iProfile.Close();
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::GetName
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::GetName( TDes& aText )
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::GetName()" );
	
    TUtil::StrCopy( aText, iProfile.DisplayName() );
	}

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::ProfileId
// -----------------------------------------------------------------------------
//
 TInt CIAUpdateFWSyncProfile::ProfileId()
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::ProfileId()" );
    TInt retval = iProfile.Identifier();
    FTRACE( FPrint( _L(
	    "[IAUPDATEFW] CIAUpdateFWSyncProfile::ProfileId() completed: Profile id = %d"),
	    retval ) );
    return retval;
	}


// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::BearerType
// -----------------------------------------------------------------------------
//
 TInt CIAUpdateFWSyncProfile::BearerType()
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::BearerType:" );
	
	TInt id = iConnection.Identifier();
	
	if (id == KUidNSmlMediumTypeInternet.iUid)
		{
		return EAspBearerInternet;
		}
	else if (id == KUidNSmlMediumTypeBluetooth.iUid)
		{
		return EAspBearerBlueTooth;
		}
	else
		{
		return EAspBearerInternet;
		}

    }
	

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::AccessPointL
// -----------------------------------------------------------------------------
//
 TInt CIAUpdateFWSyncProfile::AccessPointL()
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::AccessPointL:" );
	
	TInt num = KErrNotFound;
	
	if ( iConnectionOpen )
		{
	    if ( iConnection.Identifier() == KUidNSmlMediumTypeInternet.iUid )
	        {
        	TBuf8<KBufSize32> key;
        	TBuf<KBufSize32> value;

        	GetConnectionPropertyNameL( key, EPropertyIntenetAccessPoint );
        	TUtil::StrCopy( value, iConnection.GetPropertyL( key ) );
        	User::LeaveIfError( TUtil::StrToInt(value, num) );
	        }
		}
	return num;
	}
	
// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::SetAccessPointL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::SetAccessPointL( const TInt aId )
	{
	FTRACE(RDebug::Print(
	         _L("[IAUPDATEFW] CIAUpdateFWSyncProfile::SetAccessPointL aId (%d)"), aId));

	if ( iConnectionOpen )
		{
		
		FLOG("[IAUPDATEFW] CIAUpdateFWSyncProfile::SetAccessPointL Connection is open");
		
	    if ( iConnection.Identifier() == KUidNSmlMediumTypeInternet.iUid )
	        { 
			FLOG("[IAUPDATEFW] CIAUpdateFWSyncProfile::SetAccessPointL identifier found");
	
	       	TBuf<KBufSize32> buf;
        	TBuf8<KBufSize32> key;
        	TBuf8<KBufSize32> value;
        	
        	buf.Num(aId);
        	TUtil::StrCopy(value, buf);  // convert TDes to TDes8
        	
        	GetConnectionPropertyNameL(key, EPropertyIntenetAccessPoint);
        	
        	FTRACE(RDebug::Print(
        	    _L("[IAUPDATEFW] CIAUpdateFWSyncProfile::SetAccessPointL key (%S) value (%S)"),
        	    &key, &value ) );
        	    
        	iConnection.SetPropertyL( key, value );
	        }
	        
		}
		else
		{
		
		FLOG("[IAUPDATEFW] CIAUpdateFWSyncProfile::SetAccessPointL Connection is not open");			
		
		}
	}

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::SASyncState
// -----------------------------------------------------------------------------
//
 TInt CIAUpdateFWSyncProfile::SASyncState()
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::SASyncState()" );
	
	TSmlServerAlertedAction state = iProfile.SanUserInteraction();

    FTRACE( FPrint(
        _L( "[IAUPDATEFW] CIAUpdateFWSyncProfile::SASyncState() completed, state = %d" ),
        state ) );

	if (state == ESmlConfirmSync)
		{
		return ESASyncStateConfirm;
		}
	else if (state == ESmlDisableSync)
		{
		return ESASyncStateDisable;
		}
	else 
		{
		return ESASyncStateEnable;
		}
	}

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::SetSASyncStateL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::SetSASyncStateL(TInt aState)
    {
	if (aState == ESASyncStateConfirm)
		{
		FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::SetSASyncStateL:ESmlConfirmSync" );
		Profile().SetSanUserInteractionL(ESmlConfirmSync);
		}
	else if (aState == ESASyncStateDisable)
		{
		FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::SetSASyncStateL:ESmlDisableSync" );
		Profile().SetSanUserInteractionL(ESmlDisableSync);
		}
	else
		{
		FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::SetSASyncStateL:ESmlEnableSync" );
		Profile().SetSanUserInteractionL(ESmlEnableSync);
		}
    }




 
// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::Profile
// -----------------------------------------------------------------------------
//
 RSyncMLDevManProfile& CIAUpdateFWSyncProfile::Profile()
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::Profile()" );
    
    return iProfile;
    }



    
// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::OpenConnection
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::OpenConnection()
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::OpenConnection()" );
        
   	TInt err = KErrNone;
   	RArray<TSmlTransportId> arr;
   	
    TRAP( err, Profile().ListConnectionsL(arr) );
    if ( err != KErrNone )
    	{
    	return; 
    	}
    if ( arr.Count() == 0 )
    	{
     	arr.Close();  
    	return; // no connection
    	}
        	
    TInt transportId = arr[0];
    arr.Close();
    	
    TRAP( err, iConnection.OpenL( iProfile, transportId ) );
    if ( err == KErrNone )
    	{
    	iConnectionOpen = ETrue;
    	}

    FTRACE( FPrint( _L(
        "[IAUPDATEFW] CIAUpdateFWSyncProfile::OpenConnection() completed, err = %d" ),
        err ) );
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::Session
// -----------------------------------------------------------------------------
//
 RSyncMLSession& CIAUpdateFWSyncProfile::Session()
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::Session:" );
	
	return *iSyncSession;
	}

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::GetConnectionPropertyNameL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::GetConnectionPropertyNameL( TDes8& aText,
                                                     TInt aPropertyPos )
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::GetConnectionPropertyNameL:" );
	
	// at the moment RSyncMLTransport is only needed
	// for internet connection settings
	RSyncMLTransport transport;
	CleanupClosePushL( transport );
	transport.OpenL( Session(), KUidNSmlMediumTypeInternet.iUid );

	const CSyncMLTransportPropertiesArray&  arr = transport.Properties();

    __ASSERT_DEBUG( arr.Count() > aPropertyPos, TUtil::Panic( KErrGeneral ) );

	const TSyncMLTransportPropertyInfo& info = arr.At( aPropertyPos );
	aText = info.iName;
	CleanupStack::PopAndDestroy( &transport );
	
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::GetConnectionPropertyNameL() completed" );
	}

    
// End of File

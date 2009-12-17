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
CIAUpdateFWSyncProfile* CIAUpdateFWSyncProfile::NewLC( const TInt aApplicationId,
                                               RSyncMLSession* aSyncSession )
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::NewLC()" );
    
    CIAUpdateFWSyncProfile* self = new( ELeave ) CIAUpdateFWSyncProfile( aApplicationId,
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
 CIAUpdateFWSyncProfile* CIAUpdateFWSyncProfile::NewL( const TInt aApplicationId,
                                              RSyncMLSession* aSyncSession )
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::NewL:" );
    
    CIAUpdateFWSyncProfile* self = new( ELeave ) CIAUpdateFWSyncProfile( aApplicationId,
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
    
    iHistoryLog.Close();
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
CIAUpdateFWSyncProfile::CIAUpdateFWSyncProfile( const TInt aApplicationId,
                                        RSyncMLSession* aSyncSession )
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::CIAUpdateFWSyncProfile:" );
	
	iSyncSession = aSyncSession;
	iApplicationId = aApplicationId;
	
    iHistoryLogOpen = EFalse;
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
// CIAUpdateFWSyncProfile::CreateL
// -----------------------------------------------------------------------------
//
 TInt CIAUpdateFWSyncProfile::CreateL()
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::CreateL()" );
    
    iProfile.CreateL( *iSyncSession );
    iProfile.SetCreatorId( iApplicationId );
    iProfile.UpdateL();
    TInt id = iProfile.Identifier();
    iProfile.Close();

    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::CreateL() completed" );
    return id;
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::CreateCopyL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::CreateCopyL( TInt aProfileId )
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::CreateCopyL()" );
    
    iProfile.CreateL( *iSyncSession );
    iProfile.SetCreatorId( iApplicationId );
    iProfile.UpdateL();
    if ( !iConnectionOpen )
		{
    	OpenConnection();
		}
	if ( !iHistoryLogOpen )
		{
	    OpenHistoryLog();
		}

    CIAUpdateFWSyncProfile* profile = 
                CIAUpdateFWSyncProfile::NewL( KIAUpdateUiUid, iSyncSession );
    CleanupStack::PushL( profile );
    profile->OpenL( aProfileId, ESmlOpenRead );
    CopyValuesL( profile );
    CleanupStack::PopAndDestroy( profile );
    profile  = NULL;

    iProfile.UpdateL();
    CloseHistoryLog();
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::CreateCopyL() completed" );
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
// CIAUpdateFWSyncProfile::SetNameL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::SetNameL( const TDesC& aText )
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::SetNameL:" );
	
	iProfile.SetDisplayNameL( aText );
	}

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::CreatorId
// -----------------------------------------------------------------------------
//
 TInt CIAUpdateFWSyncProfile::CreatorId()
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::CreatorId:" );
	
	return iProfile.CreatorId();
	}

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::SetCreatorId
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::SetCreatorId( TInt aCreatorId )
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::SetCreatorId:" );
	
	iProfile.SetCreatorId( aCreatorId );
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
// CIAUpdateFWSyncProfile::DeleteAllowed
// -----------------------------------------------------------------------------
//
 TBool CIAUpdateFWSyncProfile::DeleteAllowed()
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::DeleteAllowed:" );
	
    return iProfile.DeleteAllowed();
	}

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::IsSynced
// -----------------------------------------------------------------------------
//
 TBool CIAUpdateFWSyncProfile::IsSynced()
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::IsSynced()" );
    
    TBool retVal = EFalse;
    if ( !iHistoryLogOpen )
		{
		FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::IsSynced(): Opening history log" );
	    OpenHistoryLog();
		}
		
    if ( iHistoryLogOpen )
    	{
	    FTRACE( FPrint( _L(
	        "[IAUPDATEFW] CIAUpdateFWSyncProfile::IsSynced(): Log open! Log count = %d"),
	                                                 iHistoryLog.Count() ) );
    	if ( iHistoryLog.Count() > 0 )
    	    {
            const CSyncMLHistoryJob* job = LatestHistoryJob();
            if ( job )
            	{
            	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::IsSynced(): True" );
            	if( job->LastSuccessSyncTime() != NULL )
                   {
    	        retVal = ETrue;
                   }
            	}
            else
                {
                FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::IsSynced(): False" );
                }
    	    }
    	}
    else
        {
        FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::IsSynced() Could not open history log!!!" );
        }
    CloseHistoryLog();    	

    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::IsSynced() completed" );
    return retVal;
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::LastSync
// -----------------------------------------------------------------------------
//
 TTime CIAUpdateFWSyncProfile::LastSync()
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::LastSync()" );

    if ( !iHistoryLogOpen )
		{
		FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::LastSync(): Opening history log" );
	    OpenHistoryLog();
		}
    
    TTime time = 0;
    if ( iHistoryLogOpen )
    	{
	    FTRACE( FPrint( _L(
	        "[IAUPDATEFW] CIAUpdateFWSyncProfile::LastSync(): Log open! Log count = %d"),
	                                                 iHistoryLog.Count() ) );
    	
    	if (iHistoryLog.Count() > 0)
    	    {
   	        const CSyncMLHistoryJob* job = LatestHistoryJob();
            if ( job )
            	{
                FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::LastSync() timestamp" );
            	time = job->TimeStamp();
            	}
    	    }
    	}
    else
        {
        FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::LastSync() Could not open history log!!!" );
        }

    CloseHistoryLog();    	

    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::LastSync() completed" );
    return time;
    }

 TTime CIAUpdateFWSyncProfile::LastSuccessSync()
{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::LastSuccessSynctime()" );

    if ( !iHistoryLogOpen )
		{
		FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::LastSync(): Opening history log" );
	    OpenHistoryLog();
		}
    
    TTime time = 0;
    if ( iHistoryLogOpen )
    	{
	    FTRACE( FPrint( _L(
	        "[IAUPDATEFW] CIAUpdateFWSyncProfile::LastSync(): Log open! Log count = %d"),
	                                                 iHistoryLog.Count() ) );
    	
    	if (iHistoryLog.Count() > 0)
    	    {
   	        const CSyncMLHistoryJob* job = LatestHistoryJob();
   	        iProfileId=ProfileId();
            if ( job )
            	{
                FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::LastSync() timestamp" );
            	time = job->LastSuccessSyncTime();
            	}
    	    }
    	}
    else
        {
        FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::LastSync() Could not open history log!!!" );
        }

    CloseHistoryLog();    	

    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::LastSuccessSynctime() completed" );
    return time;
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::GetServerIdL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::GetServerId( TDes& aText )
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::GetServerIdL:" );
    
	TUtil::StrCopy(aText, iProfile.ServerId());
	}

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::SetServerIdL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::SetServerIdL( const TDesC& aText )
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::SetServerIdL:" );
	
	TBuf8<KBufSize256> buf;
	TUtil::StrCopy(buf, aText);
	iProfile.SetServerIdL(buf);
	}

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::GetServerPassword
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::GetServerPassword(TDes& aText)
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::GetServerPassword:" );
	
	TUtil::StrCopy( aText, iProfile.ServerPassword() );
	}
	
// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::SetServerPasswordL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::SetServerPasswordL(const TDesC& aText)
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::SetServerPasswordL:" );
	
	TUtil::StrCopy(iBuf8, aText);    // conver TDes to TDes8
	iProfile.SetServerPasswordL(iBuf8);
	}

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::ProtocolVersion
// -----------------------------------------------------------------------------
//
 TInt CIAUpdateFWSyncProfile::ProtocolVersion()
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::ProtocolVersion:" );
	
	TSmlProtocolVersion version = iProfile.ProtocolVersion();
	return version;
	}
	
// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::SetProtocolVersionL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::SetProtocolVersionL(TInt aProtocolVersion)
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::SetProtocolVersionL:" );
	
	iProfile.SetProtocolVersionL((TSmlProtocolVersion) aProtocolVersion);
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
// CIAUpdateFWSyncProfile::SetBearerTypeL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::SetBearerTypeL(TInt aId)
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::SetBearerTypeL:" );

	if ( aId == EAspBearerInternet )
		{
		iConnection.CreateL( Profile(), KUidNSmlMediumTypeInternet.iUid );
		}
	else if ( aId == EAspBearerBlueTooth )
		{
		iConnection.CreateL( Profile(), KUidNSmlMediumTypeBluetooth.iUid );
    	}
	else
        {
        // Set default as Internet
        iConnection.CreateL( Profile(), KUidNSmlMediumTypeInternet.iUid );
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
// CIAUpdateFWSyncProfile::GetHostAddressL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::GetHostAddress(TDes& aText, TInt& aPort)
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::GetHostAddress:" );
	aText = KNullDesC;
	aPort = KDefaultHttpPort;
	
    if (!iConnectionOpen)	
    	{
      	return;
    	}
	
	if (BearerType() != EAspBearerInternet)
		{
		TUtil::StrCopy(aText, iConnection.ServerURI()); // convert TDes8 to TDes
		return;  // port number handling is for internet bearer only 
		}
	
	TUtil::StrCopy( iBuf, iConnection.ServerURI() ); // convert TDes8 to TDes
	
	TURIParser parser( iBuf );
	parser.GetUriWithoutPort( aText );
	aPort = parser.Port();
	if (aPort == KErrNotFound)
		{
		aPort = parser.DefaultPort();
		}
	}
	
// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::SetHostAddressL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::SetHostAddressL( const TDesC& aText, const TInt aPort )
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::SetHostAddressL:" );
	
    if (!iConnectionOpen)	
    	{
    	return;
    	}
	if (BearerType() != EAspBearerInternet)
		{
		// port number handling is for internet bearer only 
		TUtil::StrCopy(iBuf, aText); 
		}
	else
		{
		TURIParser parser(aText);
	    parser.GetUri(iBuf, aPort);
		}
	
	TUtil::StrCopy( iBuf8, iBuf );
	iConnection.SetServerURIL( iBuf8 );
	}

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::GetUserNameL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::GetUserName(TDes& aText)
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::GetUserNameL:" );
	
	TUtil::StrCopy( aText, iProfile.UserName() );
	}
	
// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::SetUserNameL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::SetUserNameL(const TDesC& aText)
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::SetUserNameL:" );
	
	TUtil::StrCopy(iBuf8, aText);  // conver TDes to TDes8
	iProfile.SetUserNameL(iBuf8);
	}

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::GetPasswordL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::GetPassword(TDes& aText)
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::GetPasswordL:" );
	
	TUtil::StrCopy( aText, iProfile.Password() );
	}
	
// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::SetPasswordL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::SetPasswordL(const TDesC& aText)
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::SetPasswordL:" );
	TUtil::StrCopy( iBuf8, aText );    // conver TDes to TDes8
	
	FTRACE( RDebug::Print(
	     _L("[IAUPDATEFW] CIAUpdateFWSyncProfile::SetPasswordL iBuf8 (%S)"), &iBuf8 ) );
	iProfile.SetPasswordL( iBuf8 );
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
// CIAUpdateFWSyncProfile::SetHttpUsed
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::SetHttpUsedL(TBool aEnable)
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::SetHttpUsed:" );

    if ( iConnectionOpen )
    	{
    	if ( iConnection.Identifier() == KUidNSmlMediumTypeInternet.iUid )
		    {
        	TBuf8<KBufSize32> key;
        	TBuf8<KBufSize32> value;
        	
        	if ( aEnable )
        		{
        		value.Num( KHttpUsed ); 
        		}
        	else
        		{
        		value.Num( KHttpNotUsed );
        		}
        		
        	GetConnectionPropertyNameL( key, EPropertyHttpUsed );
        	iConnection.SetPropertyL( key, value );
		    }
    	}
	}

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::HttpUsedL
// -----------------------------------------------------------------------------
//
 TBool CIAUpdateFWSyncProfile::HttpUsedL()
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::HttpUsedL:" );
    TBool retVal = EFalse; 
    if ( iConnectionOpen )
    	{
    	if ( iConnection.Identifier() == KUidNSmlMediumTypeInternet.iUid )
		    {
        	TBuf8<KBufSize32> key; 
        	TBuf<KBufSize32> value;
        	TInt intValue;
        	
        	GetConnectionPropertyNameL(key, EPropertyHttpUsed);
        	TUtil::StrCopy(value, iConnection.GetPropertyL(key));
        	
            User::LeaveIfError( TUtil::StrToInt( value, intValue ) );
            
            if ( intValue == 1 )
            	{
                retVal = ETrue;
                }
		    }
    	}
    return retVal;
	}
	
// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::GetHttpUsernameL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::GetHttpUserNameL( TDes& aText )
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::GetHttpUsernameL:" );

	aText = KNullDesC;
	
    if ( iConnectionOpen )
    	{
    	if ( iConnection.Identifier() == KUidNSmlMediumTypeInternet.iUid )
		    {
        	TBuf8<KBufSize32> key; 
        	
        	GetConnectionPropertyNameL( key, EPropertyHttpUserName );
        	TUtil::StrCopy( aText, iConnection.GetPropertyL( key ) );
		    }
    	}
  	}
	
// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::SetHttpUsernameL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::SetHttpUserNameL( const TDesC& aText )
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::SetHttpUsernameL:" );

    if ( iConnectionOpen )
    	{
    	if ( iConnection.Identifier() == KUidNSmlMediumTypeInternet.iUid )
		    {
		    TBuf8<KBufSize32> key;
	
	        GetConnectionPropertyNameL( key, EPropertyHttpUserName );
	        TUtil::StrCopy( iBuf8, aText );  // convert TDes to TDes8
	        iConnection.SetPropertyL( key, iBuf8 );
		    }
    	}
	}

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::GetHttpPasswordL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::GetHttpPasswordL( TDes& aText )
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::GetHttpPasswordL:" );
   	aText = KNullDesC;
   	
    if ( iConnectionOpen )
    	{
    	if ( iConnection.Identifier() == KUidNSmlMediumTypeInternet.iUid )
		    {
		    TBuf8<KBufSize32> key; 
	        GetConnectionPropertyNameL( key, EPropertyHttpPassword );
            TUtil::StrCopy( aText, iConnection.GetPropertyL( key ) );
		    }
    	}
	}
	
// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::SetHttpPasswordL
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::SetHttpPasswordL( const TDesC& aText )
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::SetHttpPasswordL:" );

    if ( iConnectionOpen )
    	{
    	if ( iConnection.Identifier() == KUidNSmlMediumTypeInternet.iUid )
		    {
        	TBuf8<KBufSize32> key;
        	
        	GetConnectionPropertyNameL( key, EPropertyHttpPassword );
        	TUtil::StrCopy( iBuf8, aText );  // convert TDes to TDes8
        	iConnection.SetPropertyL( key, iBuf8 );
    	    }
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
// CIAUpdateFWSyncProfile::LatestHistoryJob
// -----------------------------------------------------------------------------
//
 const CSyncMLHistoryJob* CIAUpdateFWSyncProfile::LatestHistoryJob()
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::LatestHistoryJob:" );
    
    if ( !iHistoryLogOpen )
		{
        OpenHistoryLog();
		}
		
    if ( iHistoryLogOpen )
		{
	    TInt count = iHistoryLog.Count();
        FTRACE( FPrint( _L(
            "[IAUPDATEFW] CIAUpdateFWSyncProfile::LatestHistoryJob(): Log count %d"),
            count ) );

        if ( count == 0 )
            {
            FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::LatestHistoryJob(): No history" );
            return NULL;  // profile has no history job
            }
            
        // sort array
        iHistoryLog.SortEntries( CSyncMLHistoryEntry::ESortByTime );    
        
        // try to find latest sync job (start from last array entry)
        for ( TInt index = count - 1; index >= 0; index-- )
        	{
        	FTRACE( FPrint( _L(
        	    "[IAUPDATEFW] CIAUpdateFWSyncProfile::LatestHistoryJob: count %d index %d"),
        	    count, index ) );
            //const CSyncMLHistoryEntry& entry = iHistoryLog[index];
            const CSyncMLHistoryEntry& entry = iHistoryLog.Entry( index );
            
        	FTRACE( FPrint( _L(
        	    "[IAUPDATEFW] CIAUpdateFWSyncProfile::LatestHistoryJob: Entry type = %x"),
        	    entry.EntryType().iUid ) );
            
            const CSyncMLHistoryJob* jobEntry =
                CSyncMLHistoryJob::DynamicCast( &entry );

            if ( jobEntry )
            	{
            	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::LatestHistoryJob() completed: OK" );
            	return jobEntry;
            	}
        	}
		}

    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::LatestHistoryJob() completed: No history found" );
    return NULL;  // profile has no history job
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::OpenHistoryLog
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::OpenHistoryLog()
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::OpenHistoryLog()" );
    
    TInt id = ProfileId();
   	TRAPD( err, iHistoryLog.OpenL( Session(), id ) );

   	if ( err == KErrNone )
   		{
   		FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::OpenHistoryLog(): Opened" );
   		iHistoryLogOpen = ETrue;
    	}

    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::OpenHistoryLog() completed" );
    }
    
// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::CloseHistoryLog
// -----------------------------------------------------------------------------
//
 void CIAUpdateFWSyncProfile::CloseHistoryLog()
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::CloseHistoryLog()" );
    
   	if ( iHistoryLogOpen )
   		{
   		FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::CloseHistoryLog(): Closed " );
   		iHistoryLog.Close();
   		iHistoryLogOpen = EFalse;
    	}

    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::CloseHistoryLog() completed" );
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

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncProfile::CopyValuesL
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncProfile::CopyValuesL( CIAUpdateFWSyncProfile* aSource )
    {
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::CopyValuesL()" );

    TBuf<KBufSize256> buf;
    TInt num = 0;
    
    num = aSource->ProtocolVersion();
    SetProtocolVersionL( num );
    if ( num == 1 )
    	{
    	aSource->GetServerId( buf );
    	SetServerIdL( buf );
    	}
    	
    num = aSource->BearerType();
    SetBearerTypeL( num );
    
     num = aSource->AccessPointL();
    SetAccessPointL( num );
    
    aSource->GetHostAddress( buf, num );
    SetHostAddressL( buf, num );
     
    aSource->GetUserName( buf );
    SetUserNameL(buf);
     
    aSource->GetPassword( buf );
    SetPasswordL( buf );
    
    num = aSource->SASyncState();
    SetSASyncStateL( num );
    
    if ( aSource->BearerType() == EAspBearerInternet)
    	{
    	num = aSource->HttpUsedL();
    	SetHttpUsedL(num);
    	
    	aSource->GetHttpUserNameL(buf);
    	SetHttpUserNameL(buf);
    	
    	aSource->GetHttpPasswordL(buf);
    	SetHttpPasswordL(buf);
    	}
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncProfile::CopyValuesL() completed" );
    }
    
// End of File

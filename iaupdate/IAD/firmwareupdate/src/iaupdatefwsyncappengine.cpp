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



#include "iaupdatefwsyncappengine.h"
#include "iaupdatefwdebug.h"
#include "iaupdatefwsynchandler.h"
#include "iaupdatefwfotamodel.h"
#include "iaupdateuids.h"
#include <collate.h>

// ============================ MEMBER FUNCTIONS ==============================
//

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncAppEngine::CIAUpdateFWSyncAppEngine
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIAUpdateFWSyncAppEngine::CIAUpdateFWSyncAppEngine() 
	{
//	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncAppEngine::CIAUpdateFWSyncAppEngine:" );
	
	}

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncAppEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncAppEngine::ConstructL( CIAUpdateFWFotaModel* aFotaModel )
    {
    
    iSyncMLSession.OpenL();
    iSyncHandler = CIAUpdateFWSyncHandler::NewL( &iSyncMLSession,
                                             this,
                                             aFotaModel );
                                             

    }

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncAppEngine::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateFWSyncAppEngine* CIAUpdateFWSyncAppEngine::NewL(
    CIAUpdateFWFotaModel* aFotaModel )
	{
	
	CIAUpdateFWSyncAppEngine* self = new( ELeave ) CIAUpdateFWSyncAppEngine();
    CleanupStack::PushL( self );
    self->ConstructL( aFotaModel );
    CleanupStack::Pop();
    


    return self;
	}

// ------------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------------
//
CIAUpdateFWSyncAppEngine::~CIAUpdateFWSyncAppEngine()
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncAppEngine::~CIAUpdateFWSyncAppEngine:" );
    
    delete iSyncHandler;
    delete iProfile;        
    iSyncMLSession.Close();
	}

// ------------------------------------------------------------------------------
// CIAUpdateFWSyncAppEngine::DeleteProfileL
// ------------------------------------------------------------------------------
//
void CIAUpdateFWSyncAppEngine::DeleteProfileL( TInt aProfileId )
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncAppEngine::DeleteProfileL:" );
    
    delete iProfile;
    iProfile = NULL;
    iSyncMLSession.DeleteProfileL( aProfileId );
	}

// ------------------------------------------------------------------------------
// CIAUpdateFWSyncAppEngine::CreateProfileL
// ------------------------------------------------------------------------------
//
TInt CIAUpdateFWSyncAppEngine::CreateProfileL()
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncAppEngine::CreateProfileL:" );
    
    delete iProfile;
    iProfile = NULL;
    iProfile = CIAUpdateFWSyncProfile::NewL( KIAUpdateUiUid, &iSyncMLSession );
    return iProfile->CreateL();
	}

// ------------------------------------------------------------------------------
// CIAUpdateFWSyncAppEngine::Profile
// ------------------------------------------------------------------------------
//
CIAUpdateFWSyncProfile* CIAUpdateFWSyncAppEngine::Profile( )
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncAppEngine::Profile:" );
    
    return iProfile;
	}
	
// ------------------------------------------------------------------------------
// CIAUpdateFWSyncAppEngine::CreateCopyProfileL
// ------------------------------------------------------------------------------
//
CIAUpdateFWSyncProfile* CIAUpdateFWSyncAppEngine::CreateCopyProfileL( TInt aProfileId )
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncAppEngine::CreateCopyProfileL:" );
    
    delete iProfile;
    iProfile = NULL;
    iProfile = CIAUpdateFWSyncProfile::NewL( KIAUpdateUiUid, &iSyncMLSession );
    
    iProfile->CreateCopyL( aProfileId );
    return iProfile;
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncAppEngine::Session
// -----------------------------------------------------------------------------
//
RSyncMLSession* CIAUpdateFWSyncAppEngine::Session()
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncAppEngine::Session:" );
	
	return &iSyncMLSession;
	}

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncAppEngine::OpenL
// -----------------------------------------------------------------------------
//
CIAUpdateFWSyncProfile* CIAUpdateFWSyncAppEngine::OpenProfileL( TInt aProfileId,
                                                        TInt aOpenMode )
    {
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncAppEngine::OpenL:" );
	
    delete iProfile;
    iProfile = NULL;
    iProfile = CIAUpdateFWSyncProfile::NewL( KIAUpdateUiUid, &iSyncMLSession );
   	iProfile->OpenL( aProfileId, aOpenMode );
   	return iProfile;
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncAppEngine::Close
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncAppEngine::CloseProfile()
    {
    FLOG(  "[IAUPDATEFW] CIAUpdateFWSyncAppEngine::Close:" );

    delete iProfile;
    iProfile = NULL;
    }
    
// -----------------------------------------------------------------------------
// CIAUpdateFWSyncAppEngine::SynchronizeL
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncAppEngine::SynchronizeL( TDesC& aServerName,
                                         TInt aProfileId,
                                         TInt aConnectionBearer,
                                         const TBool aUseFotaProgressNote)
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncAppEngine::SynchronizeL:" );
    	
    iSyncHandler->SynchronizeL( aServerName,
                                aProfileId,
                                aConnectionBearer,
                                aUseFotaProgressNote);
    }
    
// -----------------------------------------------------------------------------
// CIAUpdateFWSyncAppEngine::SynchronizeL
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncAppEngine::SynchronizeL( TDesC& aServerName,
                                         TInt aProfileId,
                                         TInt aJobId,
                                         TInt aConnectionBearer,
                                         const TBool aUseFotaProgressNote )
    {
   	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncAppEngine::SynchronizeL:" );

    iSyncHandler->SynchronizeL( aServerName, 
                                aProfileId, 
                                aJobId, 
                                aConnectionBearer,
                                aUseFotaProgressNote );
    }

    
// -----------------------------------------------------------------------------
// CIAUpdateFWSyncAppEngine::ServerIdFoundL
// -----------------------------------------------------------------------------
//    
TBool CIAUpdateFWSyncAppEngine::ServerIdFoundL( const TDesC& aServerId,
                                            const TInt aProfileId )
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncAppEngine::ServerIdFoundL:" );
    
	RArray<TSmlProfileId> arr;
    iSyncMLSession.ListProfilesL( arr, ESmlDevMan );
    TBool ret = EFalse;
   	CleanupClosePushL(arr);
   	
	for ( TInt index = 0; index < arr.Count(); index++ )
		{
	    TRAPD( error, OpenProfileL( arr[index], ESmlOpenRead ) );
	    if ( error == KErrNone )
	        {
	        if ( aProfileId != Profile()->ProfileId() )
	            {	        
    	        TBuf<KNSmlMaxItemLength> buf;
    	        Profile()->GetServerId( buf );
    	        if ( buf.Compare( aServerId ) == 0 )
    	            {
    	            FLOG( "[IAUPDATEFW] CIAUpdateFWSyncAppEngine::ServerIdFoundL: Match" );
                    ret = ETrue;
                    CloseProfile();
                    break;
                    }
	            }
		    CloseProfile();
	        }
		}
    CleanupStack::PopAndDestroy( &arr );
    return ret;
    }

// -----------------------------------------------------------------------------
//  CIAUpdateFWSyncAppEngine::Compare
// -----------------------------------------------------------------------------
//
TInt CIAUpdateFWSyncAppEngine::Compare( const TDesC& aLeft, const TDesC& aRight )
	{
	FLOG( "[IAUPDATEFW] CIAUpdateFWSyncAppEngine::Compare:" );
	
	// Empty Name() is always greater than non-empty Name()
    if (aLeft.Length() == 0 && aRight.Length() == 0)
        {
        return 0;
        }
    if (aLeft.Length() == 0)
        {
        return 1;
        }
    if (aRight.Length() == 0)
        {
        return -1;
        }

    // None of the Name()s was empty, use TDesC::CompareC to do the comparison.
    // This is from cntmodel.
    TCollationMethod collateMethod;
    // get the standard method
	collateMethod = *Mem::CollationMethodByIndex(0);
    // dont ignore punctuation and spaces
	collateMethod.iFlags |= TCollationMethod::EIgnoreNone; 
	TInt comparison( aLeft.CompareC(aRight, 3, &collateMethod) );
	return comparison;	
	}
	
// -----------------------------------------------------------------------------
//  CIAUpdateFWSyncAppEngine::SyncRunning
// -----------------------------------------------------------------------------
//
TBool CIAUpdateFWSyncAppEngine::SyncRunning()
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncAppEngine::SyncRunning:" );
    
    return iSyncHandler->SyncRunning();
    }

// -----------------------------------------------------------------------------
//  CIAUpdateFWSyncAppEngine::CancelSyncL
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncAppEngine::CancelSyncL()
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncAppEngine::CancelSyncL()" );
    iSyncHandler->CancelSynchronizeL();
    }

// -----------------------------------------------------------------------------
//  CIAUpdateFWSyncAppEngine::SyncCompleted
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncAppEngine::SyncCompleted( TNSmlStatus /*aStatus*/ )
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncAppEngine::SyncCompleted:" );
    
    /*if( iSyncObserver )
        {
        FLOG( "[IAUPDATEFW] CIAUpdateFWSyncAppEngine::SyncCompleted: inform observer" );
        
        iSyncObserver->SyncComplete( aStatus );
        }    */
    }
    
// -----------------------------------------------------------------------------
// CIAUpdateFWSyncAppEngine::RequestSyncStatus
// -----------------------------------------------------------------------------
//	
/*void CIAUpdateFWSyncAppEngine::RequestSyncStatus( 
                                           MNSmlDMSyncObserver* aSyncObserver )
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncAppEngine::RequestSyncStatus:" );
    
    if( aSyncObserver )
        {
        FLOG( "[IAUPDATEFW] CIAUpdateFWSyncAppEngine::RequestSyncStatus: added observer" );
        iSyncObserver = aSyncObserver;
        }
    }*/

// End of File

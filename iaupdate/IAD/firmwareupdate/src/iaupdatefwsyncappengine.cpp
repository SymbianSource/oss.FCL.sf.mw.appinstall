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
// CIAUpdateFWSyncAppEngine::SetObserver
// ------------------------------------------------------------------------------
//
void CIAUpdateFWSyncAppEngine::SetObserver( MIAUpdateFWUpdateObserver* aObserver )
    {
    if ( iSyncHandler )
        {
        iSyncHandler->SetUpdateObserver( aObserver );
        }
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
    iProfile = CIAUpdateFWSyncProfile::NewL( &iSyncMLSession );
   	iProfile->OpenL( aProfileId, aOpenMode );
   	return iProfile;
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncAppEngine::CloseProfile
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncAppEngine::CloseProfile()
    {
    FLOG(  "[IAUPDATEFW] CIAUpdateFWSyncAppEngine::CloseProfile:" );

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
//  CIAUpdateFWSyncAppEngine::SyncCompleted
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncAppEngine::SyncCompleted( TNSmlStatus /*aStatus*/ )
    {
    FLOG( "[IAUPDATEFW] CIAUpdateFWSyncAppEngine::SyncCompleted:" );
    }
    


// End of File

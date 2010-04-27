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


#include <iaupdate.rsg>
#include <StringLoader.h> 
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <DevManInternalCRKeys.h>
#include <e32property.h>

#include "iaupdatefwupdatehandler.h"
#include "iaupdatefwfotamodel.h"
#include "iaupdatefwsyncprofile.h"
#include "iaupdatefwsyncappengine.h"
#include "iaupdateprivatecrkeys.h"
#include "iaupdatefwdebug.h"
#include "iaupdatefwconst.h" //ESASyncStateDisable
#include "iaupdatefwnsmlpskeys.h"
#include "iaupdatefwnsmlcrkeys.h"

// -----------------------------------------------------------------------------
//  CIAUpdateFWUpdateHandler::NewLC()
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdateFWUpdateHandler* CIAUpdateFWUpdateHandler::NewLC()
    {
    CIAUpdateFWUpdateHandler* self = new(ELeave)CIAUpdateFWUpdateHandler();
    CleanupStack::PushL( self);
    self->ConstructL();   
    return self;
    }


// -----------------------------------------------------------------------------
//  CIAUpdateFWUpdateHandler::NewL()
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdateFWUpdateHandler* CIAUpdateFWUpdateHandler::NewL()
    {
    CIAUpdateFWUpdateHandler* self = CIAUpdateFWUpdateHandler::NewLC();
    CleanupStack::Pop(self);
    return self;
    }
    
    
// -----------------------------------------------------------------------------
//  CIAUpdateFWUpdateHandler::~CIAUpdateFWUpdateHandler()
// -----------------------------------------------------------------------------
//
CIAUpdateFWUpdateHandler::~CIAUpdateFWUpdateHandler()
    {
    TInt err = RProperty::Delete(KPSUidNSmlDMSyncApp,KNSmlCurrentFotaUpdateAppName);
    delete iFotaModel;
    delete iDMEngine;
    }


// -----------------------------------------------------------------------------
//  CIAUpdateFWUpdateHandler::FirmWareUpdatewithNSU()
// -----------------------------------------------------------------------------
//      
EXPORT_C void CIAUpdateFWUpdateHandler::FirmWareUpdatewithNSU()
    {  
    iRequest = ENSU;
    iStatus = KRequestPending;
    TRequestStatus* status = &iStatus;

    SetActive();
    User::RequestComplete( status, KErrNone );
    }

// -----------------------------------------------------------------------------
//  CIAUpdateFWUpdateHandler::IsDMSupportAvailableL()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIAUpdateFWUpdateHandler::IsDMSupportAvailableL()
    {
    return iFotaModel->IsDMSupportAvailableL();
    }

// -----------------------------------------------------------------------------
//  CIAUpdateFWUpdateHandler::FirmWareUpdatewithFOTA()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdateFWUpdateHandler::FirmWareUpdatewithFOTA()
    {
    iRequest = EFOTA;
    iStatus = KRequestPending;
    TRequestStatus* status = &iStatus;

    SetActive();
    User::RequestComplete( status, KErrNone );
    }
    

// -----------------------------------------------------------------------------
//  CIAUpdateFWUpdateHandler::CIAUpdateFWUpdateHandler()
// -----------------------------------------------------------------------------
//
CIAUpdateFWUpdateHandler::CIAUpdateFWUpdateHandler():CActive( EPriorityNormal )
    {
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
//  CIAUpdateFWUpdateHandler::ConstructL()
// -----------------------------------------------------------------------------
//
void CIAUpdateFWUpdateHandler::ConstructL()
    {
    iFotaModel = CIAUpdateFWFotaModel::NewL();    
    iDMEngine = CIAUpdateFWSyncAppEngine::NewL( iFotaModel );
    }


// -----------------------------------------------------------------------------
//  CIAUpdateFWUpdateHandler::RunL()
// -----------------------------------------------------------------------------
//
void CIAUpdateFWUpdateHandler::RunL()
    {
    switch ( iRequest )
        {
        case ENSU:
            {          
            HBufC* text1 = StringLoader::LoadLC( R_IAUPDATE_INFO_NSU_1 );  
            HBufC* text2 = StringLoader::LoadLC( R_IAUPDATE_INFO_NSU_2 );
            
            HBufC* text = HBufC::NewLC( text1->Length() +
                                        text2->Length() );
                
            text->Des() += *text1;
            text->Des() += *text2;

            HBufC* heading = StringLoader::LoadLC( R_IAUPDATE_NEW_SW_TITLE );
     
            ShowDialogL( *text, *heading );
            
            CleanupStack::PopAndDestroy( heading ); //text, heading
            CleanupStack::PopAndDestroy( text );
            CleanupStack::PopAndDestroy( text2 );
            CleanupStack::PopAndDestroy( text1 );
          
            break;
            }
        case EFOTA:
            {
            //check whether FOTA engine is available at the moment
            TBool isDmActive = EFalse;

            TInt fotamodelstate = iFotaModel->GetCurrentFwUpdState();
            
            FLOG_NUM( "[IAUPDATEFW] fota status = %d", fotamodelstate );   
            
            if ( fotamodelstate != RFotaEngineSession::EIdle )
                { 
                //if status is downloadcomplete or startingupdate
                //still allow user to continue
                if ( ( fotamodelstate != RFotaEngineSession::EDownloadComplete ) && ( fotamodelstate != RFotaEngineSession::EStartingUpdate ) )
                    {
		                //if download is suspended, try to resume it.
		                if ( fotamodelstate == RFotaEngineSession::EDownloadProgressingWithResume)
		                    {
		                    TInt result = iFotaModel->TryResumeFwUpdDownload();
						            FLOG_NUM( "[IAUPDATEFW] TryResumeFwUpdDownload result  = %d", result );   
		                    }
                    //anyway, DM is in use
                    isDmActive = ETrue;
                    }
                }
                     
            if ( isDmActive )
                {
                FLOG( "[IAUPDATEFW] CIAUpdateFWUpdateHandler::RunL DM is activated" );   
                return;
                }
            
            TInt profileId( KErrNotFound );
                 
            profileId = iFotaModel->DefaultFotaProfileIdL();
            
            if (KErrNotFound != profileId)          
                {               
                // Define the key KNSmlCurrentFotaUpdateAppName
                TInt err = RProperty::Define( KPSUidNSmlDMSyncApp,
                                              KNSmlCurrentFotaUpdateAppName,
                                              RProperty::EInt);
                
                RProperty::Set(KPSUidNSmlDMSyncApp,KNSmlCurrentFotaUpdateAppName,1 );
                
                iFotaModel->EnableFwUpdRequestL( profileId );
                StartSyncL( profileId, EFalse );
                }
            else
                {               
                FLOG( "[IAUPDATEFW] CIAUpdateFWUpdateHandler::RunL no DM profile available!" );                      
                }            
            }  
            break;
            }
        }


// -----------------------------------------------------------------------------
//  CIAUpdateFWUpdateHandler::DoCancel()
// -----------------------------------------------------------------------------
//
void CIAUpdateFWUpdateHandler::DoCancel()
    {
    }


// -----------------------------------------------------------------------------
//  CIAUpdateFWUpdateHandler::ShowDialogL()
// -----------------------------------------------------------------------------
//
void CIAUpdateFWUpdateHandler::ShowDialogL(TDesC& aText, TDesC& aHeading )
    {  
    CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL( aText );

    dlg->PrepareLC( R_IAUPDATE_MESSAGE_QUERY );

    CAknPopupHeadingPane* headingPane = dlg->Heading();
    headingPane->SetTextL( aHeading );
	    
    TInt ret = dlg->RunLD();
    }
        

// -----------------------------------------------------------------------------
//  CIAUpdateFWUpdateHandler::StartSyncL()
// -----------------------------------------------------------------------------
//
void CIAUpdateFWUpdateHandler::StartSyncL(
    const TInt aProfileId, // = KErrNotFound
    const TBool aUseFotaProgressNote ) // = EFalse
    {
    CIAUpdateFWSyncProfile* profile = 
                iDMEngine->OpenProfileL( aProfileId, ESmlOpenReadWrite );

    TInt connectionBearer = profile->BearerType();
    
    TBuf<KMaxProfileNameLength> serverName;
    profile->GetName( serverName );
  
    CRepository* centrep = NULL;
    
    // Next, we check if profile is active. If not, we change it without asking user's permission and then set it back at the end.              
    if ( profile->SASyncState() == ESASyncStateDisable )
        {
        TRAPD( err, centrep = CRepository::NewL(  KCRUidDeviceManagementInternalKeys ) );
        if (err == KErrNone)
            {
            centrep->Set(  KNSmlFotaProfileDefaultIsActive, 0 );                
            delete centrep; centrep = NULL;         
            }
    
        profile->SetSASyncStateL( ESASyncStateEnable );
        profile->SaveL(); 
        }
             
    iDMEngine->CloseProfile();      

    TRAPD( error, iDMEngine->SynchronizeL( serverName, 
                                           aProfileId, 
                                           connectionBearer,
                                           aUseFotaProgressNote ) );

    if (error != KErrNone)
        {

        CAknInformationNote* queryDialog = new (ELeave) CAknInformationNote;
        
        HBufC* error = HBufC::NewL(20);
                  TPtr ptrerror = error->Des();
                  ptrerror.Copy(_L("sync problem")); 
        queryDialog->ExecuteLD( *error  );
        }
    }

//EOF


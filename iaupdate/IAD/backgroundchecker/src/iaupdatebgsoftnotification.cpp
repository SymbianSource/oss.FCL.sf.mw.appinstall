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



// INCLUDE FILES
#include <AknDynamicSoftNotifier.h>
#include <AknDynamicSoftNotificationParams.h>

#include "iaupdatebgsoftnotification.h"
#include "iaupdatebginternalfilehandler.h"
#include "iaupdatebgconst.h"
#include "iaupdatebglogger.h"


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CIAUpdateBGSoftNotification::NewLC
// Static constructor
// -----------------------------------------------------------------------------
//
CIAUpdateBGSoftNotification* CIAUpdateBGSoftNotification::NewL( MIAUpdateBGSoftNotificationCallBack* aCallback, 
                                                                CIAUpdateBGInternalFileHandler* aInternalFile )
    {   
    CIAUpdateBGSoftNotification* self =
        new ( ELeave ) CIAUpdateBGSoftNotification( aCallback, aInternalFile );
    CleanupStack::PushL( self );
    
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CIAUpdateBGSoftNotification::~CIAUpdateBGSoftNotification()
    {
    delete iSoftkey1;
    delete iSoftkey2;

    delete iLabel;
    delete iGroupLabel;

    delete iImageData;

    delete iNotifier;
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGSoftNotification::NotificationAccepted
// Dynamic soft notification was accepted by user.
// -----------------------------------------------------------------------------
//
void CIAUpdateBGSoftNotification::NotificationAccepted( TInt /*aIdentifier*/ )
    {
    FLOG("[bgchecker] softnotification callback function ACCEPTED");
    //remove the soft notifiation id
    iInternalFile->SetSoftNotificationID( 0 );
    TRAP_IGNORE( iInternalFile->WriteControllerDataL() );
    
    iCallback->SoftNotificationCallBack( ETrue );
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGSoftNotification::NotificationCanceled
// Dynamic soft notification was canceled by user.
// -----------------------------------------------------------------------------
//
void CIAUpdateBGSoftNotification::NotificationCanceled( TInt /*aIdentifier*/ )
    {
    FLOG("[bgchecker] softnotification callback function Canceled");
    //remove the soft notifiation id
    iInternalFile->SetSoftNotificationID( 0 );
    TRAP_IGNORE( iInternalFile->WriteControllerDataL() );
          
    iCallback->SoftNotificationCallBack( EFalse );
    }
    
// -----------------------------------------------------------------------------
// CIAUpdateBGSoftNotification::ShowSoftNotificationL
// Displays a soft notification
// -----------------------------------------------------------------------------
//
void CIAUpdateBGSoftNotification::ShowSoftNotificationL()
    {
    TAknDynamicSoftNotificationParams param( KSoftNotificationPriority );
    FillNotificationParams( param );
    
    TInt oldId = iNotificationId;
    iNotificationId =
        iNotifier->SetDynamicNotificationCountL( param, iNotificationId, 1 );

    if( oldId != iNotificationId )
        {           
        FLOG("[bgchecker] softnotification save notification Id");
        iInternalFile->SetSoftNotificationID( iNotificationId );
        TRAP_IGNORE( iInternalFile->WriteControllerDataL() );
        
        iNotifier->StopObserving( oldId );
        iNotifier->StartObservingL( iNotificationId, this );
        }
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGSoftNotification::RemoveSoftNotificationL
// Cancels and removes the soft notification
// -----------------------------------------------------------------------------
//
void CIAUpdateBGSoftNotification::RemoveSoftNotificationL( TInt aNotifierId )
    {
    if ( aNotifierId != 0 )
        {
        iNotifier->CancelDynamicNotificationL( aNotifierId );
        }
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGSoftNotification::SetTextL
// Sets a text for a soft notification
// -----------------------------------------------------------------------------
//
void CIAUpdateBGSoftNotification::SetTextL( const TDesC& aText, const TDesC& aGroupText )
    {
    HBufC* txt = aText.AllocL();
    delete iLabel;
    iLabel = txt;

    txt = aGroupText.AllocL();
    delete iGroupLabel;
    iGroupLabel = txt;
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGSoftNotification::SetSoftkeyLabelsL
// Sets new labels for softkeys
// -----------------------------------------------------------------------------
//
void CIAUpdateBGSoftNotification::SetSoftkeyLabelsL(
    const TDesC& aSoftkey1Label,
    const TDesC& aSoftkey2Label )
    {
    HBufC* txt = aSoftkey1Label.AllocL();
    delete iSoftkey1;
    iSoftkey1 = txt;

    txt = aSoftkey2Label.AllocL();
    delete iSoftkey2;
    iSoftkey2 = txt;
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGSoftNotification::SetImageL
// Sets an image for a soft notification
// -----------------------------------------------------------------------------
//
void CIAUpdateBGSoftNotification::SetImageL(
    const TDesC8& aImage )
    {
    HBufC8* image = aImage.AllocL();
    delete iImageData;
    iImageData = image;
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGSoftNotification::Id
// Notification Id
// -----------------------------------------------------------------------------
//
TInt CIAUpdateBGSoftNotification::Id()
    {
    return iNotificationId;
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGSoftNotification::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdateBGSoftNotification::ConstructL()
    {
    FLOG("[bgchecker] softnotification ConstructL");
    iNotifier = CAknDynamicSoftNotifier::NewL();
    
    iInternalFile->ReadControllerDataL();
    iNotificationId = iInternalFile->SoftNotificationID();
      
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGSoftNotification::CIAUpdateBGSoftNotification
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIAUpdateBGSoftNotification::CIAUpdateBGSoftNotification( MIAUpdateBGSoftNotificationCallBack* aCallback, 
                                                          CIAUpdateBGInternalFileHandler* aInternalFile ) 
    : iCallback ( aCallback ), iInternalFile ( aInternalFile )
    {
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGSoftNotification::StartObservingIfNeededL
// -----------------------------------------------------------------------------
//
void CIAUpdateBGSoftNotification::StartObservingIfNeededL()
    {
    if ( iNotificationId )
        {
        FLOG("[bgchecker] softnotification There is a buffered softnotification");
        //a buffering soft notification
        iNotifier->StartObservingL( iNotificationId, this );
        }
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGSoftNotification::FillNotificationParams
// -----------------------------------------------------------------------------
//
void CIAUpdateBGSoftNotification::FillNotificationParams(
    TAknDynamicSoftNotificationParams& aParam )
    {
    if( iSoftkey1 && iSoftkey2 )
        {
        aParam.SetSoftkeys( *iSoftkey1, *iSoftkey2 );
        }

    if( iLabel )
        {
        aParam.SetNoteLabels( *iLabel, *iLabel );
        }

    if( iGroupLabel )
        {
        //aParam.SetGroupLabels( *iGroupLabel, *iGroupLabel );
        }

    if( iImageData )
        {
        aParam.SetImageData( *iImageData );
        }
    
    aParam.EnableObserver();
    }

//  End of File

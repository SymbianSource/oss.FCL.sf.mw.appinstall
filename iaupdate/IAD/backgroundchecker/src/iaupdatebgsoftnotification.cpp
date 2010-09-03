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



// INCLUDE FILES
#include <hbindicatorsymbian.h>
#include <hbsymbianvariant.h>

#include "iaupdatebgsoftnotification.h"
#include "iaupdatebginternalfilehandler.h"
#include "iaupdatebgconst.h"
#include "iaupdatebglogger.h"

// Indicator type
_LIT(KIndicatorTypeBgc, "com.nokia.iaupdate.indicatorplugin/1.0");

// Icon name
_LIT( KIconName, "qtg_large_swupdate" );

// ============================ MEMBER FUNCTIONS ===============================
void CIAUpdateBGSoftNotification::NotificationDialogActivated(
        const CHbDeviceNotificationDialogSymbian* /* aDialog */)
    {
    
    FLOG("[bgchecker] softnotification callback function ACCEPTED");
    
    // indicator shall not be shown 
    SetIndicatorEnabled( EFalse );
    
    // Remove (possibly) existing indicator
    RemoveIndicatorL();
    
    iCallback->SoftNotificationCallBack( ETrue );
    
    // remove dialog
    //delete iNotificationDialog;
    //iNotificationDialog = 0;
    
    return;
    }

void CIAUpdateBGSoftNotification::NotificationDialogClosed
         ( const CHbDeviceNotificationDialogSymbian* /* aDialog */, 
           TInt /* aCompletionCode*/ )
    {
    
    
    FLOG("[bgchecker] softnotification callback function Closed");

    // Set indicator, if needed
    if (IsIndicatorEnabled())
        {
        ShowIndicatorL();
        }
    
    
    iCallback->SoftNotificationCallBack( EFalse );
    
    // remove dialog
    //delete iNotificationDialog;
    //iNotificationDialog = 0;
    
    return;
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGSoftNotification::NewLC
// Static constructor
// -----------------------------------------------------------------------------
//
CIAUpdateBGSoftNotification* CIAUpdateBGSoftNotification::NewL( 
        MIAUpdateBGSoftNotificationCallBack* aCallback ) 
    {   
    CIAUpdateBGSoftNotification* self =
        new ( ELeave ) CIAUpdateBGSoftNotification( aCallback );
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

    delete iTitle;
    delete iText;
    
    delete iNotificationDialog;

    }
   
// -----------------------------------------------------------------------------
// CIAUpdateBGSoftNotification::ShowNotificationL
// Displays notification
// -----------------------------------------------------------------------------
//
void CIAUpdateBGSoftNotification::ShowNotificationL()
    {
    
    FLOG("[bgchecker] ShowNotificationL");
    
    // crete dialog, id does not exist already
    if (!iNotificationDialog)
        {
        iNotificationDialog = CHbDeviceNotificationDialogSymbian::NewL( this );
        }
    
    // enable indicator showing
    SetIndicatorEnabled( ETrue );

    // fill texts, icon & behaviour parameters
    FillNotificationParams();
    
    iNotificationDialog->SetIconNameL( KIconName );
    iNotificationDialog->ShowL();
    
    return;
    
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGSoftNotification::ShowIndicatorL
// Displays indicator
// -----------------------------------------------------------------------------
//
void CIAUpdateBGSoftNotification::ShowIndicatorL()
    {
    
    FLOG("[bgchecker] ShowIndicatorL");
    
    CHbIndicatorSymbian *ind = CHbIndicatorSymbian::NewL();
    CleanupStack::PushL( ind );
            
    TInt value = GetNrOfUpdates();
            
    // Set indicator
    CHbSymbianVariant* varValue = CHbSymbianVariant::NewL( &value,
        CHbSymbianVariant::EInt );
    CleanupStack::PushL( varValue );
    // Temporary removal 
    ind->Activate( KIndicatorTypeBgc, varValue );
    CleanupStack::PopAndDestroy( varValue );
    CleanupStack::PopAndDestroy( ind );
    
    return;
    
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGSoftNotification::Remove indicator
// Removes indicator
// -----------------------------------------------------------------------------
//
void CIAUpdateBGSoftNotification::RemoveIndicatorL()
    {
    
    FLOG("[bgchecker] RemoveIndicatorL");
    
    CHbIndicatorSymbian *ind = CHbIndicatorSymbian::NewL();
    CleanupStack::PushL( ind );
    // Temporary removal 
    ind->Deactivate( KIndicatorTypeBgc ); 
    CleanupStack::PopAndDestroy(ind);
    
    return;
    
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGSoftNotification::SetTextL
// Sets a text for a soft notification
// -----------------------------------------------------------------------------
//
void CIAUpdateBGSoftNotification::SetTextL( 
        const TDesC& aTitle, const TDesC& aText )
    {
    HBufC* txt = aTitle.AllocL();
    delete iTitle;
    iTitle = txt;

    txt = aText.AllocL();
    delete iText;
    iText = txt;
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGSoftNotification::SetNrOfUpdates
// Sets an image path for a soft notification
// -----------------------------------------------------------------------------
//
void CIAUpdateBGSoftNotification::SetNrOfUpdates( const TInt& aNrOfUpdates)
    {
    // save number of updates
    iNrOfUpdates = aNrOfUpdates;
    return;
    }
// -----------------------------------------------------------------------------
// CIAUpdateBGSoftNotification::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdateBGSoftNotification::ConstructL()
    {
    FLOG("[bgchecker] softnotification ConstructL");
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGSoftNotification::CIAUpdateBGSoftNotification
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIAUpdateBGSoftNotification::CIAUpdateBGSoftNotification( 
        MIAUpdateBGSoftNotificationCallBack* aCallback )
    : iCallback ( aCallback )
    {
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGSoftNotification::FillNotificationParams
// -----------------------------------------------------------------------------
//

void CIAUpdateBGSoftNotification::FillNotificationParams()
    {
    
    // set title, text and icon
    if ( iTitle )
        {
        iNotificationDialog->SetTitleL(iTitle->Des());
        }
    
    if ( iText )
        {
        iNotificationDialog->SetTextL(iText->Des());
        }
    
    // set wrapping, timeout and touch 
    iNotificationDialog->SetTitleTextWrapping(
            CHbDeviceNotificationDialogSymbian::TextWordWrap);
    iNotificationDialog->SetTimeout(4000); //default 3000
    iNotificationDialog->EnableTouchActivation(ETrue); // default FALSE
  
    }  
// ----------------------------------------------------------
// CIAUpdateBGSoftNotification::EnableIndicator(TBool aEnabled)
// ----------------------------------------------------------
void CIAUpdateBGSoftNotification::SetIndicatorEnabled( TBool aEnabled )
    {
    
    iActivateIndicator = aEnabled;
    
    }

// ----------------------------------------------------------
// CIAUpdateBGSoftNotification::IndicatorEnabled()
// ----------------------------------------------------------
TBool CIAUpdateBGSoftNotification::IsIndicatorEnabled()
    {
    
    return iActivateIndicator;
    
    }

// ----------------------------------------------------------
// CIAUpdateBGSoftNotification::GetNrOfUpdates()
// ----------------------------------------------------------
int CIAUpdateBGSoftNotification::GetNrOfUpdates()
    {
    
    return iNrOfUpdates;
    
    }

//  End of File

/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of CDialogWrapper
*
*/



#include <AknGlobalNote.h>
#include <avkon.rsg>
#include <bautils.h>
#include <data_caging_path_literals.hrh> 
#include <iaupdater.rsg>
#include <StringLoader.h>
#include <AknUtils.h>   //For AknTextUtils

#include "iaupdaterdialog.h"
#include "iaupdaterdefs.h"
#include "iaupdatercancelobserver.h"
#include "iaupdatedebug.h"


// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


// -----------------------------------------------------------------------------
// CIAUpdaterDialog::CIAUpdaterDialog
// C++ default constructor 
// -----------------------------------------------------------------------------
//
CIAUpdaterDialog::CIAUpdaterDialog( RFs& aFs, 
                                    MIAUpdaterCancelObserver& aObserver )
: CActive( CActive::EPriorityStandard ),
  iFs( aFs ),
  iObserver ( &aObserver ) 
    {
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CIAUpdaterDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdaterDialog* CIAUpdaterDialog::NewL( RFs& aFs, 
                                          MIAUpdaterCancelObserver& aObserver )
    {
    CIAUpdaterDialog* self =
        CIAUpdaterDialog::NewLC( aFs, aObserver );
    CleanupStack::Pop( self );
    return self;    
    }


// -----------------------------------------------------------------------------
// CIAUpdaterDialog::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdaterDialog* CIAUpdaterDialog::NewLC( RFs& aFs, 
                                           MIAUpdaterCancelObserver& aObserver )
    {
    CIAUpdaterDialog* self = 
        new( ELeave ) CIAUpdaterDialog( aFs, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


// -----------------------------------------------------------------------------
// CIAUpdaterDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdaterDialog::ConstructL()
    {
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterDialog::ConstructL begin");
    
    // Get resource file path
    TFileName fileName;
    fileName.Copy( TParsePtrC( RProcess().FileName() ).Drive() );
    fileName.Append( KDC_APP_RESOURCE_DIR );
    fileName.Append( IAUpdaterDefs::KIAUpdaterResourceFile );
    
    // Get language of resource file        
    BaflUtils::NearestLanguageFile( iFs, fileName );

    // Open resource file
    iResourceFile.OpenL( iFs, fileName );
    iResourceFile.ConfirmSignatureL();    

    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterDialog::ConstructL end");
    }

 
// -----------------------------------------------------------------------------
// CIAUpdaterDialog::~CIAUpdaterDialog()
// C++ Destructor
// -----------------------------------------------------------------------------
//
CIAUpdaterDialog::~CIAUpdaterDialog()
    {
    Cancel();
    delete iNote;
    iResourceFile.Close();    
    }


// -----------------------------------------------------------------------------
// CIAUpdaterDialog::ShowWaitingNoteL  
// Show global waiting note during installing.
// -----------------------------------------------------------------------------
// 
void CIAUpdaterDialog::ShowWaitingNoteL( const TDesC& aName, TInt aIndex, TInt aTotalCount )
	{	
	IAUPDATE_TRACE("[IAUpdater] CIAUpdaterDialog::ShowWaitingNoteL begin");
	if ( iNoteId == 0 )
		{ 		                             
	    IAUPDATE_TRACE("[IAUpdater] Creating global waiting note.");
	
        // Get localiced string from resc. file. 
        HBufC* string = ReadResourceLC( R_IAUPDATER_INSTALLING ); 
                                          
        HBufC* temp1 = HBufC::NewLC( string->Length() + aName.Length() );          
        TPtr temp1Ptr = temp1->Des();
        
        // Add pkg's name to string (U0).
        StringLoader::Format( temp1Ptr, *string, 0, aName );
                                        
        // Increase buffer length for the number.        
        HBufC* temp2 = 
            HBufC::NewLC( temp1->Length() + IAUpdaterDefs::KIAUpdaterParamLen );        
        TPtr temp2Ptr = temp2->Des();
         
        // Add index number to string (N1) 
        StringLoader::Format( temp2Ptr, *temp1, 1, aIndex );
                        
        // Increase buffer length for the number.  
        HBufC* finalString = 
            HBufC::NewLC( temp2->Length() + IAUpdaterDefs::KIAUpdaterParamLen );        
        TPtr finalPtr = finalString->Des();
        
        // Add max count number to string (N2) 
        StringLoader::Format( finalPtr, *temp2, 2, aTotalCount );
        
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( finalPtr );         
        if ( !iNote )
            {
        	iNote = CAknGlobalNote::NewL();   
    	    iNote->SetSoftkeys( R_AVKON_SOFTKEYS_CANCEL );
            }
     	
	    IAUPDATE_TRACE("[IAUpdater] Showing global waiting note.");
    	iNoteId = iNote->ShowNoteL( iStatus, EAknGlobalWaitNote, *finalString );
    	SetActive();    	    	
     
    	CleanupStack::PopAndDestroy( finalString );
    	CleanupStack::PopAndDestroy( temp2 );
    	CleanupStack::PopAndDestroy( temp1 );
    	CleanupStack::PopAndDestroy( string );
		}
	IAUPDATE_TRACE("[IAUpdater] CIAUpdaterDialog::ShowWaitingNoteL end");
	}

	
// -----------------------------------------------------------------------------
// CIAUpdaterDialog::CancelWaitingNoteL  
// Cancel global waiting note after installing.
// -----------------------------------------------------------------------------
// 
void CIAUpdaterDialog::CancelWaitingNoteL()
	{
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterDialog::CancelWaitingNoteL begin");
	
	if ( iNoteId != 0 )
		{
        IAUPDATE_TRACE("[IAUpdater] Cancel waiting note.");
	    iNote->CancelNoteL( iNoteId );
	    iNoteId = 0;
		}

    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterDialog::CancelWaitingNoteL end");
	}

	
// -----------------------------------------------------------------------------	
// CIAUpdaterDialog::LoadResourceLC
// Read resource string.
// -----------------------------------------------------------------------------
// 
HBufC* CIAUpdaterDialog::ReadResourceLC( TInt aResourceId )
    {
    TResourceReader reader;
    HBufC8* buff = iResourceFile.AllocReadLC( aResourceId );    
    reader.SetBuffer( buff );
    HBufC* text = reader.ReadHBufCL();
    CleanupStack::PopAndDestroy( buff );
    CleanupStack::PushL( text );

    return text;
    }
    
    
// ---------------------------------------------------------------------------
// CIAUpdaterDialog:::DoCancel
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdaterDialog::DoCancel()
    {
    TRAP_IGNORE( CancelWaitingNoteL() );
    }

// ---------------------------------------------------------------------------
// CIAUpdateNetworkRegistration::RunL()
// 
// ---------------------------------------------------------------------------
//
void CIAUpdaterDialog::RunL()
    {
    IAUPDATE_TRACE_1("[IAUpdater] CIAUpdaterDialog::RunL() iStatus : %d", iStatus.Int() );
    iNoteId = 0;
    if ( iStatus.Int() == EAknSoftkeyCancel )
        {
    	iObserver->UserCancel();
        }
    else
        {
        iObserver->UserExit();	
        }
    }

// ======== GLOBAL FUNCTIONS ========

//  EOF  

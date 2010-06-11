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
* Description:   This module contains the implementation of CIAUpdateAgreement class 
*                member functions.
*
*/


//INCLUDES
#include <hbaction.h>
#include <hbtextitem.h>
#include <centralrepository.h>

#include "iaupdateagreement.h"
#include "iaupdatefirsttimeinfo.h"
#include "iaupdate.hrh"
#include "iaupdateprivatecrkeys.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CIAUpdateAgreement::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIAUpdateAgreement* CIAUpdateAgreement::NewL()
    {
    CIAUpdateAgreement* self = CIAUpdateAgreement::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CIAUpdateAgreement::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateAgreement* CIAUpdateAgreement::NewLC()
    {
    CIAUpdateAgreement* self = new( ELeave ) CIAUpdateAgreement();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }



// ---------------------------------------------------------------------------
//  CIAUpdateAgreement::ConstructL
// ---------------------------------------------------------------------------
//
void CIAUpdateAgreement::ConstructL()
    {
    }    

// ---------------------------------------------------------------------------
// CIAUpdateAgreement::CIAUpdateAgreement
// constructor
// ---------------------------------------------------------------------------
//
CIAUpdateAgreement::CIAUpdateAgreement()  
    {
    }

// ---------------------------------------------------------------------------
// CIAUpdateAgreement::~CIAUpdateAgreement
// Destructor
// ---------------------------------------------------------------------------
//
CIAUpdateAgreement::~CIAUpdateAgreement()
    {
    }

// ---------------------------------------------------------------------------
// CIAUpdateAgreement::AcceptAgreementL
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateAgreement::AcceptAgreementL() 
    {
    CIAUpdateFirstTimeInfo* firstTimeInfo = CIAUpdateFirstTimeInfo::NewLC();
    TBool accepted = firstTimeInfo->AgreementAcceptedL();
    if ( !accepted )
        {
        // if configured so that automatic updates are allowed, 
        // then also disclaimer is set as accepted without showing it all
        TInt autoUpdateCheckValue( 0 );
        CRepository* cenrep(  CRepository::NewLC( KCRUidIAUpdateSettings ) );
        User::LeaveIfError( cenrep->Get( KIAUpdateAutoUpdateCheck, 
                                         autoUpdateCheckValue ) );
        CleanupStack::PopAndDestroy( cenrep );
        if ( autoUpdateCheckValue == EIAUpdateSettingValueDisableWhenRoaming ||
             autoUpdateCheckValue ==  EIAUpdateSettingValueEnable  ) 
            {
            accepted = ETrue;
            firstTimeInfo->SetAgreementAcceptedL();
            }
        }
    CleanupStack::PopAndDestroy( firstTimeInfo );
    return accepted;
    }

// ---------------------------------------------------------------------------
// CIAUpdateAgreement::SetAgreementAcceptedL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateAgreement::SetAgreementAcceptedL()
    {
    CIAUpdateFirstTimeInfo* firstTimeInfo = CIAUpdateFirstTimeInfo::NewLC();
    firstTimeInfo->SetAgreementAcceptedL();
    CleanupStack::PopAndDestroy( firstTimeInfo );
    }



// ---------------------------------------------------------------------------
// CIAUpdateAgreement::AgreementAcceptedL
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateAgreement::AgreementAcceptedL()
    {
    CIAUpdateFirstTimeInfo* firstTimeInfo = CIAUpdateFirstTimeInfo::NewLC();
	TBool ret = firstTimeInfo->AgreementAcceptedL();
	CleanupStack::PopAndDestroy( firstTimeInfo );
	return ret;
    }
    

    
    /*HBufC* text_1 = StringLoader::LoadLC( R_IAUPDATE_AGREEMENT_DIALOG_TEXT_1 );
    HBufC* text_2 = StringLoader::LoadLC( R_IAUPDATE_AGREEMENT_DIALOG_TEXT_2 );
    HBufC* text_3 = StringLoader::LoadLC( R_IAUPDATE_AGREEMENT_DIALOG_TEXT_3 );
    HBufC* text_4 = StringLoader::LoadLC( R_IAUPDATE_AGREEMENT_DIALOG_TEXT_4 );
    HBufC* text_5 = StringLoader::LoadLC( R_IAUPDATE_AGREEMENT_DIALOG_TEXT_5 );
    HBufC* text_6 = StringLoader::LoadLC( R_IAUPDATE_AGREEMENT_DIALOG_TEXT_6 );
    HBufC* text_7 = StringLoader::LoadLC( R_IAUPDATE_AGREEMENT_DIALOG_TEXT_7 );
    HBufC* text_8 = StringLoader::LoadLC( R_IAUPDATE_AGREEMENT_DIALOG_TEXT_8 );
    HBufC* text_9 = StringLoader::LoadLC( R_IAUPDATE_AGREEMENT_DIALOG_TEXT_9 );
    CArrayFix<TInt>* numberArray = 
                         new ( ELeave ) CArrayFixFlat<TInt>( 2 );
    CleanupStack::PushL( numberArray );
    numberArray->AppendL( 2007 ); 
    numberArray->AppendL( 2009 );
    //HBufC* text_copyright = StringLoader::LoadLC( R_IAUPDATE_AGREEMENT_DIALOG_COPYRIGHT, *numberArray );
    TPtr ptr = text_copyright->Des();
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );
    
    HBufC* text = HBufC::NewLC( text_1->Length() +
                                text_2->Length() + 
                                text_3->Length() + 
                                text_4->Length() + 
                                text_5->Length() + 
                                text_6->Length() + 
                                text_7->Length() + 
                                text_8->Length() + 
                                text_9->Length() + 
                                text_copyright->Length() );
        
    text->Des() += *text_1;
    text->Des() += *text_2;
    text->Des() += *text_3;
    text->Des() += *text_4;
    text->Des() += *text_5;
    text->Des() += *text_6;
    text->Des() += *text_7;
    text->Des() += *text_8;
    text->Des() += *text_9;
    text->Des() += *text_copyright;
         
    CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL( *text );
	CleanupStack::PopAndDestroy( text );
	CleanupStack::PopAndDestroy( text_copyright );
	CleanupStack::PopAndDestroy( numberArray );
	CleanupStack::PopAndDestroy( text_9 );
	CleanupStack::PopAndDestroy( text_8 );
	CleanupStack::PopAndDestroy( text_7 );
	CleanupStack::PopAndDestroy( text_6 );
	CleanupStack::PopAndDestroy( text_5 );
	CleanupStack::PopAndDestroy( text_4 );
	CleanupStack::PopAndDestroy( text_3 );
	CleanupStack::PopAndDestroy( text_2 );
	CleanupStack::PopAndDestroy( text_1 );
	dlg->PrepareLC( R_IAUPDATE_MESSAGE_QUERY );

    CAknPopupHeadingPane* headingPane = dlg->Heading();
    HBufC* title = StringLoader::LoadLC( R_IAUPDATE_AGREEMENT_DIALOG_TITLE );
	headingPane->SetTextL( *title );
	CleanupStack::PopAndDestroy( title );
	    
    dlg->ButtonGroupContainer().SetCommandSetL( aCbaResourceId );

	TInt ret = dlg->RunLD();
	
    return ret;*/
    //return KErrNone;

    
// End of File  

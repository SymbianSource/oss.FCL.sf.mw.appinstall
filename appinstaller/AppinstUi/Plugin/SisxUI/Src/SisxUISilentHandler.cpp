/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of CSisxUISilentHandler
*                class member functions.
*
*/


// INCLUDE FILES
#include <eikenv.h>
#include <apgtask.h>
#include <SWInstDefs.h>


#include "SisxUISilentHandler.h"
#include "SisxUICertificateInfo.h"
#include "SisxUIAppInfo.h"
#include "SisxUISelectionCache.h"
#include "SisxUIResult.h"


using namespace SwiUI;

	
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSisxUISilentHandler::CSisxUISilentHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSisxUISilentHandler::CSisxUISilentHandler( const TInstallOptions &aOptions, 
                                            MSisxUICancelObserver* aCancelObserver )
    : CSisxUIHandler( EModeInstall, aCancelObserver), iOptions( aOptions )
    {    
    }

// -----------------------------------------------------------------------------
// CSisxUISilentHandler::CSisxUISilentHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSisxUISilentHandler::CSisxUISilentHandler( const TUninstallOptions &aOptions, 
                                            MSisxUICancelObserver* aCancelObserver )
    : CSisxUIHandler( EModeUninstall, aCancelObserver ), iUninstallOptions( aOptions )
    {    
    }
// -----------------------------------------------------------------------------
// CSisxUISilentHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSisxUISilentHandler::ConstructL()
    {
    iCache = CSisxUISelectionCache::NewL();
    // created just to load resources
    iCommonDialogs = CommonUI::CCUIDialogs::NewL();
    }

// -----------------------------------------------------------------------------
// CSisxUISilentHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSisxUISilentHandler* CSisxUISilentHandler::NewL( const TInstallOptions &aOptions, 
                                                  MSisxUICancelObserver* aCancelObserver )
    {
    CSisxUISilentHandler* self = new( ELeave ) CSisxUISilentHandler( aOptions, aCancelObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }
    
CSisxUISilentHandler* CSisxUISilentHandler::NewL( const TUninstallOptions &aOptions, 
                                                  MSisxUICancelObserver* aCancelObserver )
    {
    CSisxUISilentHandler* self = new( ELeave ) CSisxUISilentHandler( aOptions, aCancelObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }


// Destructor
CSisxUISilentHandler::~CSisxUISilentHandler()
    {
    }

// -----------------------------------------------------------------------------
// CSisxUISilentHandler::DisplayInstallL
// Displays a dialog at the beginning of an installation with details about the 
// SISX package being installed. This dialog must be shown in TrustedUI mode.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUISilentHandler::DisplayInstallL( 
    const Swi::CAppInfo& /*aAppInfo*/,
    const CApaMaskedBitmap* /*aLogo*/,
    const RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/ )  
    {
    // Mark the integrity as done
    iIntegrityCheckDone = ETrue;    
    TBool result( ETrue );
    return result;
    }

// -----------------------------------------------------------------------------
// CSisxUISilentHandler::DisplayGrantCapabilitiesL
// displays a dialog requesting that the user grants these capabilities to the 
// SISX package being installed. This dialog must be shown in TrustedUI mode.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUISilentHandler::DisplayGrantCapabilitiesL( const Swi::CAppInfo& /*aAppInfo*/, 
                                                       const TCapabilitySet& /*aCapabilitySet*/ ) 
    {
    TBool result( EFalse );
    switch ( iOptions.iCapabilities )
        {
        case EPolicyAllowed:
            {
            result = ETrue;
            break;	
            }
        case EPolicyUserConfirm:
    	default:
            iResult = TSisxUIResult( EUiResultCapabilitiesCannotBeGranted );
            break;
        }
    return result;       
    }
										
// -----------------------------------------------------------------------------
// CSisxUISilentHandler::DisplayLanguageL
// requests preferred language and dialect information from the UI.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSisxUISilentHandler::DisplayLanguageL( const Swi::CAppInfo& /*aAppInfo*/, 
                                             const RArray<TLanguage>& aLanguages ) 
    {
    TInt languageIndex( -1 );
    TInt index( 0 ) ;
    if ( iOptions.iUsePhoneLang )
    	{
    	TLanguage phoneLanguage( User::Language() );    

        // If phones default language is available in the file, do not query the user
        for ( index = 0; index < aLanguages.Count(); index++ )
            {
            // Check if this matches the phone language        
            if( aLanguages[ index ] == phoneLanguage )
                {
                return index;
                }
	        
            // Check if user has already selected this language during installation        
            if( iCache->LanguageCached() && aLanguages[index] == iCache->Language() )
                {
                // Do not query user, we have a cached language    
                return index;            
                }        
            }
    	}
    
    if ( iOptions.iLang == ELangNone )
    	{
    	// Don't bother to see if there's ELangNone in language list
    	if ( aLanguages.Count() > 0 )
            {
            languageIndex = 0;
            }
    	}
    else
    	{
        for ( index = 0; index < aLanguages.Count(); index++ )
            {
            // Check if this matches the language defined by silent install options
            if( aLanguages[ index ] == iOptions.iLang)
                {
                languageIndex = index;
                break;
                }
	        
            // Check if user has already selected this language during installation        
            if( iCache->LanguageCached() && aLanguages[index] == iCache->Language() )
                {
                // Do not query user, we have a cached language    
                languageIndex = index;            
                break;
                }        
            }
        if ( languageIndex == -1 )
            {
            if ( aLanguages.Count() > 0 )
            	{
            	languageIndex = 0;
            	}
            else
            	{
            	iResult = TSisxUIResult( EUiResultCancelled ); 	
            	}	
            }
        else
            {
            // Cache only if no previous cache is found
            if( !iCache->LanguageCached() )
                {
                iCache->SetLanguage( aLanguages[languageIndex] );
                }     
            }
    	}
    return languageIndex;
    }
	
// -----------------------------------------------------------------------------
// CSisxUISilentHandler::DisplayDriveL
// Requests the drive to install the package on, from the UI.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSisxUISilentHandler::DisplayDriveL( const Swi::CAppInfo& /*aAppInfo*/, 
                                          TInt64 aSize,
                                          const RArray<TChar>& aDriveLetters,
                                          const RArray<TInt64>& aDriveSpaces ) 
    {
    TInt selection( -1 );    

    if ( aDriveLetters.Count() <= 0 )
        {
        iResult = TSisxUIResult( EUiResultInsufficientMemory );    
        }    
    else
        {
        // Find the drive specified in options
        for ( TInt index( aDriveLetters.Count() - 1 ); index >= 0; index-- )
            {
            TChar givenDrive( aDriveLetters[index] );
            givenDrive.Fold();
            TChar optionsDrive( iOptions.iDrive );
            optionsDrive.Fold();
            
            if ( givenDrive == optionsDrive || optionsDrive == KUserConfirmDrive )
                {
                if ( aDriveSpaces[index] > aSize )
                    {
                    selection = index;
                    break;
                    }
                else
                    {
                    iResult = TSisxUIResult( EUiResultInsufficientMemory );
                    break;
                    }
                }
            }
       	if ( selection == -1 )
            {
            iResult = TSisxUIResult( EUiResultDiskNotPresent );
            }
        }
    return selection;    
    }
	
// -----------------------------------------------------------------------------
// CSisxUISilentHandler::DisplayUpgradeL
// Displays a dialog asking whether we want to upgrade an application present 
// on the device.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUISilentHandler::DisplayUpgradeL( const Swi::CAppInfo& /*aAppInfo*/, 
                                             const Swi::CAppInfo& /*aExistingAppInfo*/ ) 
    {
    TBool result( EFalse );
    if ( iOptions.iUpgrade == EPolicyAllowed )
    	{
    	result = ETrue;
    	}
    else
    	{
    	iResult = TSisxUIResult( EUiResultCancelled );
    	}
    return result;
    }

// -----------------------------------------------------------------------------
// CSisxUISilentHandler::DisplayOptionsL
// Displays a dialog box asking which options the user wishes to install.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUISilentHandler::DisplayOptionsL( const Swi::CAppInfo& /*aAppInfo*/,
                                             const RPointerArray<TDesC>& aOptions,
                                             RArray<TBool>& aSelections ) 
    {
    TBool response ( ETrue );
    if ( aOptions.Count() < 1 )
        {
        User::Leave( KErrArgument );        
        }
    TBool allow( iOptions.iOptionalItems == EPolicyAllowed );
    // set all to 
    for( TInt index( 0 ); index < aSelections.Count(); index++ )
        {
        aSelections[index] = allow ;
        }
    if ( !allow )
        {
        iResult = TSisxUIResult( EUiResultCancelled );
        }
    return response;
    }

// -----------------------------------------------------------------------------
// CSisxUISilentHandler::HandleInstallEventL
// Informs the UI of events occurring during installation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUISilentHandler::HandleInstallEventL( const Swi::CAppInfo& aAppInfo,
                                                 Swi::TInstallEvent aEvent, 
                                                 TInt /*aValue*/, 
                                                 const TDesC& /*aDesC*/ )
    {
    switch ( aEvent )
        {
        case Swi::EEventSetProgressBarFinalValue:
            // Here we need to update the name and vendor of the main package.
            // They might have changed when a language has been selected.
            if ( iPackages.Count() > 0 )
                {                  
                iPackages[0]->SetNameL( aAppInfo.AppName() );
                iPackages[0]->SetVendorL( aAppInfo.AppVendor() );
                }
            
            break;
        case Swi::EEventOcspCheckEnd:
            iOcspInProgress = EFalse;            
            break;
        default:
            break;       
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CSisxUISilentHandler::HandleCancellableInstallEventL
// Informs the UI of cancellable events occurring during installation.  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUISilentHandler::HandleCancellableInstallEventL( const Swi::CAppInfo& /*aAppInfo*/,
                                                           Swi::TInstallCancellableEvent /*aEvent*/, 
                                                           Swi::MCancelHandler& /*aCancelHandler*/,
                                                           TInt /*aValue*/, 
                                                           const TDesC& /*aDes*/ )
    {
    // Empty
    }

// -----------------------------------------------------------------------------
// CSisxUISilentHandler::DisplaySecurityWarningL
// Displays a dialog indicating that the package is unsigned, or that signature 
// validation has failed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUISilentHandler::DisplaySecurityWarningL( const Swi::CAppInfo& aAppInfo,
                                                     Swi::TSignatureValidationResult aSigValidationResult,
                                                     RPointerArray<CPKIXValidationResultBase>& /*aPkixResults*/,
                                                     RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/,
                                                     TBool aInstallAnyway )
    {
    TBool result( EFalse );
    switch ( aSigValidationResult )
        {
        case Swi::EValidationSucceeded:       
            {
            result = ETrue;
            CSisxUIAppInfo* appInfo = CSisxUIAppInfo::NewL( aAppInfo, ETrue );
            iPackages.Append( appInfo );
            break;
            }       

            // Non fatals
        case Swi::ESignatureSelfSigned:
        case Swi::ESignatureNotPresent:
        case Swi::ENoCertificate:
        case Swi::ECertificateValidationError:
        case Swi::ENoCodeSigningExtension:
        case Swi::ENoSupportedPolicyExtension:
            // Fatals with aInstallAnyway set as EFalse
        case Swi::ESignatureCouldNotBeValidated:
        case Swi::EMandatorySignatureMissing:
            {
            // Check if we are allowed to continue installation
            if ( aInstallAnyway && iAllowUntrusted )
                {          
                if ( iOptions.iUntrusted == EPolicyAllowed )
                    {
                    result = ETrue;
                    CSisxUIAppInfo* appInfo = CSisxUIAppInfo::NewL( aAppInfo, ETrue );
                    iPackages.Append( appInfo );
                    }
                else
                    {
                    iResult = TSisxUIResult( aSigValidationResult );
                    }
                }
            else
                {
                // Fatal error, not allowed to continue
                iResult = TSisxUIResult( aSigValidationResult );
                }            
            break;
            }

        default:
            iResult = TSisxUIResult( aSigValidationResult );
            break;            
        }
    
    return result;    
    }
	
// -----------------------------------------------------------------------------
// CSisxUISilentHandler::DisplayOcspResultL
// Displays a dialog indicating that there was an error carrying out online 
// certificate status checking.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUISilentHandler::DisplayOcspResultL( const Swi::CAppInfo& /*aAppInfo*/,
                                                Swi::TRevocationDialogMessage aMessage, 
                                                RPointerArray<TOCSPOutcome>& /*aOutcomes*/, 
                                                RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/,
                                                TBool aWarningOnly )
    {
    TBool result( ETrue );

    // This will be treated as warning if engine says it's a warning and
    // ocsp setting is on. In case it's must, we abort the installation.
    if ( aWarningOnly && iOcspProc == ESisxUIOcspProcOn )
        {
        switch ( iOptions.iIgnoreOCSPWarnings )
            {
            case EPolicyAllowed:
                {
                result = ETrue;
                break;
                }
            case EPolicyUserConfirm:
            default:
                {
                result = EFalse;
                break;			
                }
            }
        if ( !result )
            {
            iResult = TSisxUIResult( EUiResultCancelled );
            }                
        }
    // error or ocsp is set to must
    else
        {
        iResult = TSisxUIResult( aMessage );
        result = EFalse; 
        }            

    return result;    
    }

// -----------------------------------------------------------------------------
// CSisxUISilentHandler::DisplayCannotOverwriteFileL
// Used to inform the user the installation they are attempting is trying to 
// overwrite a file that does not belong to it. Following this message 
// installation is aborted.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUISilentHandler::DisplayCannotOverwriteFileL( const Swi::CAppInfo& /*aAppInfo*/,
                                                        const Swi::CAppInfo& /*aInstalledAppInfo*/,
                                                        const TDesC& /*aFileName*/ )
    {
    iResult = TSisxUIResult( EUiResultUnableToOverwrite );
    }
 
// -----------------------------------------------------------------------------
// CSisxUISilentHandler::DisplayMissingDependencyL
// Used to inform the user of a missing dependency needed to install the 
// current package. It is up to the UI implementors and/or the user whether
// or not this warning can be overridden.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUISilentHandler::DisplayMissingDependencyL( const Swi::CAppInfo& /*aAppInfo*/,
                                                       const TDesC& /*aDependencyName*/,
                                                       TVersion /*aWantedVersionFrom*/,
                                                       TVersion /*aWantedVersionTo*/,
                                                       TVersion /*aInstalledVersion*/ )
    {
    TBool result( EFalse );
    iResult = TSisxUIResult( EUiResultMissingDependency );
    return result;
    }
       
// -----------------------------------------------------------------------------
// CSisxUISilentHandler::DisplayUninstallL
// Displays a dialog at the start of the uninstallation with information about 
// the application to be uninstalled. The user may cancel the uninstallation at 
// this stage. This dialog must be shown in TrustedUI mode.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUISilentHandler::DisplayUninstallL( const Swi::CAppInfo& aAppInfo )
    {
    TBool result( ETrue );
    CSisxUIAppInfo* appInfo = CSisxUIAppInfo::NewL( aAppInfo, EFalse );
    iPackages.Append( appInfo );
    return result;
    }

// -----------------------------------------------------------------------------
// CSisxUISilentHandler::DisplayTextL
// Displays a dialog containing text. This dialog has only a Continue button if 
// aOption is EInstFileTextOptionContinue, or Yes and No buttons otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUISilentHandler::DisplayTextL( const Swi::CAppInfo& aAppInfo, 
                                          Swi::TFileTextOption aOption,
                                          const TDesC& aText ) 
    {
    TBool result( EFalse );
    switch ( iOptions.iPackageInfo )
    	{
    	case EPolicyAllowed:
            {
            result = ETrue;	
            break;
            }
        case EPolicyUserConfirm:
            {
            BringToForeground( ETrue);
            result = CSisxUIHandler::DisplayTextL( aAppInfo, aOption, aText );
            BringToForeground( EFalse);
            break;	
            }
        default:
            {
            iResult = TSisxUIResult( EUiResultCancelled );	
            break;
            }
    	}
    return result;    
    }

// -----------------------------------------------------------------------------
// CSisxUISilentHandler::DisplayErrorL
// Displays a dialog showing an error message.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUISilentHandler::DisplayErrorL( const Swi::CAppInfo& /*aAppInfo*/,
                                          Swi::TErrorDialog aType, 
                                          const TDesC& /*aParam*/ ) 
    {
    iResult = TSisxUIResult( aType );
    }

// -----------------------------------------------------------------------------
// CSisxUISilentHandler::DisplayDependencyBreakL
// Displays a dialog indicating that a component which is being uninstalled is 
// a dependency of another component installed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUISilentHandler::DisplayDependencyBreakL( const Swi::CAppInfo& /*aAppInfo*/,
                                                     const RPointerArray<TDesC>& /*aComponents */)
    {
    TBool result( EFalse );
    switch ( iUninstallOptions.iBreakDependency )
    	{
    	case EPolicyAllowed:
            {
            result = ETrue;
            break;
            }
        case EPolicyUserConfirm:
        default:
            iResult = TSisxUIResult( EUiResultCancelled );
            break;
    	}
    return result;
    }
		
// -----------------------------------------------------------------------------
// CSisxUISilentHandler::DisplayApplicationsInUseL
// Informs user that the applications are in use during an (un)installation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUISilentHandler::DisplayApplicationsInUseL( const Swi::CAppInfo& /*aAppInfo*/, 
                                                       const RPointerArray<TDesC>& /*aAppNames */)
    {
    TBool result( EFalse );
    TPolicy policy;
    if ( iMode == EModeInstall )
        {
        policy = iOptions.iKillApp ;
        }
    else
        {
        policy = iUninstallOptions.iKillApp ;
        }
    switch ( policy )
    	{
    	case EPolicyAllowed:
            {
            result = ETrue;
            break;
            }
        case EPolicyUserConfirm:
        default:
            break;
    	}
    if ( !result )
        {
        iResult = TSisxUIResult( EUiResultCancelled );
        }
    return result;
    }
	
// -----------------------------------------------------------------------------
// CSisxUISilentHandler::DisplayQuestionL
// Displays a generic question dialog with a Yes/No answer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUISilentHandler::DisplayQuestionL( const Swi::CAppInfo& /*aAppInfo*/, 
                                              Swi::TQuestionDialog aQuestion, 
                                              const TDesC& /*aDes*/ )
    {
    TBool result( EFalse );
    switch ( aQuestion )
    	{
    	case Swi::EQuestionIncompatible:
            {
            switch ( iOptions.iPackageInfo )
                {
                case EPolicyAllowed:
                case EPolicyUserConfirm:
                    {
                    result = ETrue;
                    break;
                    }
                default:
                    iResult = TSisxUIResult( EUiResultCancelled );
                    break;
                }	
            break;
            }
    	case Swi::EQuestionOverwriteFile:
            {
            switch ( iOptions.iOverwrite )
                {
                case EPolicyAllowed:
                    {
                    result = ETrue;
                    break;
                    }
                case EPolicyUserConfirm:
                default:
                    iResult = TSisxUIResult( EUiResultCancelled );
                    break;
                }
            break;
            }
    	}
    return result;    
    }


// -----------------------------------------------------------------------------
// CSisxUISilentHandler::ShowFinalNoteL
// Show the final note.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
void CSisxUISilentHandler::ShowFinalNoteL()
    {
    }
    
// -----------------------------------------------------------------------------
// CSisxUISilentHandler::BringToForeground
// Swithces application to foreground or background. Used when user confirmation 
// dialog is used dialog is used in silent installation
// -----------------------------------------------------------------------------
//
void CSisxUISilentHandler::BringToForeground( TBool aForeground )
    {    	                                                
    RWsSession ws;   
                                             
    if ( ws.Connect() == KErrNone )
        {
        CleanupClosePushL(ws);        
        TApaTaskList tasklist(ws);   
        
        if ( aForeground )
            {
        	TApaTask task = tasklist.FindApp( TUid::Uid(KSWInstSvrUid) );
        	if ( task.Exists() )
        	    {
        		task.BringToForeground();
        	    }
            }
        else
            {
            TApaTask task = tasklist.FindApp( TUid::Uid(KSWInstSvrUid) );
        	if ( task.Exists() )
        	    {
        		task.SendToBackground();
        	    }	
            }
                                                                
        CleanupStack::PopAndDestroy(); //ws       
        }                           
    }

//  End of File  

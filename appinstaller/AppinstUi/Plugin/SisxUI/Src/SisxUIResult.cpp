/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of CSisxUI
*                class member functions.
*
*/


// INCLUDE FILES

#include <SWInstDefs.h>

#include "SisxUIResult.h"

using namespace SwiUI;

// -----------------------------------------------------------------------------
// TSisxUIResult::TSisxUIResult
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TSisxUIResult::TSisxUIResult()
    : iResult( EUiResultNoResult ), iDetailedErrorCode( 0 )
    {
    }

// -----------------------------------------------------------------------------
// TSisxUIResult::TSisxUIResult
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TSisxUIResult::TSisxUIResult( const TSisxResultCode& aResult )
    : iResult( aResult ), iDetailedErrorCode( 0 )
    {
    }

// -----------------------------------------------------------------------------
// TSisxUIResult::TSisxUIResult
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TSisxUIResult::TSisxUIResult( const Swi::TRevocationDialogMessage& aResult )
    {
    switch ( aResult )
        {
        case Swi::EInvalidRevocationServerUrl:
            iResult = EUiResultInvalidRevocationServerUrl;            
            break;
            
        case Swi::EUnableToObtainCertificateStatus: 
            iResult = EUiResultUnableToObtainCertificateStatus;
            break;

        case Swi::EResponseSignatureValidationFailure:
            iResult = EUiResultResponseSignatureValidationFailure;
            break;

        case Swi::EInvalidRevocationServerResponse:
            iResult = EUiResultInvalidRevocationServerResponse;
            break;

        case Swi::EInvalidCertificateStatusInformation:
            iResult = EUiResultInvalidCertificateStatusInformation;
            break;

        case Swi::ECertificateStatusIsUnknown:
            iResult = EUiResultCertificateStatusIsUnknown;
            break;

        case Swi::ECertificateStatusIsRevoked:  
            iResult = EUiResultCertificateStatusIsRevoked;
            break;

        case Swi::ECertificateStatusIsUnknownSelfSigned:
            iResult = EUiResultCertificateStatusIsUnknownSelfSigned;
            break;

        default:
            iResult = EUiResultGeneralError;
            iDetailedErrorCode = aResult;
            break; 
        }
    }

// -----------------------------------------------------------------------------
// TSisxUIResult::TSisxUIResult
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TSisxUIResult::TSisxUIResult( const Swi::TSignatureValidationResult& aResult )
    {
    switch ( aResult )
        {
        case Swi::ESignatureNotPresent:
            iResult = EUiResultSignatureNotPresent;            
            break;    
 
        case Swi::ENoCertificate:
            iResult = EUiResultNoCertificate;            
            break; 
    
        case Swi::ECertificateValidationError:
            iResult = EUiResultCertificateValidationError;            
            break; 
    
        case Swi::ENoCodeSigningExtension:
            iResult = EUiResultNoCodeSigningExtension;
            break; 
    
        case Swi::ENoSupportedPolicyExtension:
            iResult = EUiResultNoSupportedPolicyExtension;
            break;
     
        case Swi::ESignatureCouldNotBeValidated:
            iResult = EUiResultSignatureCouldNotBeValidated;
            break;  
   
        case Swi::EMandatorySignatureMissing:
            iResult = EUiResultMandatorySignatureMissing;
            break;

        case Swi::ESignatureSelfSigned:
            iResult = EUiResultSignatureSelfSigned;            
            break;
            
        default:
            iResult = EUiResultGeneralError;
            iDetailedErrorCode = aResult;
            break;            
        }
    }

// -----------------------------------------------------------------------------
// TSisxUIResult::TSisxUIResult
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TSisxUIResult::TSisxUIResult( const Swi::TErrorDialog& aResult )
    {
    switch ( aResult )
        {
        case Swi::EUiAlreadyInRom:
            iResult = EUiResultAlreadyInRom;
            break;

        case Swi::EUiMissingDependency:
            iResult = EUiResultMissingDependency;
            break;

        case Swi::EUiRequireVer:	
            iResult = EUiResultRequireVer;
            break;

        case Swi::EUiRequireVerOrGreater:
            iResult = EUiResultRequireVerOrGreater;
            break;

        case Swi::EUiFileCorrupt:	
            iResult = EUiResultFileCorrupt;
            break;

        case Swi::EUiDiskNotPresent:
            iResult = EUiResultDiskNotPresent;
            break;

        case Swi::EUiCannotRead: 
            iResult = EUiResultCannotRead;
            break;

        case Swi::EUiCannotDelete:  
            iResult = EUiResultCannotDelete;
            break;

        case Swi::EUiInvalidFileName:
            iResult = EUiResultInvalidFileName;
            break;

        case Swi::EUiFileNotFound: 
            iResult = EUiResultFileNotFound;
            break;

        case Swi::EUiInsufficientSpaceOnDrive:
        case Swi::EUiNoMemoryInDrive:
            iResult = EUiResultInsufficientSpaceOnDrive;
            break;

        case Swi::EUiCapabilitiesCannotBeGranted:
            iResult = EUiResultCapabilitiesCannotBeGranted;
            break;

        case Swi::EUiUnknownFile:
            iResult = EUiResultUnknownFile;
            break;

        case Swi::EUiMissingBasePackage:
            iResult = EUiResultMissingBasePackage;
            break;

        case Swi::EUiConstraintsExceeded:
            iResult = EUiResultConstraintsExceeded;            
            break;
            
        case Swi::EUiSIDViolation:
            iResult = EUiResultSIDViolation;            
            break;
            
        case Swi::EUiVIDViolation:
            iResult = EUiResultVIDViolation;            
            break;

        case Swi::EUiUIDPackageViolation:
            iResult = EUiResultUIDPackageViolation;
            break;            

        case Swi::EUiBlockingEclipsingFile:
            iResult = EUiResultBlockingEclipsingFile;
            break;            

        case Swi::EUiOSExeViolation:
            iResult = EUiResultOSExeViolation;
            break;

        case Swi::EUiSIDMismatch:
            iResult = EUiResultSIDMismatch;
            break;

        default:
            iResult = EUiResultGeneralError;
            iDetailedErrorCode = aResult;
            break;     
        }     
    }

// -----------------------------------------------------------------------------
// TSisxUIResult::CommonResult
// Returns the common SWInst result code for this result.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt TSisxUIResult::CommonResult() const
    {
    TInt result( KErrNone );
    
    switch ( iResult )
        {
        case EUiResultOk:
            result = KErrNone;            
            break;
            
        case EUiResultCancelled:
            result = KSWInstErrUserCancel;            
            break;

        case EUiResultAlreadyInRom:
        case EUiResultUnableToOverwrite:
        case EUiResultCannotDelete:          	
            result = KSWInstErrFileInUse;
            break;

        case EUiResultUnknownFile:
        case EUiResultFileNotFound:
        case EUiResultInvalidFileName:		
        case EUiResultFileCorrupt:
        case EUiResultCannotRead:  
            result = KSWInstErrFileCorrupted;            
            break;            

        case EUiResultInsufficientMemory:
        case EUiResultInsufficientSpaceOnDrive:
            result = KSWInstErrInsufficientMemory;
            break;            
            
        case EUiResultNoCertificate:
        case EUiResultCertificateValidationError:
        case EUiResultSignatureNotPresent:
        case EUiResultSignatureCouldNotBeValidated:
        case EUiResultNoCodeSigningExtension:
        case EUiResultNoSupportedPolicyExtension:  
        case EUiResultMandatorySignatureMissing:
        case EUiResultInvalidRevocationServerResponse:
        case EUiResultInvalidRevocationServerUrl:
        case EUiResultUnableToObtainCertificateStatus:  
        case EUiResultResponseSignatureValidationFailure:
        case EUiResultInvalidCertificateStatusInformation:
        case EUiResultCertificateStatusIsUnknown:
        case EUiResultCertificateStatusIsRevoked: 
        case EUiResultCertificateStatusIsUnknownSelfSigned:
        case EUiResultCertificateExpired:
        case EUiResultCertificateValidInFuture:
        case EUiResultSignatureSelfSigned:
        case EUiResultCapabilitiesCannotBeGranted:
        case EUiResultSIDViolation:
        case EUiResultVIDViolation:
        case EUiResultConstraintsExceeded:
        case EUiResultUIDPackageViolation:
        case EUiResultSIDMismatch:
            result = KSWInstErrSecurityFailure;
            break;

        case EUiResultAccessDenied:
        case EUiResultCannotUninstallPartialUpgrade:
        case EUiResultOSExeViolation:
            result = KSWInstErrAccessDenied;
            break;
            
        case EUiResultMissingDependency:			
        case EUiResultRequireVer:		
        case EUiResultRequireVerOrGreater:
        case EUiResultMissingBasePackage:
            result = KSWInstErrMissingDependency;
            break;  

        case EUiResultInvalidUpgrade:
        case EUiResultBlockingEclipsingFile:
            result = KSWInstUpgradeError;
            break;            

        case EUiResultGeneralError:            
        case EUiResultDiskNotPresent:
        case EUiResultNoResult:
        default:            
            result = KSWInstErrGeneralError;
            break;            
        }  

    return result;    
    }


//  End of File  

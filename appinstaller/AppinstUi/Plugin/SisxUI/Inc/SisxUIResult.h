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
* Description:   This file contains the header file of the TSisxUIResult class.
*
*/


#ifndef SISXUIRESULT_H
#define SISXUIRESULT_H

//  INCLUDES
#include <e32std.h>
#include <swi/msisuihandlers.h>

namespace SwiUI
{

/**
* Enumeration for sisx specific results / errors.
* @since 3.0
*/
enum TSisxResultCode
    {
    EUiResultNoResult = 0,                              // code not set
    EUiResultOk = 1,                                    // KErrNone
    EUiResultCancelled = 2,                             // KErrCancel, KSWInstErrUserCancel
    EUiResultGeneralError = 3,                          // some other error
    EUiResultUnableToOverwrite = 4,                     // CSisxUIHandler::DisplayCannotOverwriteFileL()
    EUiResultAlreadyInRom = 5,                          // TErrorDialog
    EUiResultMissingDependency = 6,                     // TErrorDialog
    EUiResultRequireVer = 7,                            // TErrorDialog
    EUiResultRequireVerOrGreater = 8,                   // TErrorDialog
    EUiResultFileCorrupt = 9,                           // TErrorDialog, KSWInstErrFileCorrupted
    EUiResultDiskNotPresent = 10,                       // TErrorDialog
    EUiResultCannotRead = 11,                           // TErrorDialog
    EUiResultCannotDelete = 12,                         // TErrorDialog
    EUiResultInvalidFileName = 13,                      // TErrorDialog
    EUiResultFileNotFound = 14,                         // TErrorDialog
    EUiResultInsufficientSpaceOnDrive = 15,             // TErrorDialog
    EUiResultCapabilitiesCannotBeGranted = 16,          // TErrorDialog
    EUiResultUnknownFile = 17,                          // TErrorDialog
    EUiResultMissingBasePackage = 18,                   // TErrorDialog
    EUiResultInsufficientMemory = 19,                   // KErrDiskFull
    EUiResultNoCertificate = 20,                        // TSignatureValidationResult
    EUiResultCertificateValidationError = 21,           // TSignatureValidationResult
    EUiResultSignatureNotPresent = 22,                  // TSignatureValidationResult
    EUiResultSignatureCouldNotBeValidated = 23,         // TSignatureValidationResult
    EUiResultNoCodeSigningExtension = 24,               // TSignatureValidationResult
    EUiResultNoSupportedPolicyExtension = 25,           // TSignatureValidationResult
    EUiResultMandatorySignatureMissing = 26,            // TSignatureValidationResult
    EUiResultInvalidRevocationServerUrl = 27,           // TRevocationDialogMessage
    EUiResultUnableToObtainCertificateStatus = 28,      // TRevocationDialogMessage
    EUiResultResponseSignatureValidationFailure = 29,   // TRevocationDialogMessage
    EUiResultInvalidRevocationServerResponse = 30,      // TRevocationDialogMessage
    EUiResultInvalidCertificateStatusInformation = 31,  // TRevocationDialogMessage
    EUiResultCertificateStatusIsUnknown = 32,           // TRevocationDialogMessage
    EUiResultCertificateStatusIsRevoked = 33,           // TRevocationDialogMessage
    EUiResultAccessDenied = 34,                         // KErrAccessDenied
    EUiResultCertificateStatusIsUnknownSelfSigned = 35, // TRevocationDialogMessage
    EUiResultCertificateExpired = 36,                   // CSisxUIHandler::DisplaySecurityWarningL()
    EUiResultCertificateValidInFuture = 37,             // CSisxUIHandler::DisplaySecurityWarningL()
    EUiResultConstraintsExceeded = 38,                  // TErrorDialog
    EUiResultSIDViolation = 39,                         // TErrorDialog
    EUiResultVIDViolation = 40,                         // TErrorDialog
    EUiResultCannotUninstallPartialUpgrade = 41,        // not used
    EUiResultSignatureSelfSigned = 42,                  // TSignatureValidationResult
    EUiResultInvalidUpgrade = 43,                       // KErrInvalidUpgrade
    EUiResultUIDPackageViolation = 44,                  // TErrorDialog
    EUiResultBlockingEclipsingFile = 45,                // TErrorDialog
    EUiResultOSExeViolation = 46,                       // TErrorDialog
    EUiResultSIDMismatch = 47                           // TErrorDialog
    };


/**
* Result of the installation / uninstallation operation.
*
* @lib sisxui.lib
* @since 3.0
*/
class TSisxUIResult
    {
    public:
        
        /**
        * C++ default constructor.
        */
        TSisxUIResult(); 

        /**
        * C++ default constructor.
        * @param aResult - Operation result.
        */
        TSisxUIResult( const TSisxResultCode& aResult );    

        /**
        * C++ default constructor.
        * @param aResult - Operation result.
        */
        TSisxUIResult( const Swi::TRevocationDialogMessage& aResult );

        /**
        * C++ default constructor.
        * @param aResult - Operation result.
        */
        TSisxUIResult( const Swi::TSignatureValidationResult& aResult );

        /**
        * C++ default constructor.
        * @param aResult - Operation result.
        */
        TSisxUIResult( const Swi::TErrorDialog& aResult );

        /**
        * Returns the common SWInst result code for this result.
        * @since 3.0
        * @return Common result code.
        */
        TInt CommonResult() const;        
                
    public: // Data
        
        TSisxResultCode iResult;
        TInt iDetailedErrorCode;    // when iResult is EUiResultGeneralError
    };

}

#endif      // SISXUI_H   
            
// End of File

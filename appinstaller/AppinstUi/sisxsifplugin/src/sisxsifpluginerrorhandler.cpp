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
* Description:  CSisxSifPluginError collects information about errors.
*               Error details are then passed to framework (USIF).
*
*/

#include "sisxsifpluginerrorhandler.h"      // CSisxSifPluginErrorHandler
#include <swi/sisinstallerrors.h>           // SWI error codes
#include <usif/sif/sifcommon.h>             // COpaqueNamedParams
#include "sisxsifplugin.pan"                // Panic codes

using namespace Usif;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSisxSifPluginErrorHandler::NewL()
// ---------------------------------------------------------------------------
//
CSisxSifPluginErrorHandler* CSisxSifPluginErrorHandler::NewL()
    {
    CSisxSifPluginErrorHandler* self = new ( ELeave ) CSisxSifPluginErrorHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginErrorHandler::~CSisxSifPluginErrorHandler()
// ---------------------------------------------------------------------------
//
CSisxSifPluginErrorHandler::~CSisxSifPluginErrorHandler()
    {
    delete iErrorMessage;
    delete iErrorMessageDetails;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginErrorHandler::ErrorCategory()
// ---------------------------------------------------------------------------
//
TErrorCategory CSisxSifPluginErrorHandler::ErrorCategory() const
    {
    switch( iErrorCode )
        {
        // System-wide error codes
        case KErrNone:
            return ENone;
        case KErrNotFound:
        case KErrGeneral:
            return EUnexpectedError;
        case KErrCancel:
            return EUserCancelled;
        case KErrNoMemory:
            return ELowMemory;
        case KErrNotSupported:
        case KErrArgument:
        case KErrTotalLossOfPrecision:
        case KErrBadHandle:
        case KErrOverflow:
        case KErrUnderflow:
        case KErrAlreadyExists:
        case KErrPathNotFound:
        case KErrDied:
            return EUnexpectedError;
        case KErrInUse:
            return EInstallerBusy;
        case KErrServerTerminated:
        case KErrServerBusy:
        case KErrCompletion:
        case KErrNotReady:
        case KErrUnknown:
            return EUnexpectedError;
        case KErrCorrupt:
            return ECorruptedPackage;
        case KErrAccessDenied:
            return ESecurityError;
        case KErrLocked:
        case KErrWrite:
        case KErrDisMounted:
        case KErrEof:
            return EUnexpectedError;
        case KErrDiskFull:
            return ELowDiskSpace;
        case KErrBadDriver:
        case KErrBadName:
        case KErrCommsLineFail:
        case KErrCommsFrame:
        case KErrCommsOverrun:
        case KErrCommsParity:
        case KErrTimedOut:
        case KErrCouldNotConnect:
        case KErrCouldNotDisconnect:
        case KErrDisconnected:
        case KErrBadLibraryEntryPoint:
        case KErrBadDescriptor:
        case KErrAbort:
        case KErrTooBig:
        case KErrDivideByZero:
        case KErrBadPower:
        case KErrDirFull:
        case KErrHardwareNotAvailable:
        case KErrSessionClosed:
            return EUnexpectedError;
        case KErrPermissionDenied:
            return ESecurityError;
        case KErrExtensionNotSupported:
        case KErrCommsBreak:
        case KErrNoSecureTime:
            return EUnexpectedError;

        // Native SW Installer error codes
        case KErrSISFieldIdMissing:
        case KErrSISFieldLengthMissing:
        case KErrSISFieldLengthInvalid:
        case KErrSISStringInvalidLength:
        case KErrSISSignedControllerSISControllerMissing:
        case KErrSISControllerSISInfoMissing:
        case KErrSISInfoSISUidMissing:
        case KErrSISInfoSISNamesMissing:
            return ECorruptedPackage;
        case KErrSISFieldBufferTooShort:
            return EUnexpectedError;
        case KErrSISStringArrayInvalidElement:
        case KErrSISInfoSISVendorNamesMissing:
        case KErrSISInfoSISVersionMissing:
        case KErrSISControllerSISSupportedLanguagesMissing:
        case KErrSISSupportedLanguagesInvalidElement:
        case KErrSISLanguageInvalidLength:
        case KErrSISContentsSISSignedControllerMissing:
        case KErrSISContentsSISDataMissing:
        case KErrSISDataSISFileDataUnitMissing:
        case KErrSISFileDataUnitTargetMissing:
        case KErrSISFileOptionsMissing:
        case KErrSISFileDataUnitDescriptorMissing:
        case KErrSISFileDataDescriptionMissing:
        case KErrSISContentsMissing:
        case KErrSISEmbeddedControllersMissing:
        case KErrSISEmbeddedDataUnitsMissing:
        case KErrSISControllerOptionsMissing:
        case KErrSISExpressionMissing:
        case KErrSISExpressionStringValueMissing:
        case KErrSISOptionsStringMissing:
        case KErrSISFileOptionsExpressionMissing:
        case KErrSISExpressionHeadValueMissing:
        case KErrSISEmbeddedSISOptionsMissing:
        case KErrSISInfoSISUpgradeRangeMissing:
        case KErrSISDependencyMissingUid:
        case KErrSISDependencyMissingVersion:
        case KErrSISDependencyMissingNames:
        case KErrSISPrerequisitesMissingDependency:
        case KErrSISControllerMissingPrerequisites:
        case KErrSISUpgradeRangeMissingVersion:
        case KErrSISUnexpectedFieldType:
        case KErrSISExpressionUnknownOperator:
        case KErrSISArrayReadError:
        case KErrSISArrayTypeMismatch:
        case KErrSISInvalidStringLength:
        case KErrSISCompressionNotSupported:
        case KErrSISTooDeeplyEmbedded:
            return ECorruptedPackage;
        case KErrSISInvalidTargetFile:
        case KErrSISWouldOverWrite:
            return ESecurityError;
        case KErrSISInfoMissingRemoveDirectories:
            return ECorruptedPackage;
        case KErrSISNotEnoughSpaceToInstall:
            return ELowDiskSpace;
        case KErrInstallerLeave:
        case KErrPolicyFileCorrupt:
            return EUnexpectedError;
        case KErrSignatureSchemeNotSupported:
        case KErrDigestNotSupported:
            return EApplicationNotCompatible;
        case KErrBadHash:
            return ECorruptedPackage;
        case KErrSecurityError:
            return ESecurityError;
        case KErrBadUsage:
        case KErrInvalidType:
        case KErrInvalidExpression:
        case KErrExpressionToComplex:
            return EUnexpectedError;
        case KErrMissingBasePackage:
        case KErrInvalidUpgrade:
            return EApplicationNotCompatible;
        case KErrInvalidEclipsing:
            return ESecurityError;
        case KErrWrongHeaderFormat:
            return EUnexpectedError;
        case KErrCapabilitiesMismatch:
            return ESecurityError;
        case KErrLegacySisFile:
        case KErrInvalidSoftwareTypeRegistrationFile:
            return EApplicationNotCompatible;

        // Other error codes
        default:
            __ASSERT_DEBUG( EFalse, Panic( ESisxSifUnknownErrorCode ) );
            return EUnexpectedError;
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginErrorHandler::ErrorCode()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginErrorHandler::FillOutputParamsL( COpaqueNamedParams& aOutputParams ) const
    {
    if( iErrorCode )
        {
        aOutputParams.AddIntL( KSifOutParam_ErrCategory, ErrorCategory() );
        aOutputParams.AddIntL( KSifOutParam_ErrCode, iErrorCode );
        }
    if( iExtendedErrorCode )
        {
        aOutputParams.AddIntL( KSifOutParam_ExtendedErrCode, iExtendedErrorCode );
        }
    if( iErrorMessage )
        {
        aOutputParams.AddStringL( KSifOutParam_ErrMessage, *iErrorMessage );
        }
    if( iErrorMessageDetails )
        {
        aOutputParams.AddStringL( KSifOutParam_ErrMessageDetails, *iErrorMessageDetails );
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginErrorHandler::ErrorCode()
// ---------------------------------------------------------------------------
//
TInt CSisxSifPluginErrorHandler::ErrorCode() const
    {
    return iErrorCode;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginErrorHandler::SetErrorCode()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginErrorHandler::SetErrorCode( TInt aErrorCode )
    {
    iErrorCode = aErrorCode;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginErrorHandler::ExtendedErrorCode()
// ---------------------------------------------------------------------------
//
TInt CSisxSifPluginErrorHandler::ExtendedErrorCode() const
    {
    return iExtendedErrorCode;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginErrorHandler::SetExtendedErrorCode()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginErrorHandler::SetExtendedErrorCode( TInt aExtendedErrorCode )
    {
    iExtendedErrorCode = aExtendedErrorCode;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginErrorHandler::ErrorMessage()
// ---------------------------------------------------------------------------
//
const TDesC& CSisxSifPluginErrorHandler::ErrorMessage() const
    {
    if( iErrorMessage )
        {
        return *iErrorMessage;
        }
    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginErrorHandler::SetErrorMessage()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginErrorHandler::SetErrorMessage( const TDesC& aErrorMessage )
    {
    if( iErrorMessage )
        {
        delete iErrorMessage;
        iErrorMessage = NULL;
        }
    iErrorMessage = aErrorMessage.AllocL();
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginErrorHandler::ErrorMessageDetails()
// ---------------------------------------------------------------------------
//
const TDesC& CSisxSifPluginErrorHandler::ErrorMessageDetails() const
    {
    if( iErrorMessageDetails )
        {
        return *iErrorMessageDetails;
        }
    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginErrorHandler::SetErrorMessageDetails()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginErrorHandler::SetErrorMessageDetails(
        const TDesC& aErrorMessageDetails )
    {
    if( iErrorMessageDetails )
        {
        delete iErrorMessageDetails;
        iErrorMessageDetails = NULL;
        }
    iErrorMessageDetails = aErrorMessageDetails.AllocL();
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginErrorHandler::CSisxSifPluginErrorHandler()
// ---------------------------------------------------------------------------
//
CSisxSifPluginErrorHandler::CSisxSifPluginErrorHandler()
    {
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginError::ConstructL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginErrorHandler::ConstructL()
    {
    }


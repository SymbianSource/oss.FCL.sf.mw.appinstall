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
* Description:   CNcdReportInstall implementation
*
*/


#include "ncdreportinstall.h"

#include "ncdreportmanager.h"
#include "catalogsdebug.h"
#include "ncdpanics.h"
#include "ncdnodeidentifier.h"
#include "catalogsutils.h"
#include "catalogsconstants.h"
#include "ncdrequestinstallation.h"
#include "ncdprotocol.h"
#include "catalogshttpoperation.h"
#include "catalogshttpconfig.h"
#include "ncdrequestgenerator.h"
#include "ncdproviderutils.h"
#include "ncdgeneralmanager.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CNcdReportInstall* CNcdReportInstall::NewLC(
    CNcdReportManager& aReportManager,
    const TDesC& aContentIdentifier,
    const CNcdNodeIdentifier& aMetadataId,
    const TNcdReportStatusInfo& aStatus, 
    const TDesC& aReportUri,
    const TDesC& aReportNamespace )
    {
    CNcdReportInstall* self = new( ELeave ) CNcdReportInstall( 
        aStatus, aReportManager );
    CleanupStack::PushL( self );
    self->ConstructL( 
        aContentIdentifier,
        aMetadataId,
        aReportUri,
        aReportNamespace );
    return self;
    }
        

// ---------------------------------------------------------------------------
// Create new CNcdReportInstall from the given stream
// ---------------------------------------------------------------------------
CNcdReportInstall* CNcdReportInstall::NewLC( 
    CNcdReportManager& aReportManager,
    RReadStream& aStream )
    {
    TNcdReportStatusInfo tmpStatus;
    CNcdReportInstall* self = new( ELeave ) CNcdReportInstall( 
        tmpStatus, aReportManager );
    CleanupStack::PushL( self );
    self->InternalizeL( aStream );
    return self;    
    }
    


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CNcdReportInstall::~CNcdReportInstall()
    {
    DLTRACEIN((""));
    }
    

// ---------------------------------------------------------------------------
// Overridden SetStatus
// ---------------------------------------------------------------------------
TInt CNcdReportInstall::SetStatus( const TNcdReportStatusInfo& aStatus )
    {
    DLTRACEIN((""));
    TInt err( CNcdReport::SetStatus( aStatus ) );
    
    DLTRACEOUT(("err: %d", err));
    return err;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CNcdReportInstall::CanBundle() const
    {
    return ETrue;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CNcdReportInstall::AddReportDataL( 
    CNcdRequestInstallation& aRequest ) const
    {
    DLTRACEIN((""));    
    aRequest.AddInstallDetailsL( 
        MetadataId().NodeId(),
        // convert status to actual status code
        ReportStatusToInstallReportStatus( Status() ) );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CNcdReportInstall::IsSendable() const
    {
    DLTRACEIN((""));
    TInt value = ReportStatusToInstallReportStatus( Status() );
    
    // Can send if status code is good and it's not being sent already.
    // Notice, that for now same constants are used here as in download report.
    return value != KNcdDownloadReportNotSupported && 
           !ReportTransaction() &&
           Attributes().AttributeInt32(
                ENcdReportAttributeSendable );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CNcdReportInstall::CanBeRemoved() const
    {
    DLTRACEIN((""));
    return Status().iStatus >= ENcdReportCancel && 
           Status().iStatus == Attributes().AttributeInt32( 
                ENcdReportAttributeLatestSentReport );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
HBufC8* CNcdReportInstall::CreateReportL() const
    {
    DLTRACEIN((""));
    CNcdRequestInstallation* report = 
        NcdRequestGenerator::CreateInstallationReportRequestLC();

    report->SetNamespaceL( Attributes().AttributeString16(
        ENcdReportAttributeReportNamespace ) );
    
    AddReportDataL( *report );    

    HBufC8* data = 
        ReportManager().GeneralManager().ProtocolManager().ProcessPreminetRequestL(
            ReportManager().Context(), 
            *report, 
            Attributes().AttributeString16( 
                ENcdReportAttributeReportUri ),
            ETrue );

    
    CleanupStack::PopAndDestroy( report );
    return data;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CNcdReportInstall::UpdateTransactionConfigL( 
    MCatalogsHttpOperation& aTransaction )
    {
    DLTRACEIN((""));
    aTransaction.Config().SetConnectionMethod( ConnectionMethod() );
    aTransaction.Config().SetPriority( ECatalogsPriorityLow );
    aTransaction.SetContentTypeL( KMimeTypeXml );
    }


// ---------------------------------------------------------------------------
// Returns ETrue if the status cannot change anymore
// ---------------------------------------------------------------------------
TBool CNcdReportInstall::StatusIsFinal() const
    {
    DLTRACEIN((""));
    return IsOneOf( 
        Status().iStatus, 
        ENcdReportCancel, 
        ENcdReportFail, 
        ENcdReportSuccess );    
    }


// ---------------------------------------------------------------------------
// PRIVATE METHODS
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
CNcdReportInstall::CNcdReportInstall( 
    const TNcdReportStatusInfo& aStatus,
    CNcdReportManager& aReportManager )
    : CNcdReport( ENcdReportInstall, aReportManager, aStatus )
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CNcdReportInstall::ConstructL( 
    const TDesC& aContentIdentifier,
    const CNcdNodeIdentifier& aMetadataId,
    const TDesC& aReportUri,
    const TDesC& aReportNamespace )
    {
    DLTRACEIN((""));
    BaseConstructL( aMetadataId );

    if ( !aReportUri.Length() || 
         !aReportNamespace.Length() )
        {
        DLERROR(("Either URI, report URI or report namespace is empty"));
        User::Leave( KErrArgument );
        }

    Attributes().SetAttributeL( 
        ENcdReportAttributeGenericId, 
        aContentIdentifier );

    Attributes().SetAttributeL( 
        ENcdReportAttributeReportUri, 
        aReportUri );

    Attributes().SetAttributeL( 
        ENcdReportAttributeReportNamespace, 
        aReportNamespace );

    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
TInt CNcdReportInstall::ReportStatusToInstallReportStatus( 
    const TNcdReportStatusInfo& aStatus ) const    
    {
    DLTRACEIN(( "aStatus: %d", aStatus.iStatus ));

    TInt status = 0;

    if ( ReportManager().ReportingStyle() 
         == MNcdServerReportManager::EReportingStyleS60 )
        {
        // Return the S60 error code.
        status = aStatus.iErrorCode;
        }
    else
        {
        // For now, use the same values as with the download.
        // If these are defined more accurately, then change them.
        switch ( aStatus.iStatus ) 
            {
            case ENcdReportCancel:
                {
                status = KNcdDownloadReportCancel;
                break;
                }

            case ENcdReportFail:
                {
                status = KNcdDownloadReportFail;
                break;
                }

            case ENcdReportSuccess:
                {
                status = KNcdDownloadReportSuccess;
                break;
                }

            // Unsupported cases
            case ENcdReportNone:
            case ENcdReportCreate:
            case ENcdReportStart:
            case ENcdReportPause:
            case ENcdReportResume:
                {
                DLTRACE(("Status not supported for install reporting"));
                status = KNcdDownloadReportNotSupported;
                break;
                }
                    
            default:            
                {            
                NCD_ASSERT_ALWAYS( 0, ENcdPanicIndexOutOfRange );
                break;
                }
            }        
        }
        
    DLTRACEOUT(("Install report status: %d", status));
    return status;
    }

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
* Description:   CNcdReportDownload implementation
*
*/


#include "ncdreportdownload.h"

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
CNcdReportDownload* CNcdReportDownload::NewLC(
    CNcdReportManager& aReportManager,
    const TDesC& aUri,
    const CNcdNodeIdentifier& aMetadataId,
    const TNcdReportStatusInfo& aStatus, 
    const TDesC& aReportUri,
    const TDesC& aReportNamespace )
    {
    CNcdReportDownload* self = new( ELeave ) CNcdReportDownload( 
        aStatus, aReportManager );
    CleanupStack::PushL( self );
    self->ConstructL( 
        aUri,
        aMetadataId,
        aReportUri,
        aReportNamespace );
    return self;
    }
        

// ---------------------------------------------------------------------------
// Create new CNcdReportDownload from the given stream
// ---------------------------------------------------------------------------
CNcdReportDownload* CNcdReportDownload::NewLC( 
    CNcdReportManager& aReportManager,
    RReadStream& aStream )
    {
    TNcdReportStatusInfo info;
    CNcdReportDownload* self = new( ELeave ) CNcdReportDownload( 
        info, aReportManager );
    CleanupStack::PushL( self );
    self->InternalizeL( aStream );
    return self;    
    }
    


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CNcdReportDownload::~CNcdReportDownload()
    {
    DLTRACEIN((""));
    }
    

// ---------------------------------------------------------------------------
// Overridden SetStatus
// ---------------------------------------------------------------------------
TInt CNcdReportDownload::SetStatus( const TNcdReportStatusInfo& aStatus )
    {
    DLTRACEIN((""));
    CNcdReport::SetStatus( aStatus );
    
    // Update timestamp for the current status
    TRAPD( err, UpdateReportTimeStampsL( aStatus ) );
    
    DLTRACEOUT(("err: %d", err));
    return err;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CNcdReportDownload::CanBundle() const
    {
    return ETrue;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CNcdReportDownload::AddReportDataL( 
    CNcdRequestInstallation& aRequest ) const
    {
    DLTRACEIN((""));
    
    // Calculate time from download start to current status
        
    aRequest.AddDownloadDetailsL( 
        MetadataId().NodeId(),
        // Content URI is stored as the generic id
        Attributes().AttributeString16( ENcdReportAttributeGenericId ),
        // convert status to actual status code
        ReportStatusToDownloadReportStatus( Status() ),
        // always report current total time
        TimeStamp( ENcdReportTimeTotal ) );
        
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CNcdReportDownload::IsSendable() const
    {
    DLTRACEIN((""));
    TInt value = ReportStatusToDownloadReportStatus( Status() );
    
    // Can send if status code is good and it's not being sent already
    return value != KNcdDownloadReportNotSupported && 
           !ReportTransaction() &&
           Attributes().AttributeInt32(
                ENcdReportAttributeSendable );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CNcdReportDownload::CanBeRemoved() const
    {
    DLTRACEIN((""));
    return Status().iStatus >= ENcdReportCancel && 
           Status().iStatus == Attributes().AttributeInt32( 
                ENcdReportAttributeLatestSentReport );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
HBufC8* CNcdReportDownload::CreateReportL() const
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
void CNcdReportDownload::UpdateTransactionConfigL( 
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
TBool CNcdReportDownload::StatusIsFinal() const
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
CNcdReportDownload::CNcdReportDownload( 
    const TNcdReportStatusInfo& aStatus,
    CNcdReportManager& aReportManager )
    : CNcdReport( ENcdReportDownload, aReportManager, aStatus )
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CNcdReportDownload::ConstructL( 
    const TDesC& aUri,
    const CNcdNodeIdentifier& aMetadataId,
    const TDesC& aReportUri,
    const TDesC& aReportNamespace )
    {
    DLTRACEIN((""));
    BaseConstructL( aMetadataId );

    if ( !aUri.Length() || 
         !aReportUri.Length() || 
         !aReportNamespace.Length() )
        {
        DLERROR(("Either URI, report URI or report namespace is empty"));
        User::Leave( KErrArgument );
        }
        
    Attributes().SetAttributeL( 
        ENcdReportAttributeGenericId, 
        aUri );
        
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
TInt CNcdReportDownload::ReportStatusToDownloadReportStatus( 
    const TNcdReportStatusInfo& aStatus ) const    
    {
    DLTRACEIN(("aStatus: %d", aStatus.iStatus ));

    TInt status = 0;

    if ( ReportManager().ReportingStyle() 
         == MNcdServerReportManager::EReportingStyleS60 )
        {
        // Return the S60 error code.
        status = aStatus.iErrorCode;
        }
    else
        {
        switch ( aStatus.iStatus ) 
            {
            case ENcdReportPause:
                {
                status = KNcdDownloadReportPause;
                break;
                }

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
            case ENcdReportResume:
                {
                DLTRACE(("Status not supported for download reporting"));
                status = KNcdDownloadReportNotSupported;
                break;
                }
                    
            default:            
                {            
                NCD_ASSERT_ALWAYS( 0, ENcdPanicIndexOutOfRange );
                }
            }
        }
        
    DLTRACEOUT(("Download report status: %d", status));
    return status;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CNcdReportDownload::UpdateReportTimeStampsL( 
    const TNcdReportStatusInfo& aStatus )
    {
    DLTRACEIN(( "aStatus: %d", aStatus.iStatus ));
    
    switch ( aStatus.iStatus ) 
        {
        case ENcdReportStart:
        case ENcdReportResume:
            {
            DLTRACE(("Start/resume"));
            SetCurrentTime( ENcdReportTimeStart );
            break;
            }
                
        case ENcdReportPause:
        case ENcdReportCancel:
        case ENcdReportFail:
        case ENcdReportSuccess:
            {
            DLTRACE(("Pause/cancel/fail/success"));
            // Get stop time and update total time
            SetCurrentTime( ENcdReportTimeStop );
            UpdateTotalTimeL();
            break;
            }

        // No time changes
        case ENcdReportNone:
        case ENcdReportCreate:                
            {
            DLTRACE(("Nothing to update"));
            break;
            }
                
        default:            
            {            
            NCD_ASSERT_ALWAYS( 0, ENcdPanicIndexOutOfRange );
            }
        };
    }



// ---------------------------------------------------------------------------
// Updates total time
// ---------------------------------------------------------------------------
void CNcdReportDownload::UpdateTotalTimeL() 
    {
    DLTRACEIN((""));
    TInt64 total( TimeStamp( ENcdReportTimeTotal ) );
    
    total += DurationInSecondsL( 
        ENcdReportTimeStart,
        ENcdReportTimeStop );    
        
    SetTimeStamp( ENcdReportTimeTotal, total );
    }

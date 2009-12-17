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
* Description:   CNcdReportOmaDownload implementation
*
*/


#include "ncdreportomadownload.h"
#include "ncdreportmanager.h"
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

#include "catalogsdebug.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CNcdReportOmaDownload* CNcdReportOmaDownload::NewLC(
    CNcdReportManager& aReportManager,
    const TDesC& aUri,
    const CNcdNodeIdentifier& aMetadataId,    
    const TNcdReportStatusInfo& aStatus,     
    const TDesC& aReportUri )
    {
    CNcdReportOmaDownload* self = new( ELeave ) CNcdReportOmaDownload( 
        aStatus, aReportManager );
    CleanupStack::PushL( self );
    self->ConstructL( 
        aUri,
        aMetadataId,
        aReportUri );
    return self;
    }
        

// ---------------------------------------------------------------------------
// Create new CNcdReportOmaDownload from the given stream
// ---------------------------------------------------------------------------
CNcdReportOmaDownload* CNcdReportOmaDownload::NewLC( 
    CNcdReportManager& aReportManager,
    RReadStream& aStream )
    {
    TNcdReportStatusInfo tmpStatus;
    CNcdReportOmaDownload* self = new( ELeave ) CNcdReportOmaDownload( 
        tmpStatus, aReportManager );
    CleanupStack::PushL( self );
    self->InternalizeL( aStream );
    return self;    
    }
    


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CNcdReportOmaDownload::~CNcdReportOmaDownload()
    {
    DLTRACEIN((""));
    }
    

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CNcdReportOmaDownload::IsSendable() const
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
TBool CNcdReportOmaDownload::CanBeRemoved() const
    {
    DLTRACEIN((""));
    return Status().iStatus >= ENcdReportCancel && 
           Status().iStatus == Attributes().AttributeInt32( 
                ENcdReportAttributeLatestSentReport );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
HBufC8* CNcdReportOmaDownload::CreateReportL() const
    {
    DLTRACEIN((""));
    
    TInt value = ReportStatusToDownloadReportStatus( Status() );
    HBufC8* data = NULL;
    
    switch ( value )
        {
        case KNcdDownloadReportSuccess:
            {
            DLTRACE(("Success report"));
            data = KNcdReportOmaSuccess().AllocL();
            break;
            }
        
        case KNcdDownloadReportCancel:
            {
            DLTRACE(("Cancel report"));
            data = KNcdReportOmaCancel().AllocL();
            break;
            }
        
        default:
            {
            DLERROR(("Unsupported case"));
            User::Leave( KErrNotSupported );
            }
        }
    
    return data;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CNcdReportOmaDownload::UpdateTransactionConfigL( 
    MCatalogsHttpOperation& aTransaction )
    {
    DLTRACEIN((""));
    aTransaction.Config().SetConnectionMethod( ConnectionMethod() );
    aTransaction.Config().SetPriority( ECatalogsPriorityLow );
    aTransaction.SetContentTypeL( KMimeTypeMatchTxt );
    }


// ---------------------------------------------------------------------------
// Returns ETrue if the status cannot change anymore
// ---------------------------------------------------------------------------
TBool CNcdReportOmaDownload::StatusIsFinal() const
    {
    DLTRACEIN((""));
    return IsOneOf( 
        Status().iStatus, 
        ENcdReportCancel, 
        ENcdReportSuccess );    
    }


// ---------------------------------------------------------------------------
// PRIVATE METHODS
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
CNcdReportOmaDownload::CNcdReportOmaDownload( 
    const TNcdReportStatusInfo& aStatus,
    CNcdReportManager& aReportManager )
    : CNcdReport( ENcdReportOmaDownload, aReportManager, aStatus )     
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CNcdReportOmaDownload::ConstructL( 
    const TDesC& aUri,
    const CNcdNodeIdentifier& aMetadataId,
    const TDesC& aReportUri )
    {
    DLTRACEIN((""));
    BaseConstructL( aMetadataId );
    
    if ( !aUri.Length() ||
         !aReportUri.Length() )
        {
        DLERROR(("Either URI or report URI is empty"));
        User::Leave( KErrArgument );
        }
    
    Attributes().SetAttributeL( 
        ENcdReportAttributeGenericId, 
        aUri );
        
    Attributes().SetAttributeL( 
        ENcdReportAttributeReportUri, 
        aReportUri );

    DLTRACEOUT((""));
    }



// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
TInt CNcdReportOmaDownload::ReportStatusToDownloadReportStatus( 
    const TNcdReportStatusInfo& aStatus ) const    
    {
    DLTRACEIN(("aStatus: %d", aStatus.iStatus ));

    // Here we always return the general error information instead
    // of S60 specific information. If this needs to be changed, then
    // see CNcdReportInstall::ReportStatusToInstallReportStatus
    // and CNcdReportDownload::ReportStatusToDownloadReportStatus 
    // for an example implementation.
    
    TInt status = 0;
    switch ( aStatus.iStatus ) 
        {
        
        case ENcdReportCancel:
            {
            status = KNcdDownloadReportCancel;
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
        case ENcdReportFail:        
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
        
    DLTRACEOUT(("Download report status: %d", status));
    return status;
    }



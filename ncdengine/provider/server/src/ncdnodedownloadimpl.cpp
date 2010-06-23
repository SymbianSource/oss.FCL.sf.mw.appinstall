/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements CNcdNodeDownload class
*
*/


#include <bautils.h>


#include "ncdnodedownloadimpl.h"
#include "catalogssession.h"
#include "catalogsbasemessage.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "ncdpurchasedetails.h"
#include "ncdutils.h"
#include "catalogsconstants.h"
#include "ncd_pp_dataentity.h"
#include "ncd_cp_query.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "ncdproviderutils.h"


CNcdNodeDownload::CNcdNodeDownload( NcdNodeClassIds::TNcdNodeClassId aClassId )
: CCatalogsCommunicable(),
  iClassId( aClassId )
    {
    }

void CNcdNodeDownload::ConstructL()
    {
    }


CNcdNodeDownload* CNcdNodeDownload::NewL()
    {
    CNcdNodeDownload* self =   
        CNcdNodeDownload::NewLC();
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeDownload* CNcdNodeDownload::NewLC()
    {
    CNcdNodeDownload* self = 
        new( ELeave ) CNcdNodeDownload( 
            NcdNodeClassIds::ENcdNodeDownloadClassId );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;        
    }


CNcdNodeDownload::~CNcdNodeDownload()
    {
    DLTRACEIN((""));

    // Delete member variables here.

    TInt count = iDownloadInfo.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CNcdPurchaseDownloadInfo* info =
            static_cast<CNcdPurchaseDownloadInfo*>(iDownloadInfo[i]);
        delete info;
        info = NULL;
        }
    iDownloadInfo.Reset();
    
    delete iFiles;

    DLTRACEOUT((""));
    }        

NcdNodeClassIds::TNcdNodeClassId CNcdNodeDownload::ClassId() const
    {
    return iClassId;
    }


// Internalization from the purchase history.

TBool CNcdNodeDownload::InternalizeL( const MNcdPurchaseDetails& aDetails )
    {
    DLTRACEIN(("this-ptr: %X", this));
    
    TArray<MNcdPurchaseDownloadInfo*> dlInfos( aDetails.DownloadInfoL() );
    
    TInt count = dlInfos.Count();    

    // Quit if no download data exists or it's consumable
    if ( count == 0 || ( count == 1 && 
         dlInfos[0]->ContentUsage() == 
         MNcdPurchaseDownloadInfo::EConsumable ))
        {
        // No download data found from purchase details.
        DLTRACEOUT((""));
        return EFalse;
        }

    // Check for any downloadable URIs
    TBool uriExists = EFalse;
    for ( TInt i = 0; i < count; ++i ) 
        {
        if ( dlInfos[i]->ContentUri().Length() ||
             dlInfos[i]->DescriptorUri().Length() ||
             dlInfos[i]->DescriptorData().Length() ||
             dlInfos[i]->RightsUri().Length() )
            {
            uriExists = ETrue;
            break;
            }
        }
    
    if ( !uriExists ) 
        {
        DLTRACEOUT(("No URI, no download API"));
        return EFalse;
        }
        
    iIsDownloaded = EFalse;
    
    delete iFiles;
    iFiles = NULL;
    iFiles = new (ELeave) CDesCArrayFlat( KListGranularity );
    
    RPointerArray<MNcdPurchaseDownloadInfo> tempInfos;

    // Can't use CleanupResetAndDestroyPushL because the array can't
    // be deleted with ResetAndDestroy
    TRAPD( err, 
        {
        tempInfos.ReserveL( count );
        DLTRACE(("Copying purchase download infos"));
        // Download data found from purchase details.
        for ( TInt i = 0; i < count; i++ )
            {
            CNcdPurchaseDownloadInfo* info =
                CNcdPurchaseDownloadInfo::NewLC( *dlInfos[i] );
            tempInfos.AppendL( info );
            CleanupStack::Pop( info );
            }

        // Check purchase details status.
        if ( aDetails.State() == MNcdPurchaseDetails::EStateDownloaded ||
             aDetails.State() == MNcdPurchaseDetails::EStateInstalled )
            {
            iIsDownloaded = ETrue;
            DLTRACE(("Copy file paths, iIsDownloaded = ETrue"));
            const MDesCArray& files( aDetails.DownloadedFiles() );
            
            for ( TInt i = 0; i < files.MdcaCount(); ++i )
                {
                iFiles->AppendL( files.MdcaPoint( i ) );
                }
            }

        ResetAndDestroyWithCast<CNcdPurchaseDownloadInfo>( iDownloadInfo );
        iDownloadInfo = tempInfos;
        
        });
        
    if ( err != KErrNone ) 
        {
        ResetAndDestroyWithCast<CNcdPurchaseDownloadInfo>( tempInfos );
        }
        
    DLTRACEOUT(("iIsDownloaded: %d", iIsDownloaded));
    return ETrue;
    }

const RPointerArray< MNcdPurchaseDownloadInfo >&
    CNcdNodeDownload::DownloadInfo() const
    {
    return iDownloadInfo;
    }


void CNcdNodeDownload::ReceiveMessage( MCatalogsBaseMessage* aMessage,
                                       TInt aFunctionNumber )
    {
    DLTRACEIN(("this-ptr: %X", this));    

    DASSERT( aMessage );
    
    // Now, we can be sure that rest of the time iMessage exists.
    // This member variable is set for the CounterPartLost function.
    iMessage = aMessage;
    
    TInt trapError( KErrNone );
    
    // Check which function is called by the proxy side object.
    // Function number are located in ncdnodefunctinoids.h file.
    switch( aFunctionNumber )
        {
        case NcdNodeFunctionIds::ENcdInternalize:
            // Internalize the proxy side according to the data
            // of this object.
            TRAP( trapError, InternalizeRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdRelease:
            // The proxy does not want to use this object anymore.
            // So, release the handle from the session.
            ReleaseRequest( *aMessage );
            break;

        case NcdNodeFunctionIds::ENcdFilesExist:
            TRAP( trapError, FilesExistRequestL( *aMessage ) );
            break;
        default:
            break;
        }

    if ( trapError != KErrNone )
        {
        // Because something went wrong, the complete has not been
        // yet called for the message.
        // So, inform the client about the error if the
        // message is still available.
        aMessage->CompleteAndRelease( trapError );
        }

    // Because the message should not be used after this, set it NULL.
    // So, CounterPartLost function will know that no messages are
    // waiting the response at the moment.
    iMessage = NULL;        
    
    DLTRACEOUT((""));
    }

void CNcdNodeDownload::CounterPartLost( const MCatalogsSession& aSession )
    {
    // This function may be called whenever -- when the message is waiting
    // response or when the message does not exist.
    // iMessage may be NULL here, because in the end of the
    // ReceiveMessage it is set to NULL. The life time of the message
    // ends shortly after CompleteAndRelease is called.
    if ( iMessage != NULL )
        {
        iMessage->CounterPartLost( aSession );
        }
    }
                

void CNcdNodeDownload::InternalizeRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );


    // Include all the necessary node data to the stream
    ExternalizeDataForRequestL( stream );     
    
    
    // Commits data to the stream when closing.
    CleanupStack::PopAndDestroy( &stream );


    // If this leaves, ReceiveMessage will complete the message.
    // NOTE: that here we expect that the buffer contains at least
    // some data. So, make sure that ExternalizeDataForRequestL inserts
    // something to the buffer.
    aMessage.CompleteAndReleaseL( buf->Ptr( 0 ), KErrNone );        
        
    
    DLTRACE(("Deleting the buf"));
    CleanupStack::PopAndDestroy( buf );
        
    DLTRACEOUT((""));
    }
    

void CNcdNodeDownload::ExternalizeDataForRequestL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    // Download existed. So, insert info that meta data was found.
    aStream.WriteInt32L( ClassId() );

    // Add additional content to the stream.
    // Make sure that this matches to the order that is used in the proxy
    // side when this stream is internalized.
    // NOTE: Be careful with the 8- and 16-bit descriptors. Remember to check
    // if the proxy wants the data in 16 or 8 bits?    

    aStream.WriteInt32L( iIsDownloaded );
        
    DLTRACEOUT(("iIsDownloaded: %d", iIsDownloaded ));
    }

void CNcdNodeDownload::ReleaseRequest( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));

    // Decrease the reference count for this object.
    // When the reference count reaches zero, this object will be destroyed
    // and removed from the session.
    MCatalogsSession& requestSession( aMessage.Session() );
    TInt handle( aMessage.Handle() );

    // Send complete information back to proxy.
    aMessage.CompleteAndRelease( KErrNone );
        
    // Remove this object from the session.
    requestSession.RemoveObject( handle );
        
    DLTRACEOUT((""));
    }


void CNcdNodeDownload::FilesExistRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));

    TBool filesExist = EFalse;
    if ( iIsDownloaded )
        {
        filesExist = FilesExist();
        }
    
    // If this leaves, ReceiveMessage will complete the message.
    // NOTE: that here we expect that the buffer contains at least
    // some data. So, make sure that ExternalizeDataForRequestL inserts
    // something to the buffer.
    aMessage.CompleteAndReleaseL( filesExist, KErrNone );        
        
            
    DLTRACEOUT(("filesExist: %d", filesExist));
    }
    


TBool CNcdNodeDownload::FilesExist()
    {
    DLTRACEIN((""));
    DASSERT( iFiles->MdcaCount() == iDownloadInfo.Count() );
    
    for ( TInt i = 0; i < iFiles->MdcaCount(); ++i )
        {        
        TInt shouldExist = 1;        
        
        // AttributeInt32L leaves only with KErrNone and that happens
        // if the attribute is not set which means that that download
        // is required
        TRAP_IGNORE( shouldExist = iDownloadInfo[i]->AttributeInt32L( 
            MNcdPurchaseDownloadInfo::EDownloadAttributeRequiredDownload ) );
        
        if ( !iDownloadInfo[i]->ContentUri().Length() ) 
            {
            DLTRACE(("No URI, no file"));
            shouldExist = EFalse;
            }
            
        DLTRACE(("Downloaded file should exist: %d", shouldExist));        
        
        // Dependencies and in Get-cases content file don't have to be
        // downloaded because they can already be installed. In those cases
        // shouldExist == 0 and this check is skipped
        if ( shouldExist && (
             iFiles->MdcaPoint( i ).Length() == 0 ||
             !BaflUtils::FileExists( 
                CNcdProviderUtils::FileSession(),
                iFiles->MdcaPoint( i ) ) ) )
            {            
            return EFalse;
            }            
        }
    return ETrue;
    }

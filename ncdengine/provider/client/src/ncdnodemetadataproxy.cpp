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
* Description:   Contains CNcdNodeMetadataProxy class implementation
*
*/


#include "ncdnodemetadataproxy.h"
#include "ncdnodeproxy.h"
#include "ncdnodemanagerproxy.h"
#include "ncdnodedisclaimerproxy.h"
#include "ncdnodedownloadproxy.h"
#include "ncdnodeiconproxy.h"
#include "ncdnodescreenshotproxy.h"
#include "ncdnodepurchaseproxy.h"
#include "ncdnodeuricontentproxy.h"
#include "ncdnodecontentinfoproxy.h"
#include "ncdnodeinstallproxy.h"
#include "ncdnodeactivateimpl.h"
#include "ncdnodepreviewproxy.h"
#include "ncdnodeupgradeproxy.h"
#include "ncdnodedependencyproxy.h"
#include "ncdnodeskinproxy.h"
#include "catalogsclientserver.h"
#include "ncdnodeidentifier.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "ncdnodeuserdataproxy.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsdebug.h"
#include "catalogsutils.h"
#include "ncdutils.h"
#include "ncderrors.h"


// ======== PUBLIC MEMBER FUNCTIONS ========

CNcdNodeMetadataProxy::CNcdNodeMetadataProxy( MCatalogsClientServer& aSession,
                                              TInt aHandle,
                                              CNcdNodeProxy& aNode ) 
: CNcdInterfaceBaseProxy( aSession, aHandle, &aNode ),
  iNode( aNode )
    {
    }


void CNcdNodeMetadataProxy::ConstructL()
    {
    // Register the interface
    MNcdNodeMetadata* metadata( this );
    DLTRACE(("Metadata-ptr: %X, this: %X", metadata, this));
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( metadata, this, MNcdNodeMetadata::KInterfaceUid ) );
    
    InternalizeL();
    }


CNcdNodeMetadataProxy* CNcdNodeMetadataProxy::NewL( MCatalogsClientServer& aSession,
                                                    TInt aHandle,
                                                    CNcdNodeProxy& aNode )
    {
    CNcdNodeMetadataProxy* self = 
        CNcdNodeMetadataProxy::NewLC( aSession, aHandle, aNode );
    CleanupStack::Pop( self );
    return self;
    }

CNcdNodeMetadataProxy* CNcdNodeMetadataProxy::NewLC( MCatalogsClientServer& aSession,
                                                     TInt aHandle,
                                                     CNcdNodeProxy& aNode )
    {
    CNcdNodeMetadataProxy* self = 
        new( ELeave ) CNcdNodeMetadataProxy( aSession, aHandle, aNode );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdNodeMetadataProxy::~CNcdNodeMetadataProxy()
    {
    DLTRACEIN(("Metadata: %X", this));
    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdNodeMetadata::KInterfaceUid );
    
    // Do not delete parent node. It is just a reference to the parent and the
    // parent owns this metadata, not vice versa.

    delete iIdentifier;
    iIdentifier = NULL;
    
    delete iName;
    iName = NULL;
    
    delete iDescription;
    iDescription = NULL;
    
    delete iLayoutType;
    iLayoutType = NULL;
    
    delete iActionName;
    iActionName = NULL;


    // Delete the objects that are owned by this class and provide functionality
    // through the api.
    // Notice that the api-objects are Released (not deleted) from the UI side.
    // This object owns the data and the reference counter of this object keeps
    // track of the api objects it owns. When the reference counter of this object
    // reaches zero, it means that nobody is using this object or the classes owned
    // by this object. If somebody is using the object owned by this object, 
    // the reference counter can not be zero until everything is released.
    // Thus, this object may delete the data here.

    delete iDisclaimer;
    iDisclaimer = NULL;

    DLTRACE(("Deleting iMoreInfo")); 
    delete iMoreInfo;
    iMoreInfo = NULL;

    DLTRACE(("Deleting iUserData")); 
    delete iUserData;
    iUserData = NULL;

    DLTRACE(("Deleting iDownload")); 
    delete iDownload;
    iDownload = NULL;

    DLTRACE(("Deleting iIcon")); 
    delete iIcon;
    iIcon = NULL;

    DLTRACE(("Deleting iScreenshot")); 
    delete iScreenshot;
    iScreenshot = NULL;

    DLTRACE(("Deleting iPurchase")); 
    delete iPurchase;
    iPurchase = NULL;

    DLTRACE(("Deleting iUriContent")); 
    delete iUriContent;
    iUriContent = NULL;
    
    DLTRACE(("Deleting iContentInfo")); 
    delete iContentInfo;
    iContentInfo = NULL;
    
    DLTRACE(("Delete install"));
    delete iInstall;
    iInstall = NULL;

    DLTRACE(("Deleting iPreview")); 
    delete iPreview;
    iPreview = NULL;

    DLTRACE(("Deleting iUpgrade")); 
    delete iUpgrade;
    iUpgrade = NULL;

    DLTRACE(("Deleting iDependency")); 
    delete iDependency;
    iDependency = NULL;
    
    DLTRACE(("Deleting iSkin")); 
    delete iSkin;
    iSkin = NULL;

    DLTRACE(("Deleting iActivate")); 
    delete iActivate;
    iActivate = NULL;
    
    DLTRACE(("Deleting iDetails"));    
    iDetails.ResetAndDestroy();    
    DLTRACEOUT(( "Metadata deleted, this: %x", this ));
    }

CNcdNodeIdentifier& CNcdNodeMetadataProxy::Identifier() const
    {
    return *iIdentifier;    
    }

CNcdNodeProxy& CNcdNodeMetadataProxy::Node() const
    {
    return iNode;
    }

CNcdNodeDownloadProxy* CNcdNodeMetadataProxy::Download() const
    {
    return iDownload;
    }


CNcdNodeInstallProxy* CNcdNodeMetadataProxy::Install() const
    {
    return iInstall;
    }


CNcdNodeUriContentProxy* CNcdNodeMetadataProxy::UriContent() const
    {
    return iUriContent;
    }


CNcdNodeContentInfoProxy* CNcdNodeMetadataProxy::ContentInfo() const
    {
    return iContentInfo;
    }


void CNcdNodeMetadataProxy::InternalizeL()
    {
    DLTRACEIN(("Metadata: %X", this));

    HBufC8* data( NULL );
        
    // Because we do not know the exact size of the data id, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    User::LeaveIfError(
            ClientServerSession().
                SendSyncAlloc( NcdNodeFunctionIds::ENcdInternalize,
                               KNullDesC8,
                               data,
                               Handle(),
                               0 ) );

    if ( data == NULL )
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PushL( data );


    // Read the data from the stream and insert it to the memeber variables
    RDesReadStream stream( *data );
    CleanupClosePushL( stream );
    
    InternalizeDataL( stream );
    
    // Closes the stream
    CleanupStack::PopAndDestroy( &stream ); 
    CleanupStack::PopAndDestroy( data );

    // These functions internalize objects that may exist
    // in the server side metadata.
    
    // The node should always contain user data object.
    DLINFO(("Internalize user data"));
    TRAP_IGNORE( InternalizeUserDataL() );    
    
    DLINFO(("Internalize icon"));
    TRAP_IGNORE( InternalizeIconL() );

    DLINFO(("Internalize content info"));    
    TRAP_IGNORE( InternalizeContentInfoL() );

    DLINFO(("Internalize upgrade target info"));
    TRAP_IGNORE( InternalizeUpgradeL() );

    DLINFO(("Internalize dependency target info"));
    TRAP_IGNORE( InternalizeDependencyL() );

    DLINFO(("Internalize preview"));    
    TRAP_IGNORE( InternalizePreviewL() );

    DLINFO(("Internalize node purchase"));    
    TRAP_IGNORE( InternalizePurchaseL() );
       
    DLINFO(("Internalize uri content"));    
    TRAP_IGNORE( InternalizeUriContentL() );

    DLINFO(("Internalize download"));
    TRAP_IGNORE( InternalizeDownloadL() );

    DLINFO(("Internalize install"));    
    TRAP_IGNORE( InternalizeInstallL() );

    DLINFO(("Internalize skin"));
    TRAP_IGNORE( InternalizeSkinL() );

    // Activate can exist only after install info has been gotten.
    DLINFO(("Internalize activate"));    
    TRAP_IGNORE( InternalizeActivateL() );    

    DLINFO(("Internalize screenshot"));
    TRAPD( err, InternalizeScreenshotL() );
    
    // Ignore errors caused by obsolete screenshot
    LeaveIfNotErrorL( err, KNcdErrorObsolete );
    
    DLTRACEOUT((""));    
    }


// MNcdNodeMetaData functions

const TDesC& CNcdNodeMetadataProxy::Id() const
    {
    return iIdentifier->NodeId();
    }
    
const TDesC& CNcdNodeMetadataProxy::Namespace() const
    {
    return iIdentifier->NodeNameSpace();
    }

    
const TDesC& CNcdNodeMetadataProxy::Name() const
    {
    DLTRACEIN(("Metadata: %X", this));
    DASSERT( iName );
    DLINFO(( _L("Name: %S, length: %d"), iName, 
        iName->Length() ));
    return *iName;
    }


const TDesC& CNcdNodeMetadataProxy::Description() const
    {
    DLTRACEIN(("Metadata: %X", this));
    DASSERT( iDescription );
    DLINFO(( _L("Description: %S, length: %d"), iDescription,
        iDescription->Length() ));
    return *iDescription;
    }
    
MNcdQuery* CNcdNodeMetadataProxy::Disclaimer() const
    {
    DLTRACEIN(("TotalRefCount: %d", TotalRefCount()));
    if( iDisclaimer != NULL )
        {
        iDisclaimer->AddRef();
        }
        
    DLTRACEOUT((""));
    return iDisclaimer;        
    }
    
MNcdQuery* CNcdNodeMetadataProxy::MoreInfo() const
    {
    DLTRACEIN(("TotalRefCount: %d", TotalRefCount()));
    if( iMoreInfo != NULL )
        {
        iMoreInfo->AddRef();
        }
        
    DLTRACEOUT((""));
    return iMoreInfo;        
    }
    
const RPointerArray<CNcdKeyValuePair>&
    CNcdNodeMetadataProxy::Details() const
    {
    DLTRACEIN((""));
    return iDetails;
    }
    
const TDesC& CNcdNodeMetadataProxy::ActionName() const
    {
    return *iActionName;    
    }


TBool CNcdNodeMetadataProxy::IsAlwaysVisible() const
    {
    DLTRACEIN((("isAlwaysVisible: %d"), iIsAlwaysVisible ))
    return iIsAlwaysVisible;
    }
    
const TDesC& CNcdNodeMetadataProxy::LayoutType() const 
    {
    return *iLayoutType;
    }


void CNcdNodeMetadataProxy::InternalizeDataL( RReadStream& aStream )
    {
    DLTRACEIN(("Metadata: %X", this));

    CNcdNodeIdentifier* tmpIdentifier( NULL );
    HBufC* tmpName( NULL );
    HBufC* tmpDescription( NULL );
    HBufC* tmpActionName( NULL );
    HBufC* tmpLayoutType( NULL );
    TBool tmpAlwaysVisible( EFalse );
    RPointerArray<CNcdKeyValuePair> details;
    CleanupResetAndDestroyPushL( details );

    // Get the id for the metadata class 
    TInt classId = aStream.ReadInt32L();

    if( classId != NcdNodeClassIds::ENcdNullObjectClassId )
        {
        DLINFO(("Metadata exists"));
        
        tmpIdentifier = CNcdNodeIdentifier::NewLC( aStream );
        
        InternalizeDesL( tmpName, aStream );
        CleanupStack::PushL( tmpName );

        InternalizeDesL( tmpDescription, aStream );
        CleanupStack::PushL( tmpDescription );
        
        InternalizeDesL( tmpLayoutType, aStream );
        CleanupStack::PushL( tmpLayoutType );
        
        tmpAlwaysVisible = aStream.ReadInt8L();
        
        TInt32 detailCount = aStream.ReadInt32L();
        for( TInt i = 0 ; i < detailCount ; i++ )
            {
            CNcdKeyValuePair* detail = CNcdKeyValuePair::NewLC( aStream );
            DLTRACE(( _L("Detail id=%S, value=%S"),
                &detail->Key() , &detail->Value() ));
            details.AppendL( detail );
            CleanupStack::Pop( detail );
            }

        // What is this?
        tmpActionName = KNullDesC().AllocLC();

        InternalizeDisclaimerL();
        InternalizeMoreInfoL();
        }
    else
        {
        DLINFO(("No meta data"));

        // Metadata has been created. But, during the internalization we found that it
        // does not actually exist in the server side. Maybe it has been removed.
        // Thus, leave here with KErrNotFound. So, metadata can be removed by the owner.
        User::Leave( KErrNotFound );
        }

    // Finally it is safe to insert data to the member variables

    delete iActionName;
    iActionName = tmpActionName;
    CleanupStack::Pop( tmpActionName );

    delete iLayoutType;
    iLayoutType = tmpLayoutType;
    CleanupStack::Pop( tmpLayoutType );

    delete iDescription;
    iDescription = tmpDescription;
    CleanupStack::Pop( tmpDescription );
        
    delete iName;
    iName = tmpName;
    CleanupStack::Pop( tmpName );

    delete iIdentifier;
    iIdentifier = tmpIdentifier;
    CleanupStack::Pop( tmpIdentifier );
    
    iIsAlwaysVisible = tmpAlwaysVisible;  
    
    CleanupStack::Pop( &details );
    iDetails.ResetAndDestroy();
    
    // Ownership of the array items is "transferred" to iDetails.
    iDetails = details;    

    DLTRACEOUT((""));
    }


void CNcdNodeMetadataProxy::InternalizeUserDataL()
    {
    DLTRACEIN((""));

    // Get the handle for the link data
    if ( iUserData == NULL )
        {
        DLINFO(("User data was not set. Set it now."));
        TInt userDataHandle( 0 );
        
        User::LeaveIfError(
                ClientServerSession().
                    SendSync( NcdNodeFunctionIds::ENcdUserDataHandle,
                              KNullDesC,
                              userDataHandle,
                              Handle() ) );    
        DLINFO(("Handle: %i", userDataHandle));

        // Now we can create the user data object
        // Notice that this object will add its own interfaces to the
        // querylist when the object is created.
        // Also, notice that if the proxy object leaves during the construction
        // the destructor automatically releases the handle from the server side.
        iUserData = CNcdNodeUserDataProxy::NewL( ClientServerSession(), 
                                                 userDataHandle,
                                                 *this );
        // Userdata ref counter is not increased here.
        // So, it will be zero until somebody asks for the userdata.
        // If the Release is called and
        // when the reference counter reaches zero, this node which
        // acts as parent will be deleted. The destructor of this
        // node will delete the userdata because this node owns it.
        }

    // Nothing else to do here, because user data will does not need
    // any other internalizing.

    DLTRACEOUT((""));
    }


void CNcdNodeMetadataProxy::InternalizeDisclaimerL()
    {
    DLTRACEIN((""));

    // Also update disclaimer information
    if ( iDisclaimer == NULL )
        {
        // Because disclaimer does not exist. It has to be created.
        // Get the handle from the server side metadata
        
        // Check if the metadata exists in the server side and update the information
        TInt disclaimerHandle( 0 );
            
        TInt handleError =
            ClientServerSession().
                SendSync( NcdNodeFunctionIds::ENcdDisclaimerHandle,
                          KNullDesC,
                          disclaimerHandle,
                          Handle() );


        if ( handleError == KErrNotFound )
            {
            DLINFO(("Disclaimer did not exist in server side"));
            
            // Because disclaimer did not exist in the server side
            // Nothing to do here.
            return;
            }
        else
            {
            // If error occurred then leave
            User::LeaveIfError( handleError );    
            }

        DLINFO(("Disclaimer existed"));

        // Now we can create the disclaimer object
        // Notice that this object will add its own interfaces to the
        // querylist when the object is created.
        iDisclaimer = CNcdNodeDisclaimerProxy::NewL( ClientServerSession(), 
                                                     disclaimerHandle,
                                                     this );
        }
    else       
        {
        // Notice that that if the proxy object was created above,
        // it internalizes itself during construction. So, call
        // the internalize here only if the object was not constructed
        // above.
        // Proxy object existed.
        // So, internalize it with new data.
        TRAPD( trapError, iDisclaimer->InternalizeL() );
        if ( trapError == KNcdErrorObsolete )
            {
            DLINFO(("Disclaimer was obsolete"));
            if ( iDisclaimer->MyTotalRefCount() == 0 )
                {
                DLINFO(("Total reference count is zero. So, delete object."))
                // Because total reference count is zero and the object is obsolete,
                // it should be deleted.
                delete iDisclaimer;
                }

            // Set disclaimer NULL because it has become obsolete.
            // It will be deleted automatically when the users release it
            // or it may have already been deleted above if nobody was using it.
            iDisclaimer = NULL;
            }
        else
            {
            // Let this function leave if there was another error.
            User::LeaveIfError( trapError );            
            }
        }    
        
    DLTRACEOUT((""));
    }

void CNcdNodeMetadataProxy::InternalizeMoreInfoL()
    {
    DLTRACEIN((""));

    if ( iMoreInfo == NULL )
        {
        // Because more info does not exist. It has to be created.
        // Get the handle from the server side metadata
        
        // Check if the metadata exists in the server side and update the information
        TInt moreInfoHandle( 0 );
            
        TInt handleError =
            ClientServerSession().
                SendSync( NcdNodeFunctionIds::ENcdMoreInfoHandle,
                          KNullDesC,
                          moreInfoHandle,
                          Handle() );


        if ( handleError == KErrNotFound )
            {
            DLINFO(("More info did not exist in server side"));
            
            // Because more info did not exist in the server side
            // Nothing to do here.
            return;
            }
        else
            {
            // If error occurred then leave
            User::LeaveIfError( handleError );    
            }

        DLINFO(("More info existed"));

        // Now we can create the disclaimer object
        // Notice that this object will add its own interfaces to the
        // querylist when the object is created.
        iMoreInfo = CNcdNodeDisclaimerProxy::NewL( ClientServerSession(), 
                                                   moreInfoHandle,
                                                   this );
        }
    else
        {
        // Notice that that if the proxy object was created above,
        // it internalizes itself during construction. So, call
        // the internalize here only if the object was not constructed
        // above.
        // Proxy object existed.
        // So, internalize it with new data.
        TRAPD( trapError, iMoreInfo->InternalizeL() );
        if ( trapError == KNcdErrorObsolete )
            {
            DLINFO(("More info was obsolete"));
            if ( iMoreInfo->MyTotalRefCount() == 0 )
                {
                DLINFO(("Total reference count is zero. So, delete object."))
                // Because total reference count is zero and the object is obsolete,
                // it should be deleted.
                delete iMoreInfo;
                }

            // Set object pointer to NULL because it has become obsolete.
            // It will be deleted automatically when the users release it
            // or it may have already been deleted above if nobody was using it.
            iMoreInfo = NULL;
            }
        else
            {
            // Let this function leave if there was another error.
            User::LeaveIfError( trapError );            
            }
        }    
        
    DLTRACEOUT((""));
    }

void CNcdNodeMetadataProxy::InternalizeDownloadL()
    {
    DLTRACEIN(("this-ptr: %x", this));
    
    if ( iDownload == NULL )
        {
        DLINFO(("Download did not exist"));

        // Check if the download exists in the server side and update the information
        TInt downloadHandle( 0 );
            
        TInt handleError =
            ClientServerSession().
                SendSync( NcdNodeFunctionIds::ENcdDownloadHandle,
                          KNullDesC,
                          downloadHandle,
                          Handle() );

        if ( handleError == KErrNotFound )
            {
            DLINFO(("Download did not exist in server side"));
            
            // Because download did not exist in the server side
            // Nothing to do here.
            return;
            }
        else
            {
            // If error occurred then leave
            User::LeaveIfError( handleError );    
            }

        DLINFO(("Download existed"));
                    
        // Now we can create the download object.
        // Notice that this object will add its own interfaces to the
        // querylist when the object is created.
        // Also, notice that if the proxy object leaves during the construction
        // the destructor automatically releases the handle from the server side.
        iDownload = CNcdNodeDownloadProxy::NewL( ClientServerSession(), 
                                                 downloadHandle,
                                                 *this );
        // Download ref counter is not increased here.
        // So, it will be zero until somebody asks for the download.
        // If the Release is called and
        // when the reference counter reaches zero, this object which
        // acts as parent will be deleted. The destructor of this
        // object will delete the download because this node owns it.
        }
    else
        {
        // Notice that that if the proxy object was created above,
        // it internalizes itself during construction. So, call
        // the internalize here only if the object was not constructed
        // above.
        // Proxy object existed.
        // So, internalize it with new data.
        iDownload->InternalizeL();
        }
        
    DLTRACEOUT((""));
    }


void CNcdNodeMetadataProxy::InternalizeUpgradeL()
    {
    DLTRACEIN((""));

    if ( iUpgrade == NULL )
        {
        DLINFO(("Upgrade info did not exist"));

        // The upgrade info info is part of the metadata.
        // Update the information if it exists.
        TInt upgradeHandle( 0 );
            
        TInt handleError =
            ClientServerSession().
                SendSync( NcdNodeFunctionIds::ENcdUpgradeHandle,
                          KNullDesC,
                          upgradeHandle,
                          Handle() );

        if ( handleError == KErrNotFound )
            {
            DLINFO(("Upgrade info did not exist in server side"));
            
            // Because upgrade did not exist in the server side
            // Nothing to do here.
            return;
            }
        else
            {
            // If error occurred then leave
            User::LeaveIfError( handleError );    
            }

        DLINFO(("Upgrade info existed in the server side"));
                    
        // Now we can create the upgrade info object.
        // Notice that this object will add its own interfaces to the
        // querylist when the object is created.
        // Also, notice that if the proxy object leaves during the construction
        // the destructor automatically releases the handle from the server side.
        iUpgrade = CNcdNodeUpgradeProxy::NewL( ClientServerSession(), 
                                               upgradeHandle,
                                               *this );
        // Upgrade ref counter is not increased here.
        // So, it will be zero until somebody asks for the upgrade info.
        // If the Release is called and
        // when the reference counter reaches zero, this object which
        // acts as parent will be deleted. The destructor of this
        // object will delete the upgrade info because this node owns it.
        }
    else       
        {
        // Notice that that if the proxy object was created above,
        // it internalizes itself during construction. So, call
        // the internalize here only if the object was not constructed
        // above.
        // Proxy object existed.
        // So, internalize it with new data.
        TRAPD( trapError, iUpgrade->InternalizeL() );
        if ( trapError == KNcdErrorObsolete )
            {
            DLINFO(("Upgrade was obsolete"));
            if ( iUpgrade->MyTotalRefCount() == 0 )
                {
                DLINFO(("Total reference count is zero. So, delete object."))
                // Because total reference count is zero and the object is obsolete,
                // it should be deleted.
                delete iUpgrade;
                }

            // Set object pointer to NULL because it has become obsolete.
            // It will be deleted automatically when the users release it
            // or it may have already been deleted above if nobody was using it.
            iUpgrade = NULL;
            }
        else
            {
            // Let this function leave if there was another error.
            User::LeaveIfError( trapError );            
            }
        }            
    DLTRACEOUT((""));    
    }


void CNcdNodeMetadataProxy::InternalizeDependencyL()
    {
    DLTRACEIN((""));

    if ( iDependency == NULL )
        {
        DLINFO(("Dependency info did not exist"));

        // The dependency info info is part of the metadata.
        // Update the information if it exists.
        TInt dependencyHandle( 0 );
            
        TInt handleError =
            ClientServerSession().
                SendSync( NcdNodeFunctionIds::ENcdDependencyHandle,
                          KNullDesC,
                          dependencyHandle,
                          Handle() );

        if ( handleError == KErrNotFound )
            {
            DLINFO(("Dependency info did not exist in server side"));
            
            // Because dependency did not exist in the server side
            // Nothing to do here.
            return;
            }
        else
            {
            // If error occurred then leave
            User::LeaveIfError( handleError );    
            }

        DLINFO(("Dependency info existed in the server side"));
                    
        // Now we can create the dependency info object.
        // Notice that this object will add its own interfaces to the
        // querylist when the object is created.
        // Also, notice that if the proxy object leaves during the construction
        // the destructor automatically releases the handle from the server side.
        iDependency = CNcdNodeDependencyProxy::NewL( ClientServerSession(), 
                                                     dependencyHandle,
                                                     *this );
        // Dependency ref counter is not increased here.
        // So, it will be zero until somebody asks for the dependency info.
        // If the Release is called and
        // when the reference counter reaches zero, this object which
        // acts as parent will be deleted. The destructor of this
        // object will delete the dependency info because this node owns it.
        }
    else       
        {
        // Notice that that if the proxy object was created above,
        // it internalizes itself during construction. So, call
        // the internalize here only if the object was not constructed
        // above.
        // Proxy object existed.
        // So, internalize it with new data.
        TRAPD( trapError, iDependency->InternalizeL() );
        if ( trapError == KNcdErrorObsolete )
            {
            DLINFO(("Dependency was obsolete"));
            if ( iDependency->MyTotalRefCount() == 0 )
                {
                DLINFO(("Total reference count is zero. So, delete object."))
                // Because total reference count is zero and the object is obsolete,
                // it should be deleted.
                delete iDependency;
                }

            // Set object pointer to NULL because it has become obsolete.
            // It will be deleted automatically when the users release it
            // or it may have already been deleted above if nobody was using it.
            iDependency = NULL;
            }
        else
            {
            // Let this function leave if there was another error.
            User::LeaveIfError( trapError );            
            }
        }            
    DLTRACEOUT((""));    
    }


void CNcdNodeMetadataProxy::InternalizeIconL()
    {
    DLTRACEIN((""));

    if ( iIcon == NULL )
        {
        DLINFO(("Icon did not exist"));

        // The icon info is part of the metadata.
        // Update the information if it exists.
        TInt iconHandle( 0 );
            
        TInt handleError =
            ClientServerSession().
                SendSync( NcdNodeFunctionIds::ENcdIconHandle,
                          KNullDesC,
                          iconHandle,
                          Handle() );

        if ( handleError == KErrNotFound )
            {
            DLINFO(("Icon did not exist in server side"));
            
            // Because icon did not exist in the server side
            // Nothing to do here.
            return;
            }
        else
            {
            // If error occurred then leave
            User::LeaveIfError( handleError );    
            }

        DLINFO(("Icon existed in the server side"));
                    
        // Now we can create the icon object.
        // Notice that this object will add its own interfaces to the
        // querylist when the object is created.
        // Also, notice that if the proxy object leaves during the construction
        // the destructor automatically releases the handle from the server side.
        iIcon = CNcdNodeIconProxy::NewL( ClientServerSession(), 
                                         iconHandle,
                                         *this );
        // Icon ref counter is not increased here.
        // So, it will be zero until somebody asks for the icon.
        // If the Release is called and
        // when the reference counter reaches zero, this object which
        // acts as parent will be deleted. The destructor of this
        // object will delete the icon because this object owns it.
        }
    else       
        {
        // Notice that that if the proxy object was created above,
        // it internalizes itself during construction. So, call
        // the internalize here only if the object was not constructed
        // above.
        // Proxy object existed.
        // So, internalize it with new data.
        TRAPD( trapError, iIcon->InternalizeL() );
        if ( trapError == KNcdErrorObsolete )
            {
            DLINFO(("Icon was obsolete"));
            if ( iIcon->MyTotalRefCount() == 0 )
                {
                DLINFO(("Total reference count is zero. So, delete object."))
                // Because total reference count is zero and the object is obsolete,
                // it should be deleted.
                delete iIcon;
                }

            // Set object pointer to NULL because it has become obsolete.
            // It will be deleted automatically when the users release it
            // or it may have already been deleted above if nobody was using it.
            iIcon = NULL;
            }
        else
            {
            // Let this function leave if there was another error.
            User::LeaveIfError( trapError );            
            }
        }            
    DLTRACEOUT((""));
    }

void CNcdNodeMetadataProxy::InternalizeScreenshotL()
    {
    DLTRACEIN((""));

    if ( iScreenshot == NULL )
        {
        DLINFO(("Screenshot did not exist"));

        // Check if the metadata exists in the server side.
        // The screenshot info is part of the metadata.
        // Update the information if it exists.
        TInt screenshotHandle( 0 );
            
        TInt handleError =
            ClientServerSession().
                SendSync( NcdNodeFunctionIds::ENcdScreenshotHandle,
                          KNullDesC,
                          screenshotHandle,
                          Handle() );

        if ( handleError == KErrNotFound )
            {
            DLINFO(("Screenshot did not exist in server side"));
            
            // Because screenshot did not exist in the server side.
            // Nothing to do here.
            return;
            }
        else
            {
            // If error occurred then leave
            User::LeaveIfError( handleError );    
            }

        DLINFO(("Screenshot existed in the server side"));
                    
        // Now we can create the screenshot object.
        // Notice that this object will add its own interfaces to the
        // querylist when the object is created.
        // Also, notice that if the proxy object leaves during the construction
        // the destructor automatically releases the handle from the server side.
        iScreenshot = 
            CNcdNodeScreenshotProxy::NewL( ClientServerSession(),
                                           screenshotHandle,
                                           *this );
        // Screenshot ref counter is not increased here.
        // So, it will be zero until somebody asks for the screenshot.
        // If the Release is called and
        // when the reference counter reaches zero, this object which
        // acts as parent will be deleted. The destructor of this
        // object will delete the screenshot because this node owns it.
        }
    else       
        {
        // Notice that that if the proxy object was created above,
        // it internalizes itself during construction. So, call
        // the internalize here only if the object was not constructed
        // above.
        // Proxy object existed.
        // So, internalize it with new data.
        TRAPD( trapError, iScreenshot->InternalizeL() );
        if ( trapError == KNcdErrorObsolete )
            {
            DLINFO(("Screenshot was obsolete"));
            if ( iScreenshot->MyTotalRefCount() == 0 )
                {
                DLINFO(("Total reference count is zero. So, delete object."))
                // Because total reference count is zero and the object is obsolete,
                // it should be deleted.
                delete iScreenshot;
                }

            // Set object pointer to NULL because it has become obsolete.
            // It will be deleted automatically when the users release it
            // or it may have already been deleted above if nobody was using it.
            iScreenshot = NULL;
            }
        else
            {
            // Let this function leave if there was another error.
            User::LeaveIfError( trapError );            
            }
        }            
    DLTRACEOUT((""));
    }

void CNcdNodeMetadataProxy::InternalizePurchaseL()
    {
    DLTRACEIN((""));

    TBool isPurchaseSupported = EFalse;
    // First check if this node should have purchase interface
    User::LeaveIfError( 
        ClientServerSession().
            SendSync( NcdNodeFunctionIds::ENcdIsPurchaseSupported,
                      KNullDesC,
                      isPurchaseSupported,
                      Handle() ) );
    
    if ( isPurchaseSupported && iPurchase == NULL )
        {
        DLINFO(("Purchase is supported -> create."));
        
        // Check if the metadata exists in the server side and update the information
        TInt metadataHandle( 0 );
            
        TInt handleError =
            ClientServerSession().
                SendSync( NcdNodeFunctionIds::ENcdMetadataHandle,
                          KNullDesC,
                          metadataHandle,
                          Node().Handle() );

        if ( handleError == KErrNotFound )
            {
            DLINFO(("Metadata did not exist in server side"));
            
            // Because metadata did not exist in the server side
            // Nothing to do here.
            return;
            }
        else
            {
            // If error occurred then leave
            User::LeaveIfError( handleError );    
            }        
        
        // Now we can create the purchase object.
        // Notice that if the proxy object leaves during the construction
        // the destructor automatically releases the handle from the server side.
        iPurchase = CNcdNodePurchaseProxy::NewL(
                        ClientServerSession(), 
                        metadataHandle,
                        *this );
        // Purchase ref counter is not increased here.
        // And when someone asks for the Purchase, its parent's (this)
        // ref-count is added.
        // If the Release is called and
        // when the reference counter reaches zero, this metadata which
        // acts as parent will be deleted. The destructor of this
        // metadata will delete the icon because this node owns it.
        }
    else if ( isPurchaseSupported && iPurchase )
        {
        DLINFO(("Purchase is supported -> internalize."));
        // Notice that that if the proxy object was created above,
        // it internalizes itself during construction. So, call
        // the internalize here only if the object was not constructed
        // above.
        // Proxy object existed.
        // So, internalize it with new data.
        iPurchase->InternalizeL();
        }
    else if ( !isPurchaseSupported && iPurchase )
        {
        DLTRACE(("Purchase not supported -> remove."));
        if ( iPurchase->MyTotalRefCount() == 0 )
                {
                DLINFO(("Total reference count is zero. So, delete object."))
                // Because total reference count is zero and the object is obsolete,
                // it should be deleted.
                delete iPurchase;
                }

            // Set object pointer to NULL because it has become obsolete.
            // It will be deleted automatically when the users release it
            // or it may have already been deleted above if nobody was using it.
            iPurchase = NULL;
        }
        
    DLTRACEOUT((""));
    }

void CNcdNodeMetadataProxy::InternalizeUriContentL()
    {
    DLTRACEIN((""));

    if ( iUriContent == NULL )
        {
        DLINFO(("uri content did not exist"));

        // Check if the metadata exists in the server side.
        // The uri content is part of the metadata.
        // Update the information if it exists.
        TInt uriHandle( 0 );
            
        TInt handleError =
            ClientServerSession().
                SendSync( NcdNodeFunctionIds::ENcdUriContentHandle,
                          KNullDesC,
                          uriHandle,
                          Handle() );

        if ( handleError == KErrNotFound )
            {
            DLINFO(("uri content did not exist in server side"));
            
            // Because uri content did not exist in the server side
            // Nothing to do here.
            return;
            }
        else
            {
            // If error occurred then leave
            User::LeaveIfError( handleError );    
            }

        DLINFO(("Uri content existed in the server side"));
                    
        // Now we can create the uri content object.
        // Notice that this object will add its own interfaces to the
        // querylist when the object is created.
        // Notice that if the proxy object leaves during the construction
        // the destructor automatically releases the handle from the server side.
        iUriContent = CNcdNodeUriContentProxy::NewL( ClientServerSession(), 
                                                     uriHandle,
                                                     *this );
        }
    else
        {
        // Notice that that if the proxy object was created above,
        // it internalizes itself during construction. So, call
        // the internalize here only if the object was not constructed
        // above.
        // Proxy object existed.
        // So, internalize it with new data.
        iUriContent->InternalizeL();
        }
        
    DLTRACEOUT((""));
    }

void CNcdNodeMetadataProxy::InternalizeContentInfoL()
    {
    DLTRACEIN((""));

    if ( iContentInfo == NULL )
        {
        DLINFO(("content info did not exist"));

        // Check if the metadata exists in the server side.
        // The content info is part of the metadata.
        // Update the information if it exists.
        TInt infoHandle( 0 );
            
        TInt handleError =
            ClientServerSession().
                SendSync( NcdNodeFunctionIds::ENcdContentInfoHandle,
                          KNullDesC,
                          infoHandle,
                          Handle() );

        if ( handleError == KErrNotFound )
            {
            DLINFO(("Content info did not exist in server side"));
            
            // Because content info did not exist in the server side
            // Nothing to do here.
            return;
            }
        else
            {
            // If error occurred then leave
            User::LeaveIfError( handleError );    
            }

        DLINFO(("Content info existed in the server side"));
                    
        // Now we can create the content info object.
        // Notice that this object will add its own interfaces to the
        // querylist when the object is created.
        // Also, notice that if the proxy object leaves during the construction
        // the destructor automatically releases the handle from the server side.
        iContentInfo = CNcdNodeContentInfoProxy::NewL( ClientServerSession(),
                                                       infoHandle,
                                                       *this );
        }
    else       
        {
        // Notice that that if the proxy object was created above,
        // it internalizes itself during construction. So, call
        // the internalize here only if the object was not constructed
        // above.
        // Proxy object existed.
        // So, internalize it with new data.
        TRAPD( trapError, iContentInfo->InternalizeL() );
        if ( trapError == KNcdErrorObsolete )
            {
            DLINFO(("Content info was obsolete"));
            if ( iContentInfo->MyTotalRefCount() == 0 )
                {
                DLINFO(("Total reference count is zero. So, delete object."))
                // Because total reference count is zero and the object is obsolete,
                // it should be deleted.
                delete iContentInfo;
                }

            // Set object pointer to NULL because it has become obsolete.
            // It will be deleted automatically when the users release it
            // or it may have already been deleted above if nobody was using it.
            iContentInfo = NULL;
            }
        else
            {
            // Let this function leave if there was another error.
            User::LeaveIfError( trapError );            
            }
        }            
    DLTRACEOUT((""));
    }

void CNcdNodeMetadataProxy::InternalizeInstallL()
    {
    DLTRACEIN((""));

    if ( iInstall == NULL )
        {
        DLINFO(("Node install was not set. Set it now."));
        TInt installHandle( 0 );
        
        // Get the handle for the install.
        TInt handleError =
                ClientServerSession().
                    SendSync( NcdNodeFunctionIds::ENcdInstallHandle,
                              KNullDesC,
                              installHandle,
                              Handle() );
                              
        if ( handleError == KErrNotFound )
            {
            DLINFO(("Install info did not exist in server side"));
            
            // Because content info did not exist in the server side
            // Nothing to do here.
            return;
            }
        else
            {
            // If error occurred then leave
            User::LeaveIfError( handleError );    
            }
                                  
        DLINFO(("Handle: %i", installHandle));

        // Now we can create the install object.
        // Notice that this object will add its own interfaces to the
        // querylist when the object is created.
        // Also, notice that if the proxy object leaves during the construction
        // the destructor automatically releases the handle from the server side.
        iInstall = CNcdNodeInstallProxy::NewL( ClientServerSession(), 
                                               installHandle,
                                               *this );
        }
    else
        {
        // Notice that that if the proxy object was created above,
        // it internalizes itself during construction. So, call
        // the internalize here only if the object was not constructed
        // above.
        // Proxy object existed.
        // So, internalize it with new data.
        iInstall->InternalizeL();
        }
        
    DLTRACEOUT((""));
    }

void CNcdNodeMetadataProxy::InternalizePreviewL()
    {
    DLTRACEIN((""));

    if ( iPreview == NULL )
        {
        DLINFO(("Preview did not exist"));

        TInt previewHandle( 0 );
            
        TInt handleError =
            ClientServerSession().
                SendSync( NcdNodeFunctionIds::ENcdPreviewHandle,
                          KNullDesC,
                          previewHandle,
                          Handle() );

        if ( handleError == KErrNotFound )
            {
            DLINFO(("Preview did not exist in server side"));
            
            // Because preview did not exist in the server side
            // Nothing to do here.
            return;
            }
        else
            {
            // If error occurred then leave
            User::LeaveIfError( handleError );    
            }

        DLINFO(("Preview existed in the server side"));
                    
        // Now we can create the preview object.
        // Notice that this object will add its own interfaces to the
        // querylist when the object is created.
        // Also, notice that if the proxy object leaves during the construction
        // the destructor automatically releases the handle from the server side.
        iPreview = CNcdNodePreviewProxy::NewL( ClientServerSession(), 
                                               previewHandle,
                                               *this );
        }
    else       
        {
        // Notice that that if the proxy object was created above,
        // it internalizes itself during construction. So, call
        // the internalize here only if the object was not constructed
        // above.
        // Proxy object existed.
        // So, internalize it with new data.
        TRAPD( trapError, iPreview->InternalizeL() );
        if ( trapError == KNcdErrorObsolete )
            {
            DLINFO(("Preview was obsolete"));
            if ( iPreview->MyTotalRefCount() == 0 )
                {
                DLINFO(("Total reference count is zero. So, delete object."))
                // Because total reference count is zero and the object is obsolete,
                // it should be deleted.
                delete iPreview;
                }

            // Set object pointer to NULL because it has become obsolete.
            // It will be deleted automatically when the users release it
            // or it may have already been deleted above if nobody was using it.
            iPreview = NULL;
            }
        else
            {
            // Let this function leave if there was another error.
            User::LeaveIfError( trapError );            
            }
        }            
    DLTRACEOUT((""));
    }


void CNcdNodeMetadataProxy::InternalizeSkinL()
    {
    DLTRACEIN((""));

    if( iSkin == NULL )
        {
        DLTRACE(("Skin did not exist"));

        TInt skinHandle( 0 );
            
        TInt handleError =
            ClientServerSession().
                SendSync( NcdNodeFunctionIds::ENcdSkinHandle,
                          KNullDesC,
                          skinHandle,
                          Handle() );

        if ( handleError == KErrNotFound )
            {
            DLTRACE(("Skin did not exist in server side"));
            
            // Because skin did not exist in the server side
            // Nothing to do here.
            return;
            }
        else
            {
            // If error occurred then leave
            User::LeaveIfError( handleError );    
            }

        DLTRACE(("Skin existed in the server side"));
                    
        // Now we can create the skin object.
        // Notice that this object will add its own interfaces to the
        // querylist when the object is created.
        iSkin = CNcdNodeSkinProxy::NewL( ClientServerSession(), 
                                         skinHandle,
                                         *this );
        // Skin ref counter is not increased here.
        // So, it will be zero until somebody asks for the skin.
        // If the Release is called and
        // when the reference counter reaches zero, this node which
        // acts as parent will be deleted. The destructor of this
        // node will delete the skin because this node owns it.
        }
    else       
        {
        // Notice that that if the proxy object was created above,
        // it internalizes itself during construction. So, call
        // the internalize here only if the object was not constructed
        // above.
        // Proxy object existed.
        // So, internalize it with new data.
        TRAPD( trapError, iSkin->InternalizeL() );
        if ( trapError == KNcdErrorObsolete )
            {
            DLINFO(("Skin was obsolete"));
            if ( iSkin->MyTotalRefCount() == 0 )
                {
                DLINFO(("Total reference count is zero. So, delete object."))
                // Because total reference count is zero and the object is obsolete,
                // it should be deleted.
                delete iSkin;
                }

            // Set object pointer to NULL because it has become obsolete.
            // It will be deleted automatically when the users release it
            // or it may have already been deleted above if nobody was using it.
            iSkin = NULL;
            }
        else
            {
            // Let this function leave if there was another error.
            User::LeaveIfError( trapError );            
            }
        }    
    DLTRACEOUT((""));
    }


void CNcdNodeMetadataProxy::InternalizeActivateL()
    {
    DLTRACEIN((""));

    if ( iInstall != NULL 
         && iInstall->IsInstalledL()
         && ( iInstall->IsPurpose( ENcdItemPurposeRingtone )
              || iInstall->IsPurpose( ENcdItemPurposeWallpaper )
              || iInstall->IsPurpose( ENcdItemPurposeTheme ) ) )
        {
        // Because install interface exists and something has been installed,
        // there may be something to activate. Note that items should be installed
        // before they can be activated.
        if ( iActivate == NULL )
            {
            // Create activate interface object only if it does not already exist.
            iActivate = CNcdNodeActivate::NewL( *this );        
            }
        }
    else if ( iActivate != NULL )
        {
        // Because install information did not exist or the item does not have
        // anything to activate, activate option should not exist either.
        // Destructor takes care of the removing the interface 
        // from the interface list.
        delete iActivate;
        iActivate = NULL;
        }

    DLTRACEOUT((""));
    }


    


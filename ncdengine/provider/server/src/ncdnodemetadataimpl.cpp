/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements CNcdNodeMetaData class
*
*/


#include "ncdnodemetadataimpl.h"
#include "ncdnodeidentifier.h"
#include "ncdnodedisclaimer.h"
#include "ncdnodeiconimpl.h"
#include "ncdnodescreenshotimpl.h"
#include "ncdnodeskinimpl.h"
#include "ncdnodeupgradeimpl.h"
#include "ncdnodedependencyimpl.h"
#include "ncdnodeuricontentimpl.h"
#include "ncdnodecontentinfoimpl.h"
#include "ncdnodepreviewimpl.h"
#include "ncdpurchaseoptionimpl.h"
#include "ncdpurchasedetails.h"
#include "ncdpurchasehistorydbimpl.h"
#include "ncdnodedownloadimpl.h"
#include "ncdnodeinstallimpl.h"
#include "ncdnodeuserdataimpl.h"
#include "catalogssession.h"
#include "catalogsbasemessage.h"
#include "ncdnodefunctionids.h"
#include "catalogsconstants.h"
#include "ncd_pp_dataentity.h"
#include "ncd_pp_dataentitycontent.h"
#include "ncd_cp_query.h"
#include "ncd_pp_download.h"
#include "ncd_pp_purchaseoption.h"
#include "ncdprotocoltypes.h"
#include "catalogsutils.h"
#include "ncdutils.h"
#include "ncdkeyvaluepair.h"
#include "ncdpanics.h"
#include "ncdserversubscribablecontent.h"
#include "ncdpurchasehistoryutils.h"
#include "ncderrors.h"

#include "catalogsdebug.h"


CNcdNodeMetaData::CNcdNodeMetaData(
    NcdNodeClassIds::TNcdNodeClassId aClassId,
    CNcdNodeManager& aNodeManager )
: CCatalogsCommunicable(),
  iClassId( aClassId ),
  iNodeManager( aNodeManager )  
    {
    DLTRACEIN(("Meta class id: %d", aClassId));
    }


void CNcdNodeMetaData::ConstructL( const CNcdNodeIdentifier& aIdentifier )
    {
    DLTRACEIN((""));

    // These two values has to be set. So, this metadata can be identified.
    iIdentifier = CNcdNodeIdentifier::NewL( aIdentifier );

    // The user data is part of the metadata. So, the ui may set its own info
    // for the node by using this object.
    iUserData = CNcdNodeUserData::NewL( *iIdentifier, iNodeManager );   
    
    iTimeStamp = KNullDesC().AllocL();
    iName = KNullDesC().AllocL();
    iDescription = KNullDesC().AllocL();
    iLayoutType = KNullDesC().AllocL();
    
    // This internalization is not related to purchase history so it's done
    // separately
    TRAP_IGNORE( InternalizeInstallFromContentInfoL() );
    

    TRAPD( phError,
        {
        // Get purchase details
        CNcdPurchaseDetails* details = PurchaseDetailsLC();
        
        InternalizeContentInfoL( *details );
        
        // Try to internalize URI content from purchase history
        InternalizeUriContentL( *details );                    
        
        InternalizeDependencyL( *details );
        // Try to internalize node download from purchase history.
        InternalizeDownloadL( *details );
        
        // Try to internalize node install from purchase history
        InternalizeInstallL( *details );
        
        // Try to internalize node icon from purchase history
        // (does not read icon data to memory)
        InternalizeIconL( *details );
    
        CleanupStack::PopAndDestroy( details );
        });
    
    if( phError != KErrNone && 
	phError != KErrNotFound &&
	phError != KNcdErrorNoPurchaseInformation )
        {
        DLERROR(( "phError: %d", phError ));
        User::Leave( phError );
        }

    DLTRACEOUT((""));
    }


CNcdNodeMetaData::~CNcdNodeMetaData()
    {
    DLTRACEIN((""));
    
    delete iIdentifier;
    iIdentifier = NULL;
        
    delete iTimeStamp;
    iTimeStamp = NULL;

    delete iName;
    iName = NULL;

    delete iDescription;
    iDescription = NULL;
    
    delete iLayoutType;
    iLayoutType = NULL;
            
    // Notice that CCatalogsCommunicable classes cannot be destroyed by
    // calling delete! Instead call Close to them

    if ( iUserData )
        {
        DLINFO(("Closing node user data"));
        iUserData->Close();       
        iUserData = NULL; 
        }

    if ( iDisclaimer != NULL )
        {
        iDisclaimer->Close();
        iDisclaimer = NULL;
        }
    
    if ( iIcon != NULL )
        {
        iIcon->Close();
        iIcon = NULL;        
        }

    if ( iScreenshot != NULL )
        {
        iScreenshot->Close();
        iScreenshot = NULL;
        }

    if ( iSkin != NULL )
        {
        iSkin->Close();
        iSkin = NULL;        
        }

    if( iUriContent != NULL )
        {
        iUriContent->Close();
        iUriContent = NULL;
        }

    if( iContentInfo != NULL )
        {
        iContentInfo->Close();
        iContentInfo = NULL;
        }

    if( iPreview != NULL )
        {
        iPreview->Close();
        iPreview = NULL;
        }

    if ( iUpgrade != NULL )
        {
        iUpgrade->Close();
        iUpgrade = NULL;
        }

    if ( iDependency != NULL )
        {
        iDependency->Close();
        iDependency = NULL;
        }

    if ( iDownload )
        {
        DLINFO(("Closing node download"));
        iDownload->Close();
        iDownload = NULL;
        }

    if ( iInstall )
        {
        DLINFO(("Closing node install"));
        iInstall->Close();
        iInstall = NULL;
        }
    
    if ( iMoreInfo != NULL )
        {
        iMoreInfo->Close();
        iMoreInfo = NULL;
        }
    
    iDetails.ResetAndDestroy();
    
    delete iSubscribableContent;
    iSubscribableContent = NULL;
    
    ResetAndCloseArray( iPurchaseOptions );
    
    DLTRACEOUT((""));
    }        
    

CNcdNodeManager& CNcdNodeMetaData::NodeManager() const
    {
    return iNodeManager;
    }


const CNcdNodeIdentifier& CNcdNodeMetaData::Identifier() const
    {
    return *iIdentifier;
    }


NcdNodeClassIds::TNcdNodeClassId CNcdNodeMetaData::ClassId() const
    {
    return iClassId;
    }    
    
const TDesC& CNcdNodeMetaData::TimeStamp() const
    {
    DASSERT( iTimeStamp );
    return *iTimeStamp;
    }


const TDesC& CNcdNodeMetaData::NodeName() const
    {
    DASSERT( iName );
    return *iName;
    }

void CNcdNodeMetaData::SetNodeNameL( const TDesC& aName )
    {
    HBufC* newName = aName.AllocL();
    delete iName;
    iName = newName;
    }

const TDesC& CNcdNodeMetaData::Description() const
    {
    DASSERT( iDescription );
    return *iDescription;    
    }
    
void CNcdNodeMetaData::SetDescriptionL(
    const TDesC& aDescription ) 
    {
    HBufC* newDescription = aDescription.AllocL();
    delete iDescription;
    iDescription = newDescription;
    }
    
const TDesC& CNcdNodeMetaData::LayoutType() const 
    {
    DASSERT( iLayoutType );
    return *iLayoutType;
    }
    
const CNcdNodeDisclaimer& CNcdNodeMetaData::DisclaimerL() const
    {
    if ( iDisclaimer == NULL )
        {
        User::Leave( KErrNotFound );
        }
        
    return *iDisclaimer;
    }

void CNcdNodeMetaData::SetDisclaimer( CNcdNodeDisclaimer* aDisclaimer )
    {
    if ( iDisclaimer != NULL )
        {
        iDisclaimer->Close();        
        }
    iDisclaimer = aDisclaimer;
    }


const CNcdNodeDisclaimer& CNcdNodeMetaData::MoreInfoL() const
    {
    if ( iMoreInfo == NULL )
        {
        User::Leave( KErrNotFound );
        }
        
    return *iMoreInfo;
    }

CNcdNodeIcon& CNcdNodeMetaData::IconL() const
    {
    if ( iIcon == NULL )
        {
        User::Leave( KErrNotFound );
        }

    return *iIcon;
    }


void CNcdNodeMetaData::SetIcon( CNcdNodeIcon* aIcon ) 
    {
    if ( iIcon ) 
        {
        iIcon->Close();
        }
    iIcon = aIcon;
    }
  
    
const CNcdNodeScreenshot& CNcdNodeMetaData::ScreenshotL() const
    {
    if ( iScreenshot == NULL )
        {
        User::Leave( KErrNotFound );
        }

    return *iScreenshot;
    }
    
    
const CNcdNodeSkin& CNcdNodeMetaData::SkinL() const
    {
    DASSERT( iSkin );
    return *iSkin;
    }


CNcdNodePreview& CNcdNodeMetaData::PreviewL() const
    {
    if ( iPreview == NULL )
        {
        User::Leave( KErrNotFound );
        }

    return *iPreview;
    }
    

const CNcdNodeUpgrade& CNcdNodeMetaData::UpgradeL() const
    {
    if ( iUpgrade == NULL )
        {
        User::Leave( KErrNotFound );
        }
    return *iUpgrade;
    }


const CNcdNodeDependency& CNcdNodeMetaData::DependencyL() const
    {
    if ( iDependency == NULL )
        {
        User::Leave( KErrNotFound );
        }
    return *iDependency;
    }


const CNcdNodeContentInfo& CNcdNodeMetaData::ContentInfoL() const
    {
    if ( iContentInfo == NULL )
        {
        User::Leave( KErrNotFound );      
        }
    return *iContentInfo;
    }


const CNcdServerSubscribableContent*
    CNcdNodeMetaData::SubscribableContent() const
    {
    return iSubscribableContent;
    }


const RPointerArray<CNcdPurchaseOptionImpl>& CNcdNodeMetaData::PurchaseOptions() const
    {
    return iPurchaseOptions;    
    }

CNcdPurchaseOptionImpl& CNcdNodeMetaData::PurchaseOptionByIdL(
    const TDesC& aPurchaseOptionId ) const 
    {
    DLTRACEIN((""));
    for ( TInt i = 0; i < iPurchaseOptions.Count(); i++ ) 
        {
        if ( iPurchaseOptions[i]->Id() == aPurchaseOptionId ) 
            {
            return *iPurchaseOptions[i];
            }
        }
        
    User::Leave( KErrNotFound );
    CNcdPurchaseOptionImpl* foo( NULL );
    return *foo;
    }  
    
TBool CNcdNodeMetaData::AlwaysVisible() const 
    {
    return iAlwaysVisible;
    }
  
    
void CNcdNodeMetaData::SetAlwaysVisible( TBool aValue ) 
    {
    iAlwaysVisible = aValue;
    }
 
    
void CNcdNodeMetaData::InternalizeL( MNcdPreminetProtocolDataEntity& aData )
    {
    DLTRACEIN((""));

    // First create the new values
    HBufC* tmpTimeStamp = aData.Timestamp().AllocLC();
    HBufC* tmpName = aData.Name().AllocLC();
    HBufC* tmpDescription = aData.Description().AllocLC();
    HBufC* tmpLayoutType = aData.LayoutType().AllocLC();


    DLTRACE(( _L("MetaData timestamp: %S"), tmpTimeStamp ));
    DLTRACE(( _L("MetaData name: %S"), tmpName ));
    DLTRACE(( _L("MetaData description: %S"), tmpDescription ));


    delete iLayoutType;
    iLayoutType = tmpLayoutType;
    CleanupStack::Pop( tmpLayoutType );

    delete iDescription;
    iDescription = tmpDescription;
    CleanupStack::Pop( tmpDescription );

    delete iName;
    iName = tmpName;
    CleanupStack::Pop( tmpName );    

    delete iTimeStamp;
    iTimeStamp = tmpTimeStamp;
    CleanupStack::Pop( tmpTimeStamp );

    if ( aData.Disclaimer() )
        {
        // New disclaimer info should be set
        
        if ( iDisclaimer == NULL )
            {
            // Create disclaimer because it did not exist before.
            iDisclaimer = CNcdNodeDisclaimer::NewL();
            }
        // Update disclaimer info.
        iDisclaimer->InternalizeL( *aData.Disclaimer() );        
        }
    else if ( iDisclaimer )
        {
        // Because new data does not contain disclaimer.
        // Close old one and set the value NULL.
        // Notice that CCatalogsCommunicalbe classes should be Closed
        // instead of deleting.
        // Because this object may still be left hanging for proxy object if it is used by UI,
        // set the object obsolete. So, UI will know this if it is trying to internalize 
        // the hanging object.
        iDisclaimer->SetAsObsolete( ETrue );
        iDisclaimer->Close();
        iDisclaimer = NULL;                    
        }

    if ( aData.Icon() )
        {
        // New icon info should be set.
        
        if ( iIcon == NULL )
            {
            iIcon = CNcdNodeIcon::NewL( iNodeManager, *this );            
            }
        iIcon->InternalizeL( aData );
        }
    else if ( iIcon != NULL )
        {
        // Notice that icons should not be deleted because
        // all the CCatalogsCommunicable classes should be Closed instead.
        // Because this object may still be left hanging for proxy object if it is used by UI,
        // set the object obsolete. So, UI will know this if it is trying to internalize 
        // the hanging object.
        iIcon->SetAsObsolete( ETrue );
        iIcon->Close();
        iIcon = NULL;            
        }

    if ( aData.ScreenshotCount() > 0 )
        {
        // New screenshot info should be set.
        
        if ( iScreenshot == NULL )
            {
            iScreenshot = CNcdNodeScreenshot::NewL( iNodeManager, *this );            
            }
        iScreenshot->InternalizeL( aData );
        }
    else if ( iScreenshot != NULL )
        {
        // Notice that screenshots should not be deleted because
        // all the CCatalogsCommunicable classes should be Closed instead.
        // Because this object may still be left hanging for proxy object if it is used by UI,
        // set the object obsolete. So, UI will know this if it is trying to internalize 
        // the hanging object.
        iScreenshot->SetAsObsolete( ETrue );
        iScreenshot->Close();
        iScreenshot = NULL;            
        }

    if ( aData.Skin() )
        {
        // New skin info should be set.

        if ( iSkin == NULL )
            {
            iSkin = CNcdNodeSkin::NewL();            
            }
        iSkin->InternalizeL( aData );
        }
    else if ( iSkin != NULL )
        {
        // Notice that skins should not be deleted because
        // all the CCatalogsCommunicable classes should be Closed instead.
        // Because this object may still be left hanging for proxy object if it is used by UI,
        // set the object obsolete. So, UI will know this if it is trying to internalize 
        // the hanging object.
        iSkin->SetAsObsolete( ETrue );
        iSkin->Close();
        iSkin = NULL;            
        }

    if ( aData.DownloadableContent() )
        {
        // New content info should be set.

        if ( iContentInfo == NULL )
            {
            iContentInfo = CNcdNodeContentInfo::NewL();            
            }
        iContentInfo->InternalizeL( aData );
        InternalizeInstallFromContentInfoL();
        }
    else if ( iContentInfo != NULL )
        {
        DLINFO(("No downloadable content"));
        // Notice that content info should not be deleted because
        // all the CCatalogsCommunicable classes should be Closed instead.
        // Because this object may still be left hanging for proxy object if it is used by UI,
        // set the object obsolete. So, UI will know this if it is trying to internalize 
        // the hanging object.
        iContentInfo->SetAsObsolete( ETrue );
        iContentInfo->Close();
        iContentInfo = NULL;            
        }

	if ( aData.PreviewCount() > 0 )
        {
        if ( iPreview == NULL )
            {
            iPreview = CNcdNodePreview::NewL( *this, iNodeManager );            
            }
        iPreview->InternalizeL( aData );
        }
    else if ( iPreview != NULL )
        {
        // Because this object may still be left hanging for proxy object if it is used by UI,
        // set the object obsolete. So, UI will know this if it is trying to internalize 
        // the hanging object.
        iPreview->SetAsObsolete( ETrue );
        iPreview->Close();
        iPreview = NULL;            
        }

    // Create upgrade object if necessary
    if ( aData.DownloadableContent() != NULL )
        {
        if ( iUpgrade == NULL )
            {
            iUpgrade = CNcdNodeUpgrade::NewL( *this );
            }
        TPtrC version( KNullDesC );    
        if ( iContentInfo ) 
            {
            version.Set( iContentInfo->Version() );
            }
        TRAPD( upgradeErr, iUpgrade->InternalizeL( aData, version ) );
        TBool upgradeExists = HandleContentUpgradeL();
        
        // HandleContentUpgradeL checks CNcdNodeContentInfo and
        // CNcdNodeInstall for the need of upgrade interfaces
        if ( upgradeErr == KErrNotFound && 
             !upgradeExists )
            {
            DLTRACE(("No upgrade"));                
            // The given data did not contain any information about upgrade.
            // So, delete the created upgrade object.
            // Because this object may still be left hanging for proxy object if it is used by UI,
            // set the object obsolete. So, UI will know this if it is trying to internalize 
            // the hanging object.
            iUpgrade->SetAsObsolete( ETrue );
            iUpgrade->Close();
            iUpgrade = NULL;            
            }
        
        LeaveIfNotErrorL( upgradeErr, KErrNotFound );
        }
    else if ( iUpgrade != NULL )
        {
        // Because this object may still be left hanging for proxy object if it is used by UI,
        // set the object obsolete. So, UI will know this if it is trying to internalize 
        // the hanging object.
        iUpgrade->SetAsObsolete( ETrue );
        iUpgrade->Close();
        iUpgrade = NULL;
        }


    // Create dependency object if necessary
    if ( aData.DownloadableContent() != NULL )
        {
        if ( iDependency == NULL )
            {
            iDependency = CNcdNodeDependency::NewL( *this );
            }
        TRAPD( dependencyErr, iDependency->InternalizeL( aData ) );
        if ( dependencyErr == KErrNotFound )
            {
            DLTRACE(("No dependency, deleting the object"));
            // The given data did not contain any information about dependency.
            // So, delete the created object.
            // Because this object may still be left hanging for proxy object if it is used by UI,
            // set the object obsolete. So, UI will know this if it is trying to internalize 
            // the hanging object.
            iDependency->SetAsObsolete( ETrue );
            iDependency->Close();
            iDependency = NULL;            
            }
        else if ( dependencyErr != KErrNone )
            {
            // Some error occurred. So let this leave.
            User::Leave( dependencyErr );
            }
        }
    else if ( iDependency != NULL )
        {
        DLTRACE(("Removing old dependency"));
        // Because this object may still be left hanging for proxy object if it is used by UI,
        // set the object obsolete. So, UI will know this if it is trying to internalize 
        // the hanging object.
        iDependency->SetAsObsolete( ETrue );
        iDependency->Close();
        iDependency = NULL;
        }
        


    // Subscribable content
    const MNcdPreminetProtocolDataEntityContent* subscribableContent =
        aData.SubscribableContent();
    
    if ( subscribableContent != NULL )
        {
        DLINFO(( "Subscribable content element found from protocol-object" ));
        
        if ( iSubscribableContent == NULL )
            {
            iSubscribableContent = CNcdServerSubscribableContent::NewL();
            }        
        iSubscribableContent->InternalizeL( *subscribableContent );
        }
        
        
    if ( aData.MoreInfo() )
        {
        if ( iMoreInfo == NULL )
            {
            iMoreInfo = CNcdNodeDisclaimer::NewL();
            }
        iMoreInfo->InternalizeL( *aData.MoreInfo() );        
        }
    else if ( iMoreInfo )
        {
        // Because this object may still be left hanging for proxy object if it is used by UI,
        // set the object obsolete. So, UI will know this if it is trying to internalize 
        // the hanging object.
        iMoreInfo->SetAsObsolete( ETrue );
        iMoreInfo->Close();
        iMoreInfo = NULL;                    
        }
        
    iDetails.ResetAndDestroy();        
        
    for ( TInt i = 0 ; i < aData.DetailCount() ; i++ )
        {
        DLTRACE(( _L("Detail id=%S, value=%S"),
            &aData.DetailL( i ).Id(), &aData.DetailL( i ).Value()));
        if( aData.DetailL( i ).Id() == KNullDesC )
            {
            DLTRACE(("Empty id -> not adding detail!"));
            continue;
            }
        CNcdKeyValuePair* detail = CNcdKeyValuePair::NewLC(
            aData.DetailL( i ).Id(), aData.DetailL( i ).Value() );
        iDetails.AppendL( detail );
        CleanupStack::Pop( detail );
        }

    
    // Create or reinternalize purchase options :
    
    const TInt KPurchaseOptionCount( aData.PurchaseOptionCount() );

    DLINFO(( "Amount of purchaseoptions found from protocol-object: %d",
             KPurchaseOptionCount ));
    
    TInt purchaseOptionIndex( 0 );
    while ( purchaseOptionIndex < KPurchaseOptionCount )
        {
        const MNcdPreminetProtocolPurchaseOption& tmpOption = 
            aData.PurchaseOptionL( purchaseOptionIndex );            
        InternalizePurchaseOptionL( tmpOption );        
        ++purchaseOptionIndex;
        }
    
    // Remove purchase options that were removed from the server
    RemoveNotUpdatedPurchaseOptions();

    DLTRACEOUT((""));
    }


void CNcdNodeMetaData::InternalizeContentInfoL( 
    const MNcdPurchaseDetails& aDetails )
    {
    DLTRACEIN((""));
    
    if ( !iContentInfo )
        {
        iContentInfo = CNcdNodeContentInfo::NewL();    
        }
    
    iContentInfo->InternalizeL( aDetails );    
    }


void CNcdNodeMetaData::InternalizeUriContentL( 
    const MNcdPurchaseDetails& aDetails )
    {
    DLTRACEIN((""));
    
    if ( !iUriContent )
        {
        iUriContent = CNcdNodeUriContent::NewL();    
        if ( !iUriContent->InternalizeL( aDetails ) )
            {
            iUriContent->Close();
            iUriContent = NULL;
            }
        }
    else
        {
        iUriContent->InternalizeL( aDetails );
        }
        
    DLTRACEOUT((""));    
    }


CNcdNodeDownload& CNcdNodeMetaData::DownloadL()
    {
    if( iDownload == NULL )
        {
        User::Leave( KErrNotFound );
        }
          
    return *iDownload;
    }


void CNcdNodeMetaData::InternalizeDownloadL( const MNcdPurchaseDetails& aDetails )
    {
    DLTRACEIN((""));    

    if ( !iDownload ) 
        {
        iDownload = CNcdNodeDownload::NewL();
        
        // Delete node download if the internalization failed
        if ( !iDownload->InternalizeL( aDetails ) ) 
            {
            iDownload->Close();
            iDownload = NULL;
            }
        }
    else
        {
        iDownload->InternalizeL( aDetails );
        }
    DLTRACEOUT((""));
    }


CNcdNodeInstall& CNcdNodeMetaData::InstallL()
    {
    if( iInstall == NULL )
        {
        User::Leave( KErrNotFound );
        }
          
    return *iInstall;
    }
    

void CNcdNodeMetaData::InternalizeInstallL( const MNcdPurchaseDetails& aDetails )
    {
    DLTRACEIN((""));
    
    if ( !iInstall )
        {
        iInstall = CNcdNodeInstall::NewL( *this );    
        if ( !iInstall->InternalizeL( aDetails ) )
            {
            iInstall->Close();
            iInstall = NULL;
            }
        }
    else
        {
        iInstall->InternalizeL( aDetails );
        }
    
    // Get version of bought content from purchase history so that
    // we can compare it with content info in HandleContentUpgradeL
    TRAPD( err, TCatalogsVersion::ConvertL( 
        iBoughtContentVersion, aDetails.Version() ) );
        
    LeaveIfNotErrorL( err, KErrArgument, KErrGeneral );

    DLTRACEOUT((""));    
    }
    
    
void CNcdNodeMetaData::InternalizeInstallFromContentInfoL()
    {
    DLTRACEIN((""));

    // continue either UID or identifier exists.
    if ( iContentInfo && (iContentInfo->Uid() != TUid::Null() || iContentInfo->Identifier().Length() != 0 ) )
        {
        TBool create = !iInstall;
        if ( create ) 
            {
            DLTRACE(("No install, creating"));
            iInstall = CNcdNodeInstall::NewL( *this );
            }
        
        // Only delete install if it was created in this method
        if ( !iInstall->InternalizeContentInfoL() && create ) 
            {
            DLTRACE(("App is not installed, deleting install"));
            iInstall->Close();
            iInstall = NULL;
            }
        }
    DLTRACEOUT((""));
    }


void CNcdNodeMetaData::InternalizeDependencyL( 
    const MNcdPurchaseDetails& aDetails ) 
    {
    DLTRACEIN((""));
    if ( !iDependency )
        {
        iDependency = CNcdNodeDependency::NewL( *this );    
        if ( !iDependency->InternalizeFromPurchaseDetailsL( aDetails ) )
            {
            iDependency->Close();
            iDependency = NULL;
            }
        }
    else
        {
        iDependency->InternalizeFromPurchaseDetailsL( aDetails );
        }        
    DLTRACEOUT(("dependency internalized from purchasehistory"));
    }
    
void CNcdNodeMetaData::InternalizeIconL( const MNcdPurchaseDetails& aDetails )
    {
    DLTRACEIN((""));
    if( aDetails.HasIcon() )
        {
        DLTRACE(("Purchase details have icon"));
        // Only use icon from PH if there is no icon previously
        // (icon may change on server whilst the icon in PH is the one that
        // was available during purchase ).
        if( !iIcon )
            {
            iIcon = CNcdNodeIcon::NewL( iNodeManager, *this, ETrue );
            }
        }
    }

CNcdPurchaseDetails* CNcdNodeMetaData::PurchaseDetailsLC( TBool aLoadIcon ) const
    {
    DLTRACEIN((""));
        
    return NcdPurchaseHistoryUtils::PurchaseDetailsLC( 
        iNodeManager.PurchaseHistory(),
        iIdentifier->ClientUid(),
        *iIdentifier,
        aLoadIcon );
    }


TBool CNcdNodeMetaData::HandleContentUpgradeL()
    {
    DLTRACEIN((""));
    DLNODEID(( Identifier() ));
    if ( iContentInfo ) 
        {
        DLTRACE(("Content info exists, check if content upgrades something"));

        TCatalogsVersion version;
        TRAPD( err, TCatalogsVersion::ConvertL( 
            version, iContentInfo->Version() ) );

        LeaveIfNotErrorL( err, KErrArgument, KErrGeneral );
        
        // First compare version number to purchase history because it's highest priority
        if ( version == iBoughtContentVersion ) 
            {
            if ( iUpgrade )
                {        
                // Reset content upgrade status
                iUpgrade->SetContentUpgradesL(
                    EFalse,
                    TUid::Null(),
                    KNullDesC );
                }            
            DLTRACEOUT(("Version in contentinfo matches bought version, no upgrade"));
            return EFalse;            
            }
        
        
        // ContentCount() ensures that there's actually something installed, otherwise
        // we would end up checking against the same contentinfo if the content is
        // a SIS app and some version of it is already installed
        if ( iInstall && iInstall->ContentCount() ) 
            {                
            
            TCatalogsVersion installVersion( iInstall->ContentVersion() );
            DLTRACE(("Version from install: %d.%d.%d", 
                installVersion.iMajor, installVersion.iMinor, installVersion.iBuild ));
                
            // IsAllContentInstalledL returns true if all content files are installed
            if ( installVersion != TCatalogsVersion() && 
                 version > installVersion && 
                 iInstall->IsAllContentInstalledL() )
                {
                
                if ( !iUpgrade ) 
                    {
                    DLTRACE(("Creating upgrade"));
                    iUpgrade = CNcdNodeUpgrade::NewL( *this );
                    }
             
                if ((iContentInfo->Uid() == KNullUid) &&
                    (iContentInfo->MimeType().Compare( KMimeTypeMatchWidget ) == 0 ))
                    { 
                    // Get widget Uid
                        iUpgrade->SetContentUpgradesL(
                        ETrue,
                        CNcdProviderUtils::WidgetUidL(iContentInfo->Identifier()),
                        iContentInfo->Version() );
                    }
                else
                    {
                    // content is an application upgrade
                    iUpgrade->SetContentUpgradesL(
                        ETrue,
                        iContentInfo->Uid(),
                        iContentInfo->Version() );
                    }
                    
                return ETrue;                  
                }
            }        
        // If content is an application, check if it upgrades. "Else if" is necessary
        // because otherwise content version check could be overridden by application version
        // check
        else if ( iContentInfo->Uid() != TUid::Null() ) 
            {
            TNcdApplicationStatus contentVersion( 
                ENcdApplicationNotInstalled );

            TRAPD( err, 
                contentVersion = CNcdProviderUtils::IsApplicationInstalledL( 
                iContentInfo->Uid(), 
                iContentInfo->Version() ) );

            // Ignore errors in version conversion                
            LeaveIfNotErrorL( err, KErrArgument, KErrGeneral );
                
            if ( contentVersion == ENcdApplicationOlderVersionInstalled ) 
                {
                if ( !iUpgrade ) 
                    {
                    DLTRACE(("Creating upgrade"));
                    iUpgrade = CNcdNodeUpgrade::NewL( *this );
                    }
                    
                // Set upgrade data which will be available through the API
                iUpgrade->SetContentUpgradesL( 
                    ETrue, 
                    iContentInfo->Uid(), 
                    iContentInfo->Version() );
                
                DLTRACEOUT(("Content upgrades"));    
                return ETrue;
                }
            }
        // Widget?
        else if (iContentInfo->Identifier() != KNullDesC) 
            {
            TNcdApplicationStatus contentVersion( 
            ENcdApplicationNotInstalled );

            TRAPD( err, 
                contentVersion = CNcdProviderUtils::IsWidgetInstalledL( 
                iContentInfo->Identifier(), 
                iContentInfo->Version() ) );

            // Ignore errors in version conversion                
            LeaveIfNotErrorL( err, KErrArgument, KErrGeneral );
                        
            if ( contentVersion == ENcdApplicationOlderVersionInstalled ) 
                {
                if ( !iUpgrade ) 
                    {
                    DLTRACE(("Creating upgrade"));
                    iUpgrade = CNcdNodeUpgrade::NewL( *this );
                    }
                            
                // Set upgrade data which will be available through the API
                iUpgrade->SetContentUpgradesL( 
                    ETrue, 
                    CNcdProviderUtils::WidgetUidL(iContentInfo->Identifier()), 
                    iContentInfo->Version() );
                        
                    DLTRACEOUT(("Content upgrades"));    
                    return ETrue;
                }
            }
        }
    
    if ( iUpgrade )
        {        
        // Reset content upgrade status
        iUpgrade->SetContentUpgradesL(
            EFalse,
            TUid::Null(),
            KNullDesC );
        }
    return EFalse;                
    }


void CNcdNodeMetaData::SetDeleteSoon( TBool aDeleteSoon )
    {
    iDeleteSoon = aDeleteSoon;
    }


TBool CNcdNodeMetaData::DeleteSoon() const
    {
    return iDeleteSoon;
    }

       
void CNcdNodeMetaData::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    // Set all the membervariable values to the stream. So,
    // that the stream may be used later to create a new
    // object.

    // First insert data that node manager will use to
    // create this class object
    DLTRACE(("Meta extern class id: %d", iClassId));
    aStream.WriteInt32L( iClassId );
    
    // Write the data that will be used when internalize function
    // is called.
    
    ExternalizeDesL( TimeStamp(), aStream );
    ExternalizeDesL( NodeName(), aStream );
    ExternalizeDesL( Description(), aStream );
    ExternalizeDesL( LayoutType(), aStream );
    
    aStream.WriteInt8L( iAlwaysVisible );    
    
    if ( iDisclaimer )
        {
        aStream.WriteInt32L( 1 );
        iDisclaimer->ExternalizeL( aStream );
        }
    else
        {
        aStream.WriteInt32L( 0 );
        }
    
    if ( iIcon )
        {
        aStream.WriteInt32L( 1 );
        iIcon->ExternalizeL( aStream );
        }
    else
        {
        aStream.WriteInt32L( 0 );
        }

    if ( iScreenshot )
        {
        aStream.WriteInt32L( 1 );
        iScreenshot->ExternalizeL( aStream );
        }
    else
        {
        aStream.WriteInt32L( 0 );
        }

    if ( iSkin )
        {
        aStream.WriteInt32L( 1 );
        iSkin->ExternalizeL( aStream );
        }
    else
        {
        aStream.WriteInt32L( 0 );
        }

    if ( iPreview )
        {
        aStream.WriteInt32L( 1 );
        iPreview->ExternalizeL( aStream );
        }
    else
        {
        aStream.WriteInt32L( 0 );
        }

    if ( iContentInfo ) 
        {
        aStream.WriteInt32L( 1 );
        iContentInfo->ExternalizeL( aStream );
        }
    else 
        {
        aStream.WriteInt32L( 0 );
        }

    if ( iUpgrade )
        {
        aStream.WriteInt32L( 1 );
        iUpgrade->ExternalizeL( aStream );
        }
    else
        {
        aStream.WriteInt32L( 0 );
        }

    if ( iDependency )
        {
        aStream.WriteInt32L( 1 );
        iDependency->ExternalizeL( aStream );
        }
    else
        {
        aStream.WriteInt32L( 0 );
        }

    // Subscribable content
    if ( iSubscribableContent )
        {
        aStream.WriteInt32L( ETrue );
        iSubscribableContent->ExternalizeL( aStream );
        }
    else
        {
        aStream.WriteInt32L( EFalse );
        }
   
    
    // Purchase options
        
    TInt purchaseOptionCount( iPurchaseOptions.Count() );
    DLINFO(("Externalizing purchase options: %d", purchaseOptionCount ));
    aStream.WriteInt32L( purchaseOptionCount );

    TInt purchaseOptionIndex( 0 );
    while ( purchaseOptionIndex < purchaseOptionCount )
        {
        iPurchaseOptions[purchaseOptionIndex]->ExternalizeL( aStream );
        ++purchaseOptionIndex;
        }
          
    if ( iMoreInfo )
        {
        aStream.WriteInt32L( 1 );
        iMoreInfo->ExternalizeL( aStream );
        }
    else
        {
        aStream.WriteInt32L( 0 );
        }
        
    aStream.WriteInt32L( iDetails.Count() );
    for( TInt i = 0 ; i < iDetails.Count() ; i++ )
        {
        iDetails[i]->ExternalizeL( aStream );
        }
    
    DLTRACEOUT((""));
    }


void CNcdNodeMetaData::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    // NOTICE that this internalize function supposes that
    // classid, namespace and metadataid info, that are
    // inserted during externalization, are already read from
    // the stream before calling this function.

    InternalizeDesL( iTimeStamp, aStream );
    InternalizeDesL( iName, aStream );
    InternalizeDesL( iDescription, aStream );
    InternalizeDesL( iLayoutType, aStream );
    
    iAlwaysVisible = aStream.ReadInt8L();
    
    TInt32 disclaimerExists = aStream.ReadInt32L();
    if ( disclaimerExists )
        {
        if ( iDisclaimer == NULL )
            {
            iDisclaimer = CNcdNodeDisclaimer::NewL();            
            }
        iDisclaimer->InternalizeL( aStream );
        }
    else if ( iDisclaimer )
        {
        // Because icon should not exist, Close it.
        iDisclaimer->Close();
        iDisclaimer = NULL;
        }

    TInt32 iconExists = aStream.ReadInt32L();
    if ( iconExists )
        {
        if ( iIcon == NULL )
            {
            iIcon = CNcdNodeIcon::NewL( iNodeManager, *this );            
            }
        iIcon->InternalizeL( aStream );
        }
    else if ( iIcon )
        {
        // Because icon should not exist, delete it.
        iIcon->Close();
        iIcon = NULL;
        }
        
    TInt32 screenshotExists = aStream.ReadInt32L();
    if ( screenshotExists )
        {
        if ( iScreenshot == NULL )
            {
            iScreenshot = CNcdNodeScreenshot::NewL( iNodeManager, *this );            
            }
        iScreenshot->InternalizeL( aStream );
        }
    else if ( iScreenshot )
        {
        // Because screenshot should not exist, delete it.
        iScreenshot->Close();
        iScreenshot = NULL;
        }
        
    TInt32 skinExists = aStream.ReadInt32L();
    if ( skinExists )
        {
        if ( iSkin == NULL )
            {
            iSkin = CNcdNodeSkin::NewL();            
            }
        iSkin->InternalizeL( aStream );
        }
    else if ( iSkin )
        {
        // Because icon should not exist, Close it.
        iSkin->Close();
        iSkin = NULL;
        }

    TInt32 previewExists = aStream.ReadInt32L();
    if ( previewExists )
        {
        if ( iPreview == NULL )
            {
            iPreview = CNcdNodePreview::NewL( *this, iNodeManager );            
            }
        iPreview->InternalizeL( aStream );
        }
    else if ( iPreview )
        {
        // Because preview should not exist, Close it.
        iPreview->Close();
        iPreview = NULL;
        }

    DLTRACE(("Internalizing content info"));
    TInt32 contentInfoExists = aStream.ReadInt32L();
    if ( contentInfoExists )
        {
        if ( iContentInfo == NULL )
            {
            iContentInfo = CNcdNodeContentInfo::NewL();            
            }
        iContentInfo->InternalizeL( aStream );
        InternalizeInstallFromContentInfoL();
        }
    else if ( iContentInfo )
        {
        // Because content info should not exist, Close it.
        iContentInfo->Close();
        iContentInfo = NULL;
        }

    DLTRACE(("Internalizing upgrade for metadata:"));
    DLNODEID(( Identifier() ));
    TInt32 upgradeExists = aStream.ReadInt32L();
    if ( upgradeExists )
        {
        if ( iUpgrade == NULL )
            {
            iUpgrade = CNcdNodeUpgrade::NewL( *this );            
            }
        iUpgrade->InternalizeL( aStream );
        
        // This requires that both CNcdNodeContentInfo &
        // CNcdNodeInstall are up-to-date
        HandleContentUpgradeL();
        }
    else 
        {
        if ( iUpgrade )
            {
            // Because upgrade should not exist, Close it.
            iUpgrade->Close();
            iUpgrade = NULL;
            }
        // Upgrade situation may have changed due to software
        // uninstallations so we update to current situation
        // HandleContentUpgradeL creates iUpgrade if necessary    
        HandleContentUpgradeL();
        }
        
    TInt32 dependencyExists = aStream.ReadInt32L();
    if ( dependencyExists )
        {
        if ( iDependency == NULL )
            {
            iDependency = CNcdNodeDependency::NewL( *this );            
            }
        iDependency->InternalizeL( aStream );
        }
    else if ( iDependency )
        {
        // Because preview should not exist, Close it.
        iDependency->Close();
        iDependency = NULL;
        }


    // Subscribable content
    TBool subscribable = aStream.ReadInt32L();
    if ( subscribable )
        {
        if ( iSubscribableContent == NULL )
            {
            iSubscribableContent = CNcdServerSubscribableContent::NewL();
            }        
        iSubscribableContent->InternalizeL( aStream );
        }


    // Create or reinternalize purchase options :
    
    const TInt KPurchaseOptionCount( aStream.ReadInt32L() );

    DLINFO(( "Amount of purchaseoptions found from the stream: %d",
             KPurchaseOptionCount ));
    
    TInt purchaseOptionIndex( 0 );
    while ( purchaseOptionIndex < KPurchaseOptionCount )
        {
        InternalizePurchaseOptionL( aStream );        
        ++purchaseOptionIndex;
        }
    
    // Remove purchase options that were removed from the server
    RemoveNotUpdatedPurchaseOptions();


    TInt32 moreInfoExists = aStream.ReadInt32L();
    if ( moreInfoExists )
        {
        if ( iMoreInfo == NULL )
            {
            iMoreInfo = CNcdNodeDisclaimer::NewL();            
            }
        iMoreInfo->InternalizeL( aStream );
        }
    else if ( iMoreInfo )
        {
        // Because icon should not exist, Close it.
        iMoreInfo->Close();
        iMoreInfo = NULL;
        }
    
    iDetails.ResetAndDestroy();
    TInt32 detailCount = aStream.ReadInt32L();
    for( TInt i = 0 ; i < detailCount ; i++ )
        {
        CNcdKeyValuePair* detail = CNcdKeyValuePair::NewLC( aStream );
        iDetails.AppendL( detail );
        CleanupStack::Pop( detail );
        }
    
    DLTRACEOUT((""));
    }


void CNcdNodeMetaData::ReceiveMessage( MCatalogsBaseMessage* aMessage,
                                       TInt aFunctionNumber )
    {
    DLTRACEIN((""));

    DASSERT( aMessage );
        
    // Now, we can be sure that rest of the time iMessage exists.
    // This member variable is set for the CounterPartLost function.
    iMessage = aMessage;
    
    TInt trapError( KErrNone );
    
    switch( aFunctionNumber )
        {
        case NcdNodeFunctionIds::ENcdInternalize:
            TRAP( trapError, InternalizeRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdRelease:
            ReleaseRequest( *aMessage );
            break;

        case NcdNodeFunctionIds::ENcdUserDataHandle:
            TRAP( trapError, UserDataHandleRequestL( *aMessage ) );
            break;            

        case NcdNodeFunctionIds::ENcdDisclaimerHandle:
            TRAP( trapError, DisclaimerHandleRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdIconHandle:
            TRAP( trapError, IconHandleRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdScreenshotHandle:
            TRAP( trapError, ScreenshotHandleRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdSkinHandle:
            TRAP( trapError, SkinHandleRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdUriContentHandle:
            TRAP( trapError, UriContentHandleRequestL( *aMessage ) );
            break;
            
        case NcdNodeFunctionIds::ENcdPreviewHandle:
            TRAP( trapError, PreviewHandleRequestL( *aMessage ) );
            break;            
        
        case NcdNodeFunctionIds::ENcdContentInfoHandle:
            TRAP( trapError, ContentInfoHandleRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdUpgradeHandle:
            TRAP( trapError, UpgradeHandleRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdDependencyHandle:
            TRAP( trapError, DependencyHandleRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdInternalizePurchaseHistory:
            TRAP( trapError, InternalizePurchaseHistoryRequestL( *aMessage ) );
            break;            

        case NcdNodeFunctionIds::ENcdInternalizePurchaseMeans:
            TRAP( trapError, InternalizePurchaseMeansRequestL( *aMessage ) );
            break;            

        case NcdNodeFunctionIds::ENcdPurchaseOptionIds:
            TRAP( trapError, PurchaseOptionIdsRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdInstallHandle:
            TRAP( trapError, InstallHandleRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdDownloadHandle:
            TRAP( trapError, DownloadHandleRequestL( *aMessage ) );
            break;
        
        case NcdNodeFunctionIds::ENcdMoreInfoHandle:
            TRAP( trapError, MoreInfoHandleRequestL( *aMessage ) );
            break;
            
        case NcdNodeFunctionIds::ENcdIsPurchaseSupported:
            TRAP( trapError, IsPurchaseSupportedRequestL( *aMessage ) );
            break;

        default:
            DLERROR(("Unidentified function request"));
            DASSERT( EFalse );        
            break;
        }

    if ( trapError != KErrNone )
        {
        // Because something went wrong the complete has not been
        // yet called for the message.
        // So, inform the client about the error.
        aMessage->CompleteAndRelease( trapError );
        }

    // Because the message should not be used after this, set it NULL.
    // So, CounterPartLost function will know that no messages are
    // waiting the response at the moment.
    iMessage = NULL;        

    if ( aFunctionNumber == NcdNodeFunctionIds::ENcdRelease )
        {
        // Because release was called for this object it may be time to
        // delete this object. Inform manager about the release so it may
        // close this object and clear the cache if needed.
        // Notice that if the manager closes this object then this object will
        // be deleted. It is safe to do here because no memeber variables are
        // needed here after the call.
        NodeManager().MetaDataReleased( *this );       
        }
                    
    DLTRACEOUT((""));
    }


void CNcdNodeMetaData::CounterPartLost( const MCatalogsSession& aSession )
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

void CNcdNodeMetaData::InternalizeRequestL( MCatalogsBaseMessage& aMessage ) const
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


    if ( buf->Size() > 0 ) 
        {
        DLINFO(( "Completing the message, buf len: %d", buf->Ptr(0).Length() ));
        }
    // If this leaves, ReceiveMessge will complete the message.
    aMessage.CompleteAndReleaseL( buf->Ptr( 0 ), KErrNone );
        
    
    DLINFO(("Deleting the buf"));
    CleanupStack::PopAndDestroy( buf );
        
    DLTRACEOUT((""));
    }
    

void CNcdNodeMetaData::ExternalizeDataForRequestL( RWriteStream& aStream ) const
    {
    DLTRACEIN((""));

    DLINFO(("Class ID: %d", ClassId() ));
    // Metadata existed. So, insert info that meta data was found.
    aStream.WriteInt32L( ClassId() );
    
    iIdentifier->ExternalizeL( aStream );
    ExternalizeDesL( NodeName(), aStream );
    ExternalizeDesL( Description(), aStream );
    ExternalizeDesL( LayoutType(), aStream );
    
    aStream.WriteInt8L( iAlwaysVisible );
    
    aStream.WriteInt32L( iDetails.Count() );
    for( TInt i = 0 ; i < iDetails.Count() ; i++ )
        {
        iDetails[i]->ExternalizeL( aStream );
        }

    DLTRACEOUT((""));
    }


void CNcdNodeMetaData::InternalizePurchaseMeansRequestL(
    MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));
    
    // Parse the input message and create a descriptor array containing
    // the purchase option ids which should be externalized for the request.
    CDesCArray* purchaseOptionIds = new( ELeave ) CDesCArrayFlat( 5 );
    CleanupStack::PushL( purchaseOptionIds );
    HBufC8* input = HBufC8::NewLC( aMessage.InputLength() );
    TPtr8 inputPtr = input->Des();
    aMessage.ReadInput( inputPtr );
    RDesReadStream inputStream( *input );
    CleanupClosePushL( inputStream );
    
    TInt poCount = inputStream.ReadInt32L();
    for ( TInt i = 0; i < poCount; i++ ) 
        {
        HBufC* tmpId( NULL );
        InternalizeDesL( tmpId, inputStream );
        CleanupStack::PushL( tmpId );
        purchaseOptionIds->AppendL( *tmpId );
        CleanupStack::PopAndDestroy( tmpId );
        }
        
    CleanupStack::PopAndDestroy( &inputStream );
    CleanupStack::PopAndDestroy( input );
        
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );

    // Include all the necessary purchase data to the stream
    MCatalogsSession& session = aMessage.Session();
    ExternalizePurchaseMeansForRequestL( *purchaseOptionIds, stream, session ); 
    
    // Commits data to the stream when closing.
    CleanupStack::PopAndDestroy( &stream );


    if ( buf->Size() > 0 ) 
        {
        DLINFO(( "Completing the message, buf len: %d", buf->Ptr(0).Length() ));
        }
    // If this leaves, ReceiveMessge will complete the message.
    aMessage.CompleteAndReleaseL( buf->Ptr( 0 ), KErrNone );
        
    
    DLINFO(("Deleting the buf"));
    CleanupStack::PopAndDestroy( buf );
    CleanupStack::PopAndDestroy( purchaseOptionIds );
        
    DLTRACEOUT((""));    
    }


void CNcdNodeMetaData::ExternalizePurchaseMeansForRequestL(
    const CDesCArray& aPurchaseOptionIds,
    RWriteStream& aStream,
    MCatalogsSession& aSession ) const
    {

    // Subscribable content
    if ( iSubscribableContent != NULL )
        {
        aStream.WriteInt32L( ETrue );
        iSubscribableContent->ExternalizeL( aStream );
        }
    else
        {
        aStream.WriteInt32L( EFalse );
        }
    
    
    // this is the same as handle amount
    TInt purchaseOptionAmount( aPurchaseOptionIds.Count() );
    aStream.WriteInt32L( purchaseOptionAmount );
    
    for ( TInt i = 0; i < purchaseOptionAmount; i++ ) 
        {
        CNcdPurchaseOptionImpl& tmpPurchaseOption = PurchaseOptionByIdL(
            aPurchaseOptionIds[i] );      
        TInt tmpHandle( aSession.AddObjectL( &tmpPurchaseOption ) );
        
        TRAPD( addError, aStream.WriteInt32L( tmpHandle ) );
        if ( addError != KErrNone )
            {
            // Should all other added objects be removed from
            //       the session also?
            aSession.RemoveObject( tmpHandle );
            User::Leave( addError );
            }
        }    
    }


void CNcdNodeMetaData::InternalizePurchaseHistoryRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));
    
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );


    // Include all the necessary data to the stream
    ExternalizePurchaseHistoryForRequestL( stream );     
    
    
    // Commits data to the stream when closing.
    CleanupStack::PopAndDestroy( &stream );

    if ( buf->Size() > 0 ) 
        {
        DLINFO(( "Completing the message, buf len: %d", buf->Ptr(0).Length() ));
        }
        
    // If this leaves ReceiveMessage function will complete the message.
    aMessage.CompleteAndReleaseL( buf->Ptr( 0 ), KErrNone );
        
    
    DLINFO(("Deleting the buf"));
    CleanupStack::PopAndDestroy( buf );
        
    DLTRACEOUT((""));
    }

void CNcdNodeMetaData::ExternalizePurchaseHistoryForRequestL( RWriteStream& aStream ) const
    {
    DLTRACEIN((""));

    // Insert node class id just in case somebody wants to check that
    // the data is of the right type
    aStream.WriteInt32L( ClassId() );

    // Get up to date purchase info straight from purchase history
    CNcdPurchaseDetails* purchaseDetails = NULL;
    
    TRAPD( error, 
        {        
        purchaseDetails = PurchaseDetailsLC();
        CleanupStack::Pop( purchaseDetails );
        });    
    
    // there's no problem in pushing a NULL pointer as long as it's
    // pushed with PushL or CleanupDeletePushL
    CleanupStack::PushL( purchaseDetails );
    
    if ( error == KNcdErrorNoPurchaseInformation
         ||  ( purchaseDetails && 
               purchaseDetails->PurchaseOptionId() == KNullDesC ) )
        {
        // If no info is found it means that the node is
        // not purchased.
        // Also, purchase option id is checked here. In some situations,
        // dummy purchase details may be inserted into the purchase history
        // from outside the engine. So, then the purchase option id is most likely
        // not set correctly.
        aStream.WriteInt32L( EFalse );
        CleanupStack::PopAndDestroy( purchaseDetails );
        return;
        }
    else if ( error == KErrNone )
        {
        aStream.WriteInt32L( ETrue );
        }
    else
        {
        User::Leave( error );
        }    
    
     
    const TDesC& purchasedOptionId = purchaseDetails->PurchaseOptionId();
    ExternalizeDesL( purchasedOptionId, aStream );

    TTime timeOfPurchase = purchaseDetails->PurchaseTime();
    const TInt64& integerTimeOfPurchase = timeOfPurchase.Int64();
    // Store framework provides the necessary implementation for 
    // the operator<< to externalise the 64-bit integer
    aStream << integerTimeOfPurchase;
    
    const TDesC& finalPrice = purchaseDetails->FinalPrice();
    ExternalizeDesL( finalPrice, aStream );


    CleanupStack::PopAndDestroy( purchaseDetails );
    DLTRACEOUT((""));
    }
 
    
void CNcdNodeMetaData::PurchaseOptionIdsRequestL(
    MCatalogsBaseMessage& aMessage ) const 
    {
    DLTRACEIN((""));
    
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );
    
    stream.WriteInt32L( iPurchaseOptions.Count() );
    for ( TInt i = 0; i < iPurchaseOptions.Count(); i++ ) 
        {
        ExternalizeDesL( iPurchaseOptions[i]->Id(), stream );
        }
        
    CleanupStack::PopAndDestroy( &stream );
    
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( buf->Ptr( 0 ), KErrNone );
    CleanupStack::PopAndDestroy( buf );
    }
        

void CNcdNodeMetaData::ReleaseRequest( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));

    // Decrease the reference count for this object.
    // When the reference count reaches zero, this object will be destroyed
    // and removed from the session.
    MCatalogsSession& requestSession( aMessage.Session() );
    TInt handle( aMessage.Handle() );
    aMessage.CompleteAndRelease( KErrNone );
    requestSession.RemoveObject( handle );
                
    DLTRACEOUT((""));
    }


void CNcdNodeMetaData::IconIdRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));

    // If this leaves, ReceiveMessage will complete the message.    

    if ( ! iIcon )
        {
        User::Leave( KErrNotFound );
        }

    aMessage.CompleteAndReleaseL( iIcon->IconId(), KErrNone );

    DLTRACEOUT((""));
    }


void CNcdNodeMetaData::IconDataRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));

    // If this leaves, ReceiveMessage will complete the message.    

    if ( iIcon == NULL )
        {
        User::Leave( KErrNotFound );
        }

    HBufC8* icon = iIcon->IconDataL();
    CleanupStack::PushL( icon );

    aMessage.CompleteAndReleaseL( *icon, KErrNone );
    
    CleanupStack::PopAndDestroy( icon );
    
    DLTRACEOUT((""));
    }


void CNcdNodeMetaData::UserDataHandleRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));

    if( iUserData == NULL )
        {
        DLINFO(("User data NULL"));
        User::Leave( KErrNotFound );        
        }

    // Get the session that will contain the handle of the node
    MCatalogsSession& requestSession( aMessage.Session() );

    // Add the node again to the session and get the new handle.
    // If the node already existed in the session we will still
    // get a new handle to the same object.
    TInt32 handle( requestSession.AddObjectL( iUserData ) );

    DLINFO(("User data handle: %d", handle ));
        
    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( handle, KErrNone );
        
    DLTRACEOUT((""));
    }


void CNcdNodeMetaData::DisclaimerHandleRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));    
    
    if( iDisclaimer == NULL )
        {
        DLINFO(("Disclaimer NULL"));
        User::Leave( KErrNotFound );
        }

    // Get the session that will contain the handle of the node.
    MCatalogsSession& requestSession( aMessage.Session() );

    // Add the icon to the session and get the handle.
    // If the node already existed in the session we will still
    // get a new handle to the same object.
    TInt32 handle( requestSession.AddObjectL( iDisclaimer ) );

    DLINFO(("Disclaimer handle: %d", handle ));
        
    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( handle, KErrNone );

    DLTRACEOUT((""));        
    }


void CNcdNodeMetaData::IconHandleRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));    
    
    if( iIcon == NULL )
        {
        DLINFO(("Icon NULL"));
        User::Leave( KErrNotFound );
        }
        
    if ( !iIcon->IconDataReady() ) 
        {
        DLINFO(("Icon data not ready"));
        User::Leave( KErrNotFound );
        }
        
    if ( iIcon->IconId() == KNullDesC ) 
        {
        DLINFO(("Icon id not set"));
        User::Leave( KErrNotFound );
        }

    // Get the session that will contain the handle of the node.
    MCatalogsSession& requestSession( aMessage.Session() );

    // Add the icon to the session and get the handle.
    // If the node already existed in the session we will still
    // get a new handle to the same object.
    TInt32 handle( requestSession.AddObjectL( iIcon ) );

    DLINFO(("Icon handle: %d", handle ));
        
    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( handle, KErrNone );

    DLTRACEOUT((""));        
    }


void CNcdNodeMetaData::ScreenshotHandleRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));    
    
    if( iScreenshot == NULL )
        {
        DLINFO(("Screenshot NULL"));
        User::Leave( KErrNotFound );
        }

    // Get the session that will contain the handle of the node.
    MCatalogsSession& requestSession( aMessage.Session() );

    // Add the screenshot to the session and get the handle.
    // If the node already existed in the session we will still
    // get a new handle to the same object.
    TInt32 handle( requestSession.AddObjectL( iScreenshot ) );

    DLINFO(("Screenshot handle: %d", handle ));
        
    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( handle, KErrNone );

    DLTRACEOUT((""));        
    }


void CNcdNodeMetaData::SkinHandleRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));    
    
    if( iSkin == NULL )
        {
        DLINFO(("Skin NULL"));
        User::Leave( KErrNotFound );
        }

    // Get the session that will contain the handle of the node.
    MCatalogsSession& requestSession( aMessage.Session() );

    // Add the skin to the session and get the handle.
    // If the node already existed in the session we will still
    // get a new handle to the same object.
    TInt32 handle( requestSession.AddObjectL( iSkin ) );

    DLINFO(("Skin handle: %d", handle ));
        
    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( handle, KErrNone );

    DLTRACEOUT((""));        
    }


void CNcdNodeMetaData::UriContentHandleRequestL(
    MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));    
    
    if( iUriContent == NULL )
        {
        DLINFO(("UriContent NULL"));
        User::Leave( KErrNotFound );
        }

    // Get the session that will contain the handle of the node.
    MCatalogsSession& requestSession( aMessage.Session() );


    // Add the uri content to the session and get the handle.
    // If the node already existed in the session we will still
    // get a new handle to the same object.
    TInt32 handle( requestSession.AddObjectL( iUriContent ) );

    DLINFO(("Uri content handle: %d", handle ));
        
    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( handle, KErrNone );

    DLTRACEOUT((""));        
    }


void CNcdNodeMetaData::ContentInfoHandleRequestL(
    MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));    
    
    if( iContentInfo == NULL )
        {
        DLINFO(("ContentInfo NULL"));
        User::Leave( KErrNotFound );
        }

    // Get the session that will contain the handle of the node.
    MCatalogsSession& requestSession( aMessage.Session() );


    // Add the content info to the session and get the handle.
    // If the node already existed in the session we will still
    // get a new handle to the same object.
    TInt32 handle( requestSession.AddObjectL( iContentInfo ) );

    DLINFO(("Content info handle: %d", handle ));
        
    // Send the information to the client side.
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( handle, KErrNone );

    DLTRACEOUT((""));        
    }


void CNcdNodeMetaData::PreviewHandleRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));    
    
    if( iPreview == NULL )
        {
        DLINFO(("Preview NULL"));
        User::Leave( KErrNotFound );
        }

    // Get the session that will contain the handle of the preview.
    MCatalogsSession& requestSession( aMessage.Session() );


    // Add the preview to the session and get the handle.
    // If the preview already existed in the session we will still
    // get a new handle to the same object.
    TInt32 handle( requestSession.AddObjectL( iPreview ) );

    DLINFO(("Preview handle: %d", handle ));
        
    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( handle, KErrNone );

    DLTRACEOUT((""));        
    }


void CNcdNodeMetaData::UpgradeHandleRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));    
    DLNODEID(( Identifier() ));
    HandleContentUpgradeL();
    // Upgrade interface should be provided in proxy side only if the upgrade
    // exists in server side and it has some identifiers or content that can be used to 
    // load node upgrades from web.
    if( !iUpgrade 
        || ( iUpgrade->AllUpgradesInstalledL() && !iUpgrade->ContentUpgrades() ) )
        {
        DLINFO(("Upgrade NULL or no node targets available"));
        User::Leave( KErrNotFound );
        }

    // Get the session that will contain the handle of the preview.
    MCatalogsSession& requestSession( aMessage.Session() );


    // Add the preview to the session and get the handle.
    // If the object already existed in the session we will still
    // get a new handle to the same object.
    TInt32 handle( requestSession.AddObjectL( iUpgrade ) );

    DLINFO(("Upgrade handle: %d", handle ));
        
    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( handle, KErrNone );

    DLTRACEOUT((""));        
    }


void CNcdNodeMetaData::DependencyHandleRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));    
    
    // Dependency interface should be provided in proxy side only if the dependency
    // exists in server side and it has some identifiers or content that can be used to 
    // load dependencies from web.
    // NOTE: interface will be visible even if all of the dependencies are installed
    if( !iDependency ) 
        {
        DLINFO(("Dependency NULL"));
        User::Leave( KErrNotFound );
        }

    // Get the session that will contain the handle of the preview.
    MCatalogsSession& requestSession( aMessage.Session() );


    // Add the preview to the session and get the handle.
    // If the preview already existed in the session we will still
    // get a new handle to the same object.
    TInt32 handle( requestSession.AddObjectL( iDependency ) );

    DLINFO(("Dependency handle: %d", handle ));
        
    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( handle, KErrNone );

    DLTRACEOUT((""));        
    }


void CNcdNodeMetaData::DownloadHandleRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));    
    
    if( iDownload == NULL )
        {
        DLINFO(("Node download NULL"));
        User::Leave( KErrNotFound );
        }

    // Get the session that will contain the handle of the node
    MCatalogsSession& requestSession( aMessage.Session() );

    // Add the download to the session and get the handle.
    // If the node already existed in the session we will still
    // get a new handle to the same object.
    TInt32 handle( requestSession.AddObjectL( iDownload ) );

    DLINFO(("Download handle: %d", handle ));
        
    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( handle, KErrNone );

    DLTRACEOUT((""));        
    }


void CNcdNodeMetaData::InstallHandleRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));    
    
    if( iInstall == NULL )
        {
        DLINFO(("Node install NULL"));
        User::Leave( KErrNotFound );
        }

    // Get the session that will contain the handle of the node
    MCatalogsSession& requestSession( aMessage.Session() );

    // Add the install to the session and get the handle.
    // If the node already existed in the session we will still
    // get a new handle to the same object.
    TInt32 handle( requestSession.AddObjectL( iInstall ) );

    DLINFO(("Install handle: %d", handle ));
        
    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( handle, KErrNone );

    DLTRACEOUT((""));        
    }

void CNcdNodeMetaData::MoreInfoHandleRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));    
    
    if( iMoreInfo == NULL )
        {
        DLINFO(("More info NULL"));
        User::Leave( KErrNotFound );
        }

    // Get the session that will contain the handle of the node.
    MCatalogsSession& requestSession( aMessage.Session() );

    // Add the more info to the session and get the handle.
    // If the node already existed in the session we will still
    // get a new handle to the same object.
    TInt32 handle( requestSession.AddObjectL( iMoreInfo ) );

    DLINFO(("More info handle: %d", handle ));
        
    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( handle, KErrNone );

    DLTRACEOUT((""));        
    }

void CNcdNodeMetaData::IsPurchaseSupportedRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));
    TBool isPurchaseSupported = EFalse;
    if( iPurchaseOptions.Count() > 0 )
        {
        isPurchaseSupported = ETrue;
        }
    
    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( isPurchaseSupported, KErrNone );
    }

void CNcdNodeMetaData::InternalizePurchaseOptionL(
    const MNcdPreminetProtocolPurchaseOption& aData )
    {
    DLTRACEIN((""));
    CNcdPurchaseOptionImpl& option(
        CreateOrGetPurchaseOptionL( aData.Id() ) );
    
    TRAPD( error, option.InternalizeL( aData ) );
    if ( error != KErrNone )
        {
        // If internalization fails, let's remove the option so
        // no incomplete data is left to be used
        
        // If an error occurs in the removal we don't want to
        // pass that forward. Let's pass the original error.
        TRAP_IGNORE( RemovePurchaseOptionL( aData.Id() ) );
        
        User::Leave( error );
        }
    
    // Set the option recently updated so we can later in
    // RemoveRecentlyUpdatedPurchaseOptions() differentiate
    // options that should be removed because they were not
    // received from the server anymore.
    option.SetRecentlyUpdated( ETrue );
    
    // Updates content URI if necessary
    HandleContentUriUpdateL( option );
    DLTRACEOUT(("Purchase option internalized successfully"));
    }
    

CNcdPurchaseOptionImpl& CNcdNodeMetaData::CreateOrGetPurchaseOptionL(
    const TDesC& aPurchaseOptionId )
    {
    DLTRACEIN((""));
    CNcdPurchaseOptionImpl* option( NULL );
    TRAPD( error, option = &PurchaseOptionByIdL( aPurchaseOptionId ) );
    if ( error != KErrNone && error != KErrNotFound )
        {
        User::Leave( error );
        }
    else if ( error == KErrNotFound )
        {
        // Not found, have to create
        option = CNcdPurchaseOptionImpl::NewL( *this );
        // Set option id already here so if something goes wrong
        // after appending the option into array, the option can be
        // identified and removed.
        option->SetIdL( aPurchaseOptionId );
            
        error = KErrNone;
        error = iPurchaseOptions.Append( option );
        if ( error != KErrNone )
            {
            DLERROR(("Appending purchase option failed with %d", error ));
            // When CObject base object is created its reference count
            // is set to one. Therefore close is said once.
            option->Close();
            User::Leave( error );
            }
        }
    return *option;
    }

void CNcdNodeMetaData::RemovePurchaseOptionL(
    const TDesC& aPurchaseOptionId )
    {
    DLTRACEIN((""));
    const TInt KAmountOfOptions( iPurchaseOptions.Count() );
    for ( TInt i = 0; i < KAmountOfOptions; i++ ) 
        {
        if ( iPurchaseOptions[i]->Id() == aPurchaseOptionId ) 
            {
            // Meta data has one reference count to all objects
            // so that for one Close is said to the option.
            iPurchaseOptions[i]->Close();
            iPurchaseOptions.Remove( i );
            return;
            }
        }        
    User::Leave( KErrNotFound );  
    }

void CNcdNodeMetaData::RemoveNotUpdatedPurchaseOptions()
    {
    DLTRACEIN((""));
    TInt optionsIndexer( iPurchaseOptions.Count() - 1 );
    while ( optionsIndexer > -1 )
        {
        if ( !iPurchaseOptions[optionsIndexer]->RecentlyUpdated() )
            {
            DLINFO(( "Removing purchase option that is removed from server." ));
            // Meta data has one reference count to all objects
            // so that for one Close is said to the option.
            iPurchaseOptions[optionsIndexer]->Close();
            iPurchaseOptions.Remove( optionsIndexer );
            }
        else
            {
            DLINFO(( "Resettting recently updated info of po." ));
            // Resetting the flag for later use
            iPurchaseOptions[optionsIndexer]->
                SetRecentlyUpdated( EFalse );
            }                
        --optionsIndexer;
        }
    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// Internalization from a stream is a little harder now than
// than internalizing from the protocol object. It is done
// so that the po is first internalized into temp option and then
// needed info is taken from it for example to identify the option
// among the already existing options. If the option is already
// found the temp option is externalized to stream and then
// internalized into the already existing option. If no option
// with same id is found then created temp option is appended to
// array. Usually when internalizing from a stream it means that
// we are internalizing from the database and we don't have the
// object created yet. So the latter case is the usual case.

// One drawback with this implementation is that we can use only
// little of the functions which are used when internalizing from
// the protocol.

// One other way to implement this could be 
// just to move the purchase option id first in the
// internalize/externalize so that the option id could be retrieved
// easily without no temp externalization/internalization.
// This would need some changes and would bind the order of
// externalization/internalization of variables so it is not done
// now. Also if current implementation is kept, the externalization
// internalization could be replaced with assignment opertators in
// purchase option and its composite classes.
// ---------------------------------------------------------------------------
//
void CNcdNodeMetaData::InternalizePurchaseOptionL(
    RReadStream& aStream )
    {
    DLTRACEIN((""));
    
    // Temp option for reading option id
    CNcdPurchaseOptionImpl* tmpOption =
        CNcdPurchaseOptionImpl::NewLC( *this );
    tmpOption->InternalizeL( aStream );
    const TDesC& poId( tmpOption->Id() );
    
    // Search for the option requires the purchase option id so it is the
    // only reason why the temp option is needed always in this function.
    CNcdPurchaseOptionImpl* option( NULL );
    TRAPD( error, option = &PurchaseOptionByIdL( poId ) );
    if ( error != KErrNone && error != KErrNotFound )
        {
        User::Leave( error );
        }
        
    CleanupStack::Pop( tmpOption );
                
    if ( option == NULL )
        {
        DLTRACE((""));
        // Purchase option not found, append the temp option into
        // array of purchase options
        option = tmpOption;
        
        error = KErrNone;
        error = iPurchaseOptions.Append( option );
        if ( error != KErrNone )
            {
            DLERROR(("Appending purchase option failed with %d", error ));
            // When CObject base object is created its reference count
            // is set to one. Therefore close is said once.
            option->Close();
            User::Leave( error );
            }
        }
    else
        {
        DLTRACE((""));
        CleanupClosePushL( *tmpOption );
        // We externalize the content of a option into a stream.
        // This is done to copy the content from the temp option
        // into the option already found from the array.
		
        // This could be replaced with an assignment operation
        // in CNcdPurchaseOptionImpl and in all of its composite
        // classes. Because at the moment this is a rare situation
        // (never happens at the moment) we don't use
        // any effort into it at the moment.
        //       
        CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
        CleanupStack::PushL( buf );
            
        RBufWriteStream writeStream( *buf );
        CleanupClosePushL( writeStream );
        DLINFO(( "Externalizing option in copy" ));
        tmpOption->ExternalizeL( writeStream );
        // Commits data to the stream when closing.
        CleanupStack::PopAndDestroy( &writeStream );
        
        // Now we internalize the option already found in the
        // array from the stream.        
        RDesReadStream readStream( buf->Ptr( 0 ) );
        CleanupClosePushL( readStream );
        DLINFO(( "Internalizing option in copy" ));
        TRAPD( error, option->InternalizeL( readStream ) );
        if ( error != KErrNone )
            {
            // If internalization fails, let's remove the option so
            // no incomplete data is left to be used
            
            // If an error occurs in the removal we don't want to
            // pass that forward. Let's pass the original error.
            TRAP_IGNORE( RemovePurchaseOptionL( poId ) );
            
            User::Leave( error );
            }            
        // Closes the stream
        CleanupStack::PopAndDestroy( &readStream );
        
        CleanupStack::PopAndDestroy( buf );
        CleanupStack::PopAndDestroy( tmpOption );
        }
    
    // Set the option recently updated so we can later in
    // RemoveRecentlyUpdatedPurchaseOptions() differentiate
    // options that should be removed because they were not
    // received from the server anymore.
    option->SetRecentlyUpdated( ETrue );
    }


// ---------------------------------------------------------------------------
// Updates URI content interface if necessary
// ---------------------------------------------------------------------------
//
void CNcdNodeMetaData::HandleContentUriUpdateL( 
    const CNcdPurchaseOptionImpl& aOption )
    {
    DLTRACEIN((""));    
    if ( !iUriContent || 
         !aOption.IsFree() ||          
         aOption.DownloadInfoCount() == 0 ) 
        {
        DLTRACEOUT(("No bought URI content or not free content so nothing to do"));
        return;
        }
    
    TInt count = aOption.DownloadInfoCount();
    TInt index = KErrNotFound;                
    
    DLINFO(("Going through %d download infos", count));    
    for ( TInt i = 0; i < count; ++i ) 
        {
        if ( aOption.DownloadInfo( i ).ContentUsage() == 
             MNcdPurchaseDownloadInfo::EConsumable )
            {
            DLINFO(("Download in index %d is consumable!", i));
            index = i;
            // Only one uri is supported so break immediately as we
            // come across it
            break;
            }
        }
    
    if ( index == KErrNotFound || 
         aOption.DownloadInfo( index ).ContentUri() == iUriContent->Uri() ) 
        {
        DLTRACEOUT(("Option doesn't contain URI content or URI has not changed"));
        return;
        }
    
    
    DLTRACE(("New URI differs from the old one. Updating..."));
    DLINFO(( _L("New URI: %S"), 
        &aOption.DownloadInfo( index ).ContentUri() ));
    
    CNcdPurchaseDetails* details = PurchaseDetailsLC();
    
    // Ensure that ids match, otherwise 
    // we would be updating wrong purchase option
    if ( details->PurchaseOptionId() == aOption.Id() )
        {
        // Find the correct download info for updating
        TArray<MNcdPurchaseDownloadInfo*> dlInfo ( details->DownloadInfoL() );
        
        TInt oldIndex = KErrNotFound;                
        for ( TInt i = 0; i < dlInfo.Count(); ++i )
            {
            if ( dlInfo[i]->ContentUsage() == 
                 MNcdPurchaseDownloadInfo::EConsumable )
                {
                DLINFO(("Download in index %d is consumable!", i));
                oldIndex = i;
                // Only one uri is supported so break immediately as we
                // come across it
                break;
                }
            }
            
        NCD_ASSERT_ALWAYS( oldIndex != KErrNotFound, ENcdPanicNoData );
        
        CNcdPurchaseDownloadInfo* modDownload = 
            static_cast<CNcdPurchaseDownloadInfo*>( dlInfo[oldIndex] );
        
        DLTRACE(("Updating the URI to download info"));
        modDownload->SetContentUriL( 
            aOption.DownloadInfo( index ).ContentUri() );
        
        DLTRACE(("Saving purchase"));
        iNodeManager.PurchaseHistory().SavePurchaseL( *details );
        DLTRACE(("Purchase updated"));

        iUriContent->InternalizeL( *details );
        DLTRACE(("URI content internalized"));
        }

    CleanupStack::PopAndDestroy( details );        
    
    DLTRACEOUT(("All is well"));
    }


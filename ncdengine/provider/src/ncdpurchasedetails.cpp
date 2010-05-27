/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#include <s32strm.h>

#include "ncdpurchasedetails.h"
#include "ncdpurchasedownloadinfo.h"
#include "ncdpurchaseinstallinfo.h"
#include "catalogsutils.h"
#include "catalogsconstants.h"
#include "ncdutils.h"
#include "ncdattributes.h"

#include "catalogsdebug.h"


static inline void AssignHBufC( HBufC*& aDes, HBufC* const aSource )
    {
    delete aDes;
    aDes = aSource;
    }

EXPORT_C CNcdPurchaseDetails* CNcdPurchaseDetails::NewL()
    {
    CNcdPurchaseDetails* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

EXPORT_C CNcdPurchaseDetails* CNcdPurchaseDetails::NewLC()
    {
    CNcdPurchaseDetails* self = new ( ELeave ) CNcdPurchaseDetails;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }

EXPORT_C CNcdPurchaseDetails* CNcdPurchaseDetails::NewL(
    const MNcdPurchaseDetails& aDetails )
    {
    CNcdPurchaseDetails* self = NewLC( aDetails );
    CleanupStack::Pop( self );
    return self;
    }
    
EXPORT_C CNcdPurchaseDetails* CNcdPurchaseDetails::NewLC(
    const MNcdPurchaseDetails& aDetails )
    {
    DLTRACEIN((""));
    CNcdPurchaseDetails* self = new ( ELeave ) CNcdPurchaseDetails;
    CleanupStack::PushL( self );
    self->ConstructL();
    
    self->SetCatalogSourceNameL( aDetails.CatalogSourceName() );
    self->SetClientUid( aDetails.ClientUid() );
    self->SetDescriptionL( aDetails.Description() );
    self->SetDownloadAccessPointL( aDetails.DownloadAccessPoint() );
    self->SetDownloadedFilesL( aDetails.DownloadedFiles() );
    self->SetDownloadInfoL( aDetails.DownloadInfoL() );
    self->SetEntityIdL( aDetails.EntityId() );
    self->SetFinalPriceL( aDetails.FinalPrice() );
    self->SetIconL( aDetails.Icon() );
    self->SetInstallInfoL( aDetails.InstallInfoL() );
    self->SetItemNameL( aDetails.ItemName() );
    self->SetItemPurpose( aDetails.ItemPurpose() );
    self->SetNamespaceL( aDetails.Namespace() );
    self->SetPaymentMethodNameL( aDetails.PaymentMethodName() );
    self->SetPurchaseOptionIdL( aDetails.PurchaseOptionId() );
    self->SetPurchaseOptionNameL( aDetails.PurchaseOptionName() );
    self->SetPurchaseOptionPriceL( aDetails.PurchaseOptionPrice() );
    self->SetPurchaseTime( aDetails.PurchaseTime() );
    self->SetVersionL( aDetails.Version() );
    self->SetServerUriL( aDetails.ServerUri() );
    self->SetItemType( aDetails.ItemType() );
    self->SetTotalContentSize( aDetails.TotalContentSize() );
    self->SetLastOperationTime( aDetails.LastOperationTime() );
    self->SetLastOperationErrorCode( aDetails.LastOperationErrorCode() );
    self->SetOriginNodeIdL( aDetails.OriginNodeId() );

    // Some ugly stuff for copying the attributes
    // This WILL NOT work for other MNcdPurchaseDetails-implementations
    // if there's going to be any
    const CNcdPurchaseDetails& details = 
        static_cast<const CNcdPurchaseDetails&>( aDetails );
    
    if ( details.iAttributes ) 
        {
        DeletePtr( self->iAttributes );
        self->iAttributes = CNcdAttributes::NewL( *details.iAttributes );
        }

    return self;    
    }

CNcdPurchaseDetails::~CNcdPurchaseDetails()
    {
    delete iNamespace;
    delete iEntityId;
    delete iItemName;
    delete iCatalogSourceName;
    ResetAndDestroyWithCast< CNcdPurchaseDownloadInfo >( iDownloadInfo );
    delete iPurchaseOptionId;
    delete iPurchaseOptionName;
    delete iPurchaseOptionPrice;
    delete iFinalPrice;
    delete iPaymentMethodName;
    delete iDownloadedFiles;
    
    ResetAndDestroyWithCast< CNcdPurchaseInstallInfo >( iInstallInfo );

    delete iIcon;
    delete iDownloadAccessPoint;
    delete iDescription;
    delete iVersion;
    delete iServerUri;
    delete iOriginNodeId;
    delete iAttributes;
    }

MNcdPurchaseDetails::TState CNcdPurchaseDetails::State() const
    {
    // The state is automatically checked here without using any
    // state member variable. At the moment, a member variable for 
    // the state and setter function for the state can not be used
    // because, for example, PC-client does not support it.

    // Notice, that here we do not use the last operation error code
    // to check if the last operation has failed. We just give, the current
    // state. It is up to the user of the purchase details to do more checking
    // related to the error code if more information is needed, for example, 
    // to check if some operation has failed and the state has not progressed
    // because of that.
    
    if ( iInstallInfo.Count() > 0
         && iInstallInfo.Count() >= iDownloadInfo.Count() )
        {
        // All the items have been installed.
        // So, we can suppose that the installation was a success.
        // So, return installed-state.
        return EStateInstalled;            
        }
    else if ( iDownloadedFiles 
              && iDownloadedFiles->Count() > 0 
              && iDownloadedFiles->Count() >= iDownloadInfo.Count() ) 
        {
        // All of the items have been downloaded,
        // So, we can suppose that the download was a success.
        return EStateDownloaded;            
        }
    else
        {
        // Because an item has not been yet fully downloaded or installed,
        // its state is purchased-state.
        return EStatePurchased;        
        }
    }

TUid CNcdPurchaseDetails::ClientUid() const
    {
    return iClientUid;
    }

const TDesC& CNcdPurchaseDetails::Namespace() const
    {
    return *iNamespace;
    }

const TDesC& CNcdPurchaseDetails::EntityId() const
    {
    return *iEntityId;
    }

const TDesC& CNcdPurchaseDetails::ItemName() const
    {
    return *iItemName;
    }

TUint CNcdPurchaseDetails::ItemPurpose() const
    {
    return iItemPurpose;
    }

const TDesC& CNcdPurchaseDetails::CatalogSourceName() const
    {
    return *iCatalogSourceName;
    }

TArray< MNcdPurchaseDownloadInfo* >
    CNcdPurchaseDetails::DownloadInfoL() const
    {
    return iDownloadInfo.Array();
    }

const TDesC& CNcdPurchaseDetails::PurchaseOptionId() const
    {
    return *iPurchaseOptionId;
    }

const TDesC& CNcdPurchaseDetails::PurchaseOptionName() const
    {
    return *iPurchaseOptionName;
    }

const TDesC& CNcdPurchaseDetails::PurchaseOptionPrice() const
    {
    return *iPurchaseOptionPrice;
    }

const TDesC& CNcdPurchaseDetails::FinalPrice() const
    {
    return *iFinalPrice;
    }

const TDesC& CNcdPurchaseDetails::PaymentMethodName() const
    {
    return *iPaymentMethodName;
    }

TTime CNcdPurchaseDetails::PurchaseTime() const
    {
    return iPurchaseTime;
    }

const MDesCArray& CNcdPurchaseDetails::DownloadedFiles() const
    {
    return *iDownloadedFiles;
    }

TArray< MNcdPurchaseInstallInfo* > CNcdPurchaseDetails::InstallInfoL() const
    {
    return iInstallInfo.Array();
    }

const TDesC8& CNcdPurchaseDetails::Icon() const
    {
    DASSERT( iIcon );
    return *iIcon;
    }
    
TBool CNcdPurchaseDetails::HasIcon() const
    {
    DLTRACE(("iHasIcon: %d",iHasIcon));
    return iHasIcon;
    }

const TDesC& CNcdPurchaseDetails::DownloadAccessPoint() const
    {
    return *iDownloadAccessPoint;
    }

const TDesC& CNcdPurchaseDetails::Description() const
    {
    return *iDescription;
    }

const TDesC& CNcdPurchaseDetails::Version() const
    {
    return *iVersion;
    }

const TDesC& CNcdPurchaseDetails::ServerUri() const
    {
    return *iServerUri;
    }

MNcdPurchaseDetails::TItemType CNcdPurchaseDetails::ItemType() const
    {
    return iItemType;
    }
    
TInt CNcdPurchaseDetails::TotalContentSize() const 
    {
    return iTotalContentSize;
    }
    
const TDesC& CNcdPurchaseDetails::OriginNodeId() const
    {
    return *iOriginNodeId;
    }

TTime CNcdPurchaseDetails::LastOperationTime() const
    {
    return iLastOperationTime;
    }
    
TInt CNcdPurchaseDetails::LastOperationErrorCode() const
    {
    return iLastOperationErrorCode;
    }


const TDesC& CNcdPurchaseDetails::AttributeString( 
    TPurchaseAttribute aAttribute ) const
    {
    DLTRACEIN((""));
    return iAttributes->AttributeString16( aAttribute );    
    }


TInt32 CNcdPurchaseDetails::AttributeInt32( 
    TPurchaseAttribute aAttribute ) const
    {
    return iAttributes->AttributeInt32( aAttribute );    
    }


EXPORT_C void CNcdPurchaseDetails::ExternalizeL( RWriteStream& aStream ) const
    {
    TUint count = 0;

    aStream.WriteInt32L( iClientUid.iUid );
    ExternalizeDesL( *iNamespace, aStream );
    ExternalizeDesL( *iEntityId, aStream );
    ExternalizeDesL( *iItemName, aStream );
    aStream.WriteUint32L( iItemPurpose );
    ExternalizeDesL( *iCatalogSourceName, aStream );
    
    count = iDownloadInfo.Count();
    aStream.WriteUint32L( count );
    for ( TInt i = 0; i < count; i++ )
        {
        CNcdPurchaseDownloadInfo* info =
            (CNcdPurchaseDownloadInfo*)iDownloadInfo[i];
        info->ExternalizeL( aStream );
        }
        
    ExternalizeDesL( *iPurchaseOptionId, aStream );
    ExternalizeDesL( *iPurchaseOptionName, aStream );
    ExternalizeDesL( *iPurchaseOptionPrice, aStream );
    ExternalizeDesL( *iFinalPrice, aStream );
    ExternalizeDesL( *iPaymentMethodName, aStream );
    aStream << iPurchaseTime.Int64();
    
    count = iDownloadedFiles->Count();
    aStream.WriteUint32L( count );
    for ( TInt i = 0; i < count; i++ )
        {
        ExternalizeDesL( (*iDownloadedFiles)[i], aStream );
        }
        
    count = iInstallInfo.Count();
    aStream.WriteUint32L( count );
    for ( TInt i = 0; i < count; i++ )
        {
        CNcdPurchaseInstallInfo* info =
            (CNcdPurchaseInstallInfo*)iInstallInfo[i];
        info->ExternalizeL( aStream );
        }

    aStream.WriteInt32L( iHasIcon );
    ExternalizeDesL( *iIcon, aStream );
    ExternalizeDesL( *iDownloadAccessPoint, aStream );
    ExternalizeDesL( *iDescription, aStream );
    ExternalizeDesL( *iVersion, aStream );
    ExternalizeDesL( *iServerUri, aStream );
    aStream.WriteInt32L( iItemType );
    aStream.WriteInt32L( iTotalContentSize );
    ExternalizeDesL( *iOriginNodeId, aStream );

    aStream << LastOperationTime().Int64();
    aStream.WriteInt32L( LastOperationErrorCode() );
    
    iAttributes->ExternalizeL( aStream );
    }

EXPORT_C void CNcdPurchaseDetails::InternalizeL( RReadStream& aStream )
    {
    TInt64 int64 = 0;
    TUint count = 0;
    
    iClientUid = TUid::Uid( aStream.ReadInt32L() );
    InternalizeDesL( iNamespace, aStream );
    InternalizeDesL( iEntityId, aStream );
    InternalizeDesL( iItemName, aStream );
    iItemPurpose = aStream.ReadUint32L();
    InternalizeDesL( iCatalogSourceName, aStream );
    
    count = aStream.ReadUint32L();
    for ( TInt i = 0; i < count; i++ )
        {
        CNcdPurchaseDownloadInfo* info = CNcdPurchaseDownloadInfo::NewLC();
        info->InternalizeL( aStream );
        iDownloadInfo.AppendL( info );
        CleanupStack::Pop( info );
        }
        
    InternalizeDesL( iPurchaseOptionId, aStream );
    InternalizeDesL( iPurchaseOptionName, aStream );
    InternalizeDesL( iPurchaseOptionPrice, aStream );
    InternalizeDesL( iFinalPrice, aStream );
    InternalizeDesL( iPaymentMethodName, aStream );
    aStream >> int64;
    iPurchaseTime = int64;
    
    count = aStream.ReadUint32L();
    for ( TInt i = 0; i < count; i++ )
        {
        HBufC* buf = NULL;
        InternalizeDesL( buf, aStream );
        CleanupStack::PushL( buf );
        iDownloadedFiles->AppendL( *buf );
        CleanupStack::PopAndDestroy( buf );
        }

    count = aStream.ReadUint32L();
    for ( TInt i = 0; i < count; i++ )
        {
        CNcdPurchaseInstallInfo* info = CNcdPurchaseInstallInfo::NewLC();
        info->InternalizeL( aStream );
        User::LeaveIfError( iInstallInfo.Append( info ) );
        CleanupStack::Pop( info );
        }

    iHasIcon = aStream.ReadInt32L();
    InternalizeDesL( iIcon, aStream );
    InternalizeDesL( iDownloadAccessPoint, aStream );
    InternalizeDesL( iDescription, aStream );
    InternalizeDesL( iVersion, aStream );
    InternalizeDesL( iServerUri, aStream );
    iItemType = (MNcdPurchaseDetails::TItemType)aStream.ReadInt32L();
    iTotalContentSize = aStream.ReadInt32L();
    InternalizeDesL( iOriginNodeId, aStream );
    
    TInt64 tmpLastOperationTime( 0 );
    aStream >> tmpLastOperationTime;
    iLastOperationTime = tmpLastOperationTime;
    
    iLastOperationErrorCode = aStream.ReadInt32L();
    
    iAttributes->InternalizeL( aStream );    
    }

EXPORT_C void CNcdPurchaseDetails::SetClientUid( TUid aClientUid )
    {
    iClientUid = aClientUid;
    }

EXPORT_C void CNcdPurchaseDetails::SetNamespaceL( const TDesC& aNamespace )
    {
    AssignDesL( iNamespace, aNamespace );
    }

EXPORT_C void CNcdPurchaseDetails::SetNamespace( HBufC* aNamespace )
    {
    AssignHBufC( iNamespace, aNamespace );
    }

EXPORT_C void CNcdPurchaseDetails::SetEntityIdL( const TDesC& aEntityId )
    {
    AssignDesL( iEntityId, aEntityId );
    }

EXPORT_C void CNcdPurchaseDetails::SetEntityId( HBufC* aEntityId )
    {
    AssignHBufC( iEntityId, aEntityId );
    }

EXPORT_C void CNcdPurchaseDetails::SetItemNameL( const TDesC& aItemName )
    {
    AssignDesL( iItemName, aItemName );
    }

EXPORT_C void CNcdPurchaseDetails::SetItemName( HBufC* aItemName )
    {
    AssignHBufC( iItemName, aItemName );
    }

EXPORT_C void CNcdPurchaseDetails::SetItemPurpose( TUint aItemPurpose )
    {
    iItemPurpose = aItemPurpose;
    }

EXPORT_C void CNcdPurchaseDetails::SetCatalogSourceNameL(
    const TDesC& aCatalogSourceName )
    {
    AssignDesL( iCatalogSourceName, aCatalogSourceName );
    }

EXPORT_C void CNcdPurchaseDetails::SetCatalogSourceName( HBufC* aCatalogSourceName )
    {
    AssignHBufC( iCatalogSourceName, aCatalogSourceName );
    }

EXPORT_C void CNcdPurchaseDetails::SetDownloadInfoL(
    const TArray< MNcdPurchaseDownloadInfo* >& aDownloadInfo )
    {
    DLTRACEIN((""));
    
    RPointerArray< MNcdPurchaseDownloadInfo > tempArray;
    // Can't push to cleanup stack since MNcdPurchaseDownloadInfo cannot be
    // deleted but the array owns the objects
    
    TRAPD( err, 
        {
        TInt count = aDownloadInfo.Count();
        tempArray.ReserveL( count );
        for ( TInt i = 0; i < count; i++ )
            {
            CNcdPurchaseDownloadInfo* info =
                CNcdPurchaseDownloadInfo::NewLC( *aDownloadInfo[i] );
            tempArray.AppendL( info );
            CleanupStack::Pop( info );
            }
        });
    if ( err != KErrNone ) 
        {
        DLERROR(("%d", err));
        ResetAndDestroyWithCast<CNcdPurchaseDownloadInfo>( tempArray );
        User::Leave( err );
        }
        
    ResetAndDestroyWithCast<CNcdPurchaseDownloadInfo>( iDownloadInfo );
    iDownloadInfo = tempArray;
    }



EXPORT_C void CNcdPurchaseDetails::AddDownloadInfoL( 
    MNcdPurchaseDownloadInfo* aDownloadInfo )
    {
    iDownloadInfo.AppendL( aDownloadInfo );
    }


EXPORT_C void CNcdPurchaseDetails::InsertDownloadInfoL( 
    MNcdPurchaseDownloadInfo* aDownloadInfo, 
    TInt aIndex )
    {
    iDownloadInfo.InsertL( aDownloadInfo, aIndex );
    }


EXPORT_C void CNcdPurchaseDetails::RemoveDownloadInfo( TInt aIndex )
    {
    DeleteFromArray<CNcdPurchaseDownloadInfo>( iDownloadInfo, aIndex );
    }
    
    
EXPORT_C TInt CNcdPurchaseDetails::DownloadInfoCount() const
    {
    return iDownloadInfo.Count();
    }
    
    
EXPORT_C CNcdPurchaseDownloadInfo& CNcdPurchaseDetails::DownloadInfo(
    TInt aIndex )
    {
    CNcdPurchaseDownloadInfo* ptr = static_cast<CNcdPurchaseDownloadInfo*>(
        iDownloadInfo[aIndex] );
    return *ptr;
    }
    

EXPORT_C void CNcdPurchaseDetails::SetPurchaseOptionIdL(
    const TDesC& aPurchaseOptionId )
    {
    AssignDesL( iPurchaseOptionId, aPurchaseOptionId );
    }

EXPORT_C void CNcdPurchaseDetails::SetPurchaseOptionId( HBufC* aPurchaseOptionId )
    {
    AssignHBufC( iPurchaseOptionId, aPurchaseOptionId );
    }

EXPORT_C void CNcdPurchaseDetails::SetPurchaseOptionNameL(
    const TDesC& aPurchaseOptionName )
    {
    AssignDesL( iPurchaseOptionName, aPurchaseOptionName );
    }

EXPORT_C void CNcdPurchaseDetails::SetPurchaseOptionName( HBufC* aPurchaseOptionName )
    {
    AssignHBufC( iPurchaseOptionName, aPurchaseOptionName );
    }

EXPORT_C void CNcdPurchaseDetails::SetPurchaseOptionPriceL(
    const TDesC& aPurchaseOptionPrice )
    {
    AssignDesL( iPurchaseOptionPrice, aPurchaseOptionPrice );
    }

EXPORT_C void CNcdPurchaseDetails::SetPurchaseOptionPrice(
    HBufC* aPurchaseOptionPrice )
    {
    AssignHBufC( iPurchaseOptionPrice, aPurchaseOptionPrice );
    }

EXPORT_C void CNcdPurchaseDetails::SetFinalPriceL( const TDesC& aFinalPrice )
    {
    AssignDesL( iFinalPrice, aFinalPrice );
    }

EXPORT_C void CNcdPurchaseDetails::SetFinalPrice( HBufC* aFinalPrice )
    {
    AssignHBufC( iFinalPrice, aFinalPrice );
    }

EXPORT_C void CNcdPurchaseDetails::SetPaymentMethodNameL(
    const TDesC& aPaymentMethodName )
    {
    AssignDesL( iPaymentMethodName, aPaymentMethodName );
    }

EXPORT_C void CNcdPurchaseDetails::SetPaymentMethodName( HBufC* aPaymentMethodName )
    {
    AssignHBufC( iPaymentMethodName, aPaymentMethodName );
    }

EXPORT_C void CNcdPurchaseDetails::SetPurchaseTime( TTime aPurchaseTime )
    {
    iPurchaseTime = aPurchaseTime;
    }

EXPORT_C void CNcdPurchaseDetails::SetDownloadedFiles( CDesCArray* aDownloadedFiles )
    {
    delete iDownloadedFiles;
    iDownloadedFiles = aDownloadedFiles;
    }

EXPORT_C void CNcdPurchaseDetails::SetDownloadedFilesL(
    const MDesCArray& aDownloadedFiles )
    {
    delete iDownloadedFiles;
    iDownloadedFiles = NULL;
    iDownloadedFiles = new (ELeave) CDesCArrayFlat( KListGranularity );
    for ( TInt i = 0; i < aDownloadedFiles.MdcaCount(); i++ )
        {
        iDownloadedFiles->AppendL( aDownloadedFiles.MdcaPoint( i ) );
        }
    }


EXPORT_C void CNcdPurchaseDetails::ReplaceDownloadedFileL( TInt aIndex, 
    const TDesC& aDownloadedFile )
    {    
    iDownloadedFiles->InsertL( aIndex, aDownloadedFile );
    iDownloadedFiles->Delete( aIndex + 1 );
    }
    

EXPORT_C void CNcdPurchaseDetails::AddDownloadedFileL( 
    const TDesC& aDownloadedFile )
    {
    iDownloadedFiles->AppendL( aDownloadedFile );
    }

EXPORT_C void CNcdPurchaseDetails::InsertDownloadedFileL( 
    const TDesC& aDownloadedFile, 
    TInt aIndex )
    {
    iDownloadedFiles->InsertL( aIndex, aDownloadedFile );
    }


EXPORT_C void CNcdPurchaseDetails::RemoveDownloadedFile( 
    TInt aIndex )
    {
    iDownloadedFiles->Delete( aIndex );
    }
    

EXPORT_C void CNcdPurchaseDetails::SetInstallInfoL(
    const TArray< MNcdPurchaseInstallInfo* >& aInstallInfo )
    {
    DLTRACEIN((""));
    RPointerArray<MNcdPurchaseInstallInfo> tempArray;
    
    TRAPD( err, 
        {            
        TInt count = aInstallInfo.Count();
        tempArray.ReserveL( count );
        
        for ( TInt i = 0; i < count; i++ )
            {
            CNcdPurchaseInstallInfo* info =
                CNcdPurchaseInstallInfo::NewLC( *aInstallInfo[i] );
            tempArray.AppendL( info );
            CleanupStack::Pop( info );
            }
        });
    
    if ( err != KErrNone )
        {
        DLERROR(("%d", err));
        ResetAndDestroyWithCast<CNcdPurchaseInstallInfo>( tempArray );    
        User::Leave( err );
        }
        
    ResetAndDestroyWithCast<CNcdPurchaseInstallInfo>( iInstallInfo );
    iInstallInfo = tempArray;
    }


EXPORT_C void CNcdPurchaseDetails::AddInstallInfoL( 
    MNcdPurchaseInstallInfo* aInstallInfo )
    {
    iInstallInfo.AppendL( aInstallInfo );
    }


EXPORT_C void CNcdPurchaseDetails::InsertInstallInfoL( 
    MNcdPurchaseInstallInfo* aInstallInfo,
    TInt aIndex )
    {
    iInstallInfo.InsertL( aInstallInfo, aIndex );
    }


EXPORT_C void CNcdPurchaseDetails::RemoveInstallInfo( TInt aIndex )
    {
    DeleteFromArray<CNcdPurchaseInstallInfo>( iInstallInfo, aIndex );
    }
    

EXPORT_C TInt CNcdPurchaseDetails::InstallInfoCount() const
    {
    return iInstallInfo.Count();
    }
    
    
EXPORT_C CNcdPurchaseInstallInfo& CNcdPurchaseDetails::InstallInfo( 
    TInt aIndex )
    {
    DASSERT( aIndex >= 0 && aIndex < iInstallInfo.Count() );
    CNcdPurchaseInstallInfo* ptr = static_cast<CNcdPurchaseInstallInfo*>( 
        iInstallInfo[aIndex] );
    return *ptr;
    }
    

EXPORT_C void CNcdPurchaseDetails::SetIconL( const TDesC8& aIcon )
    {
    HBufC8* tmpIcon = aIcon.AllocL();
    delete iIcon;
    iIcon = tmpIcon;
    iHasIcon = ETrue;
    }

EXPORT_C void CNcdPurchaseDetails::SetIcon( HBufC8* aIcon )
    {
    delete iIcon;
    iIcon = aIcon;
    iHasIcon = ETrue;
    }
    
EXPORT_C HBufC8* CNcdPurchaseDetails::GetIconData()
    {
    HBufC8* tmpIcon = iIcon;
    iIcon = NULL;
    return tmpIcon;
    }
    
EXPORT_C void CNcdPurchaseDetails::SetHasIcon( TBool aHasIcon )
    {
    iHasIcon = aHasIcon;
    }
    
EXPORT_C void CNcdPurchaseDetails::SetDownloadAccessPointL(
    const TDesC& aAccessPoint )
    {
    AssignDesL( iDownloadAccessPoint, aAccessPoint );
    }

EXPORT_C void CNcdPurchaseDetails::SetDownloadAccessPoint(
    HBufC* aAccessPoint )
    {
    AssignHBufC( iDownloadAccessPoint, aAccessPoint );
    }

EXPORT_C void CNcdPurchaseDetails::SetDescriptionL(
    const TDesC& aDescription )
    {
    AssignDesL( iDescription, aDescription );
    }

EXPORT_C void CNcdPurchaseDetails::SetDescription(
    HBufC* aDescription )
    {
    AssignHBufC( iDescription, aDescription );
    }

EXPORT_C void CNcdPurchaseDetails::SetVersionL(
    const TDesC& aVersion )
    {
    AssignDesL( iVersion, aVersion );
    }

EXPORT_C void CNcdPurchaseDetails::SetVersion(
    HBufC* aVersion )
    {
    AssignHBufC( iVersion, aVersion );
    }

EXPORT_C void CNcdPurchaseDetails::SetServerUriL(
    const TDesC& aServerUri )
    {
    AssignDesL( iServerUri, aServerUri );
    }

EXPORT_C void CNcdPurchaseDetails::SetServerUri(
    HBufC* aServerUri )
    {
    AssignHBufC( iServerUri, aServerUri );
    }

EXPORT_C void CNcdPurchaseDetails::SetItemType(
    TItemType aItemType )
    {
    iItemType = aItemType;
    }
    
EXPORT_C void CNcdPurchaseDetails::SetTotalContentSize( TInt aSize )
    {
    iTotalContentSize = aSize;
    }
    
EXPORT_C void CNcdPurchaseDetails::SetOriginNodeIdL( const TDesC& aOriginNodeId )
    {
    AssignDesL( iOriginNodeId, aOriginNodeId );
    }
    
EXPORT_C void CNcdPurchaseDetails::SetOriginNodeId( HBufC* aOriginNodeId )
    {
    AssignHBufC( iOriginNodeId, aOriginNodeId );
    }

EXPORT_C void CNcdPurchaseDetails::SetLastUniversalOperationTime()
    {
    TTime time;
    time.UniversalTime();
    SetLastOperationTime( time );
    }

EXPORT_C void CNcdPurchaseDetails::SetLastOperationTime( const TTime& aTime )
    {
    iLastOperationTime = aTime;
    }
    
EXPORT_C void CNcdPurchaseDetails::SetLastOperationErrorCode( TInt aError )
    {
    iLastOperationErrorCode = aError;
    }


EXPORT_C void CNcdPurchaseDetails::SetAttributeL( 
    TPurchaseAttribute aAttribute, 
    TInt32 aValue )
    {    
    iAttributes->SetAttributeL( aAttribute, aValue );
    }


EXPORT_C void CNcdPurchaseDetails::SetAttributeL( 
    TPurchaseAttribute aAttribute, 
    const TDesC& aValue )
    {    
    iAttributes->SetAttributeL( aAttribute, aValue );
    }

EXPORT_C void CNcdPurchaseDetails::ExternalizeAttributesL( 
    RWriteStream& aStream ) const
    {
    DLTRACEIN((""));
    iAttributes->ExternalizeL( aStream );
    }

EXPORT_C void CNcdPurchaseDetails::InternalizeAttributesL( 
    RReadStream& aStream )
    {
    DLTRACEIN((""));
    iAttributes->InternalizeL( aStream );
    }


void CNcdPurchaseDetails::CreateAttributesL()
    {
    DLTRACEIN((""));
    if ( !iAttributes ) 
        {
        iAttributes = CNcdAttributes::NewL( 
            MNcdPurchaseDetails::EPurchaseAttributeInternal );
        }
    }


void CNcdPurchaseDetails::ConstructL()
    {
    iNamespace = KNullDesC().AllocL();
    iEntityId = KNullDesC().AllocL();
    iItemName = KNullDesC().AllocL();
    iCatalogSourceName = KNullDesC().AllocL();
    iPurchaseOptionId = KNullDesC().AllocL();
    iPurchaseOptionName = KNullDesC().AllocL();
    iPurchaseOptionPrice = KNullDesC().AllocL();
    iFinalPrice = KNullDesC().AllocL();
    iPaymentMethodName = KNullDesC().AllocL();
    iDownloadedFiles = new (ELeave) CDesCArrayFlat( KListGranularity );
    iIcon = KNullDesC8().AllocL();
    iDownloadAccessPoint = KNullDesC().AllocL();
    iDescription = KNullDesC().AllocL();
    iVersion = KNullDesC().AllocL();
    iServerUri = KNullDesC().AllocL();
    iItemType = MNcdPurchaseDetails::EItem;
    iOriginNodeId = KNullDesC().AllocL();
    CreateAttributesL();
    }

CNcdPurchaseDetails::CNcdPurchaseDetails()
    : iPurchaseTime( 0 ),
      iHasIcon( EFalse ), 
      iTotalContentSize( 0 ), 
      iLastOperationTime( 0 ),
      iLastOperationErrorCode( KErrNone )
      
    {
    iClientUid = TUid::Null();
    }

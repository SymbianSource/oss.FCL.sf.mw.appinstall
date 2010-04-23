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
* Description:   Implementation of NcdPurchaseHistoryUtils
*
*/


#include "ncdpurchasehistoryutils.h"

#include "ncdpurchasedetails.h"
#include "ncdpurchasedownloadinfo.h"
#include "ncdpurchasehistorydbimpl.h"
#include "ncdpurchaseinstallinfo.h"
#include "ncdutils.h"
#include "ncdnodeidentifier.h"
#include "ncderrors.h"
#include "catalogsutils.h"
#include "ncddownloadinfo.h"

#include "catalogsdebug.h"

// ---------------------------------------------------------------------------
// Purchase details getter
// ---------------------------------------------------------------------------
//        
CNcdPurchaseDetails* NcdPurchaseHistoryUtils::PurchaseDetailsLC( 
    CNcdPurchaseHistoryDb& aPurchaseHistory,
    const TUid& aClientUid,
    const CNcdNodeIdentifier& aMetadataId,
    TBool aLoadIcon )
    {
    DLTRACEIN((""))    
        
    RArray<TUid> clientUid;
    CleanupClosePushL( clientUid );
    clientUid.AppendL( aClientUid );
    
    // Create a filter
    CNcdPurchaseHistoryFilter* filter = CNcdPurchaseHistoryFilter::NewL(
        clientUid.Array(), 
        aMetadataId.NodeNameSpace(), 
        aMetadataId.NodeId(), 
        0 );
        
    CleanupStack::PopAndDestroy( &clientUid );    
    CleanupStack::PushL( filter );
    
    // Get purchase ids
    RArray<TUint> purchaseIds = aPurchaseHistory.PurchaseIdsL( *filter, 
        CNcdPurchaseHistoryDb::ENewestFirst );
    
    CleanupStack::PopAndDestroy( filter );
    CleanupClosePushL( purchaseIds );
    
    // Check that there's at least one purchase    
    if ( !purchaseIds.Count() ) 
        {
        DLERROR(("No matching purchases found"));
        User::Leave( KNcdErrorNoPurchaseInformation );
        }
        
    // Choose the newest purchase
    CNcdPurchaseDetails* purchase = 
        aPurchaseHistory.PurchaseL( purchaseIds[0], aLoadIcon );
        
	CleanupStack::PopAndDestroy( &purchaseIds );	
    CleanupStack::PushL( purchase );
    return purchase;
    }
    
// ---------------------------------------------------------------------------
// Returns true if the given info is a dependency
// ---------------------------------------------------------------------------
//
TBool NcdPurchaseHistoryUtils::IsDependency( 
    const MNcdPurchaseDownloadInfo& aInfo )    
    {
    DLTRACEIN((""));
    return IsOneOf( aInfo.ContentUsage(), 
        MNcdPurchaseDownloadInfo::EDependency, 
        MNcdPurchaseDownloadInfo::ELauncher, 
        MNcdPurchaseDownloadInfo::ELauncherOpen );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void NcdPurchaseHistoryUtils::RemoveDependenciesL( 
    CNcdPurchaseDetails& aDetails )
    {
    DLTRACEIN((""));    
    TInt count = aDetails.DownloadInfoCount();
    TInt i = 0;
    
    while ( i < count && IsDependency( aDetails.DownloadInfo( i ) ) ) 
        {
        i++;
        }
    
    DLTRACE(("Deps: %d", i));
    while ( i-- ) 
        {
        aDetails.RemoveDownloadInfo( i );
        aDetails.RemoveDownloadedFile( i );
        aDetails.RemoveInstallInfo( i );
        }    
    }



// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void NcdPurchaseHistoryUtils::UpdateDependenciesL( 
    CNcdPurchaseDetails& aDetails,
    const RPointerArray<CNcdDownloadInfo>& aNewDependencies )
    {
    DLTRACEIN((""));
    
    RemoveDependenciesL( aDetails );
    
    CNcdPurchaseDownloadInfo* downloadInfo( NULL );
    CNcdPurchaseInstallInfo* installInfo( NULL );
    TInt count = aNewDependencies.Count();
    
    // Insert new dependencies to the beginning of the arrays    
    while ( count-- )
        {
        downloadInfo = 
            CNcdPurchaseDownloadInfo::NewLC( 
                *aNewDependencies[ count ] );
        aDetails.InsertDownloadInfoL( downloadInfo, 0 );           
        CleanupStack::Pop( downloadInfo );        
        
        DLTRACE(( _L("Adding install info, uid: %x, version: %S"), 
            aNewDependencies[ count ]->ContentId(), 
            &aNewDependencies[ count ]->ContentVersion() ));
            
        installInfo = CNcdPurchaseInstallInfo::NewLC();
        installInfo->SetApplicationUid( 
            aNewDependencies[ count ]->ContentId() );
        installInfo->SetApplicationVersionL( 
            aNewDependencies[ count ]->ContentVersion() );
        aDetails.InsertInstallInfoL( installInfo, 0 );           
        CleanupStack::Pop( installInfo );
        
        aDetails.InsertDownloadedFileL( KNullDesC, 0 );
        }
        
    DLTRACEOUT(("Download infos added successfully"));            
    }

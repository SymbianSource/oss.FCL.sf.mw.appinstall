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
* Description:  
*
*/


#ifndef NCDPURCHASEHISTORYUTILS_H
#define NCDPURCHASEHISTORYUTILS_H


#include <e32base.h>

class CNcdPurchaseDetails;
class CNcdPurchaseHistoryDb;
class CNcdNodeIdentifier;
class MNcdPurchaseDetails;
class CNcdDownloadInfo;
class MNcdPurchaseDownloadInfo;

/**
 * Helper methods for commonly used purchase history operations
 */
class NcdPurchaseHistoryUtils
    {
public:    

    /**
     * Get purchase history details
     */
    static CNcdPurchaseDetails* PurchaseDetailsLC( 
        CNcdPurchaseHistoryDb& aPurchaseHistory,
        const TUid& aClientUid,
        const CNcdNodeIdentifier& aMetadataId,
        TBool aLoadIcon = ETrue );
    
    static TBool IsDependency( 
        const MNcdPurchaseDownloadInfo& aInfo );

    static void RemoveDependenciesL( 
        CNcdPurchaseDetails& aDetails );
    
    static void UpdateDependenciesL( 
        CNcdPurchaseDetails& aDetails,
        const RPointerArray<CNcdDownloadInfo>& aNewDependencies );
        
        
    };
    
#endif // NCDPURCHASEHISTORYUTILS_H    
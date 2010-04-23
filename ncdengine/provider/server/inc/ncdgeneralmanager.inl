/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Inline implementation of CNcdGeneralManager
*
*/


#include "catalogsdebug.h"

inline const TUid& CNcdGeneralManager::FamilyId() const
    {
    return iFamilyId;
    }


inline const TDesC& CNcdGeneralManager::FamilyName() const
    {
    return iFamilyName;
    }

inline void CNcdGeneralManager::SetPurchaseHistory( 
    CNcdPurchaseHistoryDb& aPurchaseHistory )
    {
    iPurchaseHistory = &aPurchaseHistory;
    }

inline CNcdPurchaseHistoryDb& CNcdGeneralManager::PurchaseHistory() const
    {
    DASSERT( iPurchaseHistory );
    return *iPurchaseHistory;
    }


inline void CNcdGeneralManager::SetStorageManager( 
    MNcdStorageManager& aStorageManager )
    {
    iStorageManager = &aStorageManager;
    }

inline MNcdStorageManager& CNcdGeneralManager::StorageManager() const
    {
    DASSERT( iStorageManager );
    return *iStorageManager;
    }


inline void CNcdGeneralManager::SetConfigurationManager( 
    MNcdConfigurationManager& aConfigurationManager )
    {
    iConfigurationManager = &aConfigurationManager;
    }

inline MNcdConfigurationManager& CNcdGeneralManager::ConfigurationManager() const
    {
    DASSERT( iConfigurationManager );
    return *iConfigurationManager;
    }

inline void CNcdGeneralManager::SetProtocolManager( 
    MNcdProtocol& aProtocolManager )
    {
    iProtocolManager = &aProtocolManager;    
    }

inline MNcdProtocol& CNcdGeneralManager::ProtocolManager() const
    {
    DASSERT( iProtocolManager );
    return *iProtocolManager;
    }
   
inline void CNcdGeneralManager::SetHttpUtils( CNcdHttpUtils& aHttpUtils )
    {
    iHttpUtils = &aHttpUtils;
    }

inline CNcdHttpUtils& CNcdGeneralManager::HttpUtils() const
    {
    DASSERT( iHttpUtils );
    return *iHttpUtils;
    }


inline void CNcdGeneralManager::SetNodeManager( 
    CNcdNodeManager& aNodeManager )
    {
    iNodeManager = &aNodeManager;
    }

inline CNcdNodeManager& CNcdGeneralManager::NodeManager() const
    {
    DASSERT( iNodeManager );
    return *iNodeManager;
    }

inline void CNcdGeneralManager::SetAccessPointManager( 
    MCatalogsAccessPointManager& aAccessPointManager )
    {
    iAccessPointManager = &aAccessPointManager;
    }

inline MCatalogsAccessPointManager& CNcdGeneralManager::AccessPointManager() const
    {
    DASSERT( iAccessPointManager );
    return *iAccessPointManager;    
    }


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
* Description:   Class CNcdGeneralManager declaration
*
*/


#ifndef C_NCD_GENERALMANAGER_H
#define C_NCD_GENERALMANAGER_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION

class CNcdPurchaseHistoryDb;
class MNcdStorageManager;
class CNcdConfigurationManager;
class MNcdConfigurationManager;
class MNcdProtocol;
class CNcdHttpUtils;
class CNcdNodeManager;
class MCatalogsAccessPointManager;

/**
 *  CNcdGeneralManager
 * 
 */
class CNcdGeneralManager : public CBase
    {
public: // Constructors and destructor
    
    /**
     * Constructor
     */
    CNcdGeneralManager( 
        const TUid& aFamilyId,
        const TDesC& aFamilyName );


    /**
     * Destructor.
     */
    ~CNcdGeneralManager();


public:
    
    const TUid& FamilyId() const;
    
    const TDesC& FamilyName() const;
    
    CNcdPurchaseHistoryDb& PurchaseHistory() const;
    
    void SetPurchaseHistory( CNcdPurchaseHistoryDb& aPurchaseHistoryManager );
    
    MNcdStorageManager& StorageManager() const;
    
    void SetStorageManager( MNcdStorageManager& aStorageManager );
    
    MNcdConfigurationManager& ConfigurationManager() const;
        
    void SetConfigurationManager( 
        MNcdConfigurationManager& aConfigurationManager );
    
    
    void SetProtocolManager( MNcdProtocol& aProtocolManager );
    
    MNcdProtocol& ProtocolManager() const;
    
    void SetHttpUtils( CNcdHttpUtils& aHttpUtils );
    
    CNcdHttpUtils& HttpUtils() const;
    
    void SetNodeManager( CNcdNodeManager& aNodeManager );
    
    CNcdNodeManager& NodeManager() const;
    
    
    void SetAccessPointManager( 
        MCatalogsAccessPointManager& aAccessPointManager );
    
    MCatalogsAccessPointManager& AccessPointManager() const;
    

private:
    
    TUid iFamilyId;
    const TDesC& iFamilyName;
    CNcdPurchaseHistoryDb* iPurchaseHistory;            // not owned
    MNcdStorageManager* iStorageManager;                // not owned
    MNcdConfigurationManager* iConfigurationManager;    // not owned
    MNcdProtocol* iProtocolManager;                     // not owned
    CNcdHttpUtils* iHttpUtils;                          // not owned
    CNcdNodeManager* iNodeManager;                      // not owned
    MCatalogsAccessPointManager* iAccessPointManager;   // not owned
    
    };

#include "ncdgeneralmanager.inl"

#endif // C_NCD_GENERALMANAGER_H

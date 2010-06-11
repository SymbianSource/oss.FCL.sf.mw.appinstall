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
* Description:   Contains CNcdNodeCacheCleanerManager class
*
*/


#ifndef NCD_NODE_CACHE_CLEANER_MANAGER_H
#define NCD_NODE_CACHE_CLEANER_MANAGER_H


#include <e32base.h>

// For array and uid
#include <e32cmn.h>


class CNcdNodeManager;
class CNcdNodeDbManager;
class CNcdNodeFactory;
class CNcdNodeCacheCleaner;
class CNcdGeneralManager;

/**
 * This class object provides db cleaner maanger that can be used
 * to get db cache cleaners for certain clients that are identified
 * by the Uid.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdNodeCacheCleanerManager : public CBase
{

public:

    /**
     * NewL
     *
     * @param aNodeManager
     * @param aNodeDbManager
     * @param aDbDefaultMaxSize The default value for the maximum size of 
     * the database in bytes. If this size is exceeded, then the db should 
     * be cleaned.
     * @param aNodeFactory
     * @return CNcdNodeCacheCleanerManager* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeCacheCleanerManager* NewL( CNcdGeneralManager& aGeneralManager,
                                              CNcdNodeDbManager& aNodeDbManager,
                                              TInt aDbDefaultMaxSize,
                                              CNcdNodeFactory& aNodeFactory );

    /**
     * @see CNcdNodeCacheCleaner::NewL
     */
    static CNcdNodeCacheCleanerManager* NewLC( CNcdGeneralManager& aGeneralManager,
                                        CNcdNodeDbManager& aNodeDbManager,
                                        TInt aDbDefaultMaxSize,
                                        CNcdNodeFactory& aNodeFactory );


    /**
     * Destructor
     */
    virtual ~CNcdNodeCacheCleanerManager();


    /** 
     * Returns the cache cleaner or creates a new one if one did not exist
     * for the client of the given uid.
     *
     * @return CNcdNodeCacheCleaner& Reference to the cache cleaner that
     * corresponds the client of the given uid.
     */
    CNcdNodeCacheCleaner& CacheCleanerL( const TUid& aClientUid );


    /**
     * @return TInt The maximum size of the db in bytes.
     */
    TInt DbMaxSize() const;
    
    /**
     * This function can be used to set the client specific max size
     * for the db. This value replaces the default value that was set in
     * the constructor of this class.
     *
     * @param aDbMaxSize The maximum size for the db in bytes.
     * If this size is exceeded, then the cleanup should be started and
     * db cleaned until the db max size is not exceeded any more.
     */
    void SetDbMaxSize( const TInt aDbMaxSize );


    /** 
     * Uses all the cache cleaners to check the db sizes.
     * In other words, calls CheckDbSizeL function of all the cache cleaners
     * created by this class object.
     */
    void CheckAllL() const;


protected:

    /**
     * Constructor
     */
    CNcdNodeCacheCleanerManager( CNcdGeneralManager& aGeneralManager,
                                 CNcdNodeDbManager& aNodeDbManager,
                                 TInt aDbDefaultMaxSize,
                                 CNcdNodeFactory& aNodeFactory );

    /**
     * ConstructL
     */
    virtual void ConstructL();


    /**
     * @return CNcdNodeManager& Manager that owns this cleaner
     */
    CNcdNodeManager& NodeManager() const;

    /**
     * @return CNcdNodeDbManager& Database manager that provides functions
     * to handle database content.
     */
    CNcdNodeDbManager& NodeDbManager() const;

    /**
     * @return CNcdNodeFactory& Factory that is used to create node
     * related class objects.
     */
    CNcdNodeFactory& NodeFactory() const;


private:

    // Prevent these two if they are not implemented
    CNcdNodeCacheCleanerManager( const CNcdNodeCacheCleanerManager& aObject );
    CNcdNodeCacheCleanerManager& operator =( const CNcdNodeCacheCleanerManager& aObject );

private: // data

    CNcdGeneralManager& iGeneralManager;
    
    // Node manager that owns this class object.   
    CNcdNodeManager& iNodeManager;
    
    // Data base manager is used to remove the nodes from the db.
    CNcdNodeDbManager& iNodeDbManager;

    // This contains the max db byte size
    TInt iDbMaxSize;
    
    // The factory is used to create the correct nodes.
    CNcdNodeFactory& iNodeFactory;

    // This array contains the cleaners.
    RPointerArray<CNcdNodeCacheCleaner> iCleaners;
};

#endif // NCD_NODE_CACHE_CLEANER_MANAGER_H

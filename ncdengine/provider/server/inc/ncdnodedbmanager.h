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
* Description:   Contains CNcdNodeDbManager class
*
*/


#ifndef NCD_NODE_DB_MANAGER_H
#define NCD_NODE_DB_MANAGER_H


#include <e32base.h>
#include <f32file.h>
#include <e32cmn.h>
#include <bamdesca.h>

#include "ncdstoragedataitem.h"
#include "ncdnodeclassids.h"
#include "ncddatabaseitems.h"

class MNcdStorageManager;
class MNcdStorage;
class CNcdNodeIdentifier;

/**
 *  CNodeDbManager provides functions manage 
 *  node db information. Functions to read, write and remove
 *  data are provided.
 */
class CNcdNodeDbManager : public CBase
    {

public:

    /**
     * NewL
     *
     * @return CNcdNodeDbManager* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeDbManager* NewL(
        MNcdStorageManager& aStorageManager );

    /**
     * NewLC
     *
     * @return CNcdNodeDbManager* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeDbManager* NewLC(
        MNcdStorageManager& aStorageManager );


    /**
     * Destructor
     *
     * Deletes the nodes from the node cache.
     */
    virtual ~CNcdNodeDbManager();


public: // Node db manager configuration functions    

    /**
     * Adds the given identifier to the list of nodes that are never removed from DB.
     *
     * @param aIdentifier The node identifier.
     */
    void AddDoNotRemoveNodeIdentifierL( const CNcdNodeIdentifier& aNodeIdentifier );
    
    /**
     * Adds the given identifiers to the list of nodes that are never removed from DB.
     *
     * @param aIdentifiers The node identifiers.
     */
    void AddDoNotRemoveNodeIdentifiersL(
        const RPointerArray<CNcdNodeIdentifier>& aNodeIdentifiers );
     
    /**
     * Removes the given identifier from the list of nodes that are never removed from DB.
     *
     * @param aIdentifier The node identifier.
     */
    void RemoveDoNotRemoveNodeIdentifier( const CNcdNodeIdentifier& aNodeIdentifier );

public: // Database info functions

    /**
     * @param aClientUid The uid of the client whose storage is 
     * examined.
     * @param aSkipNamespaces The sizes of these namespaces are ignored when
     * the total size is calculated. Ownership is not transferred.
     * @return TInt the size in bytes of the whole storage
     * of the specified client.
     */
    TInt StorageSizeL( const TUid& aClientUid,
                       const MDesCArray& aSkipNamespaces );
        

public: // Database read functions
    
    /**
     * Reads binary data from the database
     * @param aIdentifier Contains id information that is
     * used to read right info from db.
     * @param aClassType Type of the data.
     * @return HBufC8* Data from db.
     */
    HBufC8* ReadDataFromDatabaseL( 
        const CNcdNodeIdentifier& aIdentifier,
        const NcdNodeClassIds::TNcdNodeClassType aClassType );
    
    /**
     * @see CNcdNodeDbManager::ReadDataFromDatabaseL
     */                                
    HBufC8* ReadDataFromDatabaseLC( 
        const CNcdNodeIdentifier& aIdentifier,
        const NcdNodeClassIds::TNcdNodeClassType aClassType );


    /** 
     * This function is used to start the database action
     * that will get the data for some object from the
     * storage database.
     * After the data has been found from the storage,
     * InternalizeL function of the given data item is called.
     *
     * @param aIdentifier Contains id information that is
     * used to read right info from db.
     * @param aDataItem InternalizeL of this object is called
     * when the data is read from db.
     * @param aClassType Type of the data.
     */    
    void StartStorageLoadActionL( 
        const CNcdNodeIdentifier& aIdentifier,
        MNcdStorageDataItem& aDataItem,
        const NcdNodeClassIds::TNcdNodeClassType aClassType );


    /** 
     *  This function will return the array about the identifiers found from
     *  the database. Parameters provide ways to limit what identifiers
     *  should be included in return array.
     *
     *  @note Identifiers are included into the array only once. So, if
     *  multiple db items with different types are having same identifier,
     *  the identifier will be added to target array only once.
     *
     *  @param RPointerArray<CNcdNodeIdentifier> Array that will contain
     *  identifiers of the database items. Ownership of the identifiers
     *  are transmitted for the user of the array.
     *  @param aClientUid Identifies the client whose data is searched.
     *  @param aSkipNamespaces This array contains the namespaces whose
     *  data should not be returned.
     *  @param aAcceptClassType This array contains the class types that
     *  are accepted for the items. The items of the class types 
     *  that are not included in this array will not be included into
     *  the return array.
     */
    void GetAllClientItemIdentifiersL( 
        RPointerArray<CNcdNodeIdentifier>& aItemIdentifiers,
        const TUid& aClientUid,
        const MDesCArray& aSkipNamespaces,
        const RArray<NcdNodeClassIds::TNcdNodeClassType>& aAcceptClassTypes );
                             

public: // Database write functions

    /**
     * This function is used to start the database action
     * that will save the data of the data item into the db.
     * The ExternalizeL function of the data item is called.
     * So, the item may insert its data into the given stream.
     *
     * @param aIdentifier Contains id information that is
     * used to read right info from db.
     * @param aDataItem ExternalizeL of this object is called
     * when the data is written into db.
     * @param aClassType Type of the data.
     */
    void SaveDataIntoDatabaseL( 
        const CNcdNodeIdentifier& aIdentifier,
        MNcdStorageDataItem& aDataItem,
        const NcdNodeClassIds::TNcdNodeClassType aClassType );        


public: // Database remove functions

    /**
     * Removes certain data row from the db table according
     * to the given values that are used as the key values.
     *
     * @param aIdentifier The identifier that contains key values.
     * @param aClassType The class type tells the general type of the data
     * that should be removed.
     */
    void RemoveDataFromDatabaseL( 
        const CNcdNodeIdentifier& aIdentifier,
        const NcdNodeClassIds::TNcdNodeClassType aClassType );        

    /**
     * Removes certain data rows from the db table according
     * to the given values that are used as the key values.
     * This works like the function above, but commits the
     * removal operation rarely. This should speed
     * up the removal process.
     *
     * @see CNcdNodeDbManager::RemoveDataFromDatabaseL
     *
     * @param aIdentifiers The identifier array that contains key values. 
     * Ownership is not tranferred.
     * @param aClassTypes The class type array tells the general types of the data
     * that should be removed. Ownership is not transferred.
     */
    void RemoveDataFromDatabaseL( 
        const RPointerArray<CNcdNodeIdentifier>& aIdentifiers,
        const RArray<NcdNodeClassIds::TNcdNodeClassType>& aClassTypes,
        TBool aCompact = ETrue );
    
    
    /**
     * Removes everything except identifiers that are in the do not remove list
     *  
     * @param aNodeIdentifier Used to get the correct storage   
     */
    void RemoveDataFromDatabaseL( 
        const CNcdNodeIdentifier& aNodeIdentifier,
        const RArray<RNcdDatabaseItems>& aDoNotRemoveItems );

    
    /**
     * Clears client's storage
     *
     * @note All of client's files and databases are 
     * deleted except the ones that are having namespaces
     * defined in the skip array.
     *
     * @param aClientUid Client's uid identifier
     * @param aSkipNamespaces Array contains the descriptors of
     * namespaces that should not be cleared. Ownership is not
     * transferred.
     */
    void ClearClientL( const TUid& aClientUid,
                       const MDesCArray& aSkipNamespaces );
    
    
protected:

    /**
     * Constructor
     */
    CNcdNodeDbManager( MNcdStorageManager& aStorageManager );

    /**
     * ConstructL
     */
    virtual void ConstructL();


    /**
     * Returns an old storage or creates a new one if necessary.
     * Storage will contain the information of the certain client
     * that has been identifier by its UID.
     *
     * @param aIdentifier Identifies what storage is wanted.
     * @return MNcdStorage& Storage matching the identifier info.
     * @exception KErrArgument if aIdentifier contains empty fields.
     * In other error situations, system wide error code.
     */
    MNcdStorage& StorageL( const CNcdNodeIdentifier& aIdentifier  ) const;


    TInt RemoveDataFromNamespaceL( 
        const RPointerArray<CNcdNodeIdentifier>& aDoNotRemoveIds,
        TInt aIndex, 
        TInt aEnd );
                          
    
private:

    // Prevent if not implemented
    CNcdNodeDbManager( const CNcdNodeDbManager& aObject );
    CNcdNodeDbManager& operator =( const CNcdNodeDbManager& aObject );
    
    
private: // data

    // Storage manager is used to load and to save data from
    // databases.
    MNcdStorageManager& iStorageManager;
    
    };


#endif // NCD_NODE_DB_MANAGER_H

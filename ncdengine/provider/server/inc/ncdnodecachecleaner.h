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
* Description:   Contains CNcdNodeCacheCleaner class
*
*/


#ifndef NCD_NODE_CACHE_CLEANER_H
#define NCD_NODE_CACHE_CLEANER_H


// For array and uid
#include <e32cmn.h>
#include <badesca.h>

// For active object
#include <e32base.h>

// For TTime
#include <e32std.h>

#include "ncdnodeclassids.h"
#include "catalogspair.h"

class CNcdNodeManager;
class CNcdNodeDbManager;
class CNcdNodeFactory;
class CNcdNodeIdentifier;
class CNcdGeneralManager;


/**
 * This class object provides active object functionality for cleanup
 * of database.
 *
 * The state of this class object is not saved into db.
 * So, in some cases nodes that have been marked for deletion
 * may be left into the database if this class object is deleted
 * before cleaning has been finished (this is a rare case though).
 * If the node data is left hanging to the db, this cleaner can
 * clean them at latest when they expire.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdNodeCacheCleaner : public CActive
    {

public:

    enum TCleanupState
        {
        /**
         * Cleanup has not been started.
         * Or, it has been stopped.
         */
        ENotStarted,

        /**
         * Special cases are given by the user of this class.
         * These objects are removed first.
         */
        ECleaningSpecialCases,
                
        /**
         * Cleans the temporary nodes that are not used anymore.
         */
        ECleaningTemporaryNodes,

        /**
         * Makes sure that there are no items just hanging in the db.
         * If this is case then something has gone wrong somewhere.
         * Clean these wonder items.
         */
        ECleaningHangingCases,

        /** 
         * Sets the information for the next cleaning.
         */
        EStartCleaningHangingSpecialCases,
        
        /**
         * This will clean special items that can not be identified
         * by using the nodeidentifiers. For these items metadata has
         * to be created from the stream and ids have to be checked
         * separately. For example icons need to be cleaned this way.
         */
        ECleaningHangingSpecialCases,

        /**
         * Finishes the cleaning of special cases.
         */
        EFinishCleaningHangingSpecialCases,

        /**
         * Start cleaning excess data if db size has been exceeded.
         */
        EStartCleaningExcess,

        /**
         * This state will remove the root and its direct children
         * from the database node array which will contain the node
         * identifiers of the nodes that may be removed from db during
         * the cleaning process.
         * The identifiers are moved to separate array that is used
         * to check if children are transparent or bundle folders that
         * require special handling.
         */
        EStartRootChildrenCheck,

        /** 
         * During this state the cleaner will check if the root children 
         * are transparent or bundle nodes and handles them accordingly.
         */        
        ECheckRootChildren,

        /**
         * This state is used when the root children have been bundle 
         * folders. The transparency of the bundle children is checked
         * and transparent folder identifiers are removed from the db
         * node identifier list. So, transparent children will not be
         * removed from the database.
         *
         */
        EHandleBundleChildren,

        /**
         * When all the expired items of certain branch have been found,
         * the cleaning process is started and items are removed from the db.
         * Expireds are only removed if database is full.
         */
        ECleaningExpireds,
                
        /**
         * Cleaning excess data because db size has been exceeded.
         * The cleaning is done by removing the deepest nodes first.
         * They are still checked to be sure that they can be removed.
         * This will loop back to ECleaningTemporaryNodes until enough
         * space is freed.
         */
        ECleaningExcess,
                                
        /**
         * Cleanup operation should be stopped and
         * when the latest operation is finished the
         * state should be set as ENotStarted.
         */
        EStopping
        };


    /**
     * NewL
     *
     * @param aNodeManager
     * @param aNodeDbManager
     * @param aDbDefaultMaxSize The default value for the maximum size of 
     * the database in bytes. If this size is exceeded, then the db should 
     * be cleaned.
     * @param aNodeFactory
     * @param aClientUid
     * @return CNcdNodeCacheCleaner* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeCacheCleaner* NewL( CNcdGeneralManager& aGeneralManager,
                                       CNcdNodeDbManager& aNodeDbManager,
                                       TInt aDbDefaultMaxSize,
                                       CNcdNodeFactory& aNodeFactory );

    /**
     * @see CNcdNodeCacheCleaner::NewL
     */
    static CNcdNodeCacheCleaner* NewLC( CNcdGeneralManager& aGeneralManager,
                                        CNcdNodeDbManager& aNodeDbManager,
                                        TInt aDbDefaultMaxSize,
                                        CNcdNodeFactory& aNodeFactory );


    /**
     * Destructor
     */
    virtual ~CNcdNodeCacheCleaner();


    /** 
     * This function is used when the identifier arrays are sorted for other
     * functionalities in this class object.
     * The sorting is done only by comparing the length of the node id value.
     * The shorter will be first in the array.
     *
     * @see RPointerArray::Sort
     */
    static TInt NodeIdentifierArraySortById( const CNcdNodeIdentifier& aNodeId1,
                                             const CNcdNodeIdentifier& aNodeId2 );


    /**
     * @return const TUid& Uid of the client
     * that this cleaner is for.
     */
    const TUid& ClientUid() const;

    
    /**
     * @return TCleanupState The state of the asynchronous cleanup.
     */
    TCleanupState CleanupState() const;   


    /**
     * This function sets the flag that informs if the cache cleaner
     * is allowed to do cleaning. If EFlase is set, then possible
     * ongoing operation is cancelled and new operations are
     * prevented. If ETrue is set, then cleaning is allowed.
     * ETrue is a default value when cleaner is created.
     *
     * @param aAllow Informs if the cleaning is allowed.
     */
    void SetAllowCleaning( TBool aAllow );
    
    
    /**
     * @see CNcdNodeCacheCleaner::SetAllowCleaning
     *
     * @param aAllow Informs if the cleaning is allowed.
     */
    TBool AllowCleaning() const;


    /**
     * Starts the asynchronous cleanup process for the client that this cleaner
     * is created for.
     */
    void StartCleanupL();

    /**
     * Stops the asynchronous cleanup.
     */
    void StopCleanup();


    /**
     * If the database size has been exceeded, then the cleaning operation
     * will be started.
     * This function may be called for example when new nodes have been added to
     * the database.
     *
     * @note If the db is full:
     * - The hierarchy will be removed from the leaves
     * towards the root node until enough data has been removed from the db.
     * - All the expired nodes will be removed. In addition to those that have
     * been registered as removable nodes. 
     */
    void CheckDbSizeL();
 
 
    /**
     * The identifiers of the given array will be used to search the nodes from 
     * the db and to remove them if they exist.
     * This function should be used if there are multiple identifiers know
     * because this temporary information is saved simultaneously then.
     *
     * @note If the given identifier already exists in the do not remove array
     * then the identifier is inserted into the waiting array. And, it will
     * be moved from the waiting array to the cleanup array when the identifier
     * is asked to be removed from the do not remove array.
     *
     * @param aIdentifiers Identifiers that inform which items should be removed
     * from the db.
     */
    void AddCleanupIdentifiersL( const RPointerArray<CNcdNodeIdentifier>& aIdentifiers );

    /**
     * Same as AddCleanupIdentifiers but gets only one identifier at a time.
     * If multiple identifiers are know same time, AddCleanupIdentifiers function
     * should be used.
     *
     * @see CNcdNodeCacheCleaner::AddCleanupIdentifiersL
     */
    void AddCleanupIdentifierL( const CNcdNodeIdentifier& aIdentifier );



    /**
     * Removes the given identifiers from the cleanup list. This can be used
     * if the given items have been updated from somewhere else, and they should not be
     * removed from the db anymore.
     *
     * @note If the identifier is located in the waiting array then it will be removed
     * from that array also.
     *
     * @param aIdentifiers Identifies data that should not be removed from the db.
     */    
    void RemoveCleanupIdentifiers( const RPointerArray<CNcdNodeIdentifier>& aIdentifiers );

    /**
     * Same as AddCleanupIdentifiers but gets only one identifier at a time.
     * If multiple identifiers are know same time, AddCleanupIdentifiers function
     * should be used.
     *
     * @see CNcdNodeCacheCleaner::RemoveCleanupIdentifiers
     */
    void RemoveCleanupIdentifier( const CNcdNodeIdentifier& aIdentifier );



    /**
     * The identifiers of the given array will be used to block the removal
     * of the given items from the db.
     * This function should be used if there are multiple identifiers
     * because this temporary information is saved simultaneously then.
     *
     * @note If the identifier exists in the cleanup array it will be moved
     * to the waiting array. This way the node can be moved back to the cleanup
     * array when the do not remove state is not required for the item anymore.
     *
     * @param aIdentifiers Identifiers that inform which items should be removed
     * from the db.
     * @param aCanRemoveParent If ETrue, node's parent may be removed from the cache
     */
    void AddDoNotRemoveIdentifiersL( 
        const RPointerArray<CNcdNodeIdentifier>& aIdentifiers,
        TBool aCanRemoveParent = EFalse );

    /**
     * Same as AddDoNotRemoveIdentifiers but gets only one identifier at a time.
     * If multiple identifiers are know same time, AddDoNotRemoveIdentifiers function
     * should be used.
     *
     * @see CNcdNodeCacheCleaner::AddDoNotRemoveIdentifiersL
     */
    void AddDoNotRemoveIdentifierL( 
        const CNcdNodeIdentifier& aIdentifier, 
        TBool aCanRemoveParent = EFalse );


    /**
     * Removes the given identifiers from the do not remove list. This can be used
     * if the given items are allowed to be removed from the db later on.
     *
     * @note If the identifier exists in the waiting array then it will be moved
     * back to the cleanup array also.
     *
     * @param aIdentifiers Identifies data that should not be removed from the db.
     */    
    void RemoveDoNotRemoveIdentifiersL( const RPointerArray<CNcdNodeIdentifier>& aIdentifiers );

    /**
     * Same as RemoveDoNotRemoveIdentifiers but gets only one identifier at a time.
     * If multiple identifiers are know same time, AddDoNotRemoveIdentifiers function
     * should be used.
     *
     * @param aForceRemove If ETrue, removes also 'floating' nodes (favorites)
     * @see CNcdNodeCacheCleaner::RemoveDoNotRemoveIdentifiers
     */
    void RemoveDoNotRemoveIdentifierL( 
        const CNcdNodeIdentifier& aIdentifier, 
        TBool aForceRemove = EFalse );



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
     * This function moves the waiting items into cleanup array 
     * and does the cleanup for all the cleanup list items.
     * The do not clean array is not reseted. So, the nodes that are in the do not clean array
     * will not be deleted. 
     * The function does the cleaning sychronously.
     * This functions is propably mainly used, when the application is shutting down
     * and it is wanted to be sure that all the possible items are removed from
     * the db.
     *
     * @note This function also cancels the possible on going asynchronous 
     * operations before starting its own cleaning.
     *
     * @note HandleCleaningL provides gentler way of removing. It does not remove items
     * that should wait until marked free for removal. 
     *
     * @see CNcdNodeCacheCleaner::HandleCleaningL
     */
    void ForceCleanupL();


    /**
     * This function is meant for the public use and uses the HandleCleaningL below
     * with both parameters set to ETrue. Because of the ETrue parameters, the
     * asynchronous process has to be stopped.
     * 
     * @note This function also cancels the possible on going asynchronous 
     * operations before starting its own cleaning.
     *
     * @see CNcdNodeCacheCleaner::HandleCleaningL
     */ 
    void HandleCleaningL();


protected:

    /** 
     * Handles the cleaning call for the db.
     * This function can be used if synchronous cleaning is wanted.
     * This function does not clean the items that are moved into
     * waiting arrays for some reasons.
     * Checks that correct identifiers and types are given for
     * dbmanager remove operation. This function cleans all the nodes
     * that are free to be removed from db.
     *
     * @note ForceCleanupL provides a hardcore way to remove everything,
     * including items that should be waiting until marked as free to remove.
     *
     * @note When ETrue values are set for the parameters, the acynhronous process
     * should be stopped before calling this function. If this not done, 
     * the cleaning process may go wrong. Thus, this function
     * is not meant for public use. 
     *
     * @see CNcdNodeCacheCleaner::ForceCleanupL
     *
     * @param aReloadNodeList If set ETrue then the node item list will be relaoded
     * from the database. Then, the parent and child comparisons will be upto date.
     * Notice, that if this is set true, the db-loading will take some time 
     * but it may be a good idea to update the list, because if the cleaning 
     * has not been done for a while the node list may have changed dramatically 
     * when other parts of the code have updated the database.
     * @param aResetNodeList If set ETrue, then the node list will be reset in the end
     * of this function. This flag is provided, so the user may save some memory if
     * the node list is not needed after this function. Be really carefull when using
     * this flag. Because in some states of this object the node list should not be cleaned.
     * Whe using this object the ETrue should most likely be used here if this object
     * is not in the middle of cleaning or when the object is about to be deleted.
     */
    void HandleCleaningL( TBool aReloadNodeList, TBool aResetNodeList );


    /**
     * Ensures that iCleanupIdentifiers does not contain any of the ids in 
     * iDoNotRemoves
     */
    void RemoveDoNotRemovesFromCleanupArray();
        
protected: // CActive

    /**
     * @see CActive::DoCancel
     */
    virtual void DoCancel();

    /**
     * @see CActive::RunL
     */
    virtual void RunL();

    /**
     * @see CActive::RunError
     */
    virtual TInt RunError( TInt aError );

protected:

    /**
     * Constructor
     *
     * @see CNcdNodeCacheCleaner::NewL
     */
    CNcdNodeCacheCleaner( CNcdGeneralManager& aGeneralManager,
                          CNcdNodeDbManager& aNodeDbManager,
                          TInt aDbDefaultMaxSize,
                          CNcdNodeFactory& aNodeFactory );

    /**
     * ConstructL
     */
    void ConstructL();

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

    typedef CCatalogsPair<CNcdNodeIdentifier, TBool> CDoNotRemoveIdentifier;
    
private:

    // Prevent these two if they are not implemented
    CNcdNodeCacheCleaner( const CNcdNodeCacheCleaner& aObject );
    CNcdNodeCacheCleaner& operator =( const CNcdNodeCacheCleaner& aObject );

    /**
     * This function is used to check if the given identifier exists in the target array.
     *
     * @param aIdentifier This identifier is searched from the array.
     * @param aTargetIdentifiers Array contains the identifiers that the given identifier
     * is compared against.
     * @return TBool ETrue if the identifier was found from the array. Else EFalse.
     */
    TBool ContainsIdentifier( 
        const CNcdNodeIdentifier& aIdentifier ,
        const RPointerArray<CDoNotRemoveIdentifier>& aTargetIdentifiers ) const;    
    
    
    // Adds given identifiers into the target array if they do not already
    // exist there. If the target array was changed due to the appending operation,
    // ETrue is returned. If nothing was appended, then EFalse is returned.
    TBool AddIdentifiersL( const RPointerArray<CNcdNodeIdentifier>& aIdentifiers,
                           RPointerArray<CNcdNodeIdentifier>& aTargetArray );
                                                      
    // Adds given identifier into the target array if it does not already
    // exist there. If the target array was changed due to the appending operation,
    // ETrue is returned. If nothing was appended, then EFalse is returned.
    TBool AddIdentifierL( const CNcdNodeIdentifier& aIdentifier,
                          RPointerArray<CNcdNodeIdentifier>& aTargetArray ); 


    // Removes all occurrences of the given identifiers from the target array 
    // if they are found.
    // ETrue is returned if any identifier was removed. If nothing was done to
    // the target array, then EFalse is returned.
    TBool RemoveIdentifiers( const RPointerArray<CNcdNodeIdentifier>& aIdentifiers,
                             RPointerArray<CNcdNodeIdentifier>& aTargetArray );    

    // Removes all occurrences of the given identifier from the target array 
    // if they are found.
    // ETrue is returned if any identifier was removed. If nothing was done to
    // the target array, then EFalse is returned.
    TBool RemoveIdentifier( const CNcdNodeIdentifier& aIdentifier,
                            RPointerArray<CNcdNodeIdentifier>& aTargetArray );

    // aForceRemove is used to indicate whether Favorite nodes should be
    // removed or not. Default is no
    TBool RemoveIdentifier( const CNcdNodeIdentifier& aIdentifier,
                            RPointerArray<CDoNotRemoveIdentifier>& aTargetArray,
                            TBool aForceRemove = EFalse );
                              
    // Resets the node item info from the db to the member array.
    // This will contain only node item infos.
    void SetAllDbStorageNodeItemsL();

    // Resets the icon item info from the db to the member array.
    // This will contain only icon item infos.
    void SetAllDbStorageIconItemsL();

    // Resets the screenshot item info from the db to the member array.
    // This will contain only screenshot item infos.
    void SetAllDbStorageScreenshotItemsL();

    // Gets the children identifiers of the cleanup array identifiers. Appends
    // the children also to the cleanup array. So, this function makes sure that
    // the hierarchies below the cleanup array identifiers are also included into
    // the array.
    void AddChildrenToCleanupArrayL();

    // Checks if the parents that have been inserted to parent waiting list
    // can be put back to the cleaning list.
    void ParentCleaningCheckL();

    // This function checks if the db contains temporary nodes and adds them to the
    // cleanup list.
    void CheckTemporaryNodesL();
    
    // Resets this object state. To be ready for the next try.
    // This function is used. If something went wrong during the
    // cleanup procedure.
    void ResetState();

    // This function can be used to print the given array into the log file.
    void ArrayDebugPrint( const RPointerArray<CNcdNodeIdentifier>& aIdentifiers ) const;

    void ArrayDebugPrint( const RPointerArray<CDoNotRemoveIdentifier>& aIdentifiers ) const;

    // Functions that are used during active cleaning.
    // These functions mainly correspond to the states of this cleaner.
    
    void CleanSpecialsL();
    void CleanTemporaryNodesL();
    void CleanHangingsL();
    void StartCleanSpecialHangingsL();
    void CleanSpecialHangingsL();
    void FinishCleanSpecialHangingsL();
    void StartCleanExcessL();
    void StartRootChildrenCheckL();
    void CheckRootChildrenL();
    void HandleBundleChildrenL();
    void CleanExpiredsL();
    void CleanExcessL();
    void StoppingL();      

                              
private: // data

    CNcdGeneralManager& iGeneralManager;
    
    // Node manager handles creation of the nodes.   
    CNcdNodeManager& iNodeManager;
    
    // Data base manager is used to remove the nodes from the db.
    CNcdNodeDbManager& iNodeDbManager;

    // This contains the client specific max db byte size
    TInt iDbMaxSize;
    
    // The factory is used to create the correct nodes.
    CNcdNodeFactory& iNodeFactory;

    // This flag that informs if the cache cleaner 
    // is allowed to do cleaning.
    TBool iAllowCleaning;
        
    // This variable is used to check if the db should be cleaned with
    // higher priority.
    CActive::TPriority iIncreasePriority;

    // This flag is used to inform if the priority should be reseted. ´
    // This is needed because the priority can not be set
    // if the object is active. Thus, it has to be done later for example
    // next time the cleaning is started.
    TBool iResetPriority;

    // This time contains the last time that the cleanup has started.
    // If no cleanup has been started in required period. Then the
    // cleanup will be automatically started when the db size is asked
    // next time.
    TTime iLastCleanupTime;

    // Informs if the cleanup has been set to 
    TCleanupState iCleanupState;

    // This index can be used to infrom what identifiers are already handled
    // and which nodes should still be checked for cleanup when expired nodes
    // are cleaned.
    TInt iExpiredCleaningIndex;
    
    // This is used to inform if the excess celanup loop has already started.
    TBool iExcessCleanupStarted;
    
    // This array actually contains only the node, which will be used
    // when the node items are searched from the db.
    RArray<NcdNodeClassIds::TNcdNodeClassType> iNodeCleanupTypes;

    // This array contains the types that can be deleted directly by
    // using the metadata identifier.
    // This does not include node type
    // because it will always be used when cleaning.
    RArray<NcdNodeClassIds::TNcdNodeClassType> iMetaCleanupTypes;

    // This contains the list of the general types of the icon items
    // that should be removed. 
    // The items of these types can not be identified directly by using
    // node or metadata identifiers.
    RArray<NcdNodeClassIds::TNcdNodeClassType> iIconCleanupTypes; 

    // This contains the list of the general types of the screenshot items
    // that should be removed. 
    // The items of these types can not be identified directly by using
    // node or metadata identifiers.
    RArray<NcdNodeClassIds::TNcdNodeClassType> iScreenshotCleanupTypes; 

    // This array contains the namespaces whose data will not be cleaned.
    CPtrCArray* iDoNotCleanNameSpaces;    
        
    // The nodes that should be cleaned from the db.
    // Notice, that this array is used for every cleanup phase. So, even
    // if the user of this class object adds identifiers during
    // the cleanup process, those nodes will be removed also when next remove
    // operation is done because the same array is used when cleanup is done.
    RPointerArray<CNcdNodeIdentifier> iCleanupIdentifiers;

    // This array will contain all the db storage node identifiers.
    // This array can be used to create the actual node items according
    // to the id info.
    RPointerArray<CNcdNodeIdentifier> iAllDbStorageNodeItems;    

    // This array will contain all the icon ids that are used to check
    // if some icons should be deleted from the db.
    RPointerArray<CNcdNodeIdentifier> iAllDbIconItems;

    // This array will contain all the screenshot ids that are used to check
    // if some screenshots should be deleted from the db.
    RPointerArray<CNcdNodeIdentifier> iAllDbScreenshotItems;

    // This array will contain all the current metadata identifiers. That can 
    // be used to create metadatas and to check if some special contents
    // should be removed.
    RPointerArray<CNcdNodeIdentifier> iDbMetaIdentifiers;

    // This list will be used if the remove operation is started and
    // during remove operation engine infroms that some nodes should
    // not be deleted after all.    
    RPointerArray< CDoNotRemoveIdentifier > iDoNotRemoves;

    // This array contains identifiers that have been initially marked for
    // cleanup but after that have also been tried to move to the do not
    // remove array. Then, the identifier will be set for waiting until
    // is requested to be removed from the do not remove array. So, then
    // the identifier will be moved from the waiting array to cleanup array.
    RPointerArray<CNcdNodeIdentifier> iWaitingIdentifiers;
    
    // This list contains the parents that have been put to cleanuplist but
    // can not be deleted because their children are in waiting list or in
    // do not remove list.
    RPointerArray<CNcdNodeIdentifier> iWaitingParentIdentifiers;

    // This array will contain identifiers of children nodes of
    // the root. These identifiers are used, when transparency and bundle
    // check are done to these nodes.
    RPointerArray<CNcdNodeIdentifier> iRootChildren;
    
    // This array will contain the children of a certain bundle when
    // the transparency check are done to the children of the bundle.
    RPointerArray<CNcdNodeIdentifier> iBundleChildren;
    };
    
#endif // NCD_NODE_CACHE_CLEANER_H    

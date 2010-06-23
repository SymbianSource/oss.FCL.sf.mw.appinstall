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
* Description:   Implements CNcdNodeCacheCleaner class
*
*/


#include "ncdnodecachecleaner.h"

#include "ncdnodeidentifierutils.h"
#include "ncdnodemanager.h"
#include "ncdnodedbmanager.h"
#include "ncdnodefactory.h"
#include "ncdnodeidentifiereditor.h"
#include "ncdnodeimpl.h"
#include "ncdnodefolder.h"
#include "ncdrootnode.h"
#include "ncdnodemetadataimpl.h"
#include "ncdnodelink.h"
#include "ncdnodeiconimpl.h"
#include "ncdnodescreenshotimpl.h"
#include "ncdnodeidentifier.h"
#include "ncdchildentity.h"
#include "catalogsconstants.h"
#include "ncdproviderdefines.h"
#include "catalogsutils.h"
#include "ncdgeneralmanager.h"

#include "catalogsdebug.h"



// Uncomment this if the node cache cleaner should be turned off.
// For example, when debugging other functionalities.
//#define NCD_NODE_CACHE_CLEANER_TURN_OFF


#ifdef CATALOGS_BUILD_CONFIG_DEBUG
// Uncomment this if the array prints should be printed when the
// debug mode is on. This is set optional because the array print
// may take a long time when data is written into the log file.
//#define NCD_NODE_CACHE_CLEANER_DEBUG_ARRAY_PRINT
#endif


// These priority values are used for this active object.
// The default value is used when normal cleaning is done
const CActive::TPriority KDefaultCleaningPriority( CActive::EPriorityIdle );
// This priority value is used if the database size has been exceeded
// and the cleaning should be done with the higher priority
const CActive::TPriority KIncreasedCleaningPriority( CActive::EPriorityLow );
// This priority value is used if the database size has been exceeded
// too much and the cleaning should be done with really high priority.
// This will slow down the other actions even more because cleaning
// takes more time.
const CActive::TPriority KMaxCleaningPriority( CActive::EPriorityStandard );

// The max db size value is divided by this number to get the
// value for the limit until which the db is freed after max size
// has been reached and cleaning started.
const TInt KMaxSizeDivider( 2 );

// If the db size is over double the accepted db size, then the max cleaning
// priority should be used. Because most likely the lower priority does not work
// efficiently enough
const TInt KMaxDbSizeRatio( 2 );

// The checking of the database should be done at least in some time intervals
// even if the db size has not been exceeded.
const TTimeIntervalMinutes KCheckPeriod( 60 );



CNcdNodeCacheCleaner* CNcdNodeCacheCleaner::NewL( CNcdGeneralManager& aGeneralManager,
                                                  CNcdNodeDbManager& aNodeDbManager,
                                                  TInt aDbDefaultMaxSize,                                                  
                                                  CNcdNodeFactory& aNodeFactory )
    {
    CNcdNodeCacheCleaner* self =   
        CNcdNodeCacheCleaner::NewLC( aGeneralManager, 
                                     aNodeDbManager, 
                                     aDbDefaultMaxSize,
                                     aNodeFactory );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeCacheCleaner* CNcdNodeCacheCleaner::NewLC( CNcdGeneralManager& aGeneralManager,
                                                   CNcdNodeDbManager& aNodeDbManager,
                                                   TInt aDbDefaultMaxSize,
                                                   CNcdNodeFactory& aNodeFactory )
    {
    CNcdNodeCacheCleaner* self = 
        new( ELeave ) CNcdNodeCacheCleaner( aGeneralManager, 
                                            aNodeDbManager,
                                            aDbDefaultMaxSize, 
                                            aNodeFactory );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;        
    }



CNcdNodeCacheCleaner::CNcdNodeCacheCleaner( CNcdGeneralManager& aGeneralManager,
                                            CNcdNodeDbManager& aNodeDbManager,
                                            TInt aDbDefaultMaxSize,
                                            CNcdNodeFactory& aNodeFactory )
: CActive( KDefaultCleaningPriority ),
  iGeneralManager( aGeneralManager ),
  iNodeManager( aGeneralManager.NodeManager() ),
  iNodeDbManager( aNodeDbManager ),
  iDbMaxSize( aDbDefaultMaxSize ),
  iNodeFactory( aNodeFactory ),
  // EFalse by default so that we don't clean anything even in error situations
  // for clients that don't allow it
  iAllowCleaning( EFalse ), 
  iIncreasePriority( KDefaultCleaningPriority ),
  iLastCleanupTime( 0 )
    {
    }


void CNcdNodeCacheCleaner::ConstructL()
    {
    // These values have to be set. 

    // These types will be removed from the db.

    // iNodeCleanupTypes are used to search the node items from db.
    // This array is only for node type. So, it contains only one value.
    iNodeCleanupTypes.AppendL( NcdNodeClassIds::ENcdNode );
    
    // Meta cleanup types are also removed from the db.
    // These types use the metadata identifier to remove the items
    // from db
    iMetaCleanupTypes.AppendL( NcdNodeClassIds::ENcdMetaData );

    // These types should be permanent info 
    // or not to be removed by this cleaner for some other reason.
    // Uncomment these if specifications change.
    //iMetaCleanupTypes.Append( NcdNodeClassIds::ENcdNodeUserData );
    //iMetaCleanupTypes.Append( NcdNodeClassIds::ENcdSubscriptionsData );

        
    // Special cleanup types are removed from the db.
    // But node or metadata ids cannot be directly used for these.
    // They require special handling to get the right ids.
    iIconCleanupTypes.AppendL( NcdNodeClassIds::ENcdIconData );

    iScreenshotCleanupTypes.AppendL( NcdNodeClassIds::ENcdScreenshotData );
    

    // These namespaces should not be cleaned by this cleaner
    iDoNotCleanNameSpaces = new(ELeave) CPtrCArray( KListGranularity );
    iDoNotCleanNameSpaces->AppendL( 
        NcdProviderDefines::KDownloadNamespace() );
    iDoNotCleanNameSpaces->AppendL( 
        NcdProviderDefines::KSubscriptionNamespace() );
    iDoNotCleanNameSpaces->AppendL( 
        NcdProviderDefines::KPreviewStorageNamespace() );

    // Because this is an active object, we need to inform
    // scheduler about it.    
    CActiveScheduler::Add( this );    
    }


CNcdNodeCacheCleaner::~CNcdNodeCacheCleaner()
    {
    DLTRACEIN((""));
    
    // It is always safe to call Cancel for active object
    // when destructor is called.
    Cancel();

    // This object is automatically removed from the active scheduler when
    // this object is deleted.

    // Delete member variables that are owned by this class object here.

    iNodeCleanupTypes.Reset();
    iMetaCleanupTypes.Reset();
    iIconCleanupTypes.Reset();

    iScreenshotCleanupTypes.Reset();
    iCleanupIdentifiers.ResetAndDestroy();
    iAllDbStorageNodeItems.ResetAndDestroy();
    iAllDbIconItems.ResetAndDestroy();

    iAllDbScreenshotItems.ResetAndDestroy();
    iDbMetaIdentifiers.ResetAndDestroy();
    iWaitingIdentifiers.ResetAndDestroy();
    iWaitingParentIdentifiers.ResetAndDestroy();
    iDoNotRemoves.ResetAndDestroy();
    iRootChildren.ResetAndDestroy();
    iBundleChildren.ResetAndDestroy();

    // Notice that this delete also deletes the objects owned by the array.
    delete iDoNotCleanNameSpaces;
        
    DLTRACEOUT((""));
    }        


TInt CNcdNodeCacheCleaner::NodeIdentifierArraySortById( const CNcdNodeIdentifier& aNodeId1,
                                                        const CNcdNodeIdentifier& aNodeId2 )
    {
    //DLTRACEIN((""));
    
    TInt depth1( 0 );
    TInt depth2( 0 );
    TInt trapError1( KErrNone );
    TInt trapError2( KErrNone );

    TRAP( trapError1, depth1 = NcdNodeIdentifierEditor::NodeDepthL( aNodeId1 ) );
    TRAP( trapError2, depth2 = NcdNodeIdentifierEditor::NodeDepthL( aNodeId2 ) );

    // Decide the order according to the depth. If the identifier is not
    // an actual nodeidentifier. Then insert it to the beginning of the array
    // and sort those identifiers by their lengths.

    if ( trapError1 == KErrNone && trapError2 == KErrNone )
        {
        // Both of the given identifiers were for the node.
            
        // This function returns zero if depths are equal and lengths are equal.
        TInt order( depth1 - depth2 );
        if ( depth1 == depth2 )
            {
            // Depths are equal. So, check if the id lengths differ.
            // If nodeid1 is shorter, then it should be first in the array.
            // If they are equal length then the order does not matter.
            // If the node1 is longer, then node2 should be first.
            order = aNodeId1.NodeId().Length() - aNodeId2.NodeId().Length();
            }
            
        // Negative number if depth1 is smaller.
        // Positive number if id1 is greater.
        return order;        
        }
    else if ( trapError1 != KErrNone && trapError2 != KErrNone )
        {
        // Because neither was not a node, use the id length to get the
        // order. This way the shortest will be the first one etc.
        DLERROR(("Neither was node"));
        DASSERT( trapError1 == KErrArgument );
        DASSERT( trapError2 == KErrArgument );

        return aNodeId1.NodeId().Length() - aNodeId2.NodeId().Length();
        }
    else if ( trapError1 != KErrNone )
        {
        // Here we should get only the KErrArgument that informs that the
        // aNodeId1 was not an actual node id. It is most likely some metadata id.
        // Insert these in the beginning of the array. So, return negative number
        DLERROR(("aNodeId1 not node"));
        DASSERT( trapError1 == KErrArgument );
        return -1;
        }        
    else
        {
        DLERROR(("aNodeId2 not node"));
        // Here we should get only the KErrArgument that informs that the
        // aNodeId2 was not an actual node id. It is most likely some metadata id.
        // Insert these in the beginning of the array. So, return positive number
        DASSERT( trapError2 == KErrArgument );
        return 1;
        }
    }


const TUid& CNcdNodeCacheCleaner::ClientUid() const
    {
    return iGeneralManager.FamilyId();
    }


CNcdNodeCacheCleaner::TCleanupState CNcdNodeCacheCleaner::CleanupState() const
    {
    return iCleanupState;
    }


void CNcdNodeCacheCleaner::SetAllowCleaning( TBool aAllow )
    {
    DLTRACEIN((""));
    if ( aAllow != AllowCleaning()
         && !aAllow )
        {
        DLINFO(("Cancel on going operation because cleaning is not allowed anymore."))
        Cancel();        
        }
    iAllowCleaning = aAllow;
    }


TBool CNcdNodeCacheCleaner::AllowCleaning() const
    {
    return iAllowCleaning;
    }


void CNcdNodeCacheCleaner::StartCleanupL()
    {
    DLTRACEIN((""));

    // Set the last cleanup time here. So, next automatic start will
    // occure after the specified period.
    iLastCleanupTime.HomeTime();

    #ifdef NCD_NODE_CACHE_CLEANER_TURN_OFF
    // Nothing to do here. Because cleaning should not be done.
    DLWARNING(("*** CNcdNodeCacheCleaner turned off ***"));
    #warning *** CNcdNodeCacheCleaner turned off ***
    return;
    #endif

    if ( !AllowCleaning() )
        {
        DLINFO(("Cleaning is not allowed."));
        return;
        }

    if ( CleanupState() != ENotStarted
         || IsActive() )
        {
        DLINFO(("Cleaning already started"));
        return;
        }

    if ( iResetPriority )
        {
        // The priority may have been increased before. 
        // But it has been marked to be resetted when possible.
        // So, reset it here. We come here only if Cancel was
        // called when the object was active.
        SetPriority( KDefaultCleaningPriority );
        iResetPriority = EFalse;
        }

    // Update the node items list. 
    // This is an initial list that will be used when the nodes are examined.
    // The node items list should not be updated anywhere else but here or
    // in the start clean excess function. If the array is updated somewhere else, it
    // may result an infinite loop if some deepest nodes cannot be removed when
    // excess cleaning is done and those nodes are in the end of the list all the time.
    // Notice, that it does not matter if all the nodes are not check this time. They
    // will be checked next time cleaning is started. Also, if some nodes are missing
    // in the list, wrong nodes will not be deleted because parent and child checks 
    // are done by using the other array such as do not remove and wait arrays.
    SetAllDbStorageNodeItemsL();
    
    // Cleaning special cases is the first thing to do when cleanup is started.        
    iCleanupState = ECleaningSpecialCases;
    
    // Mark this active object to be ready for action.
    // Instead of using some other class object to do the job,
    // this active object handles everything itself. New job
    // is started everytime when the RunL is called until there is
    // nothing to be done. Or, until user cancels or stops the action.
    iStatus = KRequestPending;

    // Let the active object know that RunL can be called when the
    // action is completed.
    SetActive();
    
    // Because there is not anything special to be done here.
    // Set the action complete. So, the RunL will be called.
    TRequestStatus* ptrStatus = &iStatus;
    User::RequestComplete( ptrStatus, KErrNone );

    DLTRACEOUT((""));
    }


void CNcdNodeCacheCleaner::StopCleanup()
    {
    DLTRACEIN((""));
    
    if ( CleanupState() != ENotStarted
         || IsActive() )
        {
        DLINFO(("Stop cleanup"));
        Cancel();
        }

    DLTRACEOUT((""));
    }


void CNcdNodeCacheCleaner::CheckDbSizeL()
    {
    DLTRACEIN((""));

    // If the db size has been exceeded, then
    // start the cleaning operation.
    // This function may be called for example 
    // when new nodes have been added to the db.
    TInt dbSize( 
        NodeDbManager().StorageSizeL( ClientUid(),
                                      *iDoNotCleanNameSpaces ) );

    TTime currentTime;
    currentTime.HomeTime();    
    TBool timePassed( EFalse );
    if ( iLastCleanupTime + KCheckPeriod < currentTime  )
        {
        timePassed = ETrue;
        }
    
    // Start cleaning if the database size is too large
    // or if it is too long time from the last cleanup.
    if ( DbMaxSize() < dbSize )
        {
        DLINFO(("Too much data in db: %d", dbSize));

        // Because the db size has been exceeded we want to clean
        // the db with higher priority than usually.
        // When the cleanup finishes, the priority should be set back
        // to lower level.
        if ( IsActive() )
            {
            if ( DbMaxSize() * KMaxDbSizeRatio < dbSize )
                {
                DLINFO(("Highest priority should be used db size is way too big"));
                // Set the priority to max, the next time RunL is called
                // because the cleaner is not doing its work efficiently enough
                iIncreasePriority = KMaxCleaningPriority;
                }
            else
                {
                DLINFO(("Increased priority should be used"));
                // Set the priority higher, the next time
                // RunL is called. The db size is too large but does not
                // exceed the "panic" level.
                iIncreasePriority = KIncreasedCleaningPriority;                
                }
            }
        else if ( DbMaxSize() * KMaxDbSizeRatio < dbSize )
            {
            DLINFO(("Not active. Max priority set"));
            SetPriority( KMaxCleaningPriority );
            }
        else
            {
            DLINFO(("Not active. Increased priority set"));
            SetPriority( KIncreasedCleaningPriority );            
            }

        // To be sure that the cleanup does not reset the priority.
        // No need to reset it because new values have been given above.
        iResetPriority = EFalse;
        
        // Start the actual cleaning.
        StartCleanupL();
        }
    else if ( timePassed )
        {
        DLINFO(("Time passed"));
        // Start the actual cleaning but no need to increase the priority.
        StartCleanupL();        
        }

    DLTRACEOUT((""));
    }


void CNcdNodeCacheCleaner::AddCleanupIdentifiersL( const RPointerArray<CNcdNodeIdentifier>& aIdentifiers )
    {
    DLTRACEIN((""));
    
    for ( TInt i = 0; i < aIdentifiers.Count(); ++i )
        {
        AddCleanupIdentifierL( *aIdentifiers[ i ] );
        }
    
    DLTRACEOUT((""));
    }

void CNcdNodeCacheCleaner::AddCleanupIdentifierL( const CNcdNodeIdentifier& aIdentifier )
    {
    DLTRACEIN((""));
    DLNODEID(( aIdentifier ));
    if ( ContainsIdentifier( aIdentifier, iDoNotRemoves ) )
        {
        // Do not remove list already contains the given identifier.
        // So, insert the given identifier to the wait list.
        // Notice that identifier should be left also into the do not remove list.
        AddIdentifierL( aIdentifier, iWaitingIdentifiers );
        }
    else if ( !NcdNodeIdentifierUtils::ContainsIdentifier(
              aIdentifier, iWaitingIdentifiers ) &&
              !NcdNodeIdentifierUtils::ContainsIdentifier(
              aIdentifier, iWaitingParentIdentifiers ) )
        {
        // Just insert the given identifier into the cleanup list unless
        // it already existed in some waiting array or in do not remove list.
        AddIdentifierL( aIdentifier, iCleanupIdentifiers );
        }
    
    // No need to check the waiting parents, because it will be checked when items
    // are going to be removed.
    
    DLTRACEOUT((""));
    }


void CNcdNodeCacheCleaner::RemoveCleanupIdentifiers( const RPointerArray<CNcdNodeIdentifier>& aIdentifiers )
    {
    DLTRACEIN((""));

    for ( TInt i = 0; i < aIdentifiers.Count(); ++i )
        {
        RemoveCleanupIdentifier( *aIdentifiers[ i ] );
        }
    
    DLTRACEOUT((""));
    }

void CNcdNodeCacheCleaner::RemoveCleanupIdentifier( const CNcdNodeIdentifier& aIdentifier )
    {
    DLTRACEIN((""));

    // If wait list contains the given identifier.
    // So, remove the given identifier from the wait list.
    RemoveIdentifier( aIdentifier, iWaitingIdentifiers );

    // If wait list contains the given identifier.
    // So, remove the given identifier from the wait list.
    RemoveIdentifier( aIdentifier, iWaitingParentIdentifiers );
    
    // Remove the given identifier from cleanup list
    RemoveIdentifier( aIdentifier, iCleanupIdentifiers );

    DLTRACEOUT((""));
    }


void CNcdNodeCacheCleaner::AddDoNotRemoveIdentifiersL( 
    const RPointerArray<CNcdNodeIdentifier>& aIdentifiers,
    TBool aCanRemoveParent )
    {
    DLTRACEIN((""));

    // Check if the identifiers should be inserted into the waiting or do not remove
    // array.    
    for ( TInt i = 0; i < aIdentifiers.Count(); ++i )
        {
        AddDoNotRemoveIdentifierL( *aIdentifiers[ i ], aCanRemoveParent );
        }
    
    DLTRACEOUT((""));
    }

void CNcdNodeCacheCleaner::AddDoNotRemoveIdentifierL( 
    const CNcdNodeIdentifier& aIdentifier, 
    TBool aCanRemoveParent )
    {
    DLTRACEIN((""));
    DLNODEID(( aIdentifier ));

    if ( RemoveIdentifier( aIdentifier, iCleanupIdentifiers ) )
        {
        // Cleanup list already contained the given identifier.
        // So, insert the given identifier to the wait list.
        AddIdentifierL( aIdentifier, iWaitingIdentifiers );
        }

    // Insert the given identifier into the do not remove list.
    // It should be ther until RemoveDoNotRemoveIdentifiers is used.
    // So, even if identifier is in waiting list it will be also,
    // in the do not remove list.
    
    // Compare given identifier to the identifiers in the array.
    TInt i = 0;
    for ( ; i < iDoNotRemoves.Count(); ++i )
        {
        if ( iDoNotRemoves[ i ]->Key().Equals( aIdentifier ) )
            {
            // Flag is only updated if it goes from EFalse -> ETrue
            if ( aCanRemoveParent ) 
                {
                DLTRACE(( _L("Updating parent removal flag for: %S"),
                    &iDoNotRemoves[ i ]->Key().NodeId() ));
                // The given identifier was found from the array
                // update 
                iDoNotRemoves[ i ]->SetValue( ETrue );
                }
            break;
            }
        }

    
    if ( i == iDoNotRemoves.Count() ) 
        {
        DLTRACE(("Adding a non-removable identifier"));
        CNcdNodeIdentifier* id = CNcdNodeIdentifier::NewLC( aIdentifier );
        // ownership is transferred
        CDoNotRemoveIdentifier* pair = new ( ELeave ) 
            CDoNotRemoveIdentifier( id, aCanRemoveParent );
        CleanupStack::Pop( id );
        
        CleanupStack::PushL( pair );
        iDoNotRemoves.AppendL( pair );
        CleanupStack::Pop( pair );
        }
            
    DLTRACEOUT((""));
    }


void CNcdNodeCacheCleaner::RemoveDoNotRemoveIdentifiersL( const RPointerArray<CNcdNodeIdentifier>& aIdentifiers )
    {
    DLTRACEIN((""));

    // Check if the identifiers should be moved from the waiting back to the
    // cleanup array and remove identifier from the do not remove array.    
    for ( TInt i = 0; i < aIdentifiers.Count(); ++i )
        {
        RemoveDoNotRemoveIdentifierL( *aIdentifiers[ i ] );
        }    

    DLTRACEOUT((""));
    }

void CNcdNodeCacheCleaner::RemoveDoNotRemoveIdentifierL( 
    const CNcdNodeIdentifier& aIdentifier, 
    TBool aForceRemove )
    {
    DLTRACEIN((""));
    DLNODEID(( aIdentifier ));
    
    TBool waitingRemoved( 
        RemoveIdentifier( aIdentifier, iWaitingIdentifiers ) );
    TBool waitingParentRemoved( 
        RemoveIdentifier( aIdentifier, iWaitingParentIdentifiers ) );
        
    if ( waitingRemoved || waitingParentRemoved )
        {
        // Wait list contained the given identifier.
        // So, move the given identifier back to the cleanup list.
        AddIdentifierL( aIdentifier, iCleanupIdentifiers );
        }
    
    RemoveIdentifier( aIdentifier, iDoNotRemoves, aForceRemove );

    DLTRACEOUT((""));
    }


TInt CNcdNodeCacheCleaner::DbMaxSize() const
    {
    DLTRACEIN((""));
    return iDbMaxSize;
    }
 
 
void CNcdNodeCacheCleaner::SetDbMaxSize( const TInt aDbMaxSize )
    {
    DLTRACEIN((""));
    iDbMaxSize = aDbMaxSize;
    }


void CNcdNodeCacheCleaner::ForceCleanupL()
    {
    DLTRACEIN((""));

    // Print array debug infos into the log file
    #ifdef NCD_NODE_CACHE_CLEANER_DEBUG_ARRAY_PRINT
    DLINFO(("iDoNotRemoves array:"));
    ArrayDebugPrint( iDoNotRemoves );

    DLINFO(("iWaitingIdentifiers array:"));
    ArrayDebugPrint( iWaitingIdentifiers );

    DLINFO(("iWaitingParentIdentifiers array:"));
    ArrayDebugPrint( iWaitingParentIdentifiers );
    #endif

    
    // iDoNotRemoves array is not reseted here. So, those nodes will not be deleted even here.
    // This way some nodes can be saved from deletion. But, the nodes that are waiting to 
    // be deleted are forced to be deleted now.

    // Move waiting identifiers to the cleanup array.
    AddIdentifiersL( iWaitingIdentifiers, iCleanupIdentifiers );
    iWaitingIdentifiers.ResetAndDestroy();

    AddIdentifiersL( iWaitingParentIdentifiers, iCleanupIdentifiers );
    iWaitingParentIdentifiers.ResetAndDestroy();
        
    // Clean the items that are in the cleanup array from db.
    HandleCleaningL();

    DLTRACEOUT((""));
    }


void CNcdNodeCacheCleaner::HandleCleaningL()
    {
    DLTRACEIN((""));

    // Asynchronous operations have to be stopped before 
    // starting synchronous cleaning. Otherwise the
    // asynchronous cleaning may work wrong later.
    Cancel();
        
    // Update the arrays and handle cleaning.
    // Thde node array will be emptied when this is done.
    HandleCleaningL( ETrue, ETrue );

    DLTRACEOUT((""));
    }


void CNcdNodeCacheCleaner::HandleCleaningL( TBool aReloadNodeList, TBool aResetNodeList )
    {
    DLTRACEIN((""));

    #ifdef NCD_NODE_CACHE_CLEANER_TURN_OFF
    // Nothing to do here. Because cleaning should not be done.
    DLWARNING(("*** CNcdNodeCacheCleaner turned off ***"));
    #warning *** CNcdNodeCacheCleaner turned off ***
    return;
    #endif

    if ( !AllowCleaning() )
        {
        DLINFO(("Cleaning is not allowed."));
        return;
        }

    // Clean all the nodes from the list.
    // Notice that we only delete the nodes here. 
    // The metadata and metadata related
    // data will be removed later, when this cleaner checks 
    // if they are left hanging and
    // do not belong to any node anymore.

    // Print array debug info into the log file
    #ifdef NCD_NODE_CACHE_CLEANER_DEBUG_ARRAY_PRINT
    DLINFO(("iCleanupIdentifiers array before parent and child check:"));
    ArrayDebugPrint( iCleanupIdentifiers );
    #endif

    if ( aReloadNodeList )
        {
        DLINFO(("Reload node list"));
        // The caller may require the list to be updated.
        // So, make sure that all the necessary ids are in the
        // list. It may also be so, that the db is empty.
        // This check is required for example if some other class wants to
        // clean some nodes.
        SetAllDbStorageNodeItemsL();        
        }

    if ( iAllDbStorageNodeItems.Count() == 0 )
        {
        // Because there are not nodeitems in the db,
        // just return.
        DLTRACEOUT(("No nodes to remove"));
        return;
        }

    // Because the name of the children starts with the same id
    // as the name of the parent, check that also the children
    // of the parents will be deleted, unless they are in do not remove list
    // or in the waiting list.
    AddChildrenToCleanupArrayL();
    ParentCleaningCheckL();
 
    // In some cases there are unremovable items in the cleanup array
    // so let's remove them
    RemoveDoNotRemovesFromCleanupArray();
 
    DLINFO(("Remove nodes from db"));
    // Print array debug info into the log file
    #ifdef NCD_NODE_CACHE_CLEANER_DEBUG_ARRAY_PRINT
    DLINFO(("iCleanupIdentifiers array:"));
    ArrayDebugPrint( iCleanupIdentifiers );
    #endif

    // Now the parents and their children are in the array.
    NodeDbManager().
        RemoveDataFromDatabaseL( iCleanupIdentifiers,
                                 iNodeCleanupTypes );
                                         
    // Because all the items have been marked as removable in db
    // manager, we can clear the array.
    iCleanupIdentifiers.ResetAndDestroy(); 

    if ( aResetNodeList )
        {
        DLINFO(("Reset node list"));
        // The list should be reseted to save some memory.
        iAllDbStorageNodeItems.ResetAndDestroy();
        }
    
    DLTRACEOUT((""));       
    }


void CNcdNodeCacheCleaner::DoCancel()
    {
    DLTRACEIN((""));
    
    // Do not continue the action any more
    // because cancel was called.
    ResetState();

    // Also, make sure that the active object will know that the
    // cancel is ready. If active process is going on, the status is
    // set to KRequestPending.
    
    // There is no need for additional User::RequestComplete 
    // call here because DoCancel is executed only if this object is active 
    // and there has always been a call to User::RequestComplete when this 
    // object has been put to active state in other function calls.

    DLTRACEOUT((""));
    }


void CNcdNodeCacheCleaner::RunL()
    {
    DLTRACEIN((""));

    if ( iIncreasePriority != KDefaultCleaningPriority )
        {
        // Increase priority flag has been set.
        // So, increase the priority from idle to suggested value.
        SetPriority( iIncreasePriority );
        iIncreasePriority = KDefaultCleaningPriority;        
        }
            
    User::LeaveIfError( iStatus.Int() );

    // Continue working if there is something to be done.
    switch ( iCleanupState )
        {
        case ECleaningSpecialCases:
            DLINFO(("KErrNone cleaning specials"));
            CleanSpecialsL();
            break;

        case ECleaningTemporaryNodes:
            DLINFO(("KErrNone cleaning temporary nodes"));
            CleanTemporaryNodesL();
            break;

        case ECleaningHangingCases:
            DLINFO(("KErrNone cleaning hangings"));
            CleanHangingsL();
            break;

        case EStartCleaningHangingSpecialCases:
            DLINFO(("KErrNone start cleaning special hangings"));
            StartCleanSpecialHangingsL();
            break;
            
        case ECleaningHangingSpecialCases:
            DLINFO(("KErrNone cleaning special hangings"));
            CleanSpecialHangingsL();
            break;

        case EFinishCleaningHangingSpecialCases:
            DLINFO(("KErrNone finish cleaning special hangings"));
            FinishCleanSpecialHangingsL();
            break;

        case EStartCleaningExcess:
            DLINFO(("KErrNone start cleaning excess"));
            StartCleanExcessL();
            break;

        case EStartRootChildrenCheck:
            DLINFO(("KErrNone start root children check"));
            StartRootChildrenCheckL();
            break;

        case ECheckRootChildren:
            DLINFO(("KErrNone check root children"));
            CheckRootChildrenL();
            break;

        case EHandleBundleChildren:
            DLINFO(("KErrNone handle bundle children"));
            HandleBundleChildrenL();
            break;

        case ECleaningExpireds:
            DLINFO(("KErrNone cleaning expireds"));
            CleanExpiredsL();
            break;
                            
        case ECleaningExcess:
            DLINFO(("KErrNone cleaning excess"));
            CleanExcessL();
            break;

        case EStopping:
            DLINFO(("KErrNone EStopping"));
            StoppingL();
            break;

        case ENotStarted:
            DLINFO(("KErrNone ENotStarted"));
            // Nothing to do here
            break;
            
        default:
            DLERROR(("KErrNone default"));
            DASSERT( EFalse );
            break;
        }

    DLTRACEOUT((""));
    }


TInt CNcdNodeCacheCleaner::RunError( TInt aError )
    {
    DLTRACEIN(( "aError: %d", aError ));
    (void) aError;
    ResetState();
    return KErrNone;
    }

CNcdNodeManager& CNcdNodeCacheCleaner::NodeManager() const
    {
    return iNodeManager;
    }


CNcdNodeDbManager& CNcdNodeCacheCleaner::NodeDbManager() const
    {
    return iNodeDbManager;
    }


CNcdNodeFactory& CNcdNodeCacheCleaner::NodeFactory() const
    {
    return iNodeFactory;
    }


TBool CNcdNodeCacheCleaner::ContainsIdentifier( 
    const CNcdNodeIdentifier& aIdentifier,
    const RPointerArray<CDoNotRemoveIdentifier>& aTargetIdentifiers ) const
    {
    const TInt count = aTargetIdentifiers.Count();    
    // Compare given identifier to the identifiers in the array.
    for ( TInt i = 0; i < count; ++i )
        {
        if ( aTargetIdentifiers[ i ]->Key().Equals( aIdentifier ) )
            {
            // The given identifier was found from the array
            return ETrue;
            }
        }
    // Identifier was not found.
    return EFalse;
    }


TBool CNcdNodeCacheCleaner::AddIdentifiersL( const RPointerArray<CNcdNodeIdentifier>& aIdentifiers,
                                             RPointerArray<CNcdNodeIdentifier>& aTargetArray )
    {
    DLTRACEIN((""));
    
    TBool targetArrayChanged( EFalse );
    TBool identifierAdded( EFalse );
    const TInt count = aIdentifiers.Count(); 
    
    for ( TInt i = 0; i < count; ++i )
        {
        DASSERT( aIdentifiers[ i ] != NULL );
        
        identifierAdded = 
            AddIdentifierL( *aIdentifiers[ i ],
                            aTargetArray );
                            
        if ( !targetArrayChanged && identifierAdded )
            {
            DLINFO(("Identifier was added"));
            // Because the identifier was added into the array.
            // This is set only once because if something was added
            // then the array has been changed.
            // But, let the loop add all the other identifiers also.
            // So, do not break here.
            targetArrayChanged = ETrue;
            }
        }

    DLTRACEOUT((""));

    // Inform the called if something was added into the target array.
    return targetArrayChanged;
    }

TBool CNcdNodeCacheCleaner::AddIdentifierL( const CNcdNodeIdentifier& aIdentifier,
                                            RPointerArray<CNcdNodeIdentifier>& aTargetArray )
    {
    DLTRACEIN((""));
    
    // Compare the given identifier to the identifiers that have already inserted into the
    // array.
    const TInt count = aTargetArray.Count(); 
    for ( TInt i = 0; i < count; ++i )
        {
        DASSERT( aTargetArray[ i ] != NULL );
        
        // If the identifier was already in the array then, do not add it again.
        if ( aIdentifier.Equals( *aTargetArray[ i ] ) )
            {
            DLINFO(("Identifier was found. Return false."));
            // The identifier was found.
            // So, return EFalse to inform that identifier was not added.
            return EFalse;
            }
        }

    // The item was not in the array yet. So, append it into the array.

    CNcdNodeIdentifier* copyIdentifier = 
        CNcdNodeIdentifier::NewLC( aIdentifier );
    aTargetArray.AppendL( copyIdentifier );
    CleanupStack::Pop( copyIdentifier );

    DLTRACEOUT((""));

    // Return ETrue, because addition was done into the target array.
    return ETrue;
    }


TBool CNcdNodeCacheCleaner::RemoveIdentifiers( const RPointerArray<CNcdNodeIdentifier>& aIdentifiers,
                                               RPointerArray<CNcdNodeIdentifier>& aTargetArray )
    {
    DLTRACEIN((""));
    
    TBool targetArrayChanged( EFalse );
    TBool identifierRemoved( EFalse );
    const TInt count = aIdentifiers.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        DASSERT( aIdentifiers[ i ] != NULL );
        
        identifierRemoved = 
            RemoveIdentifier( *aIdentifiers[ i ],
                              aTargetArray );
                            
        if ( !targetArrayChanged && identifierRemoved )
            {
            DLINFO(("Identifier was removed"));
            // The identifier was removed from the array.
            // This value is set only once because if something was added
            // then the array has been changed.
            targetArrayChanged = ETrue;
            }
        }

    DLTRACEOUT((""));

    // Inform the caller if something was added into the target array.
    return targetArrayChanged;
    }

TBool CNcdNodeCacheCleaner::RemoveIdentifier( const CNcdNodeIdentifier& aIdentifier,
                                              RPointerArray<CNcdNodeIdentifier>& aTargetArray )
    {
    DLTRACEIN((""));

    TBool removed( EFalse );
        
    // Remove all occurrences of the identifier from the target array.
    // Start from the end of the array and move towards the beginning.
    for ( TInt i = aTargetArray.Count() - 1; i >= 0; --i )
        {
        DASSERT( aTargetArray[ i ] != NULL );

        if ( aIdentifier.Equals( *aTargetArray[ i ] ) )
            {
            DLINFO(("Remove identifier"));
            // The identifier was found from the array. So, remove it.
            delete aTargetArray[ i ];
            aTargetArray.Remove( i );
            removed = ETrue;
            // Because the indexing comes from the counter value towards
            // the zero, no need to update indexing here even if the removal
            // was done above.
            }
        }

    DLTRACEOUT((""));
    
    return removed;
    }


TBool CNcdNodeCacheCleaner::RemoveIdentifier( 
    const CNcdNodeIdentifier& aIdentifier,
    RPointerArray<CDoNotRemoveIdentifier>& aTargetArray,
    TBool aForceRemove )
    {
    DLTRACEIN((""));
    // Remove all occurrences of the identifier from the target array.
    // Start from the end of the array and move towards the beginning.
    TInt i = aTargetArray.Count();
    while ( i-- )    
        {
        DASSERT( aTargetArray[ i ] != NULL );

        if ( aIdentifier.Equals( aTargetArray[ i ]->Key() ) )
            {
            // Make sure that favorites are removed only if they are actually
            // removed from favorites
            if ( aForceRemove ||
                 !aTargetArray[ i ]->Value() ) 
                {                
                DLINFO(("Remove identifier"));
                // The identifier was found from the array. So, remove it.
                delete aTargetArray[ i ];
                aTargetArray.Remove( i );
                }
            return ETrue;
            }
        }

    DLTRACEOUT((""));    
    return EFalse;
    }


void CNcdNodeCacheCleaner::SetAllDbStorageNodeItemsL()
    {
    DLTRACEIN((""));

    // Just in case, first clean the identifier list.
    iAllDbStorageNodeItems.ResetAndDestroy();
    
    // Yes, we intentionally push a member variable to the cleanupstack
    CleanupResetAndDestroyPushL( iAllDbStorageNodeItems );
    // Get the new values for cleaning.
    // Note that some of the items are not accepted
    // for this list.

    NodeDbManager().
        GetAllClientItemIdentifiersL(
            iAllDbStorageNodeItems, 
            ClientUid(),
            *iDoNotCleanNameSpaces,
            iNodeCleanupTypes );
    
    CleanupStack::Pop( &iAllDbStorageNodeItems );
    // Sort the identifiers in the array.
    // The first item is closest to the root. The last item is
    // deepest in the hierarchy.    
    // Give the function pointer of the static function as a parameter.
    
    DLTRACE(("start sorting"));
    TLinearOrder<CNcdNodeIdentifier> sorter( &NodeIdentifierArraySortById );
    iAllDbStorageNodeItems.Sort( sorter );
    DLTRACE(("sorting ended"));
    
    // Print array debug info into the log file
    #ifdef NCD_NODE_CACHE_CLEANER_DEBUG_ARRAY_PRINT
    DLINFO(("iAllDbStorageNodeItems array:"));
    ArrayDebugPrint( iAllDbStorageNodeItems );
    #endif

    DLTRACEOUT((""));
    }


void CNcdNodeCacheCleaner::SetAllDbStorageIconItemsL()
    {
    DLTRACEIN((""));
    
    // Just in case first clean the identifier list.
    iAllDbIconItems.ResetAndDestroy();
    
    // Yes, we intentionally push a member variable to the cleanupstack
    CleanupResetAndDestroyPushL( iAllDbIconItems );
    
    // Get the new values for cleaning.
    // Note that some of the items are not accepted
    // for this list.
    
    NodeDbManager().
        GetAllClientItemIdentifiersL(
            iAllDbIconItems, 
            ClientUid(),
            *iDoNotCleanNameSpaces,
            iIconCleanupTypes );
    
    CleanupStack::Pop( &iAllDbIconItems );
    // Icon ids do not need to be sorted, because they do not contain node ids but
    // are something else.
        
    // Print array debug info into the log file
    #ifdef NCD_NODE_CACHE_CLEANER_DEBUG_ARRAY_PRINT
    DLINFO(("iAllDbIconItems array:"));
    ArrayDebugPrint( iAllDbIconItems );
    #endif   
    
    DLTRACEOUT(("")); 
    }



void CNcdNodeCacheCleaner::SetAllDbStorageScreenshotItemsL()
    {
    DLTRACEIN((""));
    // Just in case first clean the identifier list.
    iAllDbScreenshotItems.ResetAndDestroy();
    
    // Yes, we intentionally push a member variable to the cleanupstack
    CleanupResetAndDestroyPushL( iAllDbScreenshotItems );
    
    // Get the new values for cleaning.
    // Note that some of the items are not accepted
    // for this list.
    NodeDbManager().
        GetAllClientItemIdentifiersL(
            iAllDbScreenshotItems, 
            ClientUid(),
            *iDoNotCleanNameSpaces,
            iScreenshotCleanupTypes );    

    CleanupStack::Pop( &iAllDbScreenshotItems );
    
    // Print array debug info into the log file
    #ifdef NCD_NODE_CACHE_CLEANER_DEBUG_ARRAY_PRINT
    DLINFO(("iAllDbScreenshotItems array:"));
    ArrayDebugPrint( iAllDbScreenshotItems );
    #endif           
    }

    
void CNcdNodeCacheCleaner::AddChildrenToCleanupArrayL()
    {
    DLTRACEIN((""));
    
    // Get the counter value. So, we know how many items are
    // in the list before children are added
    TInt counter = iCleanupIdentifiers.Count();
    
    // Check if any children are found from the db items.
    // If they are found, then move them to the cleanup array.
    CNcdNodeIdentifier* cleanupIdentifier( NULL );
    CNcdNodeIdentifier* storageItemIdentifier( NULL );
    for ( TInt i = 0; i < counter; ++i )
        {
        cleanupIdentifier = iCleanupIdentifiers[ i ];
        // Start from the end of the list. So, the indexing is
        // always correct even if an item is moved
        for ( TInt j = iAllDbStorageNodeItems.Count(); j > 0; --j )
            {
            storageItemIdentifier = iAllDbStorageNodeItems[ j - 1 ];
            if ( NcdNodeIdentifierEditor::NodeDepthL( *cleanupIdentifier ) 
                    > NcdNodeIdentifierEditor::NodeDepthL( *storageItemIdentifier ) )
                {
                // Because the db items array is sorted so that the deepest items are
                // in the end of the array, the children can not be found anymore.
                // (Notice that db items are gone through from the end of the array.)
                DLINFO(("Child parent barrier passed: %d", j - 1 ));
                break;
                }
            else if ( NcdNodeIdentifierEditor::ParentOf( *cleanupIdentifier,
                                                         *storageItemIdentifier )
                      && !ContainsIdentifier( *storageItemIdentifier, 
                                              iDoNotRemoves )
                      && !NcdNodeIdentifierUtils::ContainsIdentifier(
                        *storageItemIdentifier, iWaitingIdentifiers )
                      && !NcdNodeIdentifierUtils::ContainsIdentifier(
                        *storageItemIdentifier, iWaitingParentIdentifiers ) )
                {
                DLINFO(("Children items found and can be moved"));
                // The child is not in do not remove list or in the waiting list.
                // So, it can be inserted into the cleanup list. 
                // Notice that before cleaning it should
                // be checked that the child is not a parent of some other child that
                // can not be removed. 
                // Use this function to check that the item is not appended twice
                AddCleanupIdentifierL( *storageItemIdentifier );
                // Remove the data from its original place. Notice that identifier
                // is copied in the function above.
                delete storageItemIdentifier;
                iAllDbStorageNodeItems.Remove( j - 1 );
                }
            }
        }

    // Print array debug info into the log file
    #ifdef NCD_NODE_CACHE_CLEANER_DEBUG_ARRAY_PRINT
    DLINFO(("iCleanupIdentifiers array:"))
    ArrayDebugPrint( iCleanupIdentifiers );
    #endif
    
    DLTRACEOUT((""));
    }


void CNcdNodeCacheCleaner::ParentCleaningCheckL()
    {
    DLTRACEIN((""));

    // Print array debug info into the log file
    #ifdef NCD_NODE_CACHE_CLEANER_DEBUG_ARRAY_PRINT
    DLINFO(("iDoNotRemoves array:"));
    ArrayDebugPrint( iDoNotRemoves );
    #endif

    // Print array debug info into the log file
    #ifdef NCD_NODE_CACHE_CLEANER_DEBUG_ARRAY_PRINT
    DLINFO(("iWaitingParentIdentifiers array:"));
    ArrayDebugPrint( iWaitingParentIdentifiers );
    #endif

    // Print array debug info into the log file
    #ifdef NCD_NODE_CACHE_CLEANER_DEBUG_ARRAY_PRINT
    DLINFO(("iWaitingIdentifiers array:"));
    ArrayDebugPrint( iWaitingIdentifiers );
    #endif
    
    // Check if there are some parents that are going to be cleaned but
    // they should not be because their children are in the do not remove list
    // or in the in the waiting list
    for ( TInt i = iCleanupIdentifiers.Count(); i > 0 ; --i )
        {
        for ( TInt j  = 0; j < iDoNotRemoves.Count(); ++j )
            {
            // first check if parent can not be removed (for favorites it can be removed)
            if ( !iDoNotRemoves[ j ]->Value() && 
                 NcdNodeIdentifierEditor::ParentOf( *iCleanupIdentifiers[ i - 1 ], 
                                                    iDoNotRemoves[ j ]->Key() ) )
                {
                DLINFO(("Do not remove parent because child is do not remove: %d", i))
                // Because the cleanup identifier was a parent of something that
                // should not be removed, then do not remove the parent either,
                // but insert it to wait.
                AddIdentifierL( *iCleanupIdentifiers[ i - 1 ], iWaitingParentIdentifiers );
                delete iCleanupIdentifiers[ i - 1 ];
                iCleanupIdentifiers.Remove( i - 1 );

                // Because the identifier was now removed from the array
                // and moved to the waiting array, we skip to the next identifier check.
                // This way the i-indexing will be correct.
                break;
                }
            }
        }

    // Check the waiting identifiers in their own loop, so the i indexing
    // will be correct here if removing has been done above.            
    for ( TInt i = iCleanupIdentifiers.Count(); i > 0 ; --i )
        {
        for ( TInt k = 0; k < iWaitingIdentifiers.Count(); ++k )
            {
            if ( NcdNodeIdentifierEditor::ParentOf( *iCleanupIdentifiers[ i - 1 ], 
                                                    *iWaitingIdentifiers[ k ] ) )
                {
                DLINFO(("Do not remove parent because child is waiting: %d", i))
                // Because the cleanup identifier was a parent of something that
                // should not be removed, then do not remove the parent either,
                // but insert it to wait.
                AddIdentifierL( *iCleanupIdentifiers[ i - 1 ], 
                                iWaitingParentIdentifiers );
                delete iCleanupIdentifiers[ i - 1 ];
                iCleanupIdentifiers.Remove( i - 1 );                

                // Because the identifier was now removed from the array
                // and moved to the waiting array, we skip to the next identifier check.
                // This way the i-indexing will be correct.
                break;
                }
            }
        }

    // Now check if there are some parents waiting that should not any more
    TBool wasNotFound( ETrue );
    for ( TInt i = iWaitingParentIdentifiers.Count(); i > 0 ; --i )
        {
        wasNotFound = ETrue;
        for ( TInt j  = 0; j < iDoNotRemoves.Count(); ++j )
            {
            if ( !iDoNotRemoves[ j ]->Value() &&
                NcdNodeIdentifierEditor::ParentOf( *iWaitingParentIdentifiers[ i - 1 ], 
                                                    iDoNotRemoves[ j ]->Key() ) )
                {
                // The waiting parent should still be waiting
                wasNotFound = EFalse;
                break; 
                }
            }
        for ( TInt k  = 0; k < iWaitingIdentifiers.Count() && wasNotFound; ++k )
            {
            if ( NcdNodeIdentifierEditor::ParentOf( *iWaitingParentIdentifiers[ i - 1 ], 
                                                    *iWaitingIdentifiers[ k ] ) )
                {
                // The waiting parent should still be waiting
                wasNotFound = EFalse;
                break; 
                }
            }

        if ( wasNotFound )
            {
            DLINFO(("Waiting parent can be moved to the cleanup list: %d", i));
            // Because the waiting identifier was not a parent of something that
            // should not be removed anymore. Move it to the cleanup list.
            AddIdentifierL( *iWaitingParentIdentifiers[ i - 1 ], iCleanupIdentifiers );
            delete iWaitingParentIdentifiers[ i - 1 ];
            iWaitingParentIdentifiers.Remove( i - 1 );            
            }
        }

    // Print array debug info into the log file
    #ifdef NCD_NODE_CACHE_CLEANER_DEBUG_ARRAY_PRINT
    DLINFO(("iCleanupIdentifiers array:"));
    ArrayDebugPrint( iCleanupIdentifiers );
    #endif

    DLTRACEOUT((""));        
    }


void CNcdNodeCacheCleaner::RemoveDoNotRemovesFromCleanupArray()
    {
    DLTRACEIN((""));
    TInt doNotRemoveCount = iDoNotRemoves.Count();
    while ( doNotRemoveCount-- ) 
        {
        TInt cleanupCount = iCleanupIdentifiers.Count();
        while( cleanupCount-- ) 
            {
            if ( iDoNotRemoves[ doNotRemoveCount ]->Key().Equals( 
                *iCleanupIdentifiers[ cleanupCount ] ) ) 
                {
                DLTRACE(("Removing"));
                DLNODEID(( *iCleanupIdentifiers[ cleanupCount ] ));
                delete iCleanupIdentifiers[ cleanupCount ];
                iCleanupIdentifiers.Remove( cleanupCount );
                break;
                }
            }
        }
    }
    
    
void CNcdNodeCacheCleaner::CheckTemporaryNodesL()
    {
    DLTRACEIN((""));
    
    TInt itemCount( iAllDbStorageNodeItems.Count() );            

    CNcdNodeIdentifier* identifier( NULL );
    
    for ( TInt i = itemCount; i > 0; --i )
        {
        identifier = iAllDbStorageNodeItems[ i - 1 ];

        // All the nodes that do not start with the root id are thought to be
        // temporary nodes.
        if ( NcdNodeIdentifierEditor::IdentifiesTemporaryNodeL( *identifier ) )
            {
            DLINFO((_L("Remove temporary node: %S, %S, %S, %d"),
                    &identifier->NodeNameSpace(), &identifier->NodeId(),
                    &identifier->ServerUri(), identifier->ClientUid().iUid));
            // This is a temporary node.
            // Insert it into the cleanup array if removal is allowed.
            AddCleanupIdentifierL( *identifier );

            // Remove the data from its original place. Notice that identifier
            // is copied in the function above.
            delete iAllDbStorageNodeItems[ i - 1 ];
            iAllDbStorageNodeItems.Remove( i - 1 );
            }
        }
    
    DLTRACEOUT((""));
    }


void CNcdNodeCacheCleaner::ResetState()
    {
    DLTRACEIN((""));

    iExpiredCleaningIndex = 0;
    iExcessCleanupStarted = EFalse;
        
    // Do not waste space with old info.
    iAllDbStorageNodeItems.ResetAndDestroy();
    iAllDbIconItems.ResetAndDestroy();
    iDbMetaIdentifiers.ResetAndDestroy();
    iRootChildren.ResetAndDestroy();
    iBundleChildren.ResetAndDestroy();

    iAllDbScreenshotItems.ResetAndDestroy();
    
    // Set the priority to the correct value
    // for the next cleaning round. The priority
    // may have been changed if db was too full.
    iIncreasePriority = KDefaultCleaningPriority;
    if ( IsActive() )
        {
        DLINFO(("Active. Priority should be reseted later."));
        // Set the priority flags for the member variables.
        // This value will be used in later in StartCleanupL 
        // where the priority is set for this active object next time 
        // they are called. This check is required because DoCancel uses 
        // this function and then the active object may still be active until 
        // DoCancel finishes its job.
        iResetPriority = ETrue;        
        }
    else
        {
        DLINFO(("Not active"));
        // Set the correct priority value.
        // The priority can also be set for the active object because
        // it is not active at the moment.
        SetPriority( KDefaultCleaningPriority );        
        }
    
    // Set state and other flags. To inital values.    
    iCleanupState = ENotStarted;

    DLTRACEOUT((""));
    }


#ifdef NCD_NODE_CACHE_CLEANER_DEBUG_ARRAY_PRINT
void CNcdNodeCacheCleaner::ArrayDebugPrint( const RPointerArray<CNcdNodeIdentifier>& aIdentifiers ) const
    {
    DLTRACEIN((""));
    
    CNcdNodeIdentifier* identifier( NULL );
    (void) *identifier; // prevents compiler warning of unused variable

    for ( TInt i = 0; i < aIdentifiers.Count(); ++i )
        {
        identifier = aIdentifiers[ i ];
        DLINFO((_L("Array identifier: %S, %S, %S, %d"),
                &identifier->NodeNameSpace(), &identifier->NodeId(),
                &identifier->ServerUri(), identifier->ClientUid().iUid));
        }    

    DLTRACEOUT((""));
    }

void CNcdNodeCacheCleaner::ArrayDebugPrint( const RPointerArray<CDoNotRemoveIdentifier>& aIdentifiers ) const
    {
    DLTRACEIN((""));
    
    const CNcdNodeIdentifier* identifier( NULL );
    (void) *identifier; // prevents compiler warning of unused variable

    for ( TInt i = 0; i < aIdentifiers.Count(); ++i )
        {
        identifier = &aIdentifiers[ i ]->Key();
        DLINFO((_L("Array identifier: %S, %S, %S, %d"),
                &identifier->NodeNameSpace(), &identifier->NodeId(),
                &identifier->ServerUri(), identifier->ClientUid().iUid));
        }    

    DLTRACEOUT((""));
    }

#endif // NCD_NODE_CACHE_CLEANER_DEBUG_ARRAY_PRINT


void CNcdNodeCacheCleaner::CleanSpecialsL()
    {
    DLTRACEIN((""));

    // Try to clean some nodes if they are in the cleanuplist.
    // Do not reset the node list or delete it here.
    // This list is used throughout the cleaning process. 
    // The array might be updated only in the start excess cleaning function.
    // Other functions should not update the list, because otherwise the
    // excess cleaner may go into the infinite loop if deepest nodes cannot
    // be removed. 
    HandleCleaningL( EFalse, EFalse );

    // Next step will be cleaning temporary nodes that
    // are not used any more. If too much space is used,
    // some extra cleaning will also be done by looping these steps.        
    iCleanupState = ECleaningTemporaryNodes;

    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* ptrStatus = &iStatus;
    User::RequestComplete( ptrStatus, KErrNone );
        
    DLTRACEOUT((""));        
    }


void CNcdNodeCacheCleaner::CleanTemporaryNodesL()
    {
    DLTRACEIN((""));

    // Notice that the iAllDbStorageNodeItems may not be up to date here if
    // some new nodes have been inserted into the database after the cleaning
    // has been started. So, if those new items are temporary objects they will
    // not be checked here. The update of the array is omitted here to save some
    // time, because db-actions may take long time. Also, the array should not
    // be updated here, because otherwise excess removing may go into the infinite loop.
    // The omitting of some temporary nodes does not matter because they will be 
    // cleaned next time the cleaner starts it actions.   
    CheckTemporaryNodesL();

    // HandleCleaningL and the functions it uses can handle the removing of the
    // item from the db array.
    // Just use the old node list.
    // Do not delte the list because if excess cleaning is done, then the list should
    // not be reloaded. Reloading might cause a infinite loop.
    HandleCleaningL( EFalse, EFalse );

    iCleanupState = ECleaningHangingCases;
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* ptrStatus = &iStatus;
    User::RequestComplete( ptrStatus, KErrNone );        
            
    DLTRACEOUT((""));
    }


void CNcdNodeCacheCleaner::CleanHangingsL()
    {
    DLTRACEIN((""));
    
    // The node items list has to be updated here. So, we can be sure that
    // both the node array and metadata array are up to date when hangings
    // are searched. If node list were not up to date, then some of the
    // the metadata may be mistakenly thought to be hanging.

    // Get the node identifiers for the list. We do not use the member variable
    // iAllDbStorageNodeItems here, because it should only be updated when the
    // cleaning is started or in the excess clean function.
    // Notice that here it does not matter that the nodes are not in specific
    // order. Because all the nodes are compared to metadatas.
    RPointerArray<CNcdNodeIdentifier> dbNodeIdentifiers;
    CleanupResetAndDestroyPushL( dbNodeIdentifiers );
    NodeDbManager().
        GetAllClientItemIdentifiersL(
            dbNodeIdentifiers, 
            ClientUid(),
            *iDoNotCleanNameSpaces,
            iNodeCleanupTypes );    

    // Get all the metadata identifiers from the db.
    // (left the do not clean namespaces out because they
    // are ignored in all cases here)
    RPointerArray<CNcdNodeIdentifier> dbMetaIdentifiers;
    CleanupResetAndDestroyPushL( dbMetaIdentifiers );
    NodeDbManager().
        GetAllClientItemIdentifiersL(
            dbMetaIdentifiers, 
            ClientUid(),
            *iDoNotCleanNameSpaces,
            iMetaCleanupTypes );    

    DLINFO(("Db node count: %d", dbNodeIdentifiers.Count()));
    // Print array debug info into the log file
    #ifdef NCD_NODE_CACHE_CLEANER_DEBUG_ARRAY_PRINT
    DLINFO(("dbNodeIdentifiers array:"));
    ArrayDebugPrint( dbNodeIdentifiers );
    #endif

    DLINFO(("Db meta count: %d", dbMetaIdentifiers.Count()));
    // Print array debug info into the log file
    #ifdef NCD_NODE_CACHE_CLEANER_DEBUG_ARRAY_PRINT
    DLINFO(("dbNodeIdentifiers array:"));
    ArrayDebugPrint( dbMetaIdentifiers );
    #endif
        
    // Check if the node corresponding metadata identifier can be
    // found from the db node list.
    // If it can not be found, then the item has been left to hang in
    // the db and can be removed.
    CNcdNodeIdentifier* identifier( NULL );
    TBool nodeFound( EFalse );
    TInt j = dbMetaIdentifiers.Count();
    const TInt count = dbNodeIdentifiers.Count();
    
    while ( j-- )
        {
        nodeFound = EFalse;
                 
        for ( TInt i = 0; i < count; ++i )
            {
            // Get the metadata part of the node name
            identifier = 
                NcdNodeIdentifierEditor::
                    CreateMetaDataIdentifierL( *dbNodeIdentifiers[ i ] );
                        
            // Check if the metadata part of the node id
            // equals the metadata part of the list.
            if ( identifier->Equals( *dbMetaIdentifiers[ j ] ) )
                {
                // The identifiers equal so corresponding node was found
                nodeFound = ETrue;
                
                delete identifier;
                identifier = NULL;    

                // Because node was found the meta can be removed from the
                // list
                delete dbMetaIdentifiers[ j ];
                dbMetaIdentifiers.Remove( j );
                
                break;
                }
            else
                {
                delete identifier;
                identifier = NULL;                
                }
            }                

        if ( !nodeFound )
            {
            // If node was not found from DB, it is possibly in DoNotRemove list.
            // Don't delete the metadata of such a node.
            const TInt doNotRemovesCount = iDoNotRemoves.Count();
            for ( TInt i = 0; i < doNotRemovesCount; i++ ) 
                {
                identifier =
                    NcdNodeIdentifierEditor::CreateMetaDataIdentifierL( 
                        iDoNotRemoves[i]->Key() );
                
                // Check if the metadata part of the node id equals the metadata part
                // of the list.
                if ( identifier->Equals( *dbMetaIdentifiers[ j ] ) ) 
                    {
              
                    delete identifier;
                    identifier = NULL;
                    
                    delete dbMetaIdentifiers[ j ];
                    dbMetaIdentifiers.Remove( j );
                    
                    break;
                    }
                else 
                    {
                    delete identifier;
                    identifier = NULL;
                    }
                }
            }            
        }

    DLINFO(("Remove hangings from db: %d", dbMetaIdentifiers.Count()));
    // Print array debug info into the log file
    #ifdef NCD_NODE_CACHE_CLEANER_DEBUG_ARRAY_PRINT
    DLINFO(("dbMetaIdentifiers array:"));
    ArrayDebugPrint( dbMetaIdentifiers );
    #endif

    // No need to check do not remove items here, because all the items
    // that are hanging are independent from the nodes. That is also why,
    // we do not need to do any parent child relation checking either.
    NodeDbManager().
        RemoveDataFromDatabaseL( dbMetaIdentifiers,
                                 iMetaCleanupTypes );

    CleanupStack::PopAndDestroy( &dbMetaIdentifiers );
    CleanupStack::PopAndDestroy( &dbNodeIdentifiers );
 
    // Next handle excess cleaning
    iCleanupState = EStartCleaningHangingSpecialCases;
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* ptrStatus = &iStatus;
    User::RequestComplete( ptrStatus, KErrNone );        
   
    DLTRACEOUT((""));
    }


void CNcdNodeCacheCleaner::StartCleanSpecialHangingsL()
    {
    DLTRACEIN((""));

    // Notice that the special hanging only need metadata objects.

    // Get all the metadata identifiers from the db.
    // (left the do not clean namespaces out because they
    // are ignored in all cases here)
    // Notice that the icon array is also updated below. So,
    // the metadata and the icon infromation will be in sync.
    iDbMetaIdentifiers.ResetAndDestroy();
    NodeDbManager().
        GetAllClientItemIdentifiersL(
            iDbMetaIdentifiers, 
            ClientUid(),
            *iDoNotCleanNameSpaces,
            iMetaCleanupTypes );    

    DLINFO(("Db meta count: %d", iDbMetaIdentifiers.Count()));
    // Print array debug info into the log file
    #ifdef NCD_NODE_CACHE_CLEANER_DEBUG_ARRAY_PRINT
    DLINFO(("iDbMetaIdentifiers array:"));
    ArrayDebugPrint( iDbMetaIdentifiers );
    #endif

    // Start to clean all the special cases.
    SetAllDbStorageIconItemsL();

    SetAllDbStorageScreenshotItemsL();    

    // Next clean the excess special cases
    iCleanupState = ECleaningHangingSpecialCases;    
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* ptrStatus = &iStatus;
    User::RequestComplete( ptrStatus, KErrNone );        
        
    DLTRACEOUT((""));
    }


void CNcdNodeCacheCleaner::CleanSpecialHangingsL()
    {
    DLTRACEIN((""));

    if ( iDbMetaIdentifiers.Count() > 0 )
        {
        DLINFO(("More metas left"));

        // There are still some metadatas.
        // So, check their special contents.
        CNcdNodeMetaData* metaData( NULL );
        HBufC8* data( NULL );
        CNcdNodeIdentifier* metaDataIdentifier( iDbMetaIdentifiers[ 0 ] );
            
        // The read operation may leave for example with KErrNotFound if the
        // item was not found. Ignore the error and just continue normally.
        TRAP_IGNORE( 
            data =
                NodeDbManager().
                    ReadDataFromDatabaseL( *metaDataIdentifier,
                                           NcdNodeClassIds::ENcdMetaData ) );

        if ( data != NULL )
            {
            CleanupStack::PushL( data );
            if ( *data != KNullDesC8 )
                {
                TRAP_IGNORE( 
                    metaData = 
                        NodeFactory().CreateMetaDataL( *metaDataIdentifier, 
                                                       *data ) );
                if ( metaData != NULL )
                    {
                    DLINFO(("Metadata was found"));
                    
                    // Notice that the metadata should be closed not deleted!
                    CleanupClosePushL( *metaData );

                    // Now that we have the metadata.
                    // Check the icons
                    CNcdNodeIcon* icon( NULL );
                    TRAP_IGNORE( icon = &metaData->IconL() );
                    if ( icon != NULL )
                        {
                        DLINFO(("icon check"));
                        // Check if the icon can be found from the icon list
                        // If its found then remove it from the list.
                        CNcdNodeIdentifier* iconIdentifier( NULL );                        
                        for ( TInt i = iAllDbIconItems.Count(); i > 0; --i )
                            {
                            iconIdentifier = iAllDbIconItems[ i - 1 ];
                            if ( icon->IconId() == iconIdentifier->NodeId() )
                                {
                                // Icon was found.
                                DLINFO((_L("Icon found and no need to remove: %S"), 
                                        &iconIdentifier->NodeId()));
                                delete iAllDbIconItems[ i - 1 ];
                                iAllDbIconItems.Remove( i - 1 );
                                }
                            }
                        }


                    const CNcdNodeScreenshot* screenshot( NULL );
                    TRAP_IGNORE( screenshot = &metaData->ScreenshotL() );
                    if ( screenshot != NULL )
                        {
                        DLINFO(("screenshot check"));
                        // Check if the screenshot can be found from the icon list
                        // If its found then remove it from the list.
                        CNcdNodeIdentifier* screenshotIdentifier( NULL );                        
                        for ( TInt i = iAllDbScreenshotItems.Count(); i > 0; --i )
                            {
                            screenshotIdentifier = iAllDbScreenshotItems[ i - 1 ];
                            TInt shotCount = screenshot->ScreenshotDownloadCount();
                            DLTRACE(("Going through %d screenshots", shotCount ));
                            while ( shotCount-- ) 
                                {                                
                                if ( screenshot->ScreenshotDownloadUri( shotCount ) 
                                     == screenshotIdentifier->NodeId() )
                                    {
                                    // Screenshot was found.
                                    DLINFO(("screenshot found"));
                                    delete iAllDbScreenshotItems[ i - 1 ];
                                    iAllDbScreenshotItems.Remove( i - 1 );
                                    }
                                }
                            }
                        }

                    // When metadata is deleted, also icons, 
                    // etc. are deleted.
                    CleanupStack::PopAndDestroy( metaData );                
                    }
                }
            CleanupStack::PopAndDestroy( data );
            }

        // Remove the used identifier.
        // So, next round will have a new identifier in the beginning of the array
        delete iDbMetaIdentifiers[ 0 ];
        iDbMetaIdentifiers.Remove( 0 );

        // Continue cleaning special cases
        iCleanupState = ECleaningHangingSpecialCases;            
        }
    else
        {
        // All the metadatas have been gone through.
        // So, the special arrays will contain
        // identifiers of the items that should be removed
        // from the db.
        // So, go to the next level.
        iCleanupState = EFinishCleaningHangingSpecialCases;        
        }

    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* ptrStatus = &iStatus;
    User::RequestComplete( ptrStatus, KErrNone );        
        
    DLTRACEOUT((""));
    }


void CNcdNodeCacheCleaner::FinishCleanSpecialHangingsL()
    {
    DLTRACEIN((""));
    
    // Special arrays contain only the identifiers that
    // belong to no metadata.
    // So, delete those icons.

    DLINFO(("Remove icons from db: %d", iAllDbIconItems.Count()));
    // Print array debug info into the log file
    #ifdef NCD_NODE_CACHE_CLEANER_DEBUG_ARRAY_PRINT
    DLINFO(("iAllDbIconItems array:"));
    ArrayDebugPrint( iAllDbIconItems );
    #endif

    NodeDbManager().
        RemoveDataFromDatabaseL( iAllDbIconItems,
                                 iIconCleanupTypes );   
    iAllDbIconItems.ResetAndDestroy();


    DLINFO(("Remove screenshot from db: %d", iAllDbScreenshotItems.Count() ));
    NodeDbManager().
        RemoveDataFromDatabaseL( iAllDbScreenshotItems,
                                 iScreenshotCleanupTypes );
    iAllDbScreenshotItems.ResetAndDestroy();

    // Everything is done here.
    // So, go to the next level.
    if ( iExcessCleanupStarted )
        {
        // Because excess cleanup has already been started we have looped here back.
        // So, skip the start and go directly to the correct cleaning procedure.
        // Notice, that this also skips the expireds check, which is what we want
        // because expired should be checked only once to save some time. If for some
        // reason some nodes will expire during this cleanup it does not matter.
        // Also, transparency and bundle checks are skipped, because they will be
        // handled already during the first loop.
        // They will be cleaned another time.
        iCleanupState = ECleaningExcess;
        }
    else
        {
        // Start cleaning.
        iCleanupState = EStartCleaningExcess;        
        }
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* ptrStatus = &iStatus;
    User::RequestComplete( ptrStatus, KErrNone );            

    DLTRACEOUT((""));
    }


void CNcdNodeCacheCleaner::StartCleanExcessL()
    {
    DLTRACEIN((""));

    TInt currentDbSize(
            NodeDbManager().StorageSizeL( ClientUid(),
                                          *iDoNotCleanNameSpaces ) );
    TInt compareSize( DbMaxSize() );        

    DLINFO(("current db size: %d, compare size: %d", 
            currentDbSize, compareSize));

    TBool stopCleaning( ETrue );
    if ( compareSize < currentDbSize )
        {
        // Get the current node items from the db. These will be gone through
        // to decide what should be removed if the current database size is too
        // big.
        // This is the last change to update the list before the excess cleaning
        // starts to loop. Then the node list should not be updated because
        // that might result to a infinite loop, if deepest nodes can not be removed.
        // We will not come back here if excess cleaning is looping.
        // Notice that actually it does not matter if not all the items can not be
        // checked here. They will be checked next time the cleaning is started.
        SetAllDbStorageNodeItemsL();
        if ( iAllDbStorageNodeItems.Count() > 0 )
            {
            // We still have to cleanup some more.
            // So, make new request for the next step.
            stopCleaning = EFalse;                    
            iExcessCleanupStarted = ETrue;
            
            // Next check the root children, transparent and bundle nodes
            // and after that expired nodes before actually removing the
            // deepest nodes.
            iCleanupState = EStartRootChildrenCheck;
            }
        }

    // Nothing to clean or the database size is small enough.        
    if ( stopCleaning )
        {
        DLINFO(("Excess cleaned"));
        // Just in case somebody has added something for removal.
        // This call will update the node list once more from db
        // and reset it when cleaning is done. The array update is ok
        // here, because no additional loop will be done.
        HandleCleaningL( ETrue, ETrue );
        // Nothing to do next
        iCleanupState = EStopping;
        }

    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* ptrStatus = &iStatus;
    User::RequestComplete( ptrStatus, KErrNone );        
        
    DLTRACEOUT((""));
    }


void CNcdNodeCacheCleaner::StartRootChildrenCheckL()
    {
    DLTRACEIN((""));
    
    // Bundles are always direct children of the root.
    // Transparent folders are always direct children of the root
    // or the bundles.
    // So, start the checking from the root node and its children.
    
    CNcdNodeIdentifier* rootIdentifier =
        NcdNodeIdentifierEditor::CreateRootIdentifierForClientLC( ClientUid() );

    // As a default, make the next step to be cleaning expireds.
    // This will skip the transparent and bundle checks.
    // If the root is found below, then the state will be changed to the
    // required value.
    iCleanupState = ECleaningExpireds;
    
    // Here, we remove the root from the db node list if it is included there.
    // The root should not be removed from the db later anyway. 
    // So, we may as well remove it from the db node list now.
    iRootChildren.ResetAndDestroy();
    if ( RemoveIdentifier( *rootIdentifier, iAllDbStorageNodeItems ) )
        {
        // Root was in the list. So, it can be found from db. 
        // Thus, we can try to create the root node here.
        // Also, check the root children.
        CNcdRootNode* root( NULL );
        CNcdNode* node( NULL );
        HBufC8* nodeData( NULL );
        
        // The read operation may leave for example with KErrNotFound if the
        // item was not found. Ignore the error and just continue normally.
        TRAP_IGNORE( 
            nodeData =
                NodeDbManager().
                    ReadDataFromDatabaseL( *rootIdentifier,
                                           NcdNodeClassIds::ENcdNode ) );
        if ( nodeData != NULL )
            {
            CleanupStack::PushL( nodeData );
            if ( *nodeData != KNullDesC8 )
                {
                TRAP_IGNORE( 
                    node = 
                        NodeFactory().CreateNodeL( *rootIdentifier, 
                                                   *nodeData ) );
                                                   
                if ( node != NULL )
                    {
                    // Push the node into the cleanup stack because checking
                    // may leave.
                    CleanupClosePushL( *node );
                    if ( CNcdNodeFactory::NodeTypeL( *node ) 
                         == CNcdNodeFactory::ENcdNodeRoot )
                        {
                        // Only handle the root.
                        root = static_cast<CNcdRootNode*>( node );                       
                        // Release the node from the cleanupstack.
                        // It will be put back as a root next.
                        CleanupStack::Pop( node );
                        }
                    else
                        {
                        DLERROR(("The node with root identifier was not root."))
                        DASSERT( EFalse );
                        // Delete the node because it was not of the right type.
                        // The root will be left to NULL.
                        CleanupStack::PopAndDestroy( node );
                        node = NULL;
                        }
                    }
                        
                if ( root != NULL )
                    {
                    // Notice that the node should be closed not deleted!
                    CleanupClosePushL( *root );
                    const RPointerArray<CNcdChildEntity>& children = root->ChildArray();
                    for ( TInt i = 0; i < children.Count(); i++ )
                        {
                        // Because the root child will be added into the root list
                        // and those children are handled separately, remove
                        // the children also from the original node list.
                        // When iAllDbStorageNodeItems was initialized it contained all the
                        // identifiers of all the nodes in db.
                        // Notice, that if the children are not in the db node list, then
                        // they will not be removed from the db here.
              
                        // So, check if the identifier is found from the db list and remove it
                        // if found.
                        if ( RemoveIdentifier( children[i]->Identifier(), iAllDbStorageNodeItems ) )
                            {
                            // The root child was in the db list and it was removed.
                            // Now, insert the identifier into the temporary root list.
                            // So, these children will be checked later for transparency and
                            // bundle features.
                            AddIdentifierL( children[i]->Identifier(), iRootChildren );
                            }
                        }
                    CleanupStack::PopAndDestroy( root );
                    
                    // Because we got the root, then next step is to handle its
                    // children.
                    iCleanupState = ECheckRootChildren; 
                    }
                }

            // Delete node data.
            CleanupStack::PopAndDestroy( nodeData );
            }        
        }
        
    CleanupStack::PopAndDestroy( rootIdentifier );   

    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* ptrStatus = &iStatus;
    User::RequestComplete( ptrStatus, KErrNone );        
    
    DLTRACEOUT(("")); 
    }


void CNcdNodeCacheCleaner::CheckRootChildrenL()
    {
    DLTRACEIN((""));

    if ( iRootChildren.Count() > 0 )
        {
        DLINFO(("Root children in the array."));

        // As a default loop here after this stuff is done.
        // This way next loop will check if there are still more children
        // to be checked.
        iCleanupState = ECheckRootChildren;                        

        // All the children of the root should always be folders.
        CNcdNode* node( NULL );    
        CNcdNodeFolder* folder( NULL );
        HBufC8* nodeData( NULL );
        CNcdNodeIdentifier* rootChildIdentifier( iRootChildren[ 0 ] );

        // The read operation may leave for example with KErrNotFound if the
        // item was not found. Ignore the error and just continue normally.
        TRAP_IGNORE( 
            nodeData =
                NodeDbManager().
                    ReadDataFromDatabaseL( *rootChildIdentifier,
                                           NcdNodeClassIds::ENcdNode ) );
        if ( nodeData != NULL )
            {
            CleanupStack::PushL( nodeData );
            if ( *nodeData != KNullDesC8 )
                {
                TRAP_IGNORE( 
                    node =
                        NodeFactory().CreateNodeL( *rootChildIdentifier, 
                                                   *nodeData ) );
                if ( node != NULL )
                    {
                    // Push the node into the cleanup stack because checking
                    // may leave.
                    CleanupClosePushL( *node );
                    if ( CNcdNodeFactory::NodeTypeL( *node ) 
                         == CNcdNodeFactory::ENcdNodeFolder )
                        {
                        // Only handle the folders here.
                        // Else, the child can not be bundle or transparent.
                        folder = static_cast<CNcdNodeFolder*>( node );
                        // Remove the node from the cleanup stack but do not delete
                        // it here. It will be used as a folder next.            
                        CleanupStack::Pop( node );
                        }
                    else
                        {
                        // Delete the node because it was not of the right type.
                        DLERROR(("The root child was not a folder. Has the specification been changed?"));
                        DASSERT( EFalse );
                        CleanupStack::PopAndDestroy( node );
                        }
                    }
                        
                if ( folder != NULL )
                    {
                    // Notice that the node should be closed not deleted!
                    CleanupClosePushL( *folder );
                    CNcdNodeFactory::TNcdNodePurpose nodePurpose(
                        CNcdNodeFactory::NodePurposeL( *folder ) );
                        
                    if ( nodePurpose == CNcdNodeFactory::ENcdTransparentNode )
                        {
                        DLINFO(("Transparent folder."));
                        const RPointerArray<CNcdChildEntity>& children = folder->ChildArray();
                        for ( TInt i = 0; i < children.Count(); i++ )
                            {                            
                            // Because the folder is transparent folder,
                            // remove the child identifiers from the node list. 
                            // So, those transparent children will not be
                            // removed from the database later.

                            // Notice, that if the children of the transparent folder
                            // are not in the db list, then they will not be removed
                            // from the db.
                            RemoveIdentifier( children[i]->Identifier(), 
                                              iAllDbStorageNodeItems );
                            }
                        }
                    else if ( nodePurpose == CNcdNodeFactory::ENcdBundleNode )
                        {
                        DLINFO(("Bundle folder"))
                        // Next, we have to handle the bundle children and check
                        // if they are transparent
                        iBundleChildren.ResetAndDestroy();
                        const RPointerArray<CNcdChildEntity>& children = folder->ChildArray();
                        for ( TInt i = 0; i < children.Count(); i++ )
                            {
                            if ( RemoveIdentifier( children[i]->Identifier(),
                                                   iAllDbStorageNodeItems ) )
                                {
                                // The identifier was found from the db list and removed.
                                // So, because the node can be found from the db, it can be
                                // created. So, add it into the bundle children temporary list.
                                AddIdentifierL( children[i]->Identifier(), iBundleChildren );
                                }
                            }
                            
                        // Now that we got the children of the bundle folder,
                        // we should handle them separately during next active rounds.
                        iCleanupState = EHandleBundleChildren;                        
                        }
                    CleanupStack::PopAndDestroy( folder );                
                    }
                }

            // Delete node data.
            CleanupStack::PopAndDestroy( nodeData );
            }
            
        // Now, the root child has been checked. So, remove its identifier 
        // from the root array.
        delete iRootChildren[ 0 ];
        iRootChildren.Remove( 0 );
        }
    else
        {
        // No more children to be checked.
        // So, skip to expireds.
        iCleanupState = ECleaningExpireds;
        }

    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* ptrStatus = &iStatus;
    User::RequestComplete( ptrStatus, KErrNone );        
    
    DLTRACEOUT(("")); 
    }


void CNcdNodeCacheCleaner::HandleBundleChildrenL()
    {
    DLTRACEIN((""));
    
    // Check all the bundle children for transparent folders.
    
    if ( iBundleChildren.Count() > 0 )
        {
        DLINFO(("Bundle children in the array."));

        // As a default loop here after this stuff is done.
        // This way next loop will check if there are still more children
        // to be checked.
        iCleanupState = EHandleBundleChildren;                        

        // All the children of the root should always be folders.
        CNcdNode* node( NULL );    
        CNcdNodeFolder* folder( NULL );
        HBufC8* nodeData( NULL );
        CNcdNodeIdentifier* bundleChildIdentifier( iBundleChildren[ 0 ] );

        // The read operation may leave for example with KErrNotFound if the
        // item was not found. Ignore the error and just continue normally.
        TRAP_IGNORE( 
            nodeData =
                NodeDbManager().
                    ReadDataFromDatabaseL( *bundleChildIdentifier,
                                           NcdNodeClassIds::ENcdNode ) );
        if ( nodeData != NULL )
            {
            CleanupStack::PushL( nodeData );
            if ( *nodeData != KNullDesC8 )
                {
                TRAP_IGNORE( 
                    node =
                        NodeFactory().CreateNodeL( *bundleChildIdentifier, 
                                                   *nodeData ) );
                if ( node != NULL )
                    {
                    // Push the node into the cleanup stack because checking
                    // may leave.
                    CleanupClosePushL( *node );
                    if ( CNcdNodeFactory::NodeTypeL( *node ) 
                         == CNcdNodeFactory::ENcdNodeFolder )
                        {
                        // Only handle the folders here.
                        // Else, the child can not be transparent.
                        folder = static_cast<CNcdNodeFolder*>( node );
                        }
                    // Remove the node from the cleanup stack but do not delete
                    // it here. It will be used as a folder next.            
                    CleanupStack::Pop( node );
                    }
                        
                if ( folder != NULL )
                    {
                    // Notice that the node should be closed not deleted!
                    CleanupClosePushL( *folder );
                    CNcdNodeFactory::TNcdNodePurpose nodePurpose(
                        CNcdNodeFactory::NodePurposeL( *folder ) );
                        
                    if ( nodePurpose == CNcdNodeFactory::ENcdTransparentNode )
                        {
                        DLINFO(("Transparent folder."));
                        const RPointerArray<CNcdChildEntity>& children = folder->ChildArray();
                        for ( TInt i = 0; i < children.Count(); ++i )
                            {
                            // Because the folder is transparent folder,
                            // remove the child identifiers from the node list. 
                            // So, those transparent children will not be
                            // removed from the database.

                            // Because the root child will be added into the root list
                            // and those children are handled separately, remove
                            // the children also from the original node list.
                            // Notice, that if the children are not in the db list, then
                            // there will not be any need to remove them from the db either.
                            // So, check it here.
                            RemoveIdentifier( children[i]->Identifier(), 
                                              iAllDbStorageNodeItems );
                            }
                        }
                    CleanupStack::PopAndDestroy( folder );                
                    }
                }

            // Delete node data.
            CleanupStack::PopAndDestroy( nodeData );
            }
            
        // Now the root child has been checked. So, remove its identifier 
        // from the root array.
        delete iBundleChildren[ 0 ];
        iBundleChildren.Remove( 0 );
        }
    else
        {
        // No more bundle children to be checked.
        // So, skip back to root child checking.
        iCleanupState = ECheckRootChildren;
        }

    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* ptrStatus = &iStatus;
    User::RequestComplete( ptrStatus, KErrNone );        
        
    DLTRACEOUT((""));
    }


void CNcdNodeCacheCleaner::CleanExpiredsL()
    {
    DLTRACEIN((""));
    
    // Notice that this function is looped if there are multiple nodes in the
    // iAllDbStorageNodeItems. The array may not be uptodate if some nodes are
    // inserted into the database during this active object looping. This does
    // not matter because then only those new items will not be checked here.
    // They will be checked when the cleaner is started next time. The array
    // could be updated here every time but because db-actions may take time
    // this is omitted here.
    
    // Notice that because the db items are sorted when the array is created,
    // the removing of the children from the end list does not affect this index.
    // So, if the current item is removed then also its children are removed, but
    // the children exist after the parent in the array.
    if ( iExpiredCleaningIndex < iAllDbStorageNodeItems.Count() )
        {
        CNcdNodeIdentifier* itemIdentifier( 
            iAllDbStorageNodeItems[ iExpiredCleaningIndex ] );

        DLINFO((_L("Cleaning index: %d, id: %S"), 
                iExpiredCleaningIndex, &itemIdentifier->NodeId()));

        // Set the expire to be true by default. So, if node can not be initialized
        // correctly it will be removed also in that case.
        TBool expired( ETrue );
        TInt depth( NcdNodeIdentifierEditor::NodeDepthL( *itemIdentifier ) );

        if ( depth < 2 )
            {
            DLINFO(("No need to check expiration. Depth: %d", depth));
            // Root nodes are not removed here. They will be updated by the
            // other methods when they expire.
            // Only items that are not root or direct children of root should
            // be removed here. Notice that temporary items have depth zero, but
            // their are cleaned during temporary cleaning. So, omitting them here
            // is ok.
            // Notice also, that items directly under the root should not be
            // removed by the cleaner. For example, bundle folders should be 
            // updated only same time as root, because their info is only gotten 
            // during the root refresh.
            expired = EFalse;
            }
        else
            {
            // Node will not be some root so, check the expiration here.
            HBufC8* nodeData( NULL );
            
            // The read operation may leave for example with KErrNotFound if the
            // item was not found. Ignore the error and just continue normally.
            TRAP_IGNORE( 
                nodeData =
                    NodeDbManager().
                        ReadDataFromDatabaseL( *itemIdentifier,
                                               NcdNodeClassIds::ENcdNode ) );
            if ( nodeData != NULL )
                {                
                if ( *nodeData != KNullDesC8 )
                    {
                    CNcdNode* node( NULL );
                    TRAP_IGNORE( 
                        node = 
                            NodeFactory().CreateNodeL( *itemIdentifier, 
                                                       *nodeData ) );
                    if ( node != NULL )
                        {
                        // Notice that the node should be closed not deleted!
                        CNcdNodeLink* link = node->NodeLink();
                        if ( link )
                            {
                            // Check if the node should be marked not expired.
                            expired = link->IsExpired();
                            }
                        node->Close();              
                        }
                    }

                // Delete node data.
                delete nodeData;
                }        
            }
            
        if ( expired )
            {
            DLINFO((_L("Expired: %S"), &itemIdentifier->NodeId()));
            // Because the node was expired, or it could not be created for some reason
            // correctly. Add the identifier to the cleanup list if it is allowed action.
            // This function knows how to move the item into the waiting list.
            AddCleanupIdentifierL( *itemIdentifier );

            // Remove the data from its original place. Notice that identifier
            // is copied in the function above.
            delete itemIdentifier;
            iAllDbStorageNodeItems.Remove( iExpiredCleaningIndex );
            
            // HandleCleaningL and the functions it uses can handle the removing of the
            // item from the db array.
            HandleCleaningL( EFalse, EFalse );
            }
        else
            {
            // Increase the cleaning index by one for the next round
            // because the current item was not removed.
            ++iExpiredCleaningIndex;            
            }

        // There may still be something to be done.
        // So, make new request for the next step.
        iCleanupState = ECleaningExpireds;
        }
    else
        {
        DLINFO(("Expireds cleaned"));
        // Reset the cleaning index.
        iExpiredCleaningIndex = 0;

        // Because expireds are cleaned now. Check if the excess nodes should
        // still be removed from the database to free enough space.
        iCleanupState = ECleaningExcess;            
        }

    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* ptrStatus = &iStatus;
    User::RequestComplete( ptrStatus, KErrNone );        
        
    DLTRACEOUT((""));
    }

                 
void CNcdNodeCacheCleaner::CleanExcessL()
    {
    DLTRACEIN((""));

    TInt currentDbSize(
            NodeDbManager().StorageSizeL( ClientUid(),
                                          *iDoNotCleanNameSpaces ) );
                                          
    // Because cleaning has been started the compare size should be somewhat
    // less than the max accepted size after the cleaning operation.
    TInt compareSize( DbMaxSize() / KMaxSizeDivider );        

    DLINFO(("current db size: %d, compare size: %d", 
            currentDbSize, compareSize));

    // Here the node list may not be uptodate if database has changed during the
    // active loops. But do not update it, because otherwise we might try to remove
    // same node over and over again. If the deepest nodes are not allowed to be
    // removed.
    TInt itemCount( iAllDbStorageNodeItems.Count() );
    TInt deepestDepth( 0 );

    if ( itemCount > 0  )
        {
        // Get the depth of the final node in db list. This is the deepest node because
        // the array has been sorted.
        deepestDepth =
            NcdNodeIdentifierEditor::NodeDepthL( *iAllDbStorageNodeItems[ itemCount - 1 ] );
        if ( deepestDepth < 2 )
            {
            DLINFO(("Do not clean excess any more. Depth: %d", deepestDepth));
            // Because we should not delete root or its direct children here.
            // Reset the node items array and reset the item count. 
            // Because the item count will be zero now,
            // the cleaning will continue to the stopping state next.
            iAllDbStorageNodeItems.ResetAndDestroy();
            itemCount = iAllDbStorageNodeItems.Count();
            }
        }

    // Start actual cleaning if it is still required.
    if ( itemCount > 0 
         && compareSize < currentDbSize )
        {
        DLINFO(("Excess cleaning is required"));
        
        DLINFO(("Deepest depth: %d", deepestDepth));            

        CNcdNodeIdentifier* identifier( NULL );
        for ( TInt i = itemCount; i > 0; --i )
            {
            identifier = iAllDbStorageNodeItems[ i - 1 ];
            if ( deepestDepth != NcdNodeIdentifierEditor::NodeDepthL( *identifier ) )
                {
                // Only remove items that have the same depth at once.
                DLINFO(("All the same depth nodes added."));
                break;
                }
                
            // This identifier has also the same depth.
            // Insert it into the cleanup array if removal is allowed.
            AddCleanupIdentifierL( *iAllDbStorageNodeItems[ i - 1 ] );

            // Remove the data from its original place. Notice that identifier
            // is copied in the function above.
            delete iAllDbStorageNodeItems[ i - 1 ];
            iAllDbStorageNodeItems.Remove( i - 1 );
            
            DLINFO(("Same depth id added to array"));
            }

        // Clean the nodes from the database.
        HandleCleaningL( EFalse, EFalse );        

        // We may still have to cleanup some more.
        // So, make new request for the next step.
        // This will clean the metadata related data such as icons.
        // The cleaning will loop until enough space is released.
        iCleanupState = ECleaningTemporaryNodes;        
        }
    else
        {
        DLINFO(("Excess cleaned"));
        // Just in case somebody has added something for removal
        // while waiting this new round.
        // Because we are about to end the cleaning we can update the
        // node list one more time before final removing.
        HandleCleaningL( ETrue, ETrue );
        
        // Nothing to do next
        iCleanupState = EStopping;
        }

    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* ptrStatus = &iStatus;
    User::RequestComplete( ptrStatus, KErrNone );        
        
    DLTRACEOUT((""));
    }


void CNcdNodeCacheCleaner::StoppingL()
    {
    DLTRACEIN((""));
    
    // This function resets the member variables to default values.
    // Also, the priority of this active object is set to the default.
    ResetState();
    
    DLTRACEOUT((""));
    }


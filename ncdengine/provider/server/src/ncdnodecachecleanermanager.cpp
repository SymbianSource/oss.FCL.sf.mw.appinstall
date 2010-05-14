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
* Description:   Implements CNcdNodeCacheCleanerManager class
*
*/


#include "ncdnodecachecleanermanager.h"
#include "ncdnodecachecleaner.h"
#include "ncdnodemanager.h"
#include "ncdnodedbmanager.h"
#include "ncdnodefactory.h"
#include "catalogsconstants.h"
#include "ncdproviderdefines.h"
#include "catalogsutils.h"
#include "ncdgeneralmanager.h"

#include "catalogsdebug.h"


CNcdNodeCacheCleanerManager* 
CNcdNodeCacheCleanerManager::NewL( CNcdGeneralManager& aGeneralManager,
                                   CNcdNodeDbManager& aNodeDbManager,
                                   TInt aDbDefaultMaxSize,                                                  
                                   CNcdNodeFactory& aNodeFactory )
    {
    CNcdNodeCacheCleanerManager* self =   
        CNcdNodeCacheCleanerManager::NewLC( aGeneralManager, 
                                            aNodeDbManager, 
                                            aDbDefaultMaxSize,
                                            aNodeFactory );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeCacheCleanerManager* 
CNcdNodeCacheCleanerManager::NewLC( CNcdGeneralManager& aGeneralManager,
                                    CNcdNodeDbManager& aNodeDbManager,
                                    TInt aDbDefaultMaxSize,
                                    CNcdNodeFactory& aNodeFactory )
    {
    CNcdNodeCacheCleanerManager* self = 
        new( ELeave ) CNcdNodeCacheCleanerManager( aGeneralManager, 
                                                   aNodeDbManager,
                                                   aDbDefaultMaxSize, 
                                                   aNodeFactory );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;        
    }



CNcdNodeCacheCleanerManager::CNcdNodeCacheCleanerManager( CNcdGeneralManager& aGeneralManager,
                                                          CNcdNodeDbManager& aNodeDbManager,
                                                          TInt aDbDefaultMaxSize,
                                                          CNcdNodeFactory& aNodeFactory )
: CBase(),
  iGeneralManager( aGeneralManager ),
  iNodeManager( aGeneralManager.NodeManager() ),
  iNodeDbManager( aNodeDbManager ),
  iDbMaxSize( aDbDefaultMaxSize ),
  iNodeFactory( aNodeFactory )
    {
    }


void CNcdNodeCacheCleanerManager::ConstructL()
    {
    // These values have to be set.   
    }


CNcdNodeCacheCleanerManager::~CNcdNodeCacheCleanerManager()
    {
    DLTRACEIN((""));

    // Before deleting cache cleaners make sure that db cleaner
    // has finished its job.
    // Reload the node list just in case. Also, reset the list
    // when everything is done.
    DLINFO(("Cache cleaner force clean"));
    for ( TInt i = 0; i < iCleaners.Count(); ++i )
        {
        // Be sure to trap here, because destructor can not leave.
        TRAP_IGNORE( iCleaners[ i ]->ForceCleanupL() );       
        }

    // This manager owns the cleaners.
    // So, delete them.
    iCleaners.ResetAndDestroy();
        
    DLTRACEOUT((""));
    }        


CNcdNodeCacheCleaner& CNcdNodeCacheCleanerManager::CacheCleanerL( const TUid& aClientUid )
    {
    DLTRACEIN((""));
    
    for ( TInt i = 0; i < iCleaners.Count(); ++i )
        {
        if ( aClientUid == iCleaners[ i ]->ClientUid() )
            {
            DLTRACEOUT(("Cleaner was in the array"));
            return *iCleaners[ i ];
            }
        }

    DLINFO(("Cleaner was not in the array. So, create new."));
    CNcdNodeCacheCleaner* cleaner( 
        CNcdNodeCacheCleaner::NewLC( iGeneralManager, NodeDbManager(), 
                                     DbMaxSize(), NodeFactory() ) );
    // Array takes the ownership.
    iCleaners.AppendL( cleaner );
    CleanupStack::Pop( cleaner );
       
    DLTRACEOUT((""));   
     
    return *cleaner;
    }


TInt CNcdNodeCacheCleanerManager::DbMaxSize() const
    {
    DLTRACEIN((""));
    return iDbMaxSize;
    }
 
 
void CNcdNodeCacheCleanerManager::SetDbMaxSize( const TInt aDbMaxSize )
    {
    DLTRACEIN((""));
    iDbMaxSize = aDbMaxSize;
    }


void CNcdNodeCacheCleanerManager::CheckAllL() const
    {
    DLTRACEIN((""));
    
    for ( TInt i = 0; i < iCleaners.Count(); ++i )
        {
        iCleaners[ i ]->CheckDbSizeL();
        }
    
    DLTRACEOUT((""));
    }


CNcdNodeManager& CNcdNodeCacheCleanerManager::NodeManager() const
    {
    return iNodeManager;
    }


CNcdNodeDbManager& CNcdNodeCacheCleanerManager::NodeDbManager() const
    {
    return iNodeDbManager;
    }


CNcdNodeFactory& CNcdNodeCacheCleanerManager::NodeFactory() const
    {
    return iNodeFactory;
    }



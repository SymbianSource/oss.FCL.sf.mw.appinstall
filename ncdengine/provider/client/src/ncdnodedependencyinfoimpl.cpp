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
* Description:   CNcdNodeDependencyInfo
*
*/


#include <s32strm.h>

#include "ncdnodedependencyinfoimpl.h"
#include "ncddependencyinfo.h"
#include "ncdnodeidentifier.h"
#include "ncdnodeproxy.h"
#include "ncdnodemanagerproxy.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"


CNcdNodeDependencyInfo* CNcdNodeDependencyInfo::NewL( const CNcdDependencyInfo& aInfo,
                                                      CNcdNodeManagerProxy& aNodeManager )
    {
    CNcdNodeDependencyInfo* self = 
        CNcdNodeDependencyInfo::NewLC( aInfo, aNodeManager );
    CleanupStack::Pop( self );
    return self;            
    }
    
CNcdNodeDependencyInfo* CNcdNodeDependencyInfo::NewLC( const CNcdDependencyInfo& aInfo,
                                                       CNcdNodeManagerProxy& aNodeManager )
    {
    CNcdNodeDependencyInfo* self = 
        new( ELeave ) CNcdNodeDependencyInfo( aNodeManager );
    CleanupStack::PushL( self );
    self->ConstructL( aInfo );
    return self;            
    }


void CNcdNodeDependencyInfo::ConstructL( const CNcdDependencyInfo& aInfo  )
    {
    iUid.iUid = aInfo.Uid().iUid;

    const CNcdNodeIdentifier* tmpIdentifier( aInfo.Identifier() );
    if ( tmpIdentifier != NULL )
        {
        iIdentifier = CNcdNodeIdentifier::NewL( *tmpIdentifier );        
        }

    iName = aInfo.Name().AllocL();
    iVersion = aInfo.Version().AllocL();
    
    iDependencyState = aInfo.DependencyState();
    }


CNcdNodeDependencyInfo::CNcdNodeDependencyInfo( CNcdNodeManagerProxy& aNodeManager ): 
    CBase(),
    iNodeManager( aNodeManager )
    {
    
    }
    
CNcdNodeDependencyInfo::~CNcdNodeDependencyInfo()
    {
    
    delete iName;
    delete iVersion;
    delete iIdentifier;
    }


const TDesC& CNcdNodeDependencyInfo::Name() const
    {
    return *iName;
    }    

const TDesC& CNcdNodeDependencyInfo::Version() const
    {
    return *iVersion;
    }
    
TUid CNcdNodeDependencyInfo::Uid() const
    {
    return iUid;
    }

MNcdNode* CNcdNodeDependencyInfo::DependencyNodeL() const
    {
    DLTRACEIN((""));
    
    // Notice, that this class contains the information about
    // the metadata identifier that is used to create the node.
    // If the identifier is NULL, then the dependency was not
    // given as a node but as a content.
    if ( Identifier() == NULL )
        {
        return NULL;
        }

    // The node might have not been created yet, so create it if necessary.
    CNcdNodeProxy* node( 
        &NodeManager().
            CreateTemporaryOrSupplierNodeL(
                *Identifier() ) );
        
    // Increase the ref count by one here. 
    node->AddRef();

    return node;
    }


TNcdDependencyState CNcdNodeDependencyInfo::State() const
    {
    DLTRACEIN((""));
    return iDependencyState;
    }
    

const CNcdNodeIdentifier* CNcdNodeDependencyInfo::Identifier() const
    {
    return iIdentifier;
    }


CNcdNodeManagerProxy& CNcdNodeDependencyInfo::NodeManager() const
    {
    return iNodeManager;
    }

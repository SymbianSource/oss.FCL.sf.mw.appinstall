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
* Description:   Node load operation interface definition
*
*/
	

#ifndef M_NCDLOADNODEOPERATION_H
#define M_NCDLOADNODEOPERATION_H

#include <e32cmn.h>

#include "ncdoperation.h"
#include "catalogsarray.h"
#include "ncdinterfaceids.h"

class MNcdLoadNodeOperationObserver;

/**
 *  Load node operation interface.
 *
 *  Load node operations handle the loading of one or more nodes from a server.
 *  One typical use case is the loading of a catalog's child nodes.
 *  
 *  Node load operations send callbacks when one ore more nodes have been
 *  loaded. This can be used to display the loaded nodes even if the whole
 *  operation has not yet completed.
 *  
 *  
 *  @see MNcdNode
 *  @see MNcdLoadNodeOperationObserver
 */
class MNcdLoadNodeOperation : public MNcdOperation
    {

public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdLoadNodeOperationUid };
    
protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdLoadNodeOperation() {}

    };
	
	
#endif //  M_NCDLOADNODEOPERATION_H

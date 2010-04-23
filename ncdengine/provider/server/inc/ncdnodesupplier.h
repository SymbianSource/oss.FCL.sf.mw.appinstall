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
* Description:   Contains CNcdNodeSupplier class
*
*/


#ifndef NCDNODESUPPLIER_H
#define NCDNODESUPPLIER_H

#include "ncdnodeimpl.h"

class CNcdNodeSupplier : public CNcdNode 
{
public:
    static CNcdNodeSupplier* NewL(
        CNcdNodeManager& aNodeManager,
        const CNcdNodeIdentifier& aIdentifier );

    static CNcdNodeSupplier* NewLC(
        CNcdNodeManager& aNodeManager,
        const CNcdNodeIdentifier& aIdentifier );
    
protected: // from CNcdNode

    /**
     * Constructor.
     */
    CNcdNodeSupplier( CNcdNodeManager& aNodeManager );

    /**
     * Creates the node link if the link is not yet created.
     *
     * @return The created link or the old link if it already existed.     
     */
    virtual CNcdNodeLink* CreateLinkL();
};

#endif
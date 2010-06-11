/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains CNcdNodeSeenFolderProxy
*
*/


#ifndef C_NCDNODESEENFOLDERPROXY_H
#define C_NCDNODESEENFOLDERPROXY_H

#include "ncdinterfacebaseproxy.h"
#include "ncdnodeseenfolder.h"

class CNcdNodeFolderProxy;

/**
 * Implementation of MNcdNodeSeenFolder interface.
 *
 * @see MNcdNodeSeenFolder
 * @since S60 v3.2
 */
class CNcdNodeSeenFolderProxy : public CNcdInterfaceBaseProxy,
                                public MNcdNodeSeenFolder
    {
public:
    static CNcdNodeSeenFolderProxy* NewL(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodeFolderProxy& aNode );

    static CNcdNodeSeenFolderProxy* NewLC(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodeFolderProxy& aNode );
                                        
    ~CNcdNodeSeenFolderProxy();
    
    void InternalizeL();
    
public: // From MNcdNodeSeenFolder

    /**
     * @see MNcdNodeSeenFolder
     */
    virtual void SetContentsSeenL();
        
    /**
     * @see MNcdNodeSeenFolder
     */
    virtual TInt NewCountL( TInt aLevels ) const;
    
    /**
     * @see MNcdNodeSeenFolder
     */
    virtual RCatalogsArray<MNcdNode> NewNodesL( TInt aLevels ) const;
    
protected:
    CNcdNodeSeenFolderProxy(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodeFolderProxy& aNode );

    void ConstructL();          
    
private:
    // The parent folder.
    CNcdNodeFolderProxy& iFolder;    
    };
        
#endif // C_NCDNODESEENFOLDERPROXY_H
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
* Description:   ?description
*
*/


#ifndef C_NCDNODESEENPROXY_H
#define C_NCDNODESEENPROXY_H


#include "ncdinterfacebaseproxy.h"
#include "ncdnodeseen.h"

class CNcdNodeProxy;

/**
 *  Implementation of MNcdNodeSeen-interface
 *
 *  @see MNcdNodeSeen
 *  Implementation of MNcdNodeSeen-interface which acts also as
 *  a proxy.
 *
 *  @since S60 3.2
 */
class CNcdNodeSeenProxy : public CNcdInterfaceBaseProxy,
                          public MNcdNodeSeen
    {
public:
    static CNcdNodeSeenProxy* NewL(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodeProxy& aNode );
        
    static CNcdNodeSeenProxy* NewLC(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodeProxy& aNode );
        
    ~CNcdNodeSeenProxy();
        
    void InternalizeL();
       
    
public: // from MNcdNodeSeen

    /**
     * @see MNcdNodeSeen
     */
    virtual TBool IsSeen() const;
    
    /**
     * @see MNcdNodeSeen
     */
    void SetSeenL();

protected:

    CNcdNodeSeenProxy(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodeProxy& aNode);
        
    void ConstructL();
    
private:
    TBool iSeen;
    };

#endif // C_NCDNODESEENPROXY_H

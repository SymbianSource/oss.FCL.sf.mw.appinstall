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
* Description:   Contains CNcdParentOfTransparentNodeProxy class
*
*/


#ifndef NCD_PARENT_OF_TRANSPARENT_NODE_PROXY_H
#define NCD_PARENT_OF_TRANSPARENT_NODE_PROXY_H


#include "ncdnodefolderproxy.h"


/**
 *  Parent of transparent classes.
 *  Because of children of this class object may be transparent, it means
 *  that the child count of the server side object may not match the child count
 *  of the proxy when transparent children are replaced by their children.
 *  This class object provides the actual server side child count. That may be
 *  used for special cases.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdParentOfTransparentNodeProxy: public CNcdNodeFolderProxy 
    {

public:

    /**
     * Destructor
     *
     * @note Usually Release function should be used instead of
     * directly calling delete for this class object.
     *
     */
    virtual ~CNcdParentOfTransparentNodeProxy();


    /**
     * @return TInt The child count that the corresponding server side object has.
     */
    TInt ServerChildCount() const;


protected: // CNcdNodeFolderProxy

    /**
     * @see CNcdNodeFolderProxy::InternalizeNodeDataL
     */
    virtual void InternalizeNodeDataL( RReadStream& aStream );


protected:

    /**
     * @see CNcdNodeFolderProxy::ConstructL
     */
    CNcdParentOfTransparentNodeProxy( MCatalogsClientServer& aSession,
                                      TInt aHandle,
                                      CNcdNodeManagerProxy& aNodeManager,
                                      CNcdOperationManagerProxy& aOperationManager,
                                      CNcdFavoriteManagerProxy& aFavoriteManager );

    /**
     * ConstructL
     */
    virtual void ConstructL();
    
    /**
     * Checks from server side that is any of the transparent children expired.
     * (Needs to be checked from server side because the transparent child folders do
     * not exist on proxy side)
     *
     * @return ETrue if one or more of the transp. children is expired.
     */
    TBool IsTransparentChildExpiredL() const;


private:
    // Prevent if not implemented
    CNcdParentOfTransparentNodeProxy( const CNcdParentOfTransparentNodeProxy& aObject );
    CNcdParentOfTransparentNodeProxy& operator =( const CNcdParentOfTransparentNodeProxy& aObject );


private: // data

    TInt iServerChildCount;

    };


#endif // NCD_PARENT_OF_TRANSPARENT_NODE_PROXY_H

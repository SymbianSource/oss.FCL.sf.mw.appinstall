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
* Description:  
*
*/

	
#ifndef C_NCDEXPIREDNODE_H
#define C_NCDEXPIREDNODE_H

#include <e32cmn.h>

#include "ncdsendable.h"
#include "ncdnodeidentifier.h"

/**
 * Data structure for expired node info.
 */
class CNcdExpiredNode : public CBase, public MNcdSendable
    {        

public:


    static CNcdExpiredNode* NewL( RReadStream& aReadStream );
    
    static CNcdExpiredNode* NewLC( RReadStream& aReadStream );
    
    static CNcdExpiredNode* NewL( const CNcdNodeIdentifier& aNodeIdentifier,
        TBool aForceUpdate );
        
    static CNcdExpiredNode* NewLC( const CNcdNodeIdentifier& aNodeIdentifier,
        TBool aForceUpdate );
        
    static CNcdExpiredNode* NewL( const CNcdExpiredNode& aExpiredNode );
    
    static CNcdExpiredNode* NewLC( const CNcdExpiredNode& aExpiredNode );

    /**
     * Destructor.
     */
    virtual ~CNcdExpiredNode();
    
    /**     
     * Internalizes member data from a read stream.
     *
     * @param aStream A read stream.
     */
    virtual void InternalizeL( RReadStream& aStream );
    
    /**
     * Externalizes member data to a write stream.
     *
     * @param aStream A write stream.
     */	    
    virtual void ExternalizeL( RWriteStream& aStream ) const;

    const CNcdNodeIdentifier& NodeIdentifier() const;
    
    TBool ForceUpdate() const;

private:
    
    CNcdExpiredNode( TBool aForceUpdate );
    void ConstructL( const CNcdNodeIdentifier& aNodeIdentifier );

private:

    CNcdNodeIdentifier* iNodeIdentifier;
    TBool iForceUpdate;
    
    };
	
#endif //  C_NCDEXPIREDNODE_H

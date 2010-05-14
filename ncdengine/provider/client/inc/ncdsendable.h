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
* Description:   ?Description
*
*/
	

#ifndef M_NCDSENDABLE_H
#define M_NCDSENDABLE_H

class RReadStream;
class RWriteStream;

/**
 *  Interface for sendable objects.
 * 
 *  Used for objects that are sent over the client/server boundary.
 */
class MNcdSendable
    {        

public:

    /**     
     * Internalizes member data from a read stream.
     *
     * @param aStream A read stream.
     */
    virtual void InternalizeL( RReadStream& aStream ) = 0;
    
    /**
     * Externalizes member data to a write stream.
     *
     * @param aStream A write stream.
     */	    
    virtual void ExternalizeL( RWriteStream& aStream ) const = 0;
    
    };
	
#endif //  M_NCDSENDABLE_H

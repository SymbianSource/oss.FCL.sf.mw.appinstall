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
	

#ifndef T_NCDSENDABLEPROGRESS_H
#define T_NCDSENDABLEPROGRESS_H

#include <e32base.h>

#include "ncdsendable.h"
#include "ncdprogress.h"

/**
* Operation progress information
*/
class TNcdSendableProgress	: //public TNcdProgress,
                              public MNcdSendable
    {
    public:
        
        TNcdSendableProgress();
        
        TNcdSendableProgress( TUint aState, TInt32 aOperationId,
            TInt32 aProgress, 
            TInt32 aMaxProgress );
            
// from base class MNcdSendable

	    /**
	     * From MNcdSendable
         * Internalizes member data from a read stream.
         *
         * @param aStream A read stream.
         */
	    void InternalizeL( RReadStream& aStream );
	    
	    /**
	     * From MNcdSendable
         * Externalizes member data to a write stream.
         *
         * @param aStream A write stream.
         */	    
	    void ExternalizeL( RWriteStream& aStream ) const ;
	    
	    /**
	     * From MNcdSendable
         * Size of the object in bytes.
         * @return Size in bytes.
         */
        TInt Size() const;
	
    public:
        TUint iState;         // State of the operation
        TInt32 iOperationId;    // Unique id for the operation
        TInt32 iProgress;     // Progress of the current state
        TInt32 iMaxProgress;  // Maximum progress value for the current state
    };

#endif //  T_NCDSENDABLEPROGRESS_H

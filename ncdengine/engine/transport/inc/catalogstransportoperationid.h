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


#ifndef T_CATALOGSTRANSPORTOPERATIONID_H
#define T_CATALOGSTRANSPORTOPERATIONID_H

#include <e32base.h>

class RReadStream;
class RWriteStream;

class TCatalogsTransportOperationId
    {
    public: // Constructors

        /**
         * Zero constructor
         */
        TCatalogsTransportOperationId() 
            : iSession( 0 ), 
              iId( 0 ), 
              iSecondaryId( KErrNotFound )
            {
            }    
        
        /**
         * Constructor
         *
         * @param aSession Transport session ID
         * @param aId Transport operation ID
         */
        TCatalogsTransportOperationId( 
            TInt32 aSession, 
            TInt32 aId ) : 
            iSession( aSession ), 
            iId( aId ),
            iSecondaryId( KErrNotFound )
            {
            }
        
        
    public:
    
        /**
         * Equality-operator
         * 
         * @note Secondary ID is ignored
         */
        bool operator==( const 
            TCatalogsTransportOperationId& aOther ) const;
        
        /**
         * Compares this id with an other id
         * @param aSecond The other id
         * @return A negative number if the second id is greater than this,
         * 0 if the ids are equal and a positive number if the first id is
         * greater than the second 
         * 
         * @note Secondary ID is ignored
         */
        TInt Compare( const TCatalogsTransportOperationId& aSecond ) const;
        
        
        /**
         * Operation session ID getter
         *
         * @return Operation session
         */
        TInt32 SessionId() const;
        
        
        /**
         * Operation ID number getter.
         *
         * This contains only the actual id of the operation without
         * any session etc. information
         * @return Operation ID
         */
        TInt32 Id() const;
        
        
        /**
         * Secondary ID getter
         * 
         * @return Secondary ID
         */
        TInt32 SecondaryId() const;
        
        
        void SetSecondaryId( TInt32 aId );
        
        void ExternalizeL( RWriteStream& aStream ) const;
        void InternalizeL( RReadStream& aStream );

    private:
    
        TInt32 iSession;
        TInt32 iId;
        TInt32 iSecondaryId;
    };


// Implementations for inline methods
#include "catalogstransportoperationid.inl"

#endif //  T_CATALOGSTRANSPORTOPERATIONID_H

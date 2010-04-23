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


#ifndef C_CATALOGSHTTPHEADERS_H
#define C_CATALOGSHTTPHEADERS_H

#include "e32base.h"
#include "catalogshttpheaders.h"

class CCatalogsKeyValuePair;

/**
* HTTP header implementation
*/
class CCatalogsHttpHeaders : public CBase, public MCatalogsHttpHeaders
    {
    public:
    
        /**
        * Creator
        */
        static CCatalogsHttpHeaders* NewL();
        
        /**
        * Copy constructor
        */
        static CCatalogsHttpHeaders* NewL( const CCatalogsHttpHeaders& aOther );
        
        /**
         * Creates headers from a stream
         */
        static CCatalogsHttpHeaders* NewL( RReadStream& aStream );
        
        /**
        * Cloner
        */
        CCatalogsHttpHeaders* CloneL() const;
        
        /**
        * Destructor
        */
        ~CCatalogsHttpHeaders();
        
    public:
    
        /**
        * Adds an HTTP header
        * 
        * @param aHeader Header name
        * @param aValue Header data
        */
        void AddHeaderL( const TDesC8& aHeader, 
            const TDesC8& aValue );
        
        
        /** 
        * Removes an HTTP header
        * 
        * @param aHeader Header name
        * @return KErrNotFound if the header was not found
        */
        TInt RemoveHeader( const TDesC8& aHeader );


        /**
        * Adds an HTTP header
        * 
        * @param aHeader Header name
        * @param aValue Header data
        */
        void AddHeaderL( const TDesC16& aHeader, 
            const TDesC16& aValue );
        
        
        /** 
        * Removes an HTTP header
        * 
        * @param aHeader Header name
        * @return KErrNotFound if the header was not found
        */
        TInt RemoveHeader( const TDesC16& aHeader );
           
            
        /**
        * Returns an array of current headers
        *
        * @return Array of headers
        */
        RPointerArray<CCatalogsKeyValuePair>& Headers();
        

        /**
        * Returns an array of current headers
        *
        * @return Array of headers
        */
        const RPointerArray<CCatalogsKeyValuePair>& Headers() const;
        
        
        /**
        * Searches for the header that matches the key
        * 
        * @param aPair Wanted key
        * @return Header
        * @exception KErrNotFound if a matching header was not found
        */        
        const TDesC8& HeaderByKeyL( const TDesC8& aKey ) const;
        

        /**
        * Searches for the header that matches the key
        * 
        * @param aPair Wanted key
        * @return Header
        * @exception KErrNotFound if a matching header was not found
        */        
        const TDesC8& HeaderByKeyL( const TDesC16& aKey ) const;
        
        
        /**
         * @see MCatalogsHttpHeaders::ExternalizeL()
         */
        void ExternalizeL( RWriteStream& aStream ) const;
        
        
        /**
         * @see MCatalogsHttpHeaders::InternalizeL()
         */
        void InternalizeL( RReadStream& aStream );
        
    
    private:

        /**
        * Constructor
        */
        CCatalogsHttpHeaders();

        /**
        * Copy constructor
        *
        * If not implemented this should be prevented
        */
        CCatalogsHttpHeaders( const CCatalogsHttpHeaders& aOther );

        /**
        * Copy operator
        *
        * If not implemented this should be prevented
        */
        CCatalogsHttpHeaders& operator =( const CCatalogsHttpHeaders& aOther );
    
        /**
        * 2nd phase copy constructor
        */
        void ConstructL( const CCatalogsHttpHeaders& aOther );
            
    private:
    
        /**
        * Searches for the header that matches the key in the given 
        * pair
        * 
        * @param aPair Key-value pair that has the wanted key set
        * @return Header
        * @note aPair is popped from the Cleanupstack and deleted
        * @exception KErrNotFound if a matching header was not found
        */
        const TDesC8& FindHeaderL( CCatalogsKeyValuePair& aPair ) const;

    
    private:
    
        RPointerArray<CCatalogsKeyValuePair> iHeaders;
        
    };

#endif // C_CATALOGSHTTPHEADERS_H
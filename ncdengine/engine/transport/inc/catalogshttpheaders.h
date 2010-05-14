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


#ifndef M_CATALOGSHTTPHEADERS_H
#define M_CATALOGSHTTPHEADERS_H

#include <e32base.h>


class CCatalogsKeyValuePair;
class RReadStream;
class RWriteStream;

/**
* Interface for HTTP headers
*/
class MCatalogsHttpHeaders
    {
    public:
    
        /**
        * Adds an HTTP header
        * 
        * @param aHeader Header name
        * @param aValue Header data
        */
        virtual void AddHeaderL( const TDesC8& aHeader, 
            const TDesC8& aValue ) = 0;
        
        
        /** 
        * Removes an HTTP header
        * 
        * @param aHeader Header name
        * @return KErrNotFound if the header was not found
        */
        virtual TInt RemoveHeader( const TDesC8& aHeader ) = 0;


        /**
        * Adds an HTTP header
        * 
        * @param aHeader Header name
        * @param aValue Header data
        */
        virtual void AddHeaderL( const TDesC16& aHeader, 
            const TDesC16& aValue ) = 0;
        
        
        /** 
        * Removes an HTTP header
        * 
        * @param aHeader Header name
        * @return KErrNotFound if the header was not found
        */
        virtual TInt RemoveHeader( const TDesC16& aHeader ) = 0;
           
            
        /**
        * Returns an array of current headers
        *
        * @return Array of headers
        */
        virtual RPointerArray<CCatalogsKeyValuePair>& Headers() = 0;
        

        /**
        * Returns an array of current headers
        *
        * @return Array of headers
        */
        virtual const RPointerArray<CCatalogsKeyValuePair>& 
            Headers() const = 0;
        

        /**
        * Searches for the header that matches the key
        * 
        * @param aPair Wanted key
        * @return Header
        * @exception KErrNotFound if a matching header was not found
        */        
        virtual const TDesC8& HeaderByKeyL( const TDesC8& aKey ) const = 0;
        

        /**
        * Searches for the header that matches the key
        * 
        * @param aPair Wanted key
        * @return Header
        * @exception KErrNotFound if a matching header was not found
        */        
        virtual const TDesC8& HeaderByKeyL( const TDesC16& aKey ) const = 0;
        
        
        /**
         * Externalize
         *
         * @param aStream Target stream
         */
        virtual void ExternalizeL( RWriteStream& aStream ) const = 0;
        
        
        /**
         * Internalize
         *
         * @param aStream Source stream
         */
        virtual void InternalizeL( RReadStream& aStream ) = 0;
    
    };

#endif // M_CATALOGSHTTPHEADERS_H
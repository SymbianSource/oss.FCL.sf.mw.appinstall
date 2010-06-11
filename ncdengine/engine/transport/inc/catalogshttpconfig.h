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


#ifndef M_CATALOGSHTTPCONFIG_H
#define M_CATALOGSHTTPCONFIG_H

#include <e32base.h>
#include "catalogstransporttypes.h"
#include "catalogshttptypes.h"

class MCatalogsHttpObserver;
class MCatalogsHttpHeaders;
class RReadStream;
class RWriteStream;
class TCatalogsConnectionMethod;


/**
* Interface for setting HTTP configurations
*/
class MCatalogsHttpConfig
    {
    public:
    
        /**
        * Set operation priority
        *
        * @param aPriority Operation priority
        */        
        virtual void SetPriority( TCatalogsTransportPriority aPriority ) = 0;    
    
        
        /**
        * Returns the operation priority
        *
        * @return Operation priority
        */
        virtual TCatalogsTransportPriority Priority() const = 0;
        
        
        /**
        * Sets the source/target directory for Transport operations
        *
        * @param aDirectory Directory path
        * @note If set as empty, the directory must be set for each
        * download explicitly
        */
        virtual void SetDirectoryL( const TDesC& aDirectory ) = 0;


        /**
        * Returns the target directory 
        *        
        * @return Directory path
        */
        virtual const TDesC& Directory() const = 0;


        /**
        * Sets the source/target filename for Transport operations
        *
        * @param aFilename Filename without the path
        */
        virtual void SetFilenameL( const TDesC& aFilename ) = 0;


        /**
        * Returns the source/target filename without the path 
        *        
        * @return Filename
        */
        virtual const TDesC& Filename() const = 0;

        
        /**
         * Returns full path of the target file
         *
         * @return Filename
         */                   
        virtual HBufC* FullPathLC() const = 0;    
        
        
        /**
        * Returns the current request headers
        *
        * @return Headers
        */
        virtual MCatalogsHttpHeaders& RequestHeaders() const = 0;
                
        
        // Access point
        
        /**
        * Sets the access point
        *
        * @param aAccessPointId Access point ID
        */
        virtual void SetConnectionMethod( 
            const TCatalogsConnectionMethod& aMethod ) = 0;
        
        
        /**
        * Access point getter
        *
        * @return Default access point ID
        */
        virtual const TCatalogsConnectionMethod& ConnectionMethod() const = 0;

              
        /**
        * Sets the observer for the operation
        *
        * @param aObserver Operation observer
        */
        virtual void SetObserver( MCatalogsHttpObserver* aObserver ) = 0;
                                
                                
        /**
        * Observer getter
        *
        * @return Observer
        */
        virtual MCatalogsHttpObserver* Observer() const = 0;
        
        
        /**
        * Sets the HTTP method
        * @param aMethod HTTP method
        */
        virtual void SetHttpMethod( TCatalogsHttpMethod aMethod ) = 0;
        
        
        /**
        * Gets the HTTP method
        * @return HTTP method
        */
        virtual TCatalogsHttpMethod HttpMethod() const = 0;
        
        
        /**
         * Externalizes config to a stream
         *
         * @param aStream Target stream
         */
        virtual void ExternalizeL( RWriteStream& aStream ) const = 0;        
        
        
        /**
         * Internalizes config from a stream
         *
         * @param aStream Source stream
         */
        virtual void InternalizeL( RReadStream& aStream ) = 0;  
        
        /**
         * Options getter
         */
        virtual TUint32 Options() const = 0;
        
        /**
         * Options setter
         */
        virtual void SetOptions( TUint32 aOptions ) = 0;

        
    protected:
    
        /** 
        * Hiding the destructor from the user prevents deleting the 
        * implementing object through this interface
        */
        virtual ~MCatalogsHttpConfig() 
            {
            }
    };


#endif //  M_CATALOGSHTTPCONFIG_H

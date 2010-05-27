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


#ifndef C_CATALOGSHTTPCONFIG_H
#define C_CATALOGSHTTPCONFIG_H

#include <e32base.h>

#include "catalogshttpconfig.h"
#include "catalogsconnectionmethod.h"

class MCatalogsHttpHeaders;
class CCatalogsHttpHeaders;
class MCatalogsHttpConfigObserver;

/**
* HTTP configuration implementation
*/
class CCatalogsHttpConfig : public CBase, public MCatalogsHttpConfig
    {
    public: // Constructors and destructor
        
        /**
         * Creator
         */
        static CCatalogsHttpConfig* NewL( 
            MCatalogsHttpConfigObserver* aObserver = NULL );
        

        /**
         * Creator
         */
        static CCatalogsHttpConfig* NewL( 
            RReadStream& aStream,
            MCatalogsHttpConfigObserver* aObserver = NULL );
        
        
        /**
        * Destructor
        */
        ~CCatalogsHttpConfig();
        
    
    public: // From MCatalogsHttpConfig
    
        /**
        * @see MCatalogsHttpConfig::SetPriority()
        */            
        void SetPriority( TCatalogsTransportPriority aPriority );    
    
        
        /**
        * @see MCatalogsHttpConfig::Priority()
        */            
        TCatalogsTransportPriority Priority() const;
        
        
        /**
        * @see MCatalogsHttpConfig::SetDirectoryL()
        */            
        void SetDirectoryL( const TDesC& aDirectory );


        /**
        * @see MCatalogsHttpConfig::Directory()
        */            
        const TDesC& Directory() const;


        /**
        * @see MCatalogsHttpConfig::SetFilenameL()
        */            
        void SetFilenameL( const TDesC& aFilename );


        /**
        * @see MCatalogsHttpConfig::Filename()
        */            
        const TDesC& Filename() const;


        /**
         * @see MCatalogsHttpConfig::FullPatH()
         */
        HBufC* FullPathLC() const;
        

        /**
        * @see MCatalogsHttpConfig::RequestHeaders()
        */            
        MCatalogsHttpHeaders& RequestHeaders() const;

        
        // Access point
        
        /**
        * @see MCatalogsHttpConfig::SetConnectionMethod()
        */            
        void SetConnectionMethod( const TCatalogsConnectionMethod& aMethod );
        
        
        /**
        * @see MCatalogsHttpConfig::ConnectionMethod()
        */            
        const TCatalogsConnectionMethod& ConnectionMethod() const;


        /**
        * @see MCatalogsHttpConfig::SetObserver()
        */            
        void SetObserver( MCatalogsHttpObserver* aObserver );

        
        /**
        * @see MCatalogsHttpConfig::Observer()
        */            
        MCatalogsHttpObserver* Observer() const;


        /**
        * @see MCatalogsHttpConfig::SetHttpMethod()
        */            
        void SetHttpMethod( TCatalogsHttpMethod aMethod );
        
        
        /**
        * @see MCatalogsHttpConfig::HttpMethod()
        */            
        TCatalogsHttpMethod HttpMethod() const;
        
        
        /**
         * @see MCatalogsHttpConfig::ExternalizeL()
         */            
        void ExternalizeL( RWriteStream& aStream ) const;        
        
        
        /**
         * @see MCatalogsHttpConfig::InternalizeL()
         */            
        void InternalizeL( RReadStream& aStream );
        
        
        /**
         * @see MCatalogsHttpConfig::Options()
         */                            
        TUint32 Options() const;
                       
        /**
         * @see MCatalogsHttpConfig::Options()
         */
        void SetOptions( TUint32 aOptions );
        
        
    public:          
        
        /**
        * Clones the configuration
        *
        * @return A copy of the configuration
        */            
        CCatalogsHttpConfig* CloneL() const;


    private:
    
        /** 
        * Constructor 
        */
        CCatalogsHttpConfig( MCatalogsHttpConfigObserver* aObserver );
        

        /**
        * 2nd phase constructor
        */
        void ConstructL();   
        
        
        /**
        * Copy constructor
        */
        CCatalogsHttpConfig( const CCatalogsHttpConfig& aOther );     
        
        
        /**
        * 2nd phase copy constructor
        */
        void ConstructL( const CCatalogsHttpConfig& aOther );   
        
    private:
    
        MCatalogsHttpConfigObserver* iOwnObserver;
        TCatalogsTransportPriority iPriority;
        HBufC* iDirectory;
        HBufC* iFilename;
        CCatalogsHttpHeaders* iHeaders;
        TCatalogsConnectionMethod iConnectionMethod;
        MCatalogsHttpObserver* iObserver;     
        TCatalogsHttpMethod iHttpMethod;
        TUint32 iOptions;
    };


#endif // C_CATALOGSHTTPCONFIG_H

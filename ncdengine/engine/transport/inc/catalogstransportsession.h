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


#ifndef M_CATALOGSTRANSPORTSESSION_H
#define M_CATALOGSTRANSPORTSESSION_H


/**
* Base interface for Transport sessions
*/
class MCatalogsTransportSession
    {
    public:
    
    
        /**
         * AddRef
         * Increases the reference counter by given value.
         *
         * @param aNum is the number by which the reference count is increased.
         * @return TInt the new reference count value.
         */
        virtual TInt AddRef() = 0;		
        
        /**
         * Release
         * Releases a reference. Deletes the object if the reference count
         * reaches 0. 
         * Notice that destructor is defined as protected. So, the object of
         * this interface can be deleted using this function and only after 
         * reference count is zero.
         *
         * @return Reference count after release
         */
        virtual TInt Release() = 0;
        
        /**
         * Returns the number of registered references to the object 
         * that implements the MCatalogBase interfaces.
         *
         * @return TInt Reference count     
         */
        virtual TInt RefCount() const = 0;
    
        /**
        * Returns session ID
        *
        * @return Session ID
        */
        virtual TInt32 SessionId() const = 0;
        
        
        /**
        * Returns the type of the session.
        *
        * This value is the same is the interface ID used to create the
        * session
        *
        * @return Session type
        */
        virtual TInt SessionType() const = 0;
        
    protected:
    
        virtual ~MCatalogsTransportSession()
            {
            }
    
    };


#endif // M_CATALOGSTRANSPORTSESSION_H
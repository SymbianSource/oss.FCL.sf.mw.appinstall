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
	

#ifndef M_CATALOGSTRANSPORTOPERATION_H
#define M_CATALOGSTRANSPORTOPERATION_H


/**
* Operation progress information
*/
class TCatalogsTransportProgress	
    {
    public:
        /**
        * Constructor
        */
        TCatalogsTransportProgress( TInt aState, TInt32 aProgress, 
            TInt32 aMaxProgress ) :
            iState( aState ), 
            iProgress( aProgress ), 
            iMaxProgress( aMaxProgress )
            {			
            }
	
    public:
        TUint iState;         // State of the operation
        TInt32 iProgress;     // Progress of the current state
        TInt32 iMaxProgress;  // Maximum progress value for the current state
    };


/**
* Transport Operation interface	
*/
class MCatalogsTransportOperation //: public MCatalogsBase	
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
        * Cancel operation
        *
        * @return Error code
        */
        virtual TInt Cancel() = 0;


        /**
        * Progress information getter
        *
        * @return Progress information
        */
        virtual TCatalogsTransportProgress Progress() const = 0;
        
    protected:
    
        /**
        * Protected destructor
        */
        virtual ~MCatalogsTransportOperation() 
            {
            }
	
	};
	
	
#endif //  M_CATALOGSTRANSPORTOPERATION_H

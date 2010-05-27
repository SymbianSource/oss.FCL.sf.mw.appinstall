/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CTask class.
*
*/


#ifndef SWINSTTASK_H
#define SWINSTTASK_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include <ecom/ecom.h>

#include <SWInstUid.h>

class RWriteStream;
class RReadStream;

namespace SwiUI
{

//  CONSTANTS
const TUid KTaskInterfaceUid = { KSWInstTaskInterfaceUid } ;

/**
* Task represents a single task in task manager. This is an abstract interface
* and concrete tasks are implemented as ecom plugins.
*
* @since 3.0
*/
class CTask : public CBase
    {
    public:  // Constructors and destructor
 
        /**
        * Two-phased constructor.
        */
        inline static CTask* NewL( const TUid& aImplUid, TBool aIsPersistent );
        
        /**
        * Destructor.
        */
        inline virtual ~CTask();

    public: // New functions
        
        /**
        * Return the ECOM implementation uid of this task.
        * @since 3.0
        * @return ECOM uid of this implementation.
        */
        inline const TUid& ImplementationUid() const;

        /**
        * Indicate if this task is persistent or not.
        * @since 3.0
        * @return ETrue, if the task is persistent, EFalse otherwise.
        */
        inline TBool IsPersistent() const;

        /**
        * Gets the id of the task.
        * @since 3.0
        * @return Id of the task.
        */
        inline TInt Id() const;

        /**
        * Sets the id of the task. This should be used only by the 
        * task manager.
        * @since 3.0
        * @param aId - Id of the task.
        */
        inline void SetId( TInt aId );

        /**
        * Externalizes the task.
        * @since 3.0
        * @param aStream - The stream where to externalize.
        */      
        inline void ExternalizeL( RWriteStream& aStream ) const;

        /**
        * Internalizes the task.
        * @since 3.0
        * @param aStream - The stream where to internalize.
        */      
        inline void InternalizeL( RReadStream& aStream );
        
        /**
        * Executes the task.
        * @since 3.0
        */
        virtual void ExecuteL() = 0;
      
        /**
        * Adds a parameter to the task.
        * @since 3.0
        * @param aParam - Packaged paramter to set.
        * @param aIndex - Index of the parameter.
        */
        virtual void SetParameterL( const TDesC8& aParam, TInt aIndex ) = 0;        

    protected: // New functions        

        /**
        * Derived classes must implement this to externalize themselves.
        * @since 3.0
        * @param aStream - The stream where to externalize.
        */      
        virtual void DoExternalizeL( RWriteStream& aStream ) const = 0;

        /**
        * Derived classes must implement this to internalize themselves.
        * @since 3.0
        * @param aStream - The stream where to internalize.
        */      
        virtual void DoInternalizeL( RReadStream& aStream ) = 0;   

    private: // Data
       
        TUid iDestructKey;
        TUid iImplementationUid; 
        TBool iIsPersistent;        
        TInt iId;        
    };
}

#include "SWInstTask.inl"

#endif      // SWINSTTASK_H   
            
// End of File

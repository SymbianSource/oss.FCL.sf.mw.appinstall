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
* Description:   This file contains the header file of the CLogTask 
*                class.
*
*/


#ifndef LOGTASK_H
#define LOGTASK_H

//  INCLUDES
#include <e32base.h>
#include <SWInstTask.h>
#include <SWInstLogTaskParam.h>

namespace SwiUI
{

/**
* Base abstract class for startup list operations.
*
* @since 3.0
*/
class CLogTask : public CTask
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CLogTask* NewL();

        /**
        * Destructor.
        */
        virtual ~CLogTask();

    public: // Functions from base classes
        
        /**
        * From CTask, Executes the task.
        * @since 3.0
        */
        void ExecuteL();       

        /**
        * From CTask, Adds a parameter to the task.
        * @since 3.0
        * @param aParam - Packaged paramter to set.
        * @param aIndex - Index of the parameter.
        */
        void SetParameterL( const TDesC8& aParam, TInt aIndex );
                
     protected: // Functions from base classes.

        /**
        * From CTask, Externalizes the task.
        * @since 3.0
        * @param aStream - The stream where to externalize.
        */      
        void DoExternalizeL( RWriteStream& aStream ) const;

        /**
        * From CTask, Internalizes the task.
        * @since 3.0
        * @param aStream - The stream where to internalize.
        */      
        void DoInternalizeL( RReadStream & aStream ); 

   private: // New functions
      
        /**
        * Constructor.
        */  
        CLogTask();

        /**
        * Constructs a version string..
        * @since 3.0
        * @param aMajor - Major version number.
        * @param aMinor - Minor version number.
        * @return Created descriptor containing the version string.
        */      
        HBufC* ConstructVersionStringLC( TInt aMajor, TInt aMinor, TInt aBuild );  

        /**
        * Helper to executes the task.
        * @since 3.0
        */
        void DoExecuteL();        
   
    private: // Data

        TLogTaskParam iLogEntry;        
    };
}

#endif      // LOGTASK_H   
            
// End of File

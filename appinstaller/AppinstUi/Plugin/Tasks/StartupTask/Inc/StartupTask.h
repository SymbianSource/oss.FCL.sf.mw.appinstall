/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CStartupTask 
*               class.
*
*/


#ifndef STARTUPTASK_H
#define STARTUPTASK_H

//  INCLUDES
#include <e32base.h>
#include <SWInstTask.h>
#include "StartupItem.h"

class RDscStore;

namespace SwiUI
{

/**
* Base abstract class for startup list operations.
*
* @since 3.0
*/
class CStartupTask : public CTask
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~CStartupTask();

    public: // Functions from base classes
        
        /**
        * From CTask, Adds a parameter to the task.
        * @since 3.0
        * @param aParam - Packaged paramter to set.
        * @param aIndex - Index of the parameter.
        */
        void SetParameterL( const TDesC8& aParam, TInt aIndex );
                
    protected: // New functions
      
        /**
        * Constructor.
        */  
        CStartupTask();
        
        /**
         * Helper function to create RDscStore when needed.
         */
        static void CreateIfNotExistL( RDscStore& aDsc );

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
   
    protected: // Data

        RPointerArray<CStartupItem> iItems;        
    };
}

#endif      // STARTUPTASK_H   
            
// End of File

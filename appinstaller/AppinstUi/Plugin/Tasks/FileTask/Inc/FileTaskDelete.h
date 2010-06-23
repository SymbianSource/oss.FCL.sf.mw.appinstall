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
* Description:   This file contains the header file of the CFileTaskDelete 
*                class.
*
*/


#ifndef FILETASKDELETE_H
#define FILETASKDELETE_H

//  INCLUDES
#include <e32base.h>
#include <SWInstTask.h>
#include <SWInstFileTaskParam.h>

namespace SwiUI
{

/**
* Base abstract class for startup list operations.
*
* @since 3.0
*/
class CFileTaskDelete : public CTask
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CFileTaskDelete* NewL();

        /**
        * Destructor.
        */
        virtual ~CFileTaskDelete();

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
        CFileTaskDelete();

    private: // Data

        TFileTaskDeleteParam iParam;        
    };
}

#endif      // FILETASKDELETE_H   
            
// End of File

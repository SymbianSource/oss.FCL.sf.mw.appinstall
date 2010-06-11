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
* Description:   This file contains the header file of the CStartupItem 
*                class.
*
*/


#ifndef STARTUPITEM_H
#define STARTUPITEM_H

//  INCLUDES
#include <e32base.h>
#include <StartupItem.hrh>
#include "SWInstStartupTaskParam.h"

class RReadStream;
class RWriteStream;

namespace SwiUI
{

/**
* Class to represent a single item in startup list.
*
* @since 3.0
*/
class CStartupItem : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CStartupItem* NewL( const TStartupTaskParam& aParam );

        /**
        * Two-phased constructor.
        */
        static CStartupItem* NewL( RReadStream& aStream );

        /**
        * Destructor.
        */
        virtual ~CStartupItem();

    public: // New functions
        
        /**
        * Get the executable file of this startup item.
        * @since 3.0
        * @return Full path to executable file.
        */
        const TDesC& ExecutableFile() const;
        
        /**
        * Get the startup recovery policy for this item.
        * @since 3.0
        * @return Recovery policy.
        */
        TStartupExceptionPolicy RecoveryPolicy() const;

        /**
        * Externalize the class.
        * @since 3.0
        * @param - aStream stream where to externalize.
        */
        void ExternalizeL( RWriteStream &aStream ) const;

    private:

        /**
        * Constructor.
        */  
        CStartupItem();
        
        /**
        * 2nd phase constructor.
        */
        void ConstructL( const TStartupTaskParam& aParam );    

        /**
        * 2nd phase constructor.
        */
        void ConstructL( RReadStream& aStream );        
    
    private: // Data

        HBufC* iExecutableFile;
        TStartupExceptionPolicy iRecoveryPolicy;
    };
}

#endif      // STARTUPITEM_H   
            
// End of File

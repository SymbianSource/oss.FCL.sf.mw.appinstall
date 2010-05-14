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
* Description:   This file contains the header file of the CSisxUISelectionCache
*                class.
*
*                This class acts as a cache for user selections during
*                installation.
*
*/


#ifndef SISXUISELECTIONCACHE_H
#define SISXUISELECTIONCACHE_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>

namespace SwiUI
{
class CSisxUIHandler;

/**
* This class acts as a cache for user selections during installation.
* @lib sisxui
* @since 3.0 
*/
class CSisxUISelectionCache : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSisxUISelectionCache* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CSisxUISelectionCache();

    public:  // From base classes
             
        /**
        * Indicates if language selection is cached. 
        * @since 3.0
        * @return ETrue, EFalse
        */
        TBool LanguageCached() const;
        
        /**
        * Returns cached language.
        * @since 3.0
        * @return Cached language.
        */
        TLanguage Language() const;
        
        /**
        * Sets the cached language.
        * @since 3.0
        * @param aLanguage - Language to be cached.
        */
        void SetLanguage( const TLanguage& aLanguage );

        /**
        * Indicates if drive selection is cached. 
        * @since 3.0
        * @return ETrue, EFalse
        */
        TBool DriveCached() const;
        
        /**
        * Returns cached drive.
        * @since 3.0
        * @return Cached drive.
        */
        TDriveUnit Drive() const;
        
        /**
        * Sets the cached drive.
        * @since 3.0
        * @param aLanguage - Drive to be cached.
        */
        void SetDrive( TDriveUnit aDrive );

        /**
        * Indicates if options selection is cached. 
        * @since 3.0
        * @param aOptions - Array of current options.
        * @return ETrue, EFalse
        */
        TBool OptionsCached( const RPointerArray<TDesC>& aOptions ) const;

        /**
        * Returns cached option selections.
        * @since 3.0
        * @param aSelections - On return contains the cached selections.
        */
        void Options( RArray<TBool>& aSelections );
        
        /**
        * Sets the cached options.
        * @since 3.0
        * @param aLanguage - Drive to be cached.
        */
        void SetOptionsL( const RPointerArray<TDesC>& aOptions, const RArray<TBool>& aSelections );    
        
    private:

        /**
        * Constructor.
        */  
        CSisxUISelectionCache();
         
        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private: //  Data

        TBool iLanguageCached;
        TLanguage iLanguage;
        
        TDriveUnit iDrive;        

        RPointerArray<HBufC> iOptions;        
        RArray<TBool> iSelections;       
    };
}

#endif      // SISXUISELECTIONCACHE_H  
            
// End of File

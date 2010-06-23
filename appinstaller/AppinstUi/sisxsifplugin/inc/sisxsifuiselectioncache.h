/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CSisxSifUiSelectionCache saves user selections during
*                installation. Cached selections can be re-used, to avoid
*                displaying the same dialogs again.
*/

#ifndef C_SISXSIFUISELECTIONCACHE_H
#define C_SISXSIFUISELECTIONCACHE_H

#include <e32base.h>
#include <f32file.h>


/**
 * Saves user selections during installation. Installer may re-use
 * cached selections in order to avoid displaying the same prompts
 * several times.
 */
class CSisxSifUiSelectionCache : public CBase
    {
    public:     // constructor and destructor
        static CSisxSifUiSelectionCache* NewL();
        ~CSisxSifUiSelectionCache();

    public:     // new functions
        /**
         * Returns ETrue if language selection is currently cached, and
         * EFalse otherwise. Language selection can be cached by using
         * SetLanguage() method.
         * @return TBool -- ETrue if language is cached
         */
        TBool LanguageCached() const;
        
        /**
         * Returns the language code saved earlier with SetLanguage() method.
         * If no language has been set, then returns language code 0 (ELangTest).
         * @return TLanguage -- the cached language, or ELangTest
         */
        TLanguage Language() const;
        
        /**
        * Caches the given language code.
        * @param aLanguage - language to be cached
        */
        void SetLanguage( const TLanguage& aLanguage );

        /**
        * Returns ETrue if drive selection is currently cacehed, and EFalse
        * otherwise. Drive selection can be cached by using SetDrive() method.
        * @return TBool -- ETrue if drive is cached
        */
        TBool DriveCached() const;
        
        /**
        * Returns the drive saved earlier with SetDrive() method.
        * @return TDriveUnit -- the cached drive
        */
        TDriveUnit Drive() const;
        
        /**
        * Caches the given drive.
        * @param aLanguage -- drive to be cached
        */
        void SetDrive( TDriveUnit aDrive );

        /**
        * Returns ETrue if all options in aOptions array are cached, and EFalse
        * otherwise. Options can be cached using SetOptions() method.
        * @param aOptions -- array of options names
        * @return TBool -- ETrue if all aOptions are cached
        */
        TBool OptionsCached( const RPointerArray<TDesC>& aOptions ) const;

        /**
        * Returns the options saved earlier with SetOptions() method.
        * @param aSelections -- the cached option selections
        */
        void Options( RArray<TBool>& aSelections );
        
        /**
        * Caches the given options.
        * @param aOptions -- array of options names
        * @param aSelections -- array of option selections
        */
        void SetOptionsL( const RPointerArray<TDesC>& aOptions, const RArray<TBool>& aSelections );    
        
    private:    // new functions
        CSisxSifUiSelectionCache();
        void ConstructL();

    private:    // data
        TBool iLanguageCached;
        TLanguage iLanguage;
        TDriveUnit iDrive;        
        RPointerArray<HBufC> iOptions;        
        RArray<TBool> iSelections;       
    };


#endif      // C_SISXSIFUISELECTIONCACHE_H


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
* Description:   This file contains the header file of the CSisxUIStartupFile 
*                class.
*
*                CSisxUIStartupFile provides functions to access the special 
*                startup control files.
*
*                During the installation, the startup constrol file is copied to 
*                private import directory of the installer process. This class 
*                is used to process entries from that file. When entried have 
*                been processed, the file is moved to private directory.
*
*/


#ifndef SISXUISTARTUPFILE_H
#define SISXUISTARTUPFILE_H

//  INCLUDES
#include <e32base.h>

namespace SwiUI
{

class TStartupTaskParam;

/**
* Provides functions to access the special startup control files.
*
* @lib sisxui
* @since 3.0 
*/
class CSisxUIStartupFile : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @since 3.0
        * @param aUid - Uid of the package, whose startup file this class
        *               will represent.
        */
        static CSisxUIStartupFile* NewL( const TUid& aUid );
        
        /**
        * Destructor.
        */
        virtual ~CSisxUIStartupFile();

    public:  // New functions
         
        /**
        * Checks if new startup file exists in the import directory.
        * @since 3.0
        * @return ETrue, EFalse.
        */  
        TBool NewFileExists();

        /**
        * Checks if old, processed startup file exists in the private directory.
        * @since 3.0
        * @return ETrue, EFalse.
        */  
        TBool PrivateFileExists();

        /**
        * Read new startup file and process it's definitions. Creates a new private file
        * based on the resource file and returns all new startup items.
        * @since 3.0
        * @param aStartupItems - On return contains all startup items found from the 
        *                        startup file.
        * @param aFiles - Array of file names, which are part of the pakcage.
        * @return On success KErrNone, otherwise one of the system wide error codes.
        */  
        TInt ProcessNewFile( RArray<TStartupTaskParam>& aStartupItems, 
                             const RPointerArray<HBufC>& aFiles );

        /**
        * Deletes the old startup file from private directory and returns all startup 
        * items in that file.
        * @since 3.0
        * @param aStartupItems - On return contains all startup items found from the 
        *                        startup file.
        * @param aFiles - Array of file names, which are part of the pakcage.
        * @return On success KErrNone, otherwise one of the system wide error codes.
        */  
        TInt RemovePrivateFile( RArray<TStartupTaskParam>& aStartupItems );

        /**
        * Deletes all startup resource files from import directory. 
        * @since 3.0
        * @return On success KErrNone, otherwise one of the system wide error codes.
        */      
        static TInt ClearAllNewFiles();              
      
    private:

        /**
        * Constructor.
        */  
        CSisxUIStartupFile();
         
        /**
        * 2nd phase constructor.
        * @param aUid - Uid of the package, whose startup file this class
        *               will represent.
        */
        void ConstructL( const TUid& aUid );

        /**
        * Read new resource file and construct startup items from there.
        * @since 3.0
        * @param aStartupItems - On return contains all startup items found from the 
        *                        startup file.
        * @param aFiles - Array of file names, which are part of the pakcage.
        */  
        void ReadNewStartupItemsL( RArray<TStartupTaskParam>& aStartupItems,
                                   const RPointerArray<HBufC>& aFiles );

        /**
        * Read startup items from private file.
        * @since 3.0
        * @param aStartupItems - On return contains all startup items found from the 
        *                        startup file.
        */
        void ReadPrivateStartupItemsL( RArray<TStartupTaskParam>& aStartupItems );

        /**
        * Write startup items into private file.
        * @since 3.0
        * @param aStartupItems - The startup items to be written to the private file.
        */    
        void WritePrivateStartupItemsL( RArray<TStartupTaskParam>& aStartupItems );        

    private: //  Data

        HBufC* iPrivateFile;
        HBufC* iNewFile; 

        RFs iFileSession;
        TFileName iTemp;        
    };
}

#endif      // SISXUISTARTUPFILE_H   
            
// End of File

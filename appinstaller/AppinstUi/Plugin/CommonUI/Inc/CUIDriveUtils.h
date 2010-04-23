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
* Description:   This file contains the header file of the CCUIDriveUtils class 
*
*/


#ifndef CUIDRIVEUTILS_H
#define CUIDRIVEUTILS_H

//  INCLUDES
#include <e32base.h>

//  FORWARD DECLARATIONS

namespace SwiUI
{
namespace CommonUI
{

/**
* This class provides functions to launch a common file selection dialog.
* @lib SWInstCommonUI.lib
* @since 3.0
*/
NONSHARABLE_CLASS(CCUIDriveUtils) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aSize - Size of the package.
        * @param aDriveUnits - Array of available drives as drive units.
        * @param aDriveSpaces - Array of free space on each of the drives in the
        *                       aDriveLetters array.
        */
        static CCUIDriveUtils* NewL( TInt64 aSize,
                                     const RArray<TDriveUnit>& aDriveUnits, 
                                     const RArray<TInt64>& aDriveSpaces);
        
        /**
        * Destructor.
        */
        virtual ~CCUIDriveUtils();


    public: // New functions
        
        /**
        * Show a drive selection dialog. 
        * @since 3.0
        * @param aSelectedDrive - Contains the drive letter that user selected.
        * @return ETrue is user selected a drive, EFalse if user canceled the query.
        */
        TBool ShowSelectionDialogL( TInt& aSelectedDrive );

        /**
        * Function to get info if internal drive is present (phone memory).
        * @since 3.0
        * @return ETrue if yes, EFalse if no.
        */
        TBool IsInternalDrivePresent() const;

        /**
        * Function to get info if memory card is present.
        * @since 3.0
        * @return ETrue if yes, EFalse if no.
        */
        TBool IsCardDrivePresent() const;  
        
        /**
        * Get the free space on phone memory.
        * @since 3.0
        * @return Free space on phone memory.
        */
        TInt64 GetInternalDriveSpace() const;

        /**
        * Get the free space on memory card.
        * @since 3.0
        * @return Free space on memory card.
        */
        TInt64 GetCardDriveSpace() const;       

        /**
        * Get the free space on drive.
        * @since 3.0
        * @param aIndex - Drive index.
        * @return Free space on memory card.
        */
        TInt64 GetDriveSpace( TInt aIndex ) const;      
        
        /**
        * Get array index of the phone memory in the aDriveLetters array.
        * @since 3.0
        * @return Array index of the phone memory.
        */      
        TInt GetInternalDriveIndex() const;

        /**
        * Get array index of the memory card in the aDriveLetters array.
        * @since 3.0
        * @return Array index of the memory card.
        */  
        TInt GetCardDriveIndex() const;        
        
    private:
        
        /**
        * Constructor.
        * @param aSize - Size of the package.
        * @param aDriveUnits - Array of available drives as drive units.
        * @param aDriveSpaces - Array of free space on each of the drives in the
        *                            aDriveLetters array.
        */
        CCUIDriveUtils( TInt64 aSize,
                        const RArray<TDriveUnit>& aDriveUnits, 
                        const RArray<TInt64>& aDriveSpaces);

        /**
        * 2nd phase constructor.
        */
        void ConstructL();
             
    private: // Data

        TInt64 iSize;        
        const RArray<TDriveUnit>& iDriveUnits;        
        const RArray<TInt64>& iDriveSpaces;        

        TInt iInternalArrayIndex;
        TInt iCardArrayIndex;    
    
        TChar iCardDriveLetter;  

        TInt64 iInternalFreeMemory;
        TInt64 iCardFreeMemory;
    };
}
}

#endif      // SWCOMMONUI_H
            
// End of File

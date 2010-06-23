/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of CCUIDriveSelection
*                class member functions.
*
*/


// INCLUDE FILES
#include <StringLoader.h>
#include <pathinfo.h>
#include <data_caging_path_literals.hrh>
#include <SWInstCommonUI.rsg>
//#include <AknCommonDialogs.h> // Removed since CR 548
//#include <CAknMemorySelectionDialog.h> // Removed since CR 548
#ifdef RD_MULTIPLE_DRIVE 
//#include <CAknMemorySelectionDialogMultiDrive.h> // Removed since CR 548
//#include <AknCommonDialogsDynMem.h> // Removed since CR 548
#else
//#include <CAknMemorySelectionDialog.h> // Removed since CR 548
#endif //RD_MULTIPLE_DRIVE

#include "CUIDriveUtils.h"


using namespace SwiUI::CommonUI;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCUIDriveUtils::CCUIDriveUtils
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCUIDriveUtils::CCUIDriveUtils( TInt64 aSize,
                                const RArray<TDriveUnit>& aDriveUnits, 
                                const RArray<TInt64>& aDriveSpaces)
    : iSize( aSize ),
      iDriveUnits( aDriveUnits ),
      iDriveSpaces( aDriveSpaces ),
      iInternalArrayIndex( -1 ),
      iCardArrayIndex( -1 )
    {
  
    }

// -----------------------------------------------------------------------------
// CCUIDriveUtils::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCUIDriveUtils::ConstructL()
    {
    #ifndef RD_MULTIPLE_DRIVE
    // Find the Phone and memory card drives from the drive array
    // and store the corresponding indexes, drive letters and free memories.
    TDriveUnit phoneMemUnit( TParsePtrC( PathInfo::PhoneMemoryRootPath() ).Drive() );
    TDriveUnit mmcUnit( TParsePtrC( PathInfo::MemoryCardRootPath() ).Drive() );    

    for ( TInt index = 0; index < iDriveUnits.Count(); index++ )
        {        
        // Check if it's the phone memory
        if ( (TInt)phoneMemUnit == (TInt)iDriveUnits[index] )
            {
            iInternalArrayIndex = index;
            iInternalFreeMemory = iDriveSpaces[index];            
            }
        // Check if it's the memory card
        else if ( PathInfo::MemoryCardRootPath().Length() > 0 && 
                  (TInt)mmcUnit == (TInt)iDriveUnits[index] )
            {
            iCardArrayIndex = index;                
            iCardFreeMemory = iDriveSpaces[index];  
            }
        }
    #endif // RD_MULTIPLE_DRIVE   
    }

// -----------------------------------------------------------------------------
// CCUIDriveUtils::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCUIDriveUtils* CCUIDriveUtils::NewL(
    TInt64 aSize,
    const RArray<TDriveUnit>& aDriveUnits, 
    const RArray<TInt64>& aDriveSpaces )
    {
    CCUIDriveUtils* self = 
        new ( ELeave ) CCUIDriveUtils( aSize, aDriveUnits, aDriveSpaces );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self; 
    }

// Destructor
CCUIDriveUtils::~CCUIDriveUtils()
    {
    
    }

// -----------------------------------------------------------------------------
// CCUIDriveUtils::ShowSelectionDialogL
// Set callback for the wait dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCUIDriveUtils::ShowSelectionDialogL( TInt& aSelectedDrive )
    {  

    TBool result( EFalse );
    
    // Removed since CR 548   
/*    
    aSelectedDrive = 0;    

#ifdef RD_MULTIPLE_DRIVE           
    TBool externalDriveFound = EFalse;
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    
    // Find out if there is MMC among the available drives.
    for ( TInt index = 0; index < iDriveUnits.Count(); index++ )
        {         
        TDriveInfo info;
                              
        if ( KErrNone != fs.Drive( info, iDriveUnits[index] ) )
            {            
            // Ignore errors since the next drive might work.            
            continue;
            }
        // Note that this do not work in WINS env.
        if ( info.iDriveAtt & KDriveAttRemovable )
            {            
            externalDriveFound = ETrue;
            }                                  
        } 
   
#ifdef __WINS__
    // Removable drives are not found in WINS env. 
    externalDriveFound = ETrue;    
#endif //__WINS__    
   
    CleanupStack::PopAndDestroy( &fs );
    TInt supportedMemTypes = 0;

    // If memory card is available for Symbian SWI, show all drives.
    if ( externalDriveFound )
        {
        supportedMemTypes = AknCommonDialogsDynMem::EMemoryTypePhone|
                            AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage|
                            AknCommonDialogsDynMem::EMemoryTypeMMCExternal;                   
        }
    else
        {
         // MMC is not available for Symbian SWI. Note that user may have insert the MMC
         // but Symbian SWI has select available drives before that and we can not use
         // drvie which is not listed. 
         // Note also that if device has two MMC cards this fix do not really work.
         supportedMemTypes = AknCommonDialogsDynMem::EMemoryTypePhone|
                             AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage;   
        }
    
    CAknMemorySelectionDialogMultiDrive* memoryDialog = 
                               CAknMemorySelectionDialogMultiDrive::NewL(
                                                  	ECFDDialogTypeSave,
                                                    R_SWCOMMON_MEMORY_SELECTION_DIALOG,
                                                    EFalse,
                                                    supportedMemTypes );
                                                              
#else
    // Use ECFDDialogTypeSave to have double list box in the query
    CAknMemorySelectionDialog* memoryDialog = CAknMemorySelectionDialog::NewL(
        ECFDDialogTypeSave,
        R_SWCOMMON_MEMORY_SELECTION_DIALOG,
        ETrue );
#endif  //RD_MULTIPLE_DRIVE      
    CleanupStack::PushL( memoryDialog );    
        
    TInt size( (TInt) iSize / 1024 );
    if ( size == 0 )
        {
        size = 1;        
        }
    
    HBufC* header = StringLoader::LoadLC( R_SWCOMMON_MEMORY_HEADING, size );
    memoryDialog->SetTitleL( header->Des() );    

#ifdef RD_MULTIPLE_DRIVE
    TDriveNumber driveNumber((TDriveNumber)KErrNotFound);    
    result = memoryDialog->ExecuteL( driveNumber, NULL, NULL );
        
    if ( result != CAknCommonDialogsBase::TReturnKey(CAknCommonDialogsBase::ERightSoftkey) )
        {
    	for ( TInt index = 0; index < iDriveUnits.Count(); index++ )
            {             
            if (driveNumber == iDriveUnits[index] )  
               {
               aSelectedDrive = index;                  
               break;	
               }
            }
        }
#else    
    CAknMemorySelectionDialog::TMemory mem( CAknMemorySelectionDialog::EPhoneMemory );
    result = memoryDialog->ExecuteL( mem );

    if ( mem == CAknMemorySelectionDialog::EPhoneMemory )
        {
        // Phone memory selected
        aSelectedDrive = GetInternalDriveIndex();        
        }
    
    else
        {
        // Memory card selected
        aSelectedDrive = GetCardDriveIndex();        
        }    

#endif  //RD_MULTIPLE_DRIVE  
    CleanupStack::PopAndDestroy( 2 ); // header, memoryDialog
*/ 
    return result;    
    }    

// -----------------------------------------------------------------------------
// CCUIDriveUtils::IsInternalDrivePresent
// Set callback for the wait dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCUIDriveUtils::IsInternalDrivePresent() const
    {
    TBool result( EFalse );
    
    if ( iInternalArrayIndex > -1 )
        {
        result = ETrue;        
        }
    return result;    
    }

// -----------------------------------------------------------------------------
// CCUIDriveUtils::IsCardDrivePresent
// Set callback for the wait dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCUIDriveUtils::IsCardDrivePresent() const
    {    
    TBool result( EFalse );
    
    if ( iCardArrayIndex > -1 )
        {
        result = ETrue;        
        }
    return result;    
    }
  
// -----------------------------------------------------------------------------
// CCUIDriveUtils::GetInternalDriveSpace
// Set callback for the wait dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt64 CCUIDriveUtils::GetInternalDriveSpace() const
    {
    return iInternalFreeMemory;    
    }

// -----------------------------------------------------------------------------
// CCUIDriveUtils::GetCardDriveSpace
// Set callback for the wait dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt64 CCUIDriveUtils::GetCardDriveSpace() const
    {
    return iCardFreeMemory;    
    }


// -----------------------------------------------------------------------------
// CCUIDriveUtils::GetDriveSpace
// Get the free space on drive.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt64 CCUIDriveUtils::GetDriveSpace( TInt aIndex ) const
    {
    if ( aIndex == GetInternalDriveIndex() )
        {
        return GetInternalDriveSpace();        
        }    
    else
        {
        return GetCardDriveSpace();        
        }    
    }

// -----------------------------------------------------------------------------
// CCUIDriveUtils::GetInternalDriveSpace
// Set callback for the wait dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCUIDriveUtils::GetInternalDriveIndex() const
    {
    return iInternalArrayIndex;    
    }

// -----------------------------------------------------------------------------
// CCUIDriveUtils::GetCardDriveSpace
// Set callback for the wait dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCUIDriveUtils::GetCardDriveIndex() const
    {
    return iCardArrayIndex;    
    }

//  End of File


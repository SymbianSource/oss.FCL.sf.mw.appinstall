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
* Description:   This file contains the implementation of CSisxUIStartupFile
*                class member functions.
*
*/


// INCLUDE FILES
#include <s32file.h>
#include <barsc.h>
#include <barsread.h>
#include <data_caging_path_literals.hrh>
#include <pathinfo.h>
#include <SWInstStartupTaskParam.h>

#include "SisxUIStartupFile.h"

using namespace SwiUI;

_LIT( KNewFileFormat, "%S.rsc" );
_LIT( KNewFileMatch, "[*].rsc" );
_LIT( KPrivateFileDir, "startup\\" );
_LIT( KPrivateFileFormat, "%S.dat" );
_LIT( KImportDir, "Import\\" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSisxUIStartupFile::CSisxUIStartupFile
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSisxUIStartupFile::CSisxUIStartupFile()
    {
    }

// -----------------------------------------------------------------------------
// CSisxUIStartupFile::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSisxUIStartupFile::ConstructL( const TUid& aUid )
    {
    User::LeaveIfError( iFileSession.Connect() );

    TUidName uidStr = aUid.Name();
    TFileName tmp;
    
    iFileSession.PrivatePath( tmp );    
    tmp.Insert( 0, TParsePtrC( PathInfo::PhoneMemoryRootPath() ).Drive() );
    tmp.Append( KImportDir );      
    tmp.AppendFormat( KNewFileFormat, &uidStr ); 
    iNewFile = tmp.AllocL();
    tmp.Zero();
        
    iFileSession.PrivatePath( tmp );
    tmp.Insert( 0, TParsePtrC( PathInfo::PhoneMemoryRootPath() ).Drive() );
    tmp.Append( KPrivateFileDir );    
    iFileSession.MkDir( tmp );    
    tmp.AppendFormat( KPrivateFileFormat, &uidStr );
    iPrivateFile = tmp.AllocL();
    }

// -----------------------------------------------------------------------------
// CSisxUIStartupFile::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSisxUIStartupFile* CSisxUIStartupFile::NewL( const TUid& aUid )
    {
    CSisxUIStartupFile* self = new( ELeave ) CSisxUIStartupFile();
    CleanupStack::PushL( self );
    self->ConstructL( aUid );
    CleanupStack::Pop( self );
    return self;    
    }
    
// Destructor
CSisxUIStartupFile::~CSisxUIStartupFile()
    {
    iFileSession.Close();
    delete iPrivateFile;
    delete iNewFile;   
    }

// -----------------------------------------------------------------------------
// CSisxUIStartupFile::NewFileExists
// Checks if new startup file exists in the import directory.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUIStartupFile::NewFileExists()
    {
    TBool result ( EFalse );
    
    // See if the file exists
    TUint dummy;
    if ( iFileSession.Att( *iNewFile, dummy ) == KErrNone )
        {
        result = ETrue;
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CSisxUIStartupFile::PrivateFileExists
// Checks if old, processed startup file exists in the private directory.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUIStartupFile::PrivateFileExists()
    {
    TBool result ( EFalse );

    // See if the file exists
    TUint dummy;
    if ( iFileSession.Att( *iPrivateFile, dummy) == KErrNone )
        {
        result = ETrue;
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CSisxUIStartupFile::ProcessNewFile
// Moves the new startup file from import directory to private directory and
// returns all startup items in that file.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSisxUIStartupFile::ProcessNewFile( RArray<TStartupTaskParam>& aStartupItems,
                                         const RPointerArray<HBufC>& aFiles )
    {
    TInt result( KErrNone );
    // should check if old file is already there
    
    TRAP( result, ReadNewStartupItemsL( aStartupItems, aFiles ) );
    if ( result == KErrNone )
        { 
        TRAP( result, WritePrivateStartupItemsL( aStartupItems ) );
        }    

    if ( result != KErrNone )
        {        
        // Do some cleanup
        aStartupItems.Close();
        if ( PrivateFileExists() )
            {
            iFileSession.Delete( *iPrivateFile );
            }
        }    

    return result;
    }

// -----------------------------------------------------------------------------
// CSisxUIStartupFile::RemovePrivateFile
// Deletes the old startup file from private directory and returns all startup 
// items in that file.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSisxUIStartupFile::RemovePrivateFile( RArray<TStartupTaskParam>& aStartupItems )
    {
    TInt result( KErrNone );
    TRAP( result, ReadPrivateStartupItemsL( aStartupItems ) );
    if ( result == KErrNone )
        {
        iFileSession.Delete( *iPrivateFile );
        } 
    else
        {
        // Do some cleanup
        aStartupItems.Close();        
        }    
   
    return result;
    }

// -----------------------------------------------------------------------------
// CSisxUIStartupFile::ClearAllNewFiles
// Deletes all resource files from import directory.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSisxUIStartupFile::ClearAllNewFiles()
    {
    TInt result( KErrNone );
    RFs fileSess;
    CFileMan* fileMan = NULL;
    
    result = fileSess.Connect();
    if ( result == KErrNone )
        {
        TRAP( result, fileMan = CFileMan::NewL( fileSess ) );
        }
    
    if ( result == KErrNone )
        {  
        TFileName resFile;
        fileSess.PrivatePath( resFile );
        resFile.Insert( 0, TParsePtrC( PathInfo::PhoneMemoryRootPath() ).Drive() );
        resFile.Append( KImportDir );      
        resFile.Append( KNewFileMatch ); 
        fileMan->Delete( resFile );
        }
    
    delete fileMan;    
    fileSess.Close();
    
    return result;
    }

// -----------------------------------------------------------------------------
// CSisxUIStartupFile::ReadStartupItemsL
// Get all startup items from the given resource file.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIStartupFile::ReadNewStartupItemsL( RArray<TStartupTaskParam>& aStartupItems,
                                               const RPointerArray<HBufC>& aFiles )
    {
    RResourceFile resource;
    CleanupClosePushL( resource );
    resource.OpenL( iFileSession, *iNewFile );
  
    TInt index( 0 );    

    // Read resources
    for ( index = 1; resource.OwnsResourceId( index ); index++ )
        {
        // Create the reader
        HBufC8* resBuffer = resource.AllocReadLC( index );
        TResourceReader reader;
        reader.SetBuffer( resBuffer );
 
        // Read version byte
        TInt version = reader.ReadUint8();        
     
        // Read actual parameters
        TStartupTaskParam item;
        item.iFileName.Copy( reader.ReadTPtrC() );        
        item.iRecoveryPolicy = (TStartupExceptionPolicy) reader.ReadUint16();
        
        aStartupItems.Append( item );

        CleanupStack::PopAndDestroy( resBuffer );
        }   

    // Validate the array. Each and every executable to be added to the 
    // startup list must also be installed by this package.
    for ( index = 0; index < aStartupItems.Count(); index++ ) 
        {
        // Sanity check
        if ( !aStartupItems[index].iFileName.Length() )
            {
            continue;
            }        
             
        TBool entryFound( EFalse );        
        TInt j( 0 );        
        iTemp = aStartupItems[index].iFileName;
        iTemp.Delete( 0, 1 );        
        iTemp.Fold();        

        for ( j = 0; j < aFiles.Count(); j++ )
            {            
            if ( aFiles[j]->FindF( iTemp ) >= 0 )
                {
                entryFound = ETrue;
                // Replace the possible ! with correct drive
                if ( aStartupItems[index].iFileName[0] == '!' )
                    {
                    TBuf<1> temp;
                    temp.Append( (*aFiles[j])[0] );                    
                    aStartupItems[index].iFileName.Replace(0, 1, temp );                    
                    }
                
                break;                
                }            
            }

        if ( !entryFound )
            {
            aStartupItems.Remove( index );
            index--;            
            }        
        }    

    CleanupStack::PopAndDestroy(); // resource
    }

// -----------------------------------------------------------------------------
// CSisxUIStartupFile::ReadPrivateStartupItemsL
// Read startup items from private file.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIStartupFile::ReadPrivateStartupItemsL( RArray<TStartupTaskParam>& aStartupItems )
    {
    RFileReadStream reader;
    reader.PushL();
    
    User::LeaveIfError( reader.Open( iFileSession, *iPrivateFile, EFileRead ) );
    
    // Read the number of entries
    TInt count = reader.ReadInt32L();

     // Read file names from the file
    while ( count-- )
        {
        TStartupTaskParam item;

        HBufC* fileName = HBufC::NewLC( reader, KMaxFileName );
        item.iFileName.Copy( *fileName );        
        CleanupStack::PopAndDestroy( fileName);       

        aStartupItems.Append( item );
        }

    CleanupStack::PopAndDestroy(); // reader
    }

// -----------------------------------------------------------------------------
// CSisxUIStartupFile::WritePrivateStartupItemsL
// Write startup items into private file.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIStartupFile::WritePrivateStartupItemsL( RArray<TStartupTaskParam>& aStartupItems )
    {
    if ( PrivateFileExists() )
        {
        iFileSession.Delete( *iPrivateFile );
        }

    if ( aStartupItems.Count() > 0 )
        {
        RFileWriteStream writer;
        writer.PushL();    
        User::LeaveIfError( writer.Create( iFileSession, *iPrivateFile, EFileWrite ) );
    
        // Write the number of entries to the file
        writer.WriteInt32L( aStartupItems.Count() );
        
        // Write file names to the file
        for ( TInt index = 0; index < aStartupItems.Count(); index++ )
            {
            if ( iFileSession.IsValidName( aStartupItems[index].iFileName ) )
                {            
                writer << aStartupItems[index].iFileName;
                }
            else
                {
                User::Leave( KErrArgument );            
                }               
            }

        writer.CommitL();    
        CleanupStack::PopAndDestroy(); // writer
        }    
    }

//  End of File  

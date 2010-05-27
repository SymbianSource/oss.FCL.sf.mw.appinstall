/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/



#include <xml/parser.h>
#include <stringpool.h>
#include <utf.h>
#include <e32cmn.h>
#include <bautils.h>

#include "iaupdatexmlparser.h"
#include "iaupdatexmlsubparser.h"
#include "iaupdatedebug.h"


// The text type to parse
_LIT8( KXmlType, "text/xml" );


EXPORT_C CIAUpdateXmlParser* CIAUpdateXmlParser::NewL( 
    CIAUpdateXmlSubParser* aSubParser )
    {
    CIAUpdateXmlParser* self =
        CIAUpdateXmlParser::NewLC( aSubParser );
    CleanupStack::Pop( self );
    return self;
    }


EXPORT_C CIAUpdateXmlParser* CIAUpdateXmlParser::NewLC( 
    CIAUpdateXmlSubParser* aSubParser )
    {
    CIAUpdateXmlParser* self =
        new( ELeave ) CIAUpdateXmlParser( aSubParser );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


EXPORT_C CIAUpdateXmlParser::CIAUpdateXmlParser( 
    CIAUpdateXmlSubParser* aSubParser )
: CBase(),
  iSubParser( aSubParser )
    {
    
    }
    
    
EXPORT_C void CIAUpdateXmlParser::ConstructL()
    {
    if ( !iSubParser )
        {
        User::Leave( KErrNotFound );
        }
    iParser = Xml::CParser::NewL( KXmlType, *iSubParser );
    }


EXPORT_C CIAUpdateXmlParser::~CIAUpdateXmlParser()
    {
    delete iParser;
    delete iSubParser;
    }


EXPORT_C void CIAUpdateXmlParser::ParseFileL( 
    const TDesC& aFilePath )    
    {
    HBufC8* data( ReadFileL( aFilePath ) );
    CleanupStack::PushL( data );
    
    ParseL( *data );

    CleanupStack::PopAndDestroy( data );
    }


EXPORT_C void CIAUpdateXmlParser::ParsePrivateFileL( 
    const TDesC& aFileName )
    {
    RFs fsSession;	
    User::LeaveIfError( fsSession.Connect() );
    CleanupClosePushL( fsSession );

    // This will set the correct drive and private path 
    // for the file server session.    
    SetPrivateDriveL( fsSession, aFileName );

	TFileName configFilePath;    
    User::LeaveIfError( fsSession.SessionPath( configFilePath ) );
    configFilePath.Append( aFileName );
    
    CleanupStack::PopAndDestroy( &fsSession );    

    ParseFileL( configFilePath );    
    }


EXPORT_C void CIAUpdateXmlParser::ParseL( const TDesC8& aData )
    {
    // Use parent class functions to handle the parsing
    Parser().ParseBeginL();
    Parser().ParseL( aData );
    Parser().ParseEndL();
    }


EXPORT_C void CIAUpdateXmlParser::ParseL( const TDesC& aData )
    {
    HBufC8* utf8( ConvertUnicodeToUtf8L( aData ) );
    CleanupStack::PushL( utf8 );

    ParseL( *utf8 );

    CleanupStack::PopAndDestroy( utf8 );
    }    


EXPORT_C CIAUpdateXmlSubParser& CIAUpdateXmlParser::SubParser()
    {
    return *iSubParser;
    }


Xml::CParser& CIAUpdateXmlParser::Parser()
    {
    return *iParser;
    }

    
HBufC8* CIAUpdateXmlParser::ReadFileL( const TDesC& aFilePath )
    {
    RFs fs;
    
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    
    RFile file;
    User::LeaveIfError( file.Open( fs,
                                   aFilePath,
                                   EFileRead ) );
    CleanupClosePushL( file );
    TInt size;
    User::LeaveIfError( file.Size( size ) );
    HBufC8* buffer = HBufC8::NewLC( size );
    TPtr8 ptr( buffer->Des() );
    User::LeaveIfError( file.Read( ptr, size ) );
    CleanupStack::Pop( buffer );
    CleanupStack::PopAndDestroy( &file );    

    CleanupStack::PopAndDestroy( &fs );

    return buffer;
    }    


HBufC8* CIAUpdateXmlParser::ConvertUnicodeToUtf8L( 
    const TDesC16& aUnicodeText )
    {
    const TInt KConvertBufferSize( 64 );

    // Place converted data here, 
    // initial size double the conversion buffer.
    HBufC8* convertedData = HBufC8::NewL( KConvertBufferSize * 2 );
    CleanupStack::PushL( convertedData );
    TPtr8 destination( convertedData->Des() );

    // Create a small output buffer
    TBuf8< KConvertBufferSize > outputBuffer;
    
    // Create a buffer for the unconverted text - initialised with the 
    // input text
    TPtrC16 remainderOfUnicodeText( aUnicodeText );

    for ( ;; ) // conversion loop
        {
        // Start conversion. When the output buffer is full, return the 
        // number of characters that were not converted
        const TInt returnValue(
            CnvUtfConverter::ConvertFromUnicodeToUtf8( 
                outputBuffer, 
                remainderOfUnicodeText ) );

        // check to see that the descriptor isn’t corrupt 
        // - leave if it is
        if ( returnValue == CnvUtfConverter::EErrorIllFormedInput )
            {            
            User::Leave( KErrCorrupt );
            }
        else if ( returnValue < 0 )
            {            
            // future-proof against "TError" expanding
            User::Leave( KErrGeneral );
            }

        // Do something here to store the contents of the output buffer.
        if ( destination.Length() + outputBuffer.Length() >= 
             destination.MaxLength() )
            {
            HBufC8* newBuffer = convertedData->ReAllocL(
                ( destination.MaxLength() + outputBuffer.Length() ) * 2 );
            
            CleanupStack::Pop( convertedData );
            convertedData = newBuffer;
            CleanupStack::PushL( convertedData );
            destination.Set( convertedData->Des() );
            }

        destination.Append( outputBuffer );
        outputBuffer.Zero();

        // Finish conversion if there are no unconverted characters 
        // in the remainder buffer
        if ( returnValue == 0 ) 
            {            
            break; 
            }

        // Remove the converted source text from the remainder buffer.
        // The remainder buffer is then fed back into loop
        remainderOfUnicodeText.Set( 
            remainderOfUnicodeText.Right( returnValue ) );
        }
        
    CleanupStack::Pop( convertedData );
    return convertedData;
    }


void CIAUpdateXmlParser::SetPrivateDriveL( 
    RFs& aFs,
    const TDesC& aFileName ) const
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateXmlParser::SetPrivateDriveL() begin: %S", 
                     &aFileName);
    
    // This will set the correct drive and private path 
    // for the file server session.    

    // First try to find the file from the private directory
    // of the drive where the process exists.
    RProcess process;

    // Set the session private path according to 
    // the process file name drive.
    TInt driveNum( 
        SetSessionPrivatePathL( aFs, process.FileName() ) );

    // Get the session path that was set above.
    IAUPDATE_TRACE_1("[IAUPDATE] Find file: %d", driveNum);
    TFileName sessionPath;
    User::LeaveIfError( aFs.SessionPath( sessionPath ) );
    IAUPDATE_TRACE_1("[IAUPDATE] Session path: %S", &sessionPath);

    // Use the file finder to check if the file actually exists 
    // in the given drive path. If it does not, the file finder 
    // will automatically check from other drives. So, here we 
    // should always find the file if any exists.
    TFindFile finder( aFs );
    User::LeaveIfError( finder.FindByDir( aFileName, sessionPath ) );

    // The drive may have changed if the file was not found from
    // the first suggested drive. So, be sure to have the correct
    // private path.
    driveNum = SetSessionPrivatePathL( aFs, finder.File() );

    // Use the drive info to check if the drive is ROM drive.
    // We prefer non ROM drives. But, accept ROM if nothing else is
    // available.
    IAUPDATE_TRACE_1("[IAUPDATE] Check ROM info: %d", driveNum);
    TDriveInfo info;
    User::LeaveIfError( aFs.Drive( info, driveNum ) );
    TBool isRomDrive( info.iDriveAtt & KDriveAttRom );
    if ( !isRomDrive )
        {
        // The current file is not in ROM drive so use that.
        IAUPDATE_TRACE("[IAUPDATE] First file search done. Non ROM found.");
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateXmlParser::SetPrivateDriveL() end");
        return;
        }

    // Because previous finding was ROM file, try to find a non ROM file.
    IAUPDATE_TRACE("[IAUPDATE] Try to find non ROM file.");
    TInt findErrorCode( finder.Find() );
    if ( findErrorCode == KErrNotFound )
        {
        // Because no new file is found, use the current settings.
        IAUPDATE_TRACE("[IAUPDATE] Second search done. No file found.");
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateXmlParser::SetPrivateDriveL() end");
        return;
        }
    User::LeaveIfError( findErrorCode );

    IAUPDATE_TRACE("[IAUPDATE] New file found. Use that.");
    // Update the session path for the correct file.
    SetSessionPrivatePathL( aFs, finder.File() );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateXmlParser::SetPrivateDriveL() end");
    }


TInt CIAUpdateXmlParser::SetSessionPrivatePathL( 
    RFs& aFs,
    const TDesC& aPath ) const
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateXmlParser::SetSessionPrivateL() begin: %S", 
                     &aPath);
                     
    // Use the parser to get the drive information from the path.
    TParsePtrC parser( aPath );

    if ( !parser.DrivePresent() )
        {
        IAUPDATE_TRACE("[IAUPDATE] ERROR: Missing drive info.");
        User::Leave( KErrArgument );
        }

    // Drive check was passed above.
    // So, drive information is safe to use.
    const TDesC& drive( parser.Drive() );
    const TChar driveChar( drive[ 0 ] );
    TInt driveNum( EDriveA );
    User::LeaveIfError( 
        RFs::CharToDrive( driveChar, driveNum ) );
    IAUPDATE_TRACE_2("[IAUPDATE] Drive: %S, %d", &drive, driveNum );

    // Set the file drive to be file session private path drive.
    User::LeaveIfError( aFs.SetSessionToPrivate( driveNum ) );

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateXmlParser::SetSessionPrivateL() end: %d",
                     driveNum);

    return driveNum;
    }


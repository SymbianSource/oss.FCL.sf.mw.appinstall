/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*     Implementation to read from the config file.
*
*
*/

#include "config.h"

// ---------------------------------------------------------------------------
// WriteIntToConfigFile
//
// Write the given TUint32 value and tag into the given file.
//
// @return TInt normal Symbian error code or KErrNone if all went OK.
// ---------------------------------------------------------------------------
//
TInt WriteIntToConfigFile( RFile& aFile, const TDesC& aTag, const TUint32 aValue )
	{
	const TInt valueLength = 10; //Length of TUint32
	TBufC<valueLength> value;
	TPtr valuePtr = value.Des();

	valuePtr.AppendNum(aValue);
	TInt err = WriteToConfigFile( aFile, aTag, value );

	return err;
	}

// ---------------------------------------------------------------------------
// WriteToConfigFile
//
// Write the given value-tag pair into the given file.
//
// @return TInt normal Symbian error code or KErrNone if all went OK.
// ---------------------------------------------------------------------------
//
TInt WriteToConfigFile( RFile& aFile, const TDesC& aTag, const TDesC& aValue )
	{
	TFileText outTextFile;
	outTextFile.Set(aFile);

	HBufC* content = HBufC::New( aTag.Length() + aValue.Length());
	if(!content)
		{
		return KErrNoMemory;
		}
	CleanupStack::PushL(content);
	TPtr ptr(content->Des());

	ptr.Copy(aTag);
	ptr.Append(aValue);

	TInt err = outTextFile.Write(ptr);
	CleanupStack::PopAndDestroy(content);

	if( err != KErrNone)
		{
		return err;
		}
	return KErrNone;
	}

// ---------------------------------------------------------------------------
// EnsureNewLineAtEnd
//
// Add a newline character as a last character in the file.
//
// @return TInt normal Symbian error code or KErrNone if all went OK.
// ---------------------------------------------------------------------------
//
TInt EnsureNewLineAtEnd(RFile& aFile)
	{
	// Get last 2-bytes before EOF
	// to check for new line character
	TInt pos = -2;
	TInt err = aFile.Seek(ESeekEnd, pos);
	if(err != KErrNone)
		{
		return err;
		}

	TBuf8<2> buf;
	err = aFile.Read(buf);
	if(err != KErrNone)
		{
		return err;
		}

	// Ascii value of new line character is 10
	if(buf[0] != 10 )
		{
		TFileText outTextFile;
		outTextFile.Set(aFile);

		// Write will append newline character
		err = outTextFile.Write(KNullDesC);
		if( err != KErrNone)
			{
			return err;
			}
		}
	return KErrNone;
	}

// ---------------------------------------------------------------------------
// ReadConfigFile
//
// Reads the configuration file to aBuffer, aLineBuffer containing the pointers to lines.
//
// @return TInt normal Symbian error code or KErrNone if all went OK.
// ---------------------------------------------------------------------------
//
TInt ReadConfigFile(RFile& aFile, TDes& aBuffer, RArray< TPtrC >& aLineBuffer )
	{
	// Read text file into buffers
	TInt ret( KErrNone );
	TFileText tf;
	tf.Set( aFile );
	aBuffer.SetMax();
	TPtr ptr( aBuffer.MidTPtr( 0 ) );
	TInt used( 0 );
	do
		{
		ret = tf.Read( ptr );
		TInt len( ptr.Length() );
		if ( ( ret == KErrNone || ret == KErrEof ) && len > 0 )
			{
			// Store non-empty text line
			TInt err( aLineBuffer.Append( ptr ) );
			if ( err == KErrNone )
				{
				ptr.SetMax();
				ptr.Set( ptr.MidTPtr( len ) );
				ptr.Zero();
				used += len;
				}
			else
				{
				ret = err;
				}
			}
		}
	while ( ret == KErrNone );
	if ( ret == KErrEof )
		{
		// reached the end of file without any other error => this is OK
		ret = KErrNone;
		}
	aBuffer.SetLength( used );

	return ret;
	}

// ---------------------------------------------------------------------------
// ReadConfigFile
//
// @return HBufC containg the read config file. aLineBuffer arrays pointing to lines.
// ---------------------------------------------------------------------------
//
HBufC* ReadConfigFile( RFs& aFs, const TDesC& aFullPath, RArray< TPtrC >& aLineBuffer, TInt& aError )
	{
	RFile file;
	aError = file.Open( aFs, aFullPath, EFileRead | EFileStreamText | EFileShareReadersOnly );
	if ( aError != KErrNone )
		{
		return NULL;
		}
	TInt size( 0 );
	aError = file.Size( size );
	if ( aError != KErrNone )
		{
		file.Close();
		return NULL;
		}
	// Get text size, create buffer for text and read text file
	HBufC* ret = HBufC::New( ( size + sizeof( TText ) - 1 ) / sizeof( TText ) );
	if ( !ret )
		{
		aError = KErrNoMemory;
		file.Close();
		return NULL;
		}
	TPtr ptr( ret->Des() );
	aError = ReadConfigFile( file, ptr, aLineBuffer );
	file.Close();
	if ( aError != KErrNone )
		{
		delete ret;
		aLineBuffer.Reset();
		return NULL;
		}
	return ret;
	}

// ---------------------------------------------------------------------------
// GetConfigValue
//
// Gets string specified by tag. For examples:
// Tag1=Value1  Tag1 = aTag, Value1 will be copied to aTag
// aError Normal Symbian error code or KErrNone if all went OK.
// ---------------------------------------------------------------------------
//
TPtrC GetConfigValue( const TDesC& aTag, const RArray< TPtrC >& aLineBuffer, TInt& aError )
	{
	aError = KErrNotFound;
	TPtrC ret( KNullDesC );
	const TInt tagLen( aTag.Length() );
	const TInt count( aLineBuffer.Count() );
	for( TInt i( 0 ); i < count; ++i )
		{
		TPtrC line( aLineBuffer[ i ] );
		if ( !line.Left( tagLen ).CompareF( aTag ) )
			{
			ret.Set( line.Mid( tagLen ) );
			aError = KErrNone;
			break;
			}
		}

	return ret;
	}

TInt CompareVersions(TVersion& version1,TVersion& version2)
	{
	// Compare the versions based on major,minor and build number.
	if((version1.iMajor == version2.iMajor)&&(version1.iMinor == version2.iMinor)&&(version1.iBuild == version2.iBuild))
		{
		return EEqualVersion;
		}
	else if ((version1.iMajor > version2.iMajor) ||
			((version1.iMajor == version2.iMajor)&&(version1.iMinor > version2.iMinor)) ||
			((version1.iMajor == version2.iMajor)&&(version1.iMinor == version2.iMinor)&&(version1.iBuild >= version2.iBuild)))
		{
		return EGreaterFirstVersion;
		}
	else
		{
		return EGreaterSecondVersion;
		}
	}

TBool SetVersion(const TDesC8& aVersionPtr, TVersion& aVer)
	{
	// Function will return EFalse if aVersionPtr is not a valid
	// version string
	TLex8 lex(aVersionPtr);
	TInt count = 0;
	lex.SkipSpace();

	// Get Major Version (max length 3)
	const TInt maxMajorVersionLength = 3;
	lex.Mark();
	while ( (count<maxMajorVersionLength) && (lex.Peek() != '.') )
		{
		lex.Inc();
		++count;
		}

	if (!(lex.Peek() == '.'))
		{
		return EFalse;
		}

	TLex8 lexToken(lex.MarkedToken());
	if ( lexToken.Val(aVer.iMajor) != KErrNone )
		{
		return EFalse;
		}

	// Get Minor Version(max length 2)
	const TInt maxMinorVersionLength = 2;
	count = 0;
	lex.Inc();
	lex.Mark();
	while ( (count<maxMinorVersionLength) && (lex.Peek() != '.') )
		{
		lex.Inc();
		++count;
		}

	lexToken.Assign(lex.MarkedToken());
	if ( lexToken.Val(aVer.iMinor) != KErrNone )
		{
		return EFalse;
		}

	// Check if Build Number exists,
	// otherwise return
	if (!(lex.Peek() == '.'))
		 {
		 lex.Mark();
		 lex.SkipCharacters();
		 if (lex.TokenLength() > 0)
			 {
			 return EFalse;
			 }
		 else
			 {
			 aVer.iBuild = 0;
			 return ETrue;
			 }
		 }

	// Get Build Number(max length 5)
	const TInt maxBuildNoLength = 5;
	lex.Inc();
	lex.Mark();
	lex.SkipCharacters();

	if (lex.TokenLength() > maxBuildNoLength)
		{
		return EFalse;
		}

	lexToken.Assign(lex.MarkedToken());
	if ( lexToken.Val(aVer.iBuild) != KErrNone )
		{
		return EFalse;
		}
	return ETrue;
	}

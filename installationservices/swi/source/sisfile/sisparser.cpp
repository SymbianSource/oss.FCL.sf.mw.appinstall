/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Definition of the Swi::Sis::CParser
*
*/


#include <apadef.h>
#include "sisinstallerrors.h"
#include "sisdataprovider.h"
#include "sisparser.h"
#include "siscontents.h"
#include "swicommon.h"

using namespace Swi;
using namespace Swi::Sis;

EXPORT_C /*static*/ CContents* Parser::ContentsL(MSisDataProvider& aDataProvider)
	{
	TInt32 uid1;
	TInt32 uid2;
	TInt32 uid3;
	TUint32 uidCrc;
	
	// Check Uids and header Crc	
	ReadSymbianHeaderL(aDataProvider, uid1, uid2, uid3, uidCrc);

	// Create CContents (delegate the parsing to CContents)
	TInt64 bytesRead=0;
	CContents* ret = CContents::NewL(aDataProvider, bytesRead);
	return ret;
	}

void Parser::ReadSymbianHeaderL(MSisDataProvider& aDataProvider, TInt32& aUid1, TInt32& aUid2, TInt32& aUid3, TUint32& aUidCrc)
	{
	// Parse the SISX header, that's 4*4 bytes (UID1, UID2, UID3, UID Checksum)
	TPckg<TInt32> uid1pkg(aUid1);
	TInt err = aDataProvider.Read(uid1pkg, sizeof(TInt32));           // UID1
	if ((err != KErrNone) || (uid1pkg.Length() != sizeof(TInt32)))
		{
		User::Leave(KErrSISFieldBufferTooShort);
		}
	
	if (aUid1 != KUidSisxFile.iUid) // The first UID *must* be KUidSisxFile
		{
		
		// We could have errored out here because we
		// have a legacy SIS file, check that.
		// consume UID 2 and 3 to do the check.
		// ignore error here, we'll just leave 
		// with KErrCorrupt.
		
		TPckg<TInt32> uid2pkg(aUid2);
		aDataProvider.Read(uid2pkg, sizeof(TInt32));
		TPckg<TInt32> uid3pkg(aUid3);
		aDataProvider.Read(uid3pkg, sizeof(TInt32));
		
		if (aUid3 == KUidLegacySisFile.iUid)
			{
			User::Leave(KErrLegacySisFile);
			}
		else
			{
			User::Leave(KErrCorrupt);
			}
		}
	  
	TPckg<TInt32> uid2pkg(aUid2);
	err = aDataProvider.Read(uid2pkg, sizeof(TInt32));                // UID2
	if ((err != KErrNone) || (uid2pkg.Length() != sizeof(TInt32)))
		{
		User::Leave(KErrSISFieldBufferTooShort);
		}

	TPckg<TInt32> uid3pkg(aUid3);
	err = aDataProvider.Read(uid3pkg, sizeof(TInt32));                // UID3
	if ((err != KErrNone) || (uid3pkg.Length() != sizeof(TInt32)))
		{
		User::Leave(KErrSISFieldBufferTooShort);
		}


	TPckg<TUint32> uidcrcpkg(aUidCrc);
	err = aDataProvider.Read(uidcrcpkg, sizeof(TInt32));                // Checksum
	if ((err != KErrNone) || (uidcrcpkg.Length() != sizeof(TInt32)))
		{
		User::Leave(KErrSISFieldBufferTooShort);
		}

	User::LeaveIfError(Parser::CheckUidCrc(aUidCrc, uid1pkg, uid2pkg, uid3pkg));
	}


TInt Parser::CheckUidCrc(TUint32 aCrc, const TDesC8& aUid1, const TDesC8& aUid2, const TDesC8& aUid3)
	{
/*	_LIT8(KCrcTest, "123456789");
	TBuf8 <12> buffer(KCrcTest);
	TUint16 tcrc=0;
	Mem::Crc(tcrc, buffer.PtrZ(), 9);
*/	
	TBuf8<12> uidBits;
	uidBits.Append(aUid1);
	uidBits.Append(aUid2);
	uidBits.Append(aUid3);

	TBuf8<7> evenBits;        // 1 more byte for zero terminator
	TBuf8<7> oddBits;		  // 1 more byte for zero terminator
	evenBits.SetLength(6);
	oddBits.SetLength(6);

	for (TInt k = 0; k < 6; k++)
		{
		evenBits[k] = uidBits[k*2]; 		
		oddBits[k] = uidBits[(k*2)+1]; 		
		}

	TUint16 crcHigh(0);
	TUint16 crcLow(0);
	Mem::Crc(crcHigh, oddBits.PtrZ(), 6);
	Mem::Crc(crcLow, evenBits.PtrZ(), 6);
	TUint32 crc = (crcHigh << 16) + crcLow;
	if (aCrc != crc) 
		{
		return KErrCorrupt;
		}
	return KErrNone;
	}


EXPORT_C void Parser::CreateSisStubL(RFile& aFile, MSisDataProvider& aDataProvider)
	{
	TInt32 uid1;
	TInt32 uid2;
	TInt32 uid3;
	TUint32 uidCrc;
	
	// Read the SisX header from the data provider and write it to the file
	ReadSymbianHeaderL(aDataProvider, uid1, uid2, uid3, uidCrc);
	WriteSymbianHeaderL(aFile, uid1, uid2, uid3, uidCrc);
	
	// Write the rest of the stub (SISX format) to the file
	CContents::WriteStubFieldsL(aFile, aDataProvider);
	}
	
void Parser::WriteSymbianHeaderL(RFile& aFile, TInt32& aUid1, TInt32& aUid2, TInt32& aUid3, TUint32& aUidCrc)
	{
	TPckg<TInt32> uid1Pckg(aUid1);
	User::LeaveIfError(aFile.Write(uid1Pckg));
	TPckg<TInt32> uid2Pckg(aUid2);
	User::LeaveIfError(aFile.Write(uid2Pckg));
	TPckg<TInt32> uid3Pckg(aUid3);
	User::LeaveIfError(aFile.Write(uid3Pckg));
	TPckg<TUint32> uidCrcPckg(aUidCrc);
	User::LeaveIfError(aFile.Write(uidCrcPckg));
	}

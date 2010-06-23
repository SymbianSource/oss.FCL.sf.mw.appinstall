/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include "secutils.h"
#include <hash.h> 
#include <e32property.h>
#include <sacls.h>
#include "log.h"

namespace Swi
{

EXPORT_C TBool SecUtils::IsExeL(const TDesC& aFileName)
	{
	TEntry entry;
	EntryL(aFileName, entry);
	return IsExe(entry);
	}
	
EXPORT_C TBool SecUtils::IsDllL(const TDesC& aFileName)
	{
	TEntry entry;
	EntryL(aFileName, entry);
	return IsDll(entry);
	}

void SecUtils::EntryL(const TFileName& aFileName, TEntry& aEntry)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Entry(aFileName, aEntry));
	CleanupStack::PopAndDestroy(&fs);
	}

EXPORT_C HBufC* SecUtils::HexHashL(const TDesC& aName)
	{
	CMessageDigest* digester = CMD5::NewL();
	CleanupStack::PushL(digester);

	TPtrC8 nameHash = digester->Final(TPtrC8(REINTERPRET_CAST(const TUint8*, aName.Ptr()),
						aName.Size()));

	HBufC* hashBuf = HBufC::NewL(nameHash.Length() * 2);
	TPtr hashPtr = hashBuf->Des();
	
	_LIT(KHexDigit, "%02x");
	for (TInt i = 0; i < nameHash.Length(); i++)
		{
		hashPtr.AppendFormat(KHexDigit, nameHash[i]);
		}

	CleanupStack::PopAndDestroy(digester);
	return hashBuf;
	}

EXPORT_C TInt SecUtils::PublishPackageUid(TUid aUid, TUid (&aUidList)[KMaxUidCount])
    {
    TInt i=0;
    TInt count = aUidList[0].iUid;
    
    if (count>=KMaxUidCount-1)
        return KErrOverflow;
    
    aUidList[++count] = aUid;    
    aUidList[0].iUid++;
    
    TInt *tempUidList = (TInt*)aUidList;
    TBuf<KMaxUidCount*sizeof(TUid)+1> buffer;
    TInt *bufPtr = (TInt*)buffer.Ptr();
    buffer.SetLength((count+1)*sizeof(TUid));
    do        
        {
        *bufPtr = *tempUidList;
        }while(++i<=count && bufPtr++ && tempUidList++);

    return(RProperty::Set(KUidSystemCategory, KSWIUidsCurrentlyBeingProcessed, buffer));
    }

EXPORT_C TBool SecUtils::IsPackageUidPresent(TUid aUid, const TUid (&aUidList)[KMaxUidCount])
    {
    TInt count = aUidList[0].iUid;
    for(TInt i=1;i<=count;i++)
        {
        if(aUidList[i]==aUid)
            return ETrue;
        }
    return EFalse;
    }

} // namespace Swi

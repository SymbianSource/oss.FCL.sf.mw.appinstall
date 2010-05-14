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
* CSisRegistryFile class - implementation
*
*/


/**
 @file 
 @released
 @internalComponent 
*/

#include<s32mem.h>
#include<f32file.h>

#include "arrayutils.h"

#include "sisregistryfile.h"
#include "sisregistrycache.h"
#include "sisregistryobject.h"
#include "sisregistryutil.h"

using namespace Swi;

CSisRegistryFile* CSisRegistryFile::NewL(const CSisRegistryPackage& aPackage, CSisRegistryCache& aCache)
	{
	CSisRegistryFile* self = NewLC(aPackage, aCache);
	CleanupStack::Pop(self);
	return self;
	}

CSisRegistryFile* CSisRegistryFile::NewLC(const CSisRegistryPackage& aPackage, CSisRegistryCache& aCache)
	{
	CSisRegistryFile* self = new(ELeave) CSisRegistryFile;
	CleanupStack::PushL(self);
	self->ConstructL(aPackage, aCache);
	return self;
	}


CSisRegistryFile::~CSisRegistryFile()
	{
	delete iObject;
	iFileStream.Close();
	}

void CSisRegistryFile::ReloadL(const TFileName& fileName, CSisRegistryCache& aCache)	
    {
	RFs& hFs = aCache.RFsHandle();
	User::LeaveIfError(iFileStream.Open(hFs, fileName, EFileRead | EFileShareReadersOnly));

	delete iObject;
	iObject = 0;	 
	iObject  = CSisRegistryObject::NewL(iFileStream);
	}

void CSisRegistryFile::ConstructL(const CSisRegistryPackage& aPackage, CSisRegistryCache& aCache)
//
// 2nd phase construct 
//
	{	
   	HBufC* name = SisRegistryUtil::BuildEntryFileNameLC(aPackage.Uid(), aPackage.Index());
	ReloadL(*name, aCache);
	CleanupStack::PopAndDestroy(name);
	
	// before we allocate the subsession ID, ensure that the file we've loaded matches the 
	// package passed to us.
	if (!(aPackage == *iObject))
		{
		User::Leave(KErrNotFound);
		}
	
	iSubsessionId = aCache.AllocateSubsessionId();
	}

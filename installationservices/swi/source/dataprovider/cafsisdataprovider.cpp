/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation of the CCafSisDataProvider
*
*/


#include <caf/caf.h> 
#include <caf/content.h>
#include <caf/data.h>

#include "cafsisdataprovider.h"

using namespace Swi;

EXPORT_C CCafSisDataProvider* CCafSisDataProvider::NewL(const TFileName& aFileName)
	{
	CCafSisDataProvider* self=CCafSisDataProvider::NewLC(aFileName);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCafSisDataProvider* CCafSisDataProvider::NewLC(const TFileName& aFileName)
	{
	CCafSisDataProvider* self=new(ELeave) CCafSisDataProvider();
	CleanupStack::PushL(self);
	self->ConstructL(aFileName);
	return self;	
	}

EXPORT_C CCafSisDataProvider* CCafSisDataProvider::NewL(RFile& aFileHandle)
	{
	CCafSisDataProvider* self=CCafSisDataProvider::NewLC(aFileHandle);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCafSisDataProvider* CCafSisDataProvider::NewLC(RFile& aFileHandle)
	{
	CCafSisDataProvider* self=new(ELeave) CCafSisDataProvider();
	CleanupStack::PushL(self);
	self->ConstructL(aFileHandle);
	return self;	
	}

CCafSisDataProvider::CCafSisDataProvider()
	{
	}

void CCafSisDataProvider::ConstructL(const TFileName& aFileName)
	{
	// Create a CContent object
	// CAF will figure out the appropriate agent
	iContent = ContentAccess::CContent::NewL(aFileName);
	}

void CCafSisDataProvider::ConstructL(RFile& aFileHandle)
	{
	// Create a CContent object
	// CAF will figure out the appropriate agent
	iContent = ContentAccess::CContent::NewL(aFileHandle);
	}

EXPORT_C CCafSisDataProvider::~CCafSisDataProvider()
	{
	delete iData;
	delete iContent;
	}

ContentAccess::CData& CCafSisDataProvider::Data() const
  	{
  	__ASSERT_DEBUG(iData, User::Panic(_L("CCafSisDataProvider::DataL"),0));
  	return *iData;
  	}

// From MSisDataProvider

EXPORT_C TInt CCafSisDataProvider::Read(TDes8& aDes)
	{
	return Data().Read(aDes);
	}

EXPORT_C TInt CCafSisDataProvider::Read(TDes8& aDes, TInt aLength)
	{
	return Data().Read(aDes, aLength);
	}

EXPORT_C TInt CCafSisDataProvider::Seek(TSeek aMode, TInt64& aPos)
	{
	if (aPos < TInt64(KMinTInt32) || TInt64(KMaxTInt32) < aPos)
		{
		return KErrNotSupported;
		}

	TInt pos = I64INT(aPos);
	TInt32 err = Data().Seek(aMode, pos);
	aPos = pos;
	return err;
	}

EXPORT_C void CCafSisDataProvider::OpenDrmContentL(ContentAccess::TIntent aIntent)
	{
	if(iData)
		{
		delete iData;
		iData = NULL;
		}

	// Create a CData object to read the content
	// At this stage we do not know how the content will be used.
	iData = iContent->OpenContentL(aIntent); 
	User::LeaveIfError(Data().EvaluateIntent(aIntent));
	}

EXPORT_C TInt CCafSisDataProvider::ExecuteDrmIntent(ContentAccess::TIntent aIntent)
	{
	return Data().ExecuteIntent(aIntent);
	}

// Methods says whether the content object is protected or not.
EXPORT_C TBool CCafSisDataProvider::IsContentProtected()
	{	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	TInt value = 0;	    
    TInt err = iContent->GetAttribute(ContentAccess::EIsProtected, value);    
    return err == KErrNone && value;
#else
	__ASSERT_ALWAYS(EFalse, User::Invariant());
	return EFalse;
#endif	
	}

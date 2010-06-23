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
* Definition of the Swi::Sis::CCrc
*
*/


#include "siscrc.h"

using namespace Swi::Sis;

/*static*/ CCrc* CCrc::NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CCrc* self = new(ELeave) CCrc();
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

/*static*/ CCrc* CCrc::NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CCrc* self = NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

CCrc::CCrc()
	{
	}

CCrc::~CCrc()
	{
	}

void CCrc::ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	
	TInt64 fieldOffset = aBytesRead;
	
	if (aTypeReadBehaviour == EReadType)
		{
		TFieldType fieldType;
		CField::ReadEnumL<TFieldType,TUint32>(aDataProvider, fieldType, aBytesRead);

		if ((fieldType != EFieldTypeControllerCRC) || (fieldType != EFieldTypeDataCRC))
			{
			User::Leave(KErrSISUnexpectedFieldType);			
			}
		}
	else
		{
		CField::ConstructL(aDataProvider, EFieldTypeControllerCRC, aBytesRead, aTypeReadBehaviour);			
		}

	CField::ReadTTypeL(aDataProvider, iChecksum, aBytesRead);
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	}

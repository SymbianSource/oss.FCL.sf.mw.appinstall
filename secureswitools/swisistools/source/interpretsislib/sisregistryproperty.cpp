/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "sisregistryproperty.h"
#include "deserialiser.h"
#include "serialiser.h"

#include "sisproperty.h"

SisRegistryProperty::SisRegistryProperty(const CSISProperty& aProp) 
: iKey(aProp.Key()), iValue(aProp.Value())
{
}


SisRegistryProperty::~SisRegistryProperty ()
	{
	}

void SisRegistryProperty::Internalize(Deserialiser& des)
	{
	des >> iKey >> iValue;
	}
void SisRegistryProperty::Externalize(Serialiser& ser)
	{
	ser << iKey << iValue;
	}



#ifdef _MSC_VER

Deserialiser& operator>>(Deserialiser& aInput, std::vector<SisRegistryProperty*>& val)
	{
	TUint32 size = 0;
	aInput>> size;
	val.resize(size);
	for (TUint32 i = 0; i < size ; ++i)
		{
		val[i] = new SisRegistryProperty;
		aInput >> *val[i];
		}
	return aInput;
	}
#endif 

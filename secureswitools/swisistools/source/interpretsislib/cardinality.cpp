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


#include <stdexcept>
#include "cardinality.h"
#include "deserialiser.h"
#include "serialiser.h"

const TInt KShiftCardinality8=1;
const TInt KShiftCardinality16=2;
const TInt KShiftCardinality32=3;

const TUint8  KMaxTUint8=0xFF;
const TUint16 KMaxTUint16=0xFFFF;

Cardinality::Cardinality ()
	{
	}

Cardinality::~Cardinality ()
	{
	}

void Cardinality::Internalize(Deserialiser& des)
	{
	TUint8 x;
	des >> x;
	TUint32 n=x;
	if ((n&0x1)==0)
		n>>=KShiftCardinality8;
	else if ((n&0x2)==0)
		{
		des >> x;
		n+=x<<8;
		n>>=KShiftCardinality16;
		}
	else if ((n&0x4)==0)
		{
		des >> x;
		TUint32 t = x << 16;
		des >> x;
		t += x << 8;
		des >> x;
		t += x;
		n+=TUint32(iSize)<<8; // platform dependency
		n>>=KShiftCardinality32;
		}
	else
		{
		throw std::runtime_error("corrupt cardinality");
		}
	iSize=n;
	}
void Cardinality::Externalize(Serialiser& ser)
	{
	TUint32 n=iSize;
	if (n<=(KMaxTUint8>>KShiftCardinality8))
		{
			TUint8 x = n<<KShiftCardinality8;
			ser << x;
		}
	else if (n<=(KMaxTUint16>>KShiftCardinality16))
		{
			TUint16 x = (TUint16)(n<<KShiftCardinality16)+0x1;
			ser << x;
		}
	else
		{
			n = (n<<KShiftCardinality32)+0x3;
			ser << n;
		}

	}


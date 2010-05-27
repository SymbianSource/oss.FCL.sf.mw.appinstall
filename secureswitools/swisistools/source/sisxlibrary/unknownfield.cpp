/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifdef GENERATE_ERRORS

#include "unknownfield.h"

CSISUnknownField::CSISUnknownField()
	: iDigit(rand() & 0xFF)
	{
	InsertMembers();
	}	

CSISUnknownField::CSISUnknownField(const CSISUnknownField& aInitialiser)
	: iDigit (aInitialiser.iDigit)
	{
	InsertMembers();
	}

std::string CSISUnknownField::Name() const
	{
	return "Unknown";
	}



#endif //GENERATE_ERRORS

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


/**
 @file 
 @internalComponent 
*/

#ifdef GENERATE_ERRORS

#ifndef __UNKNOWNFIELD_H__
#define __UNKNOWNFIELD_H__


#include "structure.h"
#include "sisblob.h"
#include "numeric.h"

class CSISUnknownField : public CStructure <CSISFieldRoot::ESISUnknown>
	{
	
public:
	CSISUnknownField();
	CSISUnknownField(const CSISUnknownField& aInitialiser);

	virtual std::string Name () const;

private:
	void InsertMembers();

private:
	CSISUInt8 iDigit;
	
	};

inline void CSISUnknownField::InsertMembers()
	{
	InsertMember(iDigit);
	}



#endif //__UNKNOWNFIELD_H__

#endif //GENERATE_ERRORS

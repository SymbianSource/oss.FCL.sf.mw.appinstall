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
* For validating SIS file format.
* @internalComponent
*
*/


#include "preamble.h"
#include "filecontents.h"


/**
This is a new function added in order to check the type of SIS file,as to whether it
is a normal SIS file or not.
@param FileContent
@return true if the file is normal sis file
*/
bool Preamble::IsPreamble(CFileContents& aFileContents)
	{
	bool sisFile = true;
	int nUIDs = sizeof(iUIDs)/sizeof(iUIDs[0]);

	for (int i=0; i<nUIDs; ++i)
		{
		iUIDs[i] = aFileContents.GetTUint32 ();	
		}

	if ((iUIDs[0] != KUIDSisxFile))
		{
		sisFile = false;
		}

	return sisFile;
	}


bool Preamble::IsUidValid (int aUid)
	{
	
	#ifndef NDEBUG
		std::cout << "Stored UID:" << iUIDs[EPackageUid] << std::endl;
		std::cout << "Input UID:" << aUid << std::endl;
	#endif
	
	return iUIDs[EPackageUid] == aUid;
	}

TUint32	Preamble::iUIDs[EUidCount] = {0,0,0,0};

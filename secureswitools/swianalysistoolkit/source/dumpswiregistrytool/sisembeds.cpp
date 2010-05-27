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


#include "sisembeds.h"

SISEmbeds::SISEmbeds()
	:iEmbeddedPkgUid (0) ,
	iEmbeddedPkgName (0) ,
	iEmbeddedVendorName (0) ,
	iEmbeddedPkgIndex (0)
	{
	}

void SISEmbeds::ExtractEmbeds(StreamReader& aReader )
	{
	iEmbeddedPkgUid = aReader.ReadInt32(); 
	iEmbeddedPkgName = aReader.ReadDescriptor();
	iEmbeddedVendorName = aReader.ReadDescriptor();
	iEmbeddedPkgIndex = aReader.ReadInt32();       
	}

void SISEmbeds::DisplayEmbeddedInfo()
	{
	cout << "Embedded Package UID : " << std::hex << "0x" << iEmbeddedPkgUid << endl;
	cout << "Embedded Package Name : " << iEmbeddedPkgName << endl;
	cout << "Embedded Package Vendor Name :" << iEmbeddedVendorName << endl;
	cout << "Embedded Package Index : " << std::dec << iEmbeddedPkgIndex << endl << endl;
	}

SISEmbeds::~SISEmbeds()
	{
	delete [] iEmbeddedPkgName;
	delete [] iEmbeddedVendorName;
	}



		
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


#include "registrypackage.h"

RegistryPackage::RegistryPackage()
	:iUid (0) ,
	iPkgName (0) ,
	iVendorName (0) ,
	iPkgIndex (0)
	{
	}

void RegistryPackage::ExtractPackage(StreamReader& aReader)  
	{
	iUid = aReader.ReadInt32(); 
	iPkgName = aReader.ReadDescriptor();
	iVendorName =  aReader.ReadDescriptor();
	iPkgIndex = aReader.ReadInt32();
	}

void RegistryPackage::DisplayPackage()
	{	
	cout << "Package UID :" << std::hex << "0x" << iUid << endl;
	cout << "Package Name :" << iPkgName << endl;
	cout << "Vendor :" << iVendorName << endl;
	cout << "Package Index :" << iPkgIndex << endl;
	}

RegistryPackage::~RegistryPackage()
	{
	delete [] iPkgName;
	delete [] iVendorName;
	}

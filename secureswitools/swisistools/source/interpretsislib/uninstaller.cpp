/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#pragma warning (disable: 4786)

#include <iostream>
#include <sstream>


#include "uninstaller.h"
#include "errors.h"


Uninstaller::Uninstaller( SisRegistry& aReg, const CParameterList& aParamList )
:  iRegistry(aReg), iParamList(aParamList)
{
}


void Uninstaller::Uninstall()
{
	const CParameterList::OptionList& uids = iParamList.PkgUidsToRemove();

	// Get the package UID
	for (CParameterList::OptionList::const_iterator curr = uids.begin(); curr != uids.end(); ++curr)
	{
		TUint32 uid;

	    bool convertedOk = false;
		
		std::wstring temp= *curr;
		std::wistringstream stringStream( temp );

		// Check for hex prefix
		if ( (temp.substr( 0, 2 ) == (L"0x") || temp.substr( 0, 2 ) == (L"0X")) && 
				curr->length() > 2 )
		{
			// The UID is in Hex, skip "0x" prefix
			stringStream.ignore( 2 );
			stringStream >> std::hex >> uid;
			convertedOk = ( !stringStream.fail() );
		}
		else
		{
			// The UID is in Decimal
			stringStream >> uid;
			convertedOk = ( !stringStream.fail() );
		}
		
		if (convertedOk)
		{
			RemovePkg(uid);
		}
		else
		{
			std::wostringstream os;

			os << L" Invalid Package UID " << std::hex << uid;
			std::wstring msg = os.str();
			throw InterpretSisError( msg, CMDLINE_ERROR );
		}
	}
	
	// Regenerate SIS stub registries
	iRegistry.GenerateStubRegistry();
}


void Uninstaller::RemovePkg(const TUint32 aUid)
{
	if (iRegistry.IsInstalled(aUid))
	{
		LINFO(L"Removing UID 0x" << std::hex << aUid);
		iRegistry.RemovePkg(aUid);
	}
	else
	{
		// No SisRegistry file found!!
		std::wostringstream os;

		os << L" SIS Registry entry for 0x" << std::hex << aUid << L" Not found";
		std::wstring msg = os.str();
		throw InterpretSisError( msg, PACKAGE_NOT_FOUND );
	}
}

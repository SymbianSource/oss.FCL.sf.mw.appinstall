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
* Note: This file may contain code to generate corrupt files for test purposes.
* Such code is excluded from production builds by use of compiler defines;
* it is recommended that such code should be removed if this code is ever published publicly.
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#include "exception.h"

// NOTE:
// If makesis really does suffer from out of memory problems, this code will need to be
// made more sophisticated


const wchar_t* CSISException::widewhat() const throw()
	{
	static std::wstring description;
	switch (iCategory)
		{
	case EFileFormat:
		description = L"SISfile error";
		break;
	case EVerification:
		description = L"verification failure";
		break;
	case EMemory:
		description = L"memory error";
		break;
	case EFileProblem:
		description = L"file I/O fault";
		break;
	case ESyntax:
		description = L"syntax error";
		break;
	case ECompress:
		description = L"compression fault";
		break;
	case ELanguage:
		description = L"bad language count";
		break;
	case EUID:
		description = L"different UID";
		break;
	case EInvalidDestination:
		description = L"invalid destination path or syntax";
		break;
	case EInvalidInstallOption:
		description = L"Invalid Install Option";
		break;
	case ENotSigned :
		description = L"not signed";
		break;
	case EDirIsFile:
		description = L"directory specified is a file";
		break;
	case EPermissionDenied:
		description = L"permission was denied when using directory/file";
		break;
	case ENotSupported:
		description = L"operation not supported";
		break;
	case ECrypto:
		description = L"encryption error";
		break;
		}
	
	description += L", ";
	description += iDescription;
	return (description.c_str ()); 
	}


const char* CSISException::what() const throw()
	{
	return wstring2string (widewhat ()).c_str ();
	}

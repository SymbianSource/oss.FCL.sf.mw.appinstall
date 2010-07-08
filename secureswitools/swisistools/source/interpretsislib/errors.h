/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @file ERRORS.H
 @publishedPartner
 @released
*/
#ifndef	ERRORS_H
#define	ERRORS_H

#pragma warning (disable: 4786)

#include <stdexcept>
// System includes
#include <string>

// User includes
#include "is_utils.h"



enum ErrorCodes
    {
	SUCCESS = 0,
	MISSING_DEPENDENCY = -1,
	SIS_NOT_SUPPORTED = -2,
	ATTEMPT_TO_UPGRADE_ROM_PKG = -3,
	DATA_CAGE_VIOLATION = -4,
	ECLIPSING_VIOLATION = -5,
	FILE_ERROR = -6,
	CMDLINE_ERROR = -7,
	OVERWRITE_ORPHAN = -8,
	DUPLICATE_SID = -9,
	INVALID_SIS = -10,
	STD_EXCEPTION = -11,
	UNKNOWN_EXCEPTION = -12,
    CONFIG_ERROR = -13,
    ROM_MANAGER = -14,
    DIRECTORY_CREATION_ERROR = -15,
	PA_NO_TARGET_FILE = -16,
	ATTEMPT_TO_UNINSTALL_ROM_PKG = -17,
	INVALID_UPGRADE = -18,
	INVALID_REGISTRY_VERSION = -19,
	SISREGISTRY_ERROR = -20,
	MISSING_BASE_PACKAGE = -21,
	PACKAGE_NOT_FOUND = -22,
	ENV_NOT_SPECIFIED = -23,
	DB_EXCEPTION = -24,
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	RSC_PARSING_ERROR = -25,
#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	

	// errors related to database registration
	GLOBALID_CREATION_ERROR = -40,
	DATABASE_ENTRY_MISSING = -41,
	DATABASE_UPDATE_FAILED = -42,
	LIBRARYLOAD_ERROR = -43
    };


class InterpretSisError : public std::runtime_error
    {
public:
	inline InterpretSisError()
        : std::runtime_error( "InterpretSis Error" ), iCode( SUCCESS )
        {
        }

    inline InterpretSisError( const std::string& aError, ErrorCodes aCode )
        : std::runtime_error( aError ), iCode( aCode )
        {
        }

	inline InterpretSisError( const std::wstring& aError, ErrorCodes aCode )
        : std::runtime_error( wstring2string(aError) ), iCode( aCode )
        {
        }

public:
	ErrorCodes GetErrorCode() const { return iCode; }

private:
	ErrorCodes iCode;
    };

/** 
 * @internalComponent
 * @released
 */
class InvalidSis : public InterpretSisError
    {
public:
	inline InvalidSis( const std::string& aPackage, const std::string& aError, ErrorCodes aErr )
        : InterpretSisError(aPackage + " - " + aError, aErr) 
        {
        }
    };


#endif	/* ERRORS_H */

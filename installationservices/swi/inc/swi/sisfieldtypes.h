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
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef	__SISFIELDTYPES_H__
#define	__SISFIELDTYPES_H__

// NOTE: This file is common to PC side (MakeSIS etc.) and device side (SW Install) code
#include "installtypes.h"

namespace Swi
{
namespace Sis
{
/**
The size of Length fields used in Type/Length/Value triples in SIS files
@internalTechnology
*/
typedef	TInt64	TFieldLength;

/**
Defines values of Type fields used in Type/Length/Value triples in SIS files
@internalTechnology
*/
enum TFieldType
	{
	EFieldTypeInvalid					=  0,
	EFieldTypeString					=  1,
	EFieldTypeArray						=  2,
	EFieldTypeCompressed				=  3,
	EFieldTypeVersion					=  4,
	EFieldTypeVersionRange				=  5,
	EFieldTypeDate						=  6,
	EFieldTypeTime						=  7,
	EFieldTypeDateTime					=  8,
	EFieldTypeUID						=  9,
	EFieldTypeUnused1					= 10,
	EFieldTypeLanguage					= 11,
	EFieldTypeContents					= 12,
	EFieldTypeController				= 13,
	EFieldTypeInfo						= 14,
	EFieldTypeSupportedLanguages		= 15,
	EFieldTypeSupportedOptions			= 16,
	EFieldTypePrerequisites				= 17,
	EFieldTypeDependency				= 18,
	EFieldTypeProperties				= 19,
	EFieldTypeProperty					= 20,
	EFieldTypeSignatures				= 21,
	EFieldTypeCertificateChain			= 22,
	EFieldTypeLogo						= 23,
	EFieldTypeFileDescription			= 24,
	EFieldTypeHash						= 25,
	EFieldTypeIf						= 26,
	EFieldTypeElseIf					= 27,
	EFieldTypeInstallBlock				= 28,
	EFieldTypeExpression				= 29,
	EFieldTypeData						= 30,
	EFieldTypeDataUnit					= 31,
	EFieldTypeFileData					= 32,
	EFieldTypeSupportedOption			= 33,
	EFieldTypeControllerCRC				= 34,
	EFieldTypeDataCRC					= 35,
	EFieldTypeSignature					= 36,
	EFieldTypeBlob						= 37,
	EFieldTypeSignatureAlgorithm		= 38,
	EFieldTypeSignatureCertificateChain	= 39,
	EFieldTypeDataIndex					= 40,
	EFieldTypeCapabilities				= 41,
	};

/**
The type of compression within an EFieldTypeCompressed entity
@internalTechnology
*/
enum TCompressionAlgorithm
	{
	ECompressNone = 0,		//The data is uncompressed
	ECompressDeflate,	 	//The data is compressed according to RFC 1951
	};

/**
The operators allowed in conditional blocks
@internalTechnology
*/
enum TOperator
	{
	// Binary Operators
	EBinOpEqual = 1,		// equal to
	EBinOpNotEqual,			// not equal to
	EBinOpGreaterThan,		// greater than
	EBinOpLessThan,			// less than
	EBinOpGreaterOrEqual,	// greater than or equal to
	EBinOpLessOrEqual,		// less than or equal to
		
	// Logical Operators
	ELogOpAnd,				// logical AND
	ELogOpOr,				// logical OR

	// Unary Operators
	EUnaryOpNot,			// NOT() - logical NOT
		
	// Functions
	EFuncExists,			// EXISTS() - Checks if the file exists
	EFuncAppProperties,		// APPPROP() - Queries application properties
	EFuncDevProperties,

	// Primitives
	EPrimTypeString,		// This expression holds a string value
	EPrimTypeOption,		// This expression is an option, identified by string
	EPrimTypeVariable,		// This expression is a variable, identified by string
	EPrimTypeNumber,		// This expression holds a number value
	};

/**
The operations that may be performed on a single file at installation
@internalTechnology
*/
enum TSISFileOperation
	{
	EOpInstall	= 1,		// Install File
	EOpRun		= 2,		// Run File
	EOpText		= 4,		// Display File as Text
	EOpNull		= 8,		// File is to be removed at uninstallation time
	};

/**
The options associated with the preceding operations
@internalTechnology
*/
enum TSISFileOperationOptions
	{
	// EOpRun options
	EInstFileRunOptionInstall			= 1<<1,		// Run at installation
	EInstFileRunOptionUninstall			= 1<<2,		// Run at uninstallation
	EInstFileRunOptionByMimeType		= 1<<3,		// Run using MIME type
	EInstFileRunOptionWaitEnd			= 1<<4,		// Wait for end before continuing
	EInstFileRunOptionSendEnd			= 1<<5,		// Terminate after (un)install ends
	EInstFileRunOptionBeforeShutdown	= 1<<6,		// Run at uninstallation , before any running applications are shut down.
	EInstFileRunOptionAfterInstall		= 1<<7,		// Run after install
	// EOpText options
	EInstFileTextOptionContinue			= 1<<9,		// Continue button
	EInstFileTextOptionSkipIfNo			= 1<<10,	// Yes/No - skip next file if user selects no	
	EInstFileTextOptionAbortIfNo		= 1<<11,	// Yes/No - abort       install if user selects no
	EInstFileTextOptionExitIfNo			= 1<<12,	// Yes/No - uninstall if user selects no
	EInstFileTextOptionForceAbort		= 1<<13,	// Continue button .Installation aborts. 
	// EOpInstall
	EInstVerifyOnRestore				= 1<<15,	// Verify on Restore
	};

/**
The types of hashing algorithms that are accepted
@internalTechnology
*/
enum TSISHashAlgorithm
	{
	EHashAlgSHA1 	= 1,		// SHA-1 hash algorithm
	};

} // namespace Sis
} // namespace Swi

#endif	/* __SISFIELDTYPES_H__ */

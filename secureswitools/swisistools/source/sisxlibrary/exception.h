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
 @publishedPartner
 @released
*/

#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include <exception>

#include "utility.h"


class CSISException : public std::exception 
	{
public:
	/**
	 * Enumeration which represents errors while handling a sis file.
	 */
	typedef enum 
		{
		ENone,
		EFileFormat,
		EVerification,
		EMemory,
		EFileProblem,
		ESyntax,
		ECompress,
		ECrypto,
		ELanguage,
		EUID,
		ENotSigned,
		EIllegal,
		EInvalidDestination,
		EInvalidInstallOption,
		EDirIsFile,
		EPermissionDenied,
		ELegacyFormat,
		ENotSupported
		} TCategory;

public:
	/**
	 * Constructor
	 * @param aCategory type of exception
	 * @param aContext context for the exception
	 */
	explicit CSISException (const TCategory aCategory, const std::string& aContext);
	/**
	 * Constructor
	 * @param aCategory type of exception
	 * @param aContext context for the exception
	 */
	explicit CSISException (const TCategory aCategory, const std::wstring& aContext);
	/**
	 * Cleanup owned resources
	 */
	virtual ~CSISException() throw ();
	/**
	 * Gets the exception detail or message.
	 */
	virtual const char *what () const throw() ;
	/**
	 * Gets the exception detail or message.
	 */
	virtual const wchar_t *widewhat () const throw();
	/**
	 * Gets the exception category
	 */
	TCategory ErrorCategory() const;

	/**
	 * Throws an exception if the condition is true.
	 * @param aCondidtion Throws an exception if the condition is true.
	 * @param aCategory type of exception
	 * @param aContext context for the exception
	 */
	static void ThrowIf (const bool aCondition, const TCategory aCategory, const std::string& aContext);
	/**
	 * Throws an exception if the condition is true.
	 * @param aCondidtion Throws an exception if the condition is true.
	 * @param aCategory type of exception
	 * @param aContext context for the exception
	 */
	static void ThrowIf (const bool aCondition, const TCategory aCategory, const std::wstring& aContext);


private:
	std::wstring iDescription;
	TCategory iCategory;
	};


inline CSISException::CSISException (const TCategory aCategory, const std::string& aContext) :
		std::exception (),
		iDescription (string2wstring (aContext)),
		iCategory (aCategory)
	{
	}
	

inline CSISException::CSISException (const TCategory aCategory, const std::wstring& aContext) :
		std::exception (),
		iDescription (aContext),
		iCategory (aCategory)
	{
	}
	

inline CSISException::~CSISException () throw()
	{
	}


inline void CSISException::ThrowIf (const bool aCondition, const TCategory aCategory, const std::string& aContext)
	{
	if (aCondition)
		{
		throw CSISException (aCategory, string2wstring (aContext));
		}
	}

inline void CSISException::ThrowIf (const bool aCondition, const TCategory aCategory, const std::wstring& aContext)
	{
	if (aCondition)
		{
		throw CSISException (aCategory, aContext);
		}
	}

inline CSISException::TCategory CSISException::ErrorCategory() const
	{
	return iCategory;
	}


#endif // __EXCEPTION_H__


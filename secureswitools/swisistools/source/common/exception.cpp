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
* CException - Generic exception handler for db manager.
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#include "exception.h"

std::string CException::EmptyMessage = "No Message available";

/**
	Constructs the exception handler object through setting the message.
	This is to be used when only message is available for the exception 
	to be raised.

	@param	aMessage exception details in readable text format.	
 */
DllExport CException::CException(const std::string& aMessage)
	: iMessage(aMessage),
	iCode(ExceptionCodes::EUnintializedCode)
	{}

/**
	Constructs the exception handler object through setting the error code.
	This is to be used when only an error code is available for the exception 
	to be raised.

	@param aCode error code of the exception.
 */
DllExport CException::CException(int aCode)
	: iMessage(CException::EmptyMessage) ,
	iCode(aCode)
	{}

/**
	Constructs the exception handler object through setting the message and 
	error code.	This is to be used when both the message and the error code  
	is available for the exception to be raised.

	@param	aMessage exception details in readable text format.	
	@param aCode error code of the exception.
 */
DllExport CException::CException(const std::string& aMessage, int aCode)
	: iMessage(aMessage),
	iCode(aCode)
	{}

/**
	Constructs the exception handler object through setting the message and 
	error code.	This is to be used when both the message and the error code  
	is available for the exception to be raised.

	@param	aMessage exception details in readable text format.	
	@param aCode error code of the exception.
 */
DllExport CException::CException(const char* aMessage, int aCode)
	: iMessage(aMessage),
	iCode(aCode)
	{}

/**
	Retrieves the message details of an exception handler.
 */
DllExport const std::string& CException::GetMessageA() const
	{
	return iMessage;
	}

/**
	Retrieves the error code of an exception handler.
 */
DllExport int CException::GetCode() const 
	{
	return iCode;
	}
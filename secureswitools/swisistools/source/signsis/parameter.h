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
* processes command line parameters
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __PARAMETER_H__
#define __PARAMETER_H__

#ifndef __TOOLS2_LINUX__
#include <tchar.h>
#endif // __TOOLS2_LINUX__

#include <string>
#include "sissignaturealgorithm.h"
#include "utility_interface.h"

class CParameter 
	{
public:
	CParameter ();

	bool CommandLine (int argc, _TCHAR* argv[]);
	bool Sign () const;
	bool Unsign () const;
	bool Verbose () const;
	bool Report () const;
	bool Dump () const;
	const std::wstring& Sis () const;
	const std::wstring& Certificate () const;
	const std::wstring& Key () const;
	const std::wstring& PassPhrase () const;
	const std::wstring& Output () const;
	CSISSignatureAlgorithm::TAlgorithm Algorithm () const;

	/**
	Return the certificate extarct flag to which is set to true when -p is specified 
	as command line option
	*/
	static bool ExtractCert(){return  iExtractCert;}

private:

	void ShowBanner ();
	std::string GetVersionInfo (char *aRequest);

private:
	bool								iSign;
	bool								iUnsign;
	bool								iReport;
	bool								iVerbose;
	bool								iDump;
	std::wstring						iSISFileName;
	std::wstring						iCertificate;
	std::wstring						iKey;
	std::wstring						iPassPhrase;
	std::wstring						iOutput;
	CSISSignatureAlgorithm::TAlgorithm	iAlgorithm;
	static bool							iExtractCert;
	};

inline bool CParameter::Sign () const
	{
	return iSign;
	}

inline bool CParameter::Unsign () const
	{
	return iUnsign;
	}

inline bool CParameter::Verbose () const
	{
	return iVerbose;
	}

inline bool CParameter::Report () const
	{
	return iReport;
	}

inline const std::wstring& CParameter::Sis () const
	{
	return iSISFileName;
	}

inline const std::wstring& CParameter::Certificate () const
	{
	return iCertificate;
	}

inline const std::wstring& CParameter::Key () const
	{
	return iKey;
	}

inline const std::wstring& CParameter::PassPhrase () const
	{
	return iPassPhrase;
	}

inline const std::wstring& CParameter::Output () const
	{
	return iOutput;
	}

inline CSISSignatureAlgorithm::TAlgorithm CParameter::Algorithm () const
	{
	return iAlgorithm;
	}

inline bool CParameter::Dump () const
	{
	return iDump;
	}

#endif // __PARAMETER_H__

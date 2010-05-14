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
* As specified in SGL.GT0188.251
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __SISEXPRESSION_H__
#define __SISEXPRESSION_H__


#include "structure.h"
#include "sisstring.h"
#include "sequence.h"
#include "numeric.h"
#include "sislanguage.h"

const std::wstring KFuncVersionPrefix = L"\\sys\\install\\pkgversion\\?";
const std::wstring KFuncSupportedLanguagePrefix = L"\\sys\\install\\supportedlanguage\\?";

class CSISExpression : public CStructure <CSISFieldRoot::ESISExpression>

	{
public:
	typedef enum {
		EOpNone = 0,
		EBinOpEqual,
		EBinOpNotEqual,
		EBinOpGreaterThan,
		EBinOpLessThan,
		EBinOpGreaterThanOrEqual,
		EBinOpLessThanOrEqual,
		ELogOpAnd,
		ELogOpOr,
		EUnaryOpNot,
		EFuncExists,
		EFuncAppProperties,
		EFuncDevProperties,
		EPrimTypeString,
		EPrimTypeOption,
		EPrimTypeVariable,
		EPrimTypeNumber,
		EOpUnknown
	} TOperator;

	void InsertMembers ();

private:
	void AddLeaf ();
	void AddLeaf (const CSISExpression& aExpression);

public:
	CSISExpression ();
	CSISExpression (const bool aRequired);
	CSISExpression (const TOperator aOperator);
	CSISExpression (const CSISExpression& aInitialiser);

	virtual void Verify (const TUint32 aLanguages) const;
	virtual std::string Name () const;
#ifdef GENERATE_ERRORS
	virtual void CreateDefects ();
#endif // GENERATE_ERRORS

	CSISExpression& operator = (const CSISExpression& aExpression);

	const CSISExpression& 	LHS () const;
	CSISExpression& 		LHS ();
	const CSISExpression& 	RHS () const;
	CSISExpression& 		RHS ();
	
	void SetValue (const CSISString& aValue);
	void SetValue (const TUint32 aValue);
	void SetAlwaysTrue ();
	void SetOperator (const TOperator aOperator);
	void SetOperator (const TOperator aOperator, const CSISExpression& aLHS);
	void SetLanguage ();
	void SetNumeric (const TInt32 aValue);
	void SetLanguageComparision (const TInt32 aValue);
	void SetVariable (const std::wstring& aIdentifier);
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;
	inline CSISUInt32 Operator() const;
	inline const CSISString& String() const;
	inline TInt IntValue() const;
		
	void 		WriteVersionCondition (std::wostream& aStream, bool aVerbose) const;
	void 		WriteSupportedLanguageCondition (std::wostream& aStream, bool aVerbose) const;
	static bool	ExtractNextToken(std::wstring& aParseString, std::wstring& aTokenString);
	static bool	IsHexadecimal(const std::wstring& aString);
	static bool	IsHexadecimal(const std::wstring& aString, TUint32& aHexValue);
	static bool	IsDecimal(const std::wstring& aString, TInt& aDecimalValue);

private:
	CSISUInt32													iOperator;
	CSISInt32													iInteger;
	CSISString													iString;
	CSequence <CSISExpression, CSISFieldRoot::ESISExpression>	iLeaf;
	};



inline CSISExpression::CSISExpression () :
		iString (false)
	{ 
	InsertMembers (); 
	}


inline CSISExpression::CSISExpression (const bool aRequired) :
		CStructure <CSISFieldRoot::ESISExpression> (aRequired),
		iString (false)
	{ 
	InsertMembers (); 
	}


inline CSISExpression::CSISExpression (const TOperator aOperator) :
		iOperator (aOperator),
		iString (false)
	{ 
	InsertMembers (); 
	}


inline void CSISExpression::AddLeaf ()
	{ 
	assert (iLeaf.size () < 2); 
	iLeaf.Push (CSISExpression ()); 
	}


inline void CSISExpression::AddLeaf (const CSISExpression& aExpression)
	{ 
	assert (iLeaf.size () < 2); 
	iLeaf.Push (aExpression); 
	}


inline const CSISExpression& CSISExpression::LHS () const
	{ 
	assert (iLeaf.size () > 0); 
	return iLeaf [0]; 
	}


inline CSISExpression& CSISExpression::LHS ()
	{ 
	assert (iLeaf.size () > 0); 
	return iLeaf [0]; 
	}


inline const CSISExpression& CSISExpression::RHS () const
	{ 
	return iLeaf.Last (); 
	}


inline CSISExpression& CSISExpression::RHS ()
	{ 
	return iLeaf.Last ();
	}


inline void CSISExpression::SetValue (const CSISString& aValue)
	{ 
	iString = aValue; 
	}


inline void CSISExpression::SetValue (const TUint32 aValue)
	{ 
	iInteger = aValue; 
	}


inline void CSISExpression::SetAlwaysTrue ()
	{ 
	SetOperator (EUnaryOpNot); 
	RHS ().SetOperator (EPrimTypeNumber); 
	RHS ().SetValue (0); 
	}

inline void CSISExpression::SetOperator (const TOperator aOperator)
	{ 
	SetOperator (aOperator, CSISExpression ()); 
	}

#ifdef GENERATE_ERRORS
inline void CSISExpression::CreateDefects ()
	{
	if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugInvalidValues))
		{
		iOperator = rand ();
		}
	}
#endif // GENERATE_ERRORS

inline void CSISExpression::SetNumeric (const TInt32 aValue)
	{
	iOperator =	EPrimTypeNumber;
	iInteger = aValue;	
	}

inline std::string CSISExpression::Name () const
	{
	return "Expression";
	}

inline CSISUInt32 CSISExpression::Operator() const
	{
	return iOperator;
	}

inline const CSISString& CSISExpression::String() const
	{
	return iString;
	}


inline TInt CSISExpression::IntValue() const
	{
	return iInteger;
	}

#endif // __SISEXPRESSION_H__


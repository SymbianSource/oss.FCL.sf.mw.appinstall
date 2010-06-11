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
* TExpressionResult
*
*/


inline TExpressionResult::TType TExpressionResult::Type() const
	{
	return iType;	
	}

inline TExpressionResult operator!=(const TExpressionResult& aLhs, const TExpressionResult& aRhs)
	{
	if (aLhs.Type()!=aRhs.Type())
		{
		User::Leave(KErrInvalidExpression);
		}
	if (aLhs.Type()==TExpressionResult::EInt)
		{
		return aLhs.IntegerValueL()!=aRhs.IntegerValueL();
		}
	else
		{
		return aLhs.StringValueL()!=aRhs.StringValueL();
		}
	}

inline TExpressionResult operator==(const TExpressionResult& aLhs, const TExpressionResult& aRhs)
	{
	if (aLhs.Type()!=aRhs.Type())
		{
		User::Leave(KErrInvalidExpression);
		}
	if (aLhs.Type()==TExpressionResult::EInt)
		{
		return aLhs.IntegerValueL()==aRhs.IntegerValueL();
		}
	else
		{
		return aLhs.StringValueL()==aRhs.StringValueL();
		}
	}

inline TExpressionResult operator<(const TExpressionResult& aLhs, const TExpressionResult& aRhs)
	{
	if (aLhs.Type()==TExpressionResult::EInt && aRhs.Type()==TExpressionResult::EInt)
		{
		return aLhs.IntegerValueL() < aRhs.IntegerValueL();
		}
	User::Leave(KErrInvalidExpression);
	return TExpressionResult(0); // Never Reached
	}

inline TExpressionResult operator>(const TExpressionResult& aLhs, const TExpressionResult& aRhs)
	{
	if (aLhs.Type()==TExpressionResult::EInt && aRhs.Type()==TExpressionResult::EInt)
		{
		return aLhs.IntegerValueL() > aRhs.IntegerValueL();
		}
	User::Leave(KErrInvalidExpression);
	return TExpressionResult(0); // Never Reached
	}

inline TExpressionResult operator<=(const TExpressionResult& aLhs, const TExpressionResult& aRhs)
	{
	if (aLhs.Type()==TExpressionResult::EInt && aRhs.Type()==TExpressionResult::EInt)
		{
		return aLhs.IntegerValueL() <= aRhs.IntegerValueL();
		}
	User::Leave(KErrInvalidExpression);
	return TExpressionResult(0); // Never Reached
	}

inline TExpressionResult operator>=(const TExpressionResult& aLhs, const TExpressionResult& aRhs)
	{
	if (aLhs.Type()==TExpressionResult::EInt && aRhs.Type()==TExpressionResult::EInt)
		{
		return aLhs.IntegerValueL() >= aRhs.IntegerValueL();
		}
	User::Leave(KErrInvalidExpression);
	return TExpressionResult(0); // Never Reached
	}

inline TExpressionResult operator!(const TExpressionResult& aExpression)
	{
	if (aExpression.Type()!=TExpressionResult::EInt)
		{
		User::Leave(KErrInvalidExpression);
		}
	return TExpressionResult(!aExpression.IntegerValueL()); // Will Never Leave
	}

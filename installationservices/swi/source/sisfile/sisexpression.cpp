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
* Definition of the Swi::Sis::CExpression
*
*/


/**
 @file sisexpression.cpp
*/

#include "sisexpression.h"
#include "sisstring.h"

using namespace Swi::Sis;

EXPORT_C /*static*/ CExpression* CExpression::NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CExpression* self = new(ELeave) CExpression();
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C /*static*/ CExpression* CExpression::NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CExpression* self = NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CExpression* CExpression::NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CExpression* self = new (ELeave) CExpression;
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C CExpression* CExpression::NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CExpression* self = CExpression::NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

CExpression::CExpression()
	{
	}

EXPORT_C CExpression::~CExpression()
	{
	delete iLeftExpression;
	delete iRightExpression;
	delete iString;
	}

void CExpression::ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeExpression, aBytesRead, aTypeReadBehaviour);

	CField::ReadEnumL<TOperator,TUint32>(aDataProvider, iOperator, aBytesRead);

	CField::ReadTTypeL(aDataProvider, iIntegerValue, aBytesRead);
	
	switch (iOperator)
		{
	case EBinOpEqual:
	case EBinOpNotEqual:
	case EBinOpGreaterThan:
	case EBinOpLessThan:
	case EBinOpGreaterOrEqual:
	case EBinOpLessOrEqual:
	case ELogOpAnd:
	case ELogOpOr:
	case EFuncAppProperties:
		iLeftExpression=CExpression::NewL(aDataProvider, aBytesRead);
		iRightExpression=CExpression::NewL(aDataProvider, aBytesRead);
		break;
		
	case EUnaryOpNot:
	case EFuncDevProperties:
		iLeftExpression=CExpression::NewL(aDataProvider, aBytesRead);
		break;

	case EFuncExists:
	case EPrimTypeString:
		iString=CString::NewL(aDataProvider, aBytesRead);
		break;
		
	case EPrimTypeOption:	// All these are stored using iIntegerValue only
	case EPrimTypeVariable:
	case EPrimTypeNumber:
		break;
		
	default:
		User::Leave(KErrSISExpressionUnknownOperator);
		}

	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);	
	}
	
void CExpression::ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeExpression, aBytesRead, aTypeReadBehaviour);

	CField::ReadEnumL<TOperator,TUint32>(aDataProvider, iOperator, aBytesRead);

	CField::ReadTTypeL(aDataProvider, iIntegerValue, aBytesRead);
	
	switch (iOperator)
		{
	case EBinOpEqual:
	case EBinOpNotEqual:
	case EBinOpGreaterThan:
	case EBinOpLessThan:
	case EBinOpGreaterOrEqual:
	case EBinOpLessOrEqual:
	case ELogOpAnd:
	case ELogOpOr:
	case EFuncAppProperties:
		iLeftExpression=CExpression::NewL(aDataProvider, aBytesRead);
		iRightExpression=CExpression::NewL(aDataProvider, aBytesRead);
		break;
		
	case EUnaryOpNot:
	case EFuncDevProperties:
		iLeftExpression=CExpression::NewL(aDataProvider, aBytesRead);
		break;

	case EFuncExists:
	case EPrimTypeString:
		iString=CString::NewL(aDataProvider, aBytesRead);
		break;
		
	case EPrimTypeOption:	// All these are stored using iIntegerValue only
	case EPrimTypeVariable:
	case EPrimTypeNumber:
		break;
		
	default:
		User::Leave(KErrSISExpressionUnknownOperator);
		}

	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);	
	}


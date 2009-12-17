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
* Definition of the CUserSelections
*
*/


/**
 @file
*/

#include "userselections.h"
#include "sisregistrysession.h"

using namespace Swi;

/*static*/ CUserSelections* CUserSelections::NewL()
	{
	CUserSelections* self = new(ELeave) CUserSelections();
	return self;
	}

/*static*/ CUserSelections* CUserSelections::NewLC()
	{
	CUserSelections* self = CUserSelections::NewL();
	CleanupStack::PushL(self);
	return self;
	}

CUserSelections::CUserSelections() : iDrive(KNoDriveSelected)
	{
	}

EXPORT_C void CUserSelections::SetOptionsL(const RArray<TBool>& aOptions)
	{
	//Delete already existing selected options
	iOptions.Close();
	for (TInt i=0; i < aOptions.Count(); ++i)
		{
		User::LeaveIfError(iOptions.Append(aOptions[i]));
		}
	}

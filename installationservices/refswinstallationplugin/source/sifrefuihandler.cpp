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
* This file implements UI callbacks for the SIF Reference Installer
*
*/


#include "sifrefuihandler.h"

using namespace Usif;

TBool MSifRefUIHandler::ConfirmationUIHandler(const TDesC& aQuestion)
	{
	TPtrC QuestionToBeDisplayed(aQuestion);
	return ETrue;
	}

void MSifRefUIHandler::ErrorDescriptionUIHandler(const TDesC& /*aDescription*/)
	{
	}

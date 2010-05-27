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
* Definition of the CUninstallationPlanner
*
*/


/**
 @file
*/

#include "uninstallationplanner.h"
#include "application.h"
#include "plan.h"

#include "siscontentprovider.h"
#include "sisinstallblock.h"
#include "siscontroller.h"
#include "sisstring.h"
#include "sisinstallationresult.h"
#include "sissupportedlanguages.h"
#include "sislanguage.h"

#include <swi/msisuihandlers.h>

#include "sisregistrywritablesession.h"
#include "sisregistrywritableentry.h"
#include "sisregistryfiledescription.h"
#include "sisregistrypackage.h"

#include "cleanuputils.h"

#include "plan.h"

using namespace Swi;
using namespace Swi::Sis;

CUninstallationPlanner* CUninstallationPlanner::NewL(RUiHandler& aUiHandler, CInstallationResult& aResult)
	{
	CUninstallationPlanner* self = CUninstallationPlanner::NewLC(aUiHandler, aResult);
	CleanupStack::Pop(self);
	return self;
	}

CUninstallationPlanner* CUninstallationPlanner::NewLC(RUiHandler& aUiHandler, CInstallationResult& aResult)
	{
	CUninstallationPlanner* self = new(ELeave) CUninstallationPlanner(aUiHandler, aResult);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CUninstallationPlanner::~CUninstallationPlanner()
	{
	}

CUninstallationPlanner::CUninstallationPlanner(RUiHandler& aUiHandler, CInstallationResult& aResult) 
	: CPlanner(aUiHandler, aResult)
	{
	}

void CUninstallationPlanner::ConstructL()
	{
	CPlanner::ConstructL();
	}

void CUninstallationPlanner::PlanUninstallationL(CSisRegistryPackage& aPackage)
	{
	Plan().SetApplication(UninstallPackageL(aPackage, ETrue));
	SetProgressBarFinalValueL(Plan().FinalProgressBarValue());
	// coverity[memory_leak]
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
TInt32 CUninstallationPlanner::FinalProgressBarValue() const
	{
	return Plan().FinalProgressBarValue();
	}
#endif

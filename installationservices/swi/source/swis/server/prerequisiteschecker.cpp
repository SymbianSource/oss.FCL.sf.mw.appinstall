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


#include "prerequisiteschecker.h"
#include "sisversionrange.h"
#include "sisversion.h"
#include "sisprerequisites.h"
#include "siscontroller.h"
#include "sisdependency.h"
#include "sisuid.h"
#include "sisinfo.h"
#include "checkedversion.h"
#include "sisinstallblock.h"
#include "log.h"

#include "sisregistryentry.h"
#include "sisinstallerrors.h"

#include <swi/msisuihandlers.h>

using namespace Swi;
using namespace Swi::Sis;

/*static*/ CPrerequisitesChecker* CPrerequisitesChecker::NewLC(
	RUiHandler& aInstallerUI, const Sis::CController& aController,
	CInstallationResult& aInstallationResult, const CContentProvider& aProvider,
	const Sis::CController& aMainController)
	{
	CPrerequisitesChecker* self = new(ELeave) CPrerequisitesChecker(aInstallerUI, aController, aInstallationResult, aProvider, aMainController);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CPrerequisitesChecker::CPrerequisitesChecker(
	RUiHandler& aInstallerUI, const Sis::CController& aController,
	CInstallationResult& aInstallationResult, const CContentProvider& aProvider,
	const Sis::CController& aMainController)
	: CRequisitesChecker(aInstallerUI, aInstallationResult, aProvider), iController(aController), iMainController(aMainController)
	{
	}

CPrerequisitesChecker::~CPrerequisitesChecker()
	{
	}

void CPrerequisitesChecker::ConstructL() 
	{
	CRequisitesChecker::ConstructL();
	}

void CPrerequisitesChecker::CheckPrerequisitesL()
	{
	CheckEmbeddingDepthL(iController);
	CheckDependenciesL();
	}

void CPrerequisitesChecker::CheckEmbeddingDepthL(const Sis::CController& aController, TInt aCurrentDepth)
	{
	if (aCurrentDepth++ >= KMaximumEmbeddingDepth)
		{
		DEBUG_PRINTF2(_L8("Prerequisites - Controller with UID 0x%08x too deeply embedded"), aController.Info().Uid().Uid().iUid)
		User::Leave (KErrSISTooDeeplyEmbedded);
		}

	const RPointerArray<CController>& embeddeds = aController.InstallBlock().EmbeddedControllers();
	for (TInt i=0; i<embeddeds.Count(); i++)
		{
		CheckEmbeddingDepthL(*(embeddeds[i]), aCurrentDepth);
		}
	}

void CPrerequisitesChecker::CheckDependenciesL()	
	{
	if (!AnyOnDeviceL(iController.Prerequisites().TargetDevices()) ||
		!AllOnDeviceOrSISL(iController.Prerequisites().Dependencies(), iMainController))
		{
		User::Leave(KErrSISPrerequisitesMissingDependency);
		}
	}

;

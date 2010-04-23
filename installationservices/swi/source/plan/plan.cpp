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
* Implementation of the base plan class
*
*/


#include "plan.h"
#include "application.h"
#include "sisuihandler.h"
#include "progressbar.h"
#include "sisregistryfiledescription.h"

using namespace Swi;

CPlan::CPlan()
	{
	}

EXPORT_C CPlan::~CPlan()
	{
	delete iApplicationName;
	delete iVendorName;
	delete iAppInfo;
	delete iApplication;
	iFilesToRunBeforeShutdown.ResetAndDestroy();
	iFilesToRunAfterInstall.ResetAndDestroy();
	iAppArcRegFiles.ResetAndDestroy();
#ifdef  SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
	iAffectedApps.Close();
#endif
	}

EXPORT_C CPlan* CPlan::NewL()
	{
	CPlan* self=new (ELeave) CPlan();
	return self;
	}

EXPORT_C void CPlan::SetApplicationInformationL(const TDesC& aApplicationName, const TDesC& aVendorName, const TVersion& aVersion)
	{
	iApplicationName=aApplicationName.AllocL();
	iVendorName=aVendorName.AllocL();
	SetAppInfoL(TAppInfo(*iApplicationName, *iVendorName, aVersion));
	}
EXPORT_C void CPlan::SetAppInfoL(const TAppInfo& aAppInfo)
	{
	delete iAppInfo;
	iAppInfo=0;
	iAppInfo=new(ELeave) TAppInfo(aAppInfo);
	}
	
EXPORT_C const TAppInfo& CPlan::AppInfoL() const
	{
	if (!iAppInfo)
		{
		User::Leave(KErrInstallerLeave);
		}
	
	return *iAppInfo;
	}
	
EXPORT_C const CApplication& CPlan::ApplicationL() const
	{
	if (!iApplication)
		{
		User::Leave(KErrInstallerLeave);
		}
	return *iApplication;
	}

EXPORT_C void CPlan::SetApplication(CApplication* aApplication)
	{
	iApplication=aApplication;
	}


EXPORT_C void CPlan::AddInstallFileForProgress(TInt64 aFileSize)
	{
	// Increment for file extraction (scaled by file size)
	iFinalProgressBarValue+= ProgressBarFileIncrement(aFileSize);
	// Increment for file install/copy
	iFinalProgressBarValue+= KProgressBarEndIncrement;
	}

EXPORT_C void CPlan::AddUninstallFileForProgress()
	{
	iFinalProgressBarValue+= KProgressBarUninstallAmount;
	}
	  
EXPORT_C TInt32 CPlan::FinalProgressBarValue() const
	{
	return iFinalProgressBarValue;
	}
	
EXPORT_C void CPlan::RunFilesBeforeShutdownL(const CSisRegistryFileDescription& aFileDescription)
	{
	CSisRegistryFileDescription* fileDescription = CSisRegistryFileDescription::NewLC(aFileDescription);
	iFilesToRunBeforeShutdown.AppendL(fileDescription);
	CleanupStack::Pop(fileDescription);
 	}
 	
EXPORT_C const RPointerArray<CSisRegistryFileDescription>& CPlan::FilesToRunBeforeShutdown() const
	{
 	return iFilesToRunBeforeShutdown;
	}

EXPORT_C void CPlan::RunFileAfterInstallL(const Sis::CFileDescription& aFileDescription, TChar aDrive, TBool aIsStub)
	{
	CSisRegistryFileDescription* fileDescription = CSisRegistryFileDescription::NewLC(aFileDescription, aDrive, aIsStub);
	iFilesToRunAfterInstall.AppendL(fileDescription);
	CleanupStack::Pop(fileDescription);
	}

EXPORT_C const RPointerArray<CSisRegistryFileDescription>& CPlan::FilesToRunAfterInstall() const
	{
 	return iFilesToRunAfterInstall;
	}

EXPORT_C void CPlan::SetContainsPlugins(TBool aContainsPlugins)
	{
	iContainsPlugins = aContainsPlugins;
	}

EXPORT_C TBool CPlan::ContainsPlugins() const
{	
	return iContainsPlugins;
}

EXPORT_C void CPlan::AddAppArcRegFileL(const TDesC& aFilename) 
{
	HBufC* tmp = aFilename.AllocLC() ;
	iAppArcRegFiles.AppendL(tmp);
	CleanupStack::Pop(tmp);
}

EXPORT_C const RPointerArray<TDesC>& CPlan::AppArcRegFiles() const
	{
	return iAppArcRegFiles;
	}

EXPORT_C void CPlan::ResetAppArcRegFiles()
	{
	iAppArcRegFiles.ResetAndDestroy();
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK 
EXPORT_C void CPlan::SetAffectedApps(RArray<TAppUpdateInfo>& aAppInfo)
    {
    TInt count = aAppInfo.Count();
    for(TInt i = 0 ; i< count ; i++)
        {
        iAffectedApps.Append(aAppInfo[i]);
        }
    }
    
EXPORT_C void CPlan::GetAffectedApps(RArray<TAppUpdateInfo>& aAppInfo) const
    {
    TInt count = iAffectedApps.Count();
    for(TInt i = 0 ; i< count ; i++)
        {
        aAppInfo.Append(iAffectedApps[i]);
        }
    }

EXPORT_C void CPlan::ResetAffectedApps()
    {
    iAffectedApps.Reset();    
    }
#endif

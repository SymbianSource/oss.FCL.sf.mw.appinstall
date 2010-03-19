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
* Implementation for the subsessions of RSoftwareComponentRegistry. 
* See class and function declarations for more detail.
*
*/


#include "scr.h"
#include "scrclient.inl"
#include "scrcommon.h"
#include "usiflog.h"
#include <scs/streamingarray.h>

using namespace Usif;

//
// RSoftwareComponentRegistryView
//

TInt RSoftwareComponentRegistryView::SendSyncMessage(TInt aFunction, const TIpcArgs& aArgs) const
	{
	return CallSubsessionFunction(aFunction, aArgs);
	}

EXPORT_C RSoftwareComponentRegistryView::RSoftwareComponentRegistryView()
/**
	This constructor provides a single point of definition from
	which the superclass constructor is called.
 */
	: RScsClientSubsessionBase()
	{
	// empty
	}

EXPORT_C void RSoftwareComponentRegistryView::Close()
	{
	RScsClientSubsessionBase::Close();
	}

EXPORT_C void RSoftwareComponentRegistryView::OpenViewL(const RSoftwareComponentRegistry& aCompReg, CComponentFilter* aFilter)
	{
	DEBUG_PRINTF(_L("Creating a subsession to the SCR server for the components view."));
	
	TInt err = CreateSubsession(aCompReg, ESubSessCreateComponentsView, TIpcArgs());
	if(KErrNone != err)
		{
		DEBUG_PRINTF2(_L("Failed to create the subsession to the SCR server for the components view (Error:%d)"), err);
		User::Leave(err);
		}
	
	RBuf8 buf;
	buf.CleanupClosePushL();
	ExternalizeObjectL(aFilter, buf);
	
	DEBUG_PRINTF(_L("Sending the request to create a component view on the server side."));
	TIpcArgs args(&buf);
	User::LeaveIfError(CallSubsessionFunction(EOpenComponentsView, args));
	CleanupStack::PopAndDestroy(&buf);
	}

EXPORT_C CComponentEntry* RSoftwareComponentRegistryView::NextComponentL(TLanguage aLocale) const
	{
	CComponentEntry* entry = CComponentEntry::NewLC();
	if(!NextComponentL(*entry, aLocale))
		{
		CleanupStack::PopAndDestroy(entry);
		return NULL;
		}
	CleanupStack::Pop(entry);
	return entry;
	}

EXPORT_C TBool RSoftwareComponentRegistryView::NextComponentL(CComponentEntry& aEntry, TLanguage aLocale) const
	{
	DEBUG_PRINTF(_L("Sending the request to retrieve the next component from the component view."));
	TIpcArgs args(aLocale);
	TInt argNum = 1;
	return GetObjectL(*this, aEntry, EGetNextComponentSize, EGetNextComponentData, argNum, args);
	}

EXPORT_C void RSoftwareComponentRegistryView::NextComponentSetL(TInt aMaxCount, RPointerArray<CComponentEntry>& aComponentList, TLanguage aLocale) const
	{
	DEBUG_PRINTF(_L("Sending the request to retrieve the next set of components from the component view."));
	TIpcArgs args(aMaxCount, aLocale);
	TInt argNum = 2;
	GetObjectArrayL(*this, EGetNextComponentSetSize, EGetNextComponentSetData, argNum, args, aComponentList);
	}

//
// RSoftwareComponentRegistryFilesList
//

TInt RSoftwareComponentRegistryFilesList::SendSyncMessage(TInt aFunction, const TIpcArgs& aArgs) const
	{
	return CallSubsessionFunction(aFunction, aArgs);
	}

EXPORT_C RSoftwareComponentRegistryFilesList::RSoftwareComponentRegistryFilesList()
/**
	This constructor provides a single point of definition from
	which the superclass constructor is called.
 */
	: RScsClientSubsessionBase()
	{
	// empty
	}

EXPORT_C void RSoftwareComponentRegistryFilesList::OpenListL(const RSoftwareComponentRegistry& aCompReg, TComponentId aComponentId)
	{
	DEBUG_PRINTF(_L("Creating a subsession to the SCR server for the file list."));
		
	TInt err = CreateSubsession(aCompReg, ESubSessCreateFileList, TIpcArgs());
	if(KErrNone != err)
		{
		DEBUG_PRINTF2(_L("Failed to create the subsession to the SCR server for the file list (Error:%d)."), err);
		User::Leave(err);
		}
	
	DEBUG_PRINTF(_L("Sending a request to create a file list on the server side."));
	TIpcArgs args(aComponentId);
	User::LeaveIfError(CallSubsessionFunction(EOpenFileList, args));
	}

EXPORT_C HBufC* RSoftwareComponentRegistryFilesList::NextFileL() const
	{
	TIpcArgs args(TIpcArgs::ENothing);
	TInt argNum = 0;
	return GetObjectL<HBufC>(*this, EGetNextFileSize, EGetNextFileData, argNum, args);
	}

EXPORT_C void RSoftwareComponentRegistryFilesList::NextFileSetL(TInt aMaxCount, RPointerArray<HBufC>& aFileList) const
	{
	DEBUG_PRINTF(_L("Sending the request to retrieve the next set of components from the component view."));
	TIpcArgs args(aMaxCount);
	TInt argNum = 1;
	GetObjectArrayL(*this, EGetNextFileSetSize, EGetNextFileSetData, argNum, args, aFileList);
	}

EXPORT_C void RSoftwareComponentRegistryFilesList::Close()
	{
	RScsClientSubsessionBase::Close();
	}

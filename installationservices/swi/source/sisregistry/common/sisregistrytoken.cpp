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
* CSisRegistryToken - registry entry class implementation
*
*/


/**
 @file 
 @released
 @internalComponent
*/
#include <s32strm.h>
#include "sisregistrypackage.h"
#include "sisregistrytoken.h"
#include "controllerinfo.h"
#include "arrayutils.h" // for sids as it has the in/externalization 
#include "log.h"
#include "versionutil.h"
#include "userselections.h"

using namespace Swi;


EXPORT_C CSisRegistryToken* CSisRegistryToken::NewL()
	{
	CSisRegistryToken* self = CSisRegistryToken::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSisRegistryToken* CSisRegistryToken::NewLC()
	{
	CSisRegistryToken* self = new(ELeave) CSisRegistryToken();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CSisRegistryToken* CSisRegistryToken::NewL(RReadStream& aStream)
	{
	CSisRegistryToken* self = CSisRegistryToken::NewLC(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSisRegistryToken* CSisRegistryToken::NewLC(RReadStream& aStream)
	{
	CSisRegistryToken* self = new(ELeave) CSisRegistryToken();
	CleanupStack::PushL(self);
	self->ConstructL(aStream);
	return self;
	}

EXPORT_C CSisRegistryToken* CSisRegistryToken::NewL(const CSisRegistryToken& aToken)
	{
	CSisRegistryToken* self = CSisRegistryToken::NewLC(aToken);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSisRegistryToken* CSisRegistryToken::NewLC(const CSisRegistryToken& aToken)
	{
	CSisRegistryToken* self = new(ELeave) CSisRegistryToken();
	CleanupStack::PushL(self);
	self->ConstructL(aToken);
	return self;
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
CSisRegistryToken::CSisRegistryToken(): 
	iDrives(0)
	{
	}
#else
CSisRegistryToken::CSisRegistryToken(): 
	iDrives(0),iCompletelyPresent(ETrue)
	{
	}
#endif

CSisRegistryToken::~CSisRegistryToken()
	{
	iSids.Reset();
	iControllerInfo.ResetAndDestroy();
	}

void CSisRegistryToken::ConstructL()
	{
	}
	
void CSisRegistryToken::ConstructL(const CSisRegistryToken& aToken)
	{
	CSisRegistryPackage::ConstructL(aToken);
	CopyArrayL(iSids, aToken.iSids);
	iDrives = aToken.iDrives;

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	iCurrentDrives = aToken.iCurrentDrives;
#endif

	CopyPointerArrayL(iControllerInfo, aToken.iControllerInfo);
	iVersion = aToken.iVersion;
	iLanguage = aToken.iLanguage;
	iSelectedDrive = aToken.iSelectedDrive;
	}
	
void CSisRegistryToken::ConstructL(RReadStream& aStream)
	{
	InternalizeL(aStream);
	}
	
EXPORT_C void CSisRegistryToken::ExternalizeL(RWriteStream& aStream) const
	{
	// the package section
	CSisRegistryPackage::ExternalizeL(aStream);
	aStream.WriteUint32L(iDrives);
	
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Write a zero here, to maintain BC.
	// Previously the boolean "iCompletelyPresent" was externalized here
	aStream.WriteUint32L(0);
#endif

	ExternalizeArrayL(iSids, aStream);
	ExternalizePointerArrayL(iControllerInfo, aStream);
	
	// Version
	aStream << iVersion;
	// user selected options	
	aStream.WriteInt32L(static_cast<TInt32>(iLanguage));
	aStream.WriteUint32L(static_cast<TUint32>(iSelectedDrive));	

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// not used at present
	aStream.WriteInt32L(iUnused1);
	aStream.WriteInt32L(iUnused2); 
#endif
	}

EXPORT_C void CSisRegistryToken::InternalizeL(RReadStream& aStream)
	{
	CSisRegistryPackage::InternalizeL(aStream);
	iDrives = aStream.ReadUint32L();

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
	// Read a 32 bit int here to maintain BC. Previously this was the
	// boolean "iCompletelyPresent"
	aStream.ReadUint32L();
#endif
	
	InternalizeArrayL(iSids, aStream);
	InternalizePointerArrayL(iControllerInfo, aStream);

	// Version
	aStream >> iVersion;
	// user selected options
	iLanguage = static_cast<TLanguage>(aStream.ReadInt32L());
	//use unsigned as we use binary mask
	iSelectedDrive = static_cast<TChar>(aStream.ReadUint32L());

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// not used at present
	iUnused1 = aStream.ReadInt32L();
	iUnused2 = aStream.ReadInt32L(); 
#endif
	}

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
EXPORT_C void CSisRegistryToken::SetFixedDrives(TUint aFixedDrives)
	{
	iCurrentDrives |= (aFixedDrives & iDrives);
	}
	
EXPORT_C void CSisRegistryToken::AddRemovableDrive(TInt aDrive)
	{
	// Add it to the array iff it exists in the fixed drive mask
	iCurrentDrives |= (iDrives & (1 << aDrive));
	}

EXPORT_C void CSisRegistryToken::RemoveRemovableDrive(TInt aDrive)
	{
	iCurrentDrives &= ~(1<<aDrive);
	}
#endif


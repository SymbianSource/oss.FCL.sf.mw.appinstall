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
*
*/


#ifndef	UNINSTALLER_H
#define	UNINSTALLER_H

/**
* @file UNINSTALLER.H
*
* @internalComponent
* @released
*/

#include "parameterlist.h"
#include "sisregistry.h"


class Uninstaller
	{
public:
	Uninstaller(SisRegistry& aReg, const CParameterList& aParamList);
	void Uninstall();

private:
	void RemovePkg(const TUint32 aUid);

private:
	SisRegistry& iRegistry;		// The current sis registry
	const CParameterList& iParamList;	// Command line options
	};

#endif	/* UNINSTALLER_H */

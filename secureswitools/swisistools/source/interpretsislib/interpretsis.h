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


/**
 @file 
 @publishedPartner
 @released
*/
#ifndef __INTERPRETSIS_H__
#define __INTERPRETSIS_H__

#include <iostream>
#include <memory>

#include "parameterlist.h"
#include "rommanager.h"
#include "configmanager.h"
#include "sisregistry.h"
#include "installer.h"


class CInterpretSIS
	{
public:
	typedef std::auto_ptr<CParameterList> TParamPtr;
	typedef std::auto_ptr<RomManager> TRomManagerPtr;
public:
	/**
	 * Constructor
	 * 
	 * @param aParamList parameters used for installation.
	 * This class takes ownership of the instance (of type  
	 * CParameterList) pointed by aParamList. 
	 */
	CInterpretSIS(TParamPtr aParamList);
	/**
	 * Cleanup owned resources
	 */
	~CInterpretSIS();
	
public:
	/**
	 * Install the SIS files present specified while creating this class.
	 */
	int Install();
	/**
	 * Un-install installed applications specified while creating this class.
	 */
	void Uninstall();

	const SisRegistry* GetSisRegistry() const;	
	
private:
	void ConstructL();
	
private:
	TParamPtr 		iParamList;
	TRomManagerPtr	iRomManager;
	ConfigManager*	iConfigManager;
	SisRegistry*	iSisRegistry;
	Installer*		iInstaller;
	};

#endif //__INTERPRETSIS_H__

/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*/
#include <captestframework/aprcaptestutility.h>

class CSwiLauncherCapTest : public CDefaultCapabilityTest
	{
public:
	static CSwiLauncherCapTest* NewL();
	
private:
	CSwiLauncherCapTest();
	
	void RunTestL();
	
	void ConstructL();
	};

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
class CSwiLauncherInstallServerUtilCapTest : public CDefaultCapabilityTest
	{
public:
	static CSwiLauncherInstallServerUtilCapTest* NewL();
	
private:
	CSwiLauncherInstallServerUtilCapTest();
	
	void RunTestL();
	
	void ConstructL();
	};

class CSwiLauncherSisRegistryServerUtilCapTest : public CDefaultCapabilityTest
    {
public:
    static CSwiLauncherSisRegistryServerUtilCapTest* NewL();
    
private:
    CSwiLauncherSisRegistryServerUtilCapTest();
    
    void RunTestL();
    
    void ConstructL();
    };

class CSisLauncherSwiSidTest : public CDefaultCapabilityTest
    {
public:
    static CSisLauncherSwiSidTest* NewL();
    
private:
    CSisLauncherSwiSidTest();
    
    void RunTestL();
    
    void ConstructL();
    };

#endif

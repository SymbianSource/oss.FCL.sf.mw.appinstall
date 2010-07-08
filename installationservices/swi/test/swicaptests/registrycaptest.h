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

/** Tests registry public interface */
class CPublicRegistryCapTest : public CDefaultCapabilityTest
	{
public:
	static CPublicRegistryCapTest* NewL();

private:
	CPublicRegistryCapTest();
	
	void RunTestL();

	void ConstructL();
	};

/** Tests registry public interface with ReadUserData*/
class CReadUserDataRegistryCapTest : public CDefaultCapabilityTest
	{
public:
	static CReadUserDataRegistryCapTest* NewL();
	
private:
	CReadUserDataRegistryCapTest();
	
	void RunTestL();

	void ConstructL();
	};
	
/** Tests registry private interface */
class CPrivateRegistryCapTest : public CDefaultCapabilityTest
	{
public:
	static CPrivateRegistryCapTest* NewL();
	
private:
	CPrivateRegistryCapTest();
	
	void RunTestL();

	void ConstructL();
	};
	
	
/** Tests registry private interface */
class CDaemonRegistryCapTest : public CDefaultCapabilityTest
	{
public:
	static CDaemonRegistryCapTest* NewL();
	
private:
	CDaemonRegistryCapTest();
	
	void RunTestL();

	void ConstructL();
	};


/** Tests registry revocation interface */
class CRevocationRegistryCapTest : public CDefaultCapabilityTest
	{
public:
	static CRevocationRegistryCapTest* NewL();
	
private:
	CRevocationRegistryCapTest();
	
	void RunTestL();

	void ConstructL();
	};

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
/** Tests registry interfaces available to SIF server only*/
class CSifServerRegistryCapTest : public CDefaultCapabilityTest
	{
public:
	static CSifServerRegistryCapTest* NewL();
	
private:
	CSifServerRegistryCapTest();
	
	void RunTestL();

	void ConstructL();
	};

/** Tests registry interfaces to add reg file data to scr with TCB capability*/
class CSisRegistryTCBCapTest : public CDefaultCapabilityTest
    {
public:
    static CSisRegistryTCBCapTest* NewL();
    
private:
    CSisRegistryTCBCapTest();
    
    void RunTestL();

    void ConstructL();
    };

#endif

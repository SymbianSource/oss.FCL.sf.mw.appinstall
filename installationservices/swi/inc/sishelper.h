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
* SISHelper server header file
*
*/


/**
 @file 
 @internalTechnology
*/

#ifndef __SISHELPER_H__
#define __SISHELPER_H__

#include "sisdataprovider.h"
#include "sishelperclient.h"

 
namespace Swi
{

namespace Sis
	{
	class CContents;
	}

/**
 * Timer that shuts down SISHelper server if it is idle for a predefined 
 * amount of time.
 *
 * @internalTechnology
 * @released
 */
class CSisHelperSession;
class CSisHelperShutdown : public CTimer
	{
#ifdef __WINS__ // Occassionally, there are transient problems in the emulator that cause the timer to kick in sooner than needed (DEF104102)
	enum {KShutdownDelay=5*1000*1000}; // 5 seconds
#else
	enum {KShutdownDelay=2*1000*1000}; // 2 seconds
#endif
public:
	inline CSisHelperShutdown();
	virtual ~CSisHelperShutdown();
	inline void ConstructL();
	inline void Start();
private:
	void RunL();
	};

/**
 * This class provides interface for SWIS to get SIS file data and other 
 * parameters. Since the only think using it is SWIS, the SisHelper client 
 * code is built into SWIS and is not available to any other program.
 *
 * @internalTechnology
 * @released
 */
class CSisRegistryPackage;
class CSisHelperServer : public CPolicyServer
	{
public:
	IMPORT_C static CSisHelperServer* NewLC(TSisHelperStartParams& aParams);
	IMPORT_C static CSisHelperServer* NewL(TSisHelperStartParams& aParams);
	~CSisHelperServer();
	void AddSession();
	void DropSession();
	TInt GetSisFileDrive();
	TBool IsSisFileReadOnly();
	const TDesC& GetSisFileNameL();
	
private:
	CSisHelperServer();
	void ConstructL(TSisHelperStartParams& aParams);
	CSession2* NewSessionL(const TVersion& aVersion, 
		const RMessage2& aMessage) const;

	// Starting SISHelper
public:
	/// Gets the main package entry associated with a UID. Ownership is transferred to the client.
	static CSisRegistryPackage* MainPackageEntryL(TUid aUid); 
	static TInt StartSisHelper(TSisHelperStartParams& aParams, RThread& aServer);
	static void Abort();
private:
	static TInt SisHelperThreadFunction(TAny* aPtr); // SISHelper thread EP

private:
	TInt iSessionCount;
	CSisHelperShutdown* iShutdown; ///< Ensures the server is there for client
	
	/**
	 * Data provider instance implementation pointer; if a pointer to it is 
	 * passed when creating the server, it is just stored in this member.
	 * If a file name is passed,  a file data provider is created and 
	 * stored in this member.
	 */
	MSisDataProvider* iDataProvider;
	TBool iDeleteDataProvider;
	RFs iFs;
	TBool iSessionConnected;
	TInt iSisFileDrive;
	TBool iSisFileReadOnly;
	HBufC* iSisFileName;
	
private:
	// Server Policies
  	static const TUint iRangeCount=1;
  	static const TInt iRanges[iRangeCount];
  	static const TUint8 iElementsIndex[iRangeCount];
  	static const CPolicyServer::TPolicyElement iPolicyElements[2];
  	static const CPolicyServer::TPolicy iPolicy;
	};

/**
 * SISHelper server session class that serves requests from SWIS
 *
 * @internalTechnology
 * @released
 */
class CSisHelperSession : public CSession2
	{
public:
	CSisHelperSession(MSisDataProvider& aDataProvider);
	void CreateL();
	void Send(const TDesC& aMessage);
private:
	~CSisHelperSession();
	inline CSisHelperServer& Server();
	void ServiceL(const RMessage2& aMessage);
	void ServiceError(const RMessage2& aMessage, TInt aError);
	inline TBool ReceivePending() const;
	
	void GetControllerL(const RMessage2& aMessage);
	void ExtractFileL(const RMessage2& aMessage);
	void CreateSisStubL(const RMessage2& aMessage);
	void IsStubL(const RMessage2& aMessage);
	void IsDrmProtectedL(const RMessage2& aMessage);
	void GetEquivalentLanguagesL(const RMessage2& aMessage);
	void GetSisFileDriveL(const RMessage2& aMessage);
	void IsSisFileReadOnlyL(const RMessage2& aMessage);
	void GetSisFileNameL(const RMessage2& aMessage);
	void GetControllerFromSisL(const RMessage2& aMessage);

	// Functions for asynchronous extraction
	void SetupAsyncExtractionL(const RMessage2& aMessage);
	void AsyncExtractionL(const RMessage2& aMessage);
	void EndAsyncExtractionL(const RMessage2& aMessage);
	void CleanupAsyncExtractionL();
	/**
	 * Utility function that supply SWIS with drive and free space information
	 *
	 * @param aDriveLetters Array of drive letters available for installation
	 * @param aDriveSpaces  Array of drive free spaces for installation
	 */
	static void FillDrivesAndSpacesL(RArray<TChar>& aDriveLetters, 
		RArray<TInt64>& aDriveSpaces);

	// access the current Contents object
	Sis::CContents& CurrentContentsL();
private:
	RMessagePtr2 iReceiveMsg;
	TInt iReceiveLen;
	MSisDataProvider& iDataProvider;
	Sis::CContents* iCurrentContents;

	// Members for asynchronous extraction
	RFs iAsyncFs;
	RFile iAsyncFile;
	TBool iInAsyncExtraction;
	TInt32 iAsyncFileIndex;
	TInt32 iAsyncDataUnit;
	};

} // namespace Swi

#endif // __SISHELPER_H__

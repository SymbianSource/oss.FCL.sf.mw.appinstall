/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Scr Accessor - server and implementation
*
*/


/**
 @file 
 @test
 @internalComponent
*/

#include "tscraccessor_server_session.h"
#include "tscraccessor_common.h"
#include "tscraccessor_client.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "screntries_internal.h"
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
#include <scs/cleanuputils.h>
#include <usif/usiferror.h>
#include <usif/scr/screntries_platform.h>
using namespace Usif;

/////////////////////// Utility functions //////////////////////////////
void StartTimer(TTime& aTimer)
	{
	aTimer.HomeTime();
	}

TInt StopTimer(TTime aStartTimer)
	{
	TTime endTime;
	endTime.HomeTime();
		
	TTimeIntervalMicroSeconds duration = endTime.MicroSecondsFrom(aStartTimer);
	TInt actualDuration = I64INT(duration.Int64())/1000; // in millisecond
	return actualDuration;
	}

//////////////////////// Server implementation /////////////////////////

CServer2* CScrAccessServer::NewLC()
	{
	CScrAccessServer* self=new(ELeave) CScrAccessServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CScrAccessServer::CScrAccessServer()
	:CServer2(0, ESharableSessions)
	{
	}

CScrAccessServer::~CScrAccessServer()
	{
	iScrSession.Close();
	}

void CScrAccessServer::ConstructL()
	{
	StartL(KScrAccessServerName);
	}

CSession2* CScrAccessServer::NewSessionL(const TVersion&, const RMessage2&) const
	{
	return new(ELeave) CScrAccessSession();
	}

void CScrAccessServer::AddSession()
	{
	++iSessionCount;
	}

void CScrAccessServer::DropSession()
	{
	--iSessionCount;
	if(!iSessionCount)
		CActiveScheduler::Stop();
	}


//////////////////////// Session implementation /////////////////////////
CScrAccessSession::CScrAccessSession()
	{
	}

void CScrAccessSession::CreateL()
	{
	Server().AddSession();
	}

CScrAccessSession::~CScrAccessSession()
	{
	Server().DropSession();
	}

HBufC* ReadDescFromMessageLC(const RMessage2& aMessage, TInt aSlot)
	{
	TInt len = aMessage.GetDesLengthL(aSlot);
	HBufC* desc = HBufC::NewLC(len);
	TPtr ptrDesc(desc->Des());
	aMessage.ReadL(aSlot, ptrDesc);
	return desc;
	}

void VerifySoftwareTypeAdditionL(TInt& aError, RScrAccessor::TAccessorOperationResult& aOpResult, RSoftwareComponentRegistry& aScrSession, const TDesC& aMimeType, TUid aExpectedPluginUid)
	{
	// Do verification, there is no error until this step
	if(KErrNone == aError)
		{ // if adding software type operation is successful, check if the software type has really been added to teh sCR.
		  // Get the plugin uid of the newly added MIME type
		TUid retrievedPluginUid;
		TRAP(aError, retrievedPluginUid = aScrSession.GetPluginUidL(aMimeType));
		if(KErrNone == aError)
			{ // if the plugin uid is found, then compare it with the expected one.
			if(retrievedPluginUid != aExpectedPluginUid)
				aOpResult = RScrAccessor::EOpFailUnexpectedPluginUid;
			}
		else if(KErrNotFound == aError)
			aOpResult = RScrAccessor::EOpFailSwTypeNotFound;
		// if the returned error is different from KErrNone and KErrNotFound, pass it to the client.
		}
	}

void GenerateTestDataL(RPointerArray<HBufC>& aMimeTypes, const TDesC& aBaseMimeTypeName, TInt aMimeTypeNum, RPointerArray<CLocalizedSoftwareTypeName>& aLocalizedSwTypeNames, const TDesC& aBaseSwTypeName, TInt aSwTypesNum)
	{
	RBuf buf;
	buf.CreateL(aBaseMimeTypeName.Length() + 3); // 3 is extra bytes to append number to the buffer
	buf.CleanupClosePushL();
	buf.Copy(aBaseMimeTypeName);
	
	for(TInt i=0; i<aMimeTypeNum; ++i)
		{
		buf.AppendNum(i);
		HBufC *name = buf.AllocLC();
		aMimeTypes.AppendL(name);
		CleanupStack::Pop(name);
		buf.SetLength(buf.Length()-1);
		}
	buf.Close();
	
	buf.CreateL(aBaseSwTypeName.Length() + 3);
	buf.Copy(aBaseSwTypeName);
	
	for(TInt i=0; i<aSwTypesNum; ++i)
		{
		buf.AppendNum(i);
		CLocalizedSoftwareTypeName *localizedName = CLocalizedSoftwareTypeName::NewL(buf, TLanguage(i+1));
		CleanupStack::PushL(localizedName); // Use NewL for coverage
		aLocalizedSwTypeNames.AppendL(localizedName);
		CleanupStack::Pop(localizedName); // ownership is transferred
		}
	CleanupStack::PopAndDestroy(&buf);
	}

TInt AddSoftwareTypeL(RSoftwareComponentRegistry& aScrSession, const TDesC& aUniqueSwTypeName, TUid aSifPluginUid, RArray<TCustomAccessInfo> aInstallerSids, const TDesC& aBaseMimeTypeName, TInt aMimeTypeNum, const TDesC& aBaseSwTypeName, TInt aSwTypesNum, const TDesC& aLauncherExecutable, RScrAccessor::TAccessorOperationResult& aOpResult)
	{
	RPointerArray<HBufC> mimeTypesArray;
	CleanupResetAndDestroyPushL(mimeTypesArray);
	RPointerArray<CLocalizedSoftwareTypeName> localizedSwNames;
	CleanupResetAndDestroyPushL(localizedSwNames);
						
	GenerateTestDataL(mimeTypesArray, aBaseMimeTypeName, aMimeTypeNum, localizedSwNames, aBaseSwTypeName, aSwTypesNum);
	
	RPointerArray<CLocalizedSoftwareTypeName> *plocalizedSwNames = 	&localizedSwNames;
	if(!aSwTypesNum)
		plocalizedSwNames = NULL;
	
	HBufC *mimeType2BChecked (0);
	if(mimeTypesArray.Count() > 0)
		mimeType2BChecked = mimeTypesArray[0]->AllocLC();
	else
		mimeType2BChecked = KNullDesC().AllocLC();
	
	Usif::CSoftwareTypeRegInfo* swType = Usif::CSoftwareTypeRegInfo::NewL(aUniqueSwTypeName);
	CleanupStack::PushL(swType);
	swType->SetSifPluginUid(aSifPluginUid);
	for(TInt i=0;i<aInstallerSids.Count(); ++i)
		{
		swType->SetCustomAccessL(aInstallerSids[i]);
		}
	if(aMimeTypeNum)
	    {
	    for(TInt i=0;i<mimeTypesArray.Count(); ++i)
	        {
	        swType->SetMimeTypeL(mimeTypesArray[i]->Des());
	        }
        }
	
	swType->SetLauncherExecutableL(aLauncherExecutable);
	if(plocalizedSwNames)
	    {
	    for(TInt i=0;i<plocalizedSwNames->Count(); ++i)
	        {
	        swType->SetLocalizedSoftwareTypeNameL(plocalizedSwNames->operator [](i)->Locale(), plocalizedSwNames->operator [](i)->NameL());
	        }
	    }
	
	TRAPD(err, aScrSession.AddSoftwareTypeL(*swType));
	VerifySoftwareTypeAdditionL(err, aOpResult, aScrSession, *mimeType2BChecked, aSifPluginUid);
	CleanupStack::PopAndDestroy(4, &mimeTypesArray); // mimeTypesArray, localizedSwNames, mimeType2BChecked
	return err;
	}

void CScrAccessSession::ServiceL(const RMessage2& aMessage)
	{
	TInt err(0);
	TTime timer;
	StartTimer(timer);
	TInt actualTestDuration(0);
	
	RScrAccessor::TAccessorOperationResult opResult = RScrAccessor::EOpSucessful;
	
	TScrAccessMessages f = static_cast<TScrAccessMessages>(aMessage.Function());
	
	if(EAddSoftwareType == f || EDeleteSoftwareType == f)
		{
		User::LeaveIfError(Server().ScrSession().Connect());
		// create transaction on SCR for mutable operations
		Server().ScrSession().CreateTransactionL();
		}
	
	switch (f)
		{
		case EAddSoftwareType:
			{
			RScrAccessor::TAccessorOperationType opType = static_cast<RScrAccessor::TAccessorOperationType>(aMessage.Int0());
			
			TUid localizedSifPluginUid = {0xA01B7222};
			RArray<TCustomAccessInfo> locSidArray;
			CleanupClosePushL(locSidArray);
			               
			TCustomAccessInfo id1(TSecureId(0x10285BC9), static_cast<TAccessMode>(1));
			locSidArray.AppendL(id1);
			                    
			_LIT(KMimeTypeBaseNameLocalized, "test_mimetype_localizedinstaller");
			_LIT(KSwTypeBaseLocalizableName, "test_localizedinstaller_name");
			_LIT(KSwTypeUniqueNameLocalized, "test_localizedinstaller_uniquename");
								
			switch(opType)
				{
				case RScrAccessor::EAddNonLocalizedSoftwareType:
					{	
					TUid sifPluginUid = {0xA01B7211};
			
					RArray<TCustomAccessInfo> sidArray;
					CleanupClosePushL(sidArray);
					    
					TCustomAccessInfo id1(TSecureId(0x10285BC9), static_cast<TAccessMode>(1));
					TCustomAccessInfo id2(TSecureId(0xAAEEDD11), static_cast<TAccessMode>(1));
					sidArray.AppendL(id1);
					sidArray.AppendL(id2);
					    
					_LIT(KMimeTypeBaseName, "test_mimetype_nonlocalizedinstaller");
					_LIT(KSwTypeUniqueName, "test_nonlocalizedinstaller_uniquename");
										
					err = AddSoftwareTypeL(Server().ScrSession(), KSwTypeUniqueName, sifPluginUid, sidArray,  KMimeTypeBaseName, 2, KNullDesC, 0, KNullDesC, opResult);
					CleanupStack::PopAndDestroy(&sidArray);
					break;
					}
				case RScrAccessor::EAddLocalizedSoftwareType:
					{
					err = AddSoftwareTypeL(Server().ScrSession(), KSwTypeUniqueNameLocalized, localizedSifPluginUid, locSidArray, KMimeTypeBaseNameLocalized, 2, KSwTypeBaseLocalizableName, 2, KNullDesC, opResult);
					break;
					}
				case RScrAccessor::EAddSofwtareTypeWithoutMimeTypes:
					{
					TUid sifPluginUid = {0xA01B7333};
					RArray<TCustomAccessInfo> sidArray;
					CleanupClosePushL(sidArray);
					                    
					TCustomAccessInfo id1(TSecureId(0x10285BC9), static_cast<TAccessMode>(1));
					sidArray.AppendL(id1);	
					_LIT(KSwTypeUniqueName, "test_nomimeinstaller_uniquename");
					
					err = AddSoftwareTypeL(Server().ScrSession(), KSwTypeUniqueName, sifPluginUid, sidArray, KNullDesC, 0, KNullDesC, 0, KNullDesC, opResult);
					CleanupStack::PopAndDestroy(&sidArray);
					break;
					} 
				case RScrAccessor::EAddLocalizedSoftwareTypeWithDifferentPluginUid:
					{
					TUid localizedDifferentSifPluginUid = {0xCC1B7333};
					err = AddSoftwareTypeL(Server().ScrSession(), KSwTypeUniqueNameLocalized, localizedDifferentSifPluginUid, locSidArray, KMimeTypeBaseNameLocalized, 2, KSwTypeBaseLocalizableName, 2, KNullDesC, opResult);
					if(KErrAlreadyExists == err)
						err = KErrNone; // The expected result is KErrAlreadyExists. So return no error to the test client.
					break;
					}
				case RScrAccessor::EAddLocalizedSoftwareTypeWithExtraName:
					{
					err = AddSoftwareTypeL(Server().ScrSession(), KSwTypeUniqueNameLocalized, localizedSifPluginUid, locSidArray, KMimeTypeBaseNameLocalized, 2, KSwTypeBaseLocalizableName, 3, KNullDesC, opResult);
					if(KErrAlreadyExists == err)
						err = KErrNone; // The expected result is KErrAlreadyExists. So return no error to the test client.
					break;
					}
				case RScrAccessor::EAddLocalizedSoftwareTypeWithMissingName:
					{
					err = AddSoftwareTypeL(Server().ScrSession(), KSwTypeUniqueNameLocalized, localizedSifPluginUid, locSidArray, KMimeTypeBaseNameLocalized, 2, KSwTypeBaseLocalizableName, 1, KNullDesC, opResult);
					if(KErrAlreadyExists == err)
						err = KErrNone; // The expected result is KErrAlreadyExists. So return no error to the test client.
					break;
					}
				case RScrAccessor::EAddLocalizedSoftwareTypeWithExtraMime:
					{
					err = AddSoftwareTypeL(Server().ScrSession(), KSwTypeUniqueNameLocalized, localizedSifPluginUid, locSidArray, KMimeTypeBaseNameLocalized, 3, KSwTypeBaseLocalizableName, 2, KNullDesC, opResult);
					if(KErrAlreadyExists == err)
						err = KErrNone; // The expected result is KErrAlreadyExists. So return no error to the test client.
					break;
					}
				case RScrAccessor::EAddLocalizedSoftwareTypeWithMissingMime:
					{
					err = AddSoftwareTypeL(Server().ScrSession(), KSwTypeUniqueNameLocalized, localizedSifPluginUid, locSidArray, KMimeTypeBaseNameLocalized, 1, KSwTypeBaseLocalizableName, 2, KNullDesC, opResult);
					if(KErrAlreadyExists == err)
						err = KErrNone; // The expected result is KErrAlreadyExists. So return no error to the test client.
					break;
					}
				case RScrAccessor::EAddMultipleSidWithLauncherExecutable:
				    {
				    TUid sifPluginUid = {0xA01B7212};
				    _LIT(KSwTypeUniqueName, "test_uniquename");
				    _LIT(KMimeTypeBaseName, "test_mimetype");
				    _LIT(KLauncherExecutable, "LauncherExecutable");
				    RArray<TCustomAccessInfo> sidArray;
				    CleanupClosePushL(sidArray);
				                            
				    TCustomAccessInfo id1(TSecureId(0x10285BC9), static_cast<TAccessMode>(1));
				    TCustomAccessInfo id2(TSecureId(0xAAEEDD11), static_cast<TAccessMode>(1));
				    TCustomAccessInfo id3(TSecureId(0xAAEEEE11), static_cast<TAccessMode>(1));

				    sidArray.AppendL(id1);
				    sidArray.AppendL(id2);
				    sidArray.AppendL(id3);
				    
				    err = AddSoftwareTypeL(Server().ScrSession(), KSwTypeUniqueName, sifPluginUid, sidArray, KMimeTypeBaseName, 1, KNullDesC, 0, KLauncherExecutable, opResult);
				    CleanupStack::PopAndDestroy(&sidArray);
				    break;
				    }
				} // switch(opType)
			CleanupStack::PopAndDestroy(&locSidArray);
			actualTestDuration = StopTimer(timer);
			break;
			}
		case EDeleteSoftwareType:
			{
			_LIT(KSwTypeUniqueName, "test_nonlocalizedinstaller_uniquename");
			_LIT(KMimeTypeBaseName, "test_mimetype_nonlocalizedinstaller");
			RPointerArray<HBufC> deletedMimeTypes;
			CleanupResetAndDestroyPushL(deletedMimeTypes);
			TRAP(err, Server().ScrSession().DeleteSoftwareTypeL(KSwTypeUniqueName,deletedMimeTypes));
			actualTestDuration = StopTimer(timer);
				
			// If the delete operation is successfull, check the returned MIME types deleted are as expected.
			// If the delete operation fails, just return the error code to teh client.
			if(KErrNone == err) 
				{
				RPointerArray<HBufC> mimeTypesArray;
				CleanupResetAndDestroyPushL(mimeTypesArray);
				RPointerArray<CLocalizedSoftwareTypeName> localizedSwNames;
				GenerateTestDataL(mimeTypesArray, KMimeTypeBaseName, 2, localizedSwNames, KNullDesC, 0);

				TInt expectedMimeTypesCount = mimeTypesArray.Count();
				TInt retrievedMimeTypesCount = deletedMimeTypes.Count();
				if(retrievedMimeTypesCount != expectedMimeTypesCount)
					opResult = RScrAccessor::EOpFailUnexpectedMimeTypeNum;
				else
					{
					for(TInt i=0; i<retrievedMimeTypesCount; ++i)
						{
						if(*(deletedMimeTypes[i]) != *(mimeTypesArray[i]))
							opResult = RScrAccessor::EOpFailUnexpectedMimeTypeNum;;
						} // for
					} // if-else
				
				TRAP(err, Server().ScrSession().GetPluginUidL(KSwTypeUniqueName));
				if(KErrNone == err)
					{
					// The plugin UID has been retrieved successfully. This is unexpected result.
					opResult = RScrAccessor::EOpFailSwTypeStillExists;
					}
				else if(KErrSifUnsupportedSoftwareType == err)
					{
					// The plugin UID couldn't be found as expected. As the operation is successfull,
					// return KErrNone to the client
					err = KErrNone;
					}
				CleanupStack::PopAndDestroy(&mimeTypesArray);
				} 
			CleanupStack::PopAndDestroy(&deletedMimeTypes);
			break;
			}
		case EDeleteFile:
			{
			HBufC *file = ReadDescFromMessageLC(aMessage, 0);
			RFs fs;
			User::LeaveIfError(fs.Connect());
			CleanupClosePushL(fs);
			err = fs.Delete(*file);
			CleanupStack::PopAndDestroy(2, file); // file, fs
			break;
			}
		case ECopyFile:
			{
			HBufC *sourceFile = ReadDescFromMessageLC(aMessage, 0);
			HBufC *targetFile = ReadDescFromMessageLC(aMessage, 1);
			RFs fs;
			User::LeaveIfError(fs.Connect());
			CleanupClosePushL(fs);
			
			CFileMan* fileManager = CFileMan::NewL(fs);
			CleanupStack::PushL(fileManager);
			err = fileManager->Copy(*sourceFile, *targetFile, 0);

			// Reset the read-only attribute on the copied file
			TTime time;
			User::LeaveIfError(fileManager->Attribs(*targetFile, 0, KEntryAttReadOnly, time));		
			CleanupStack::PopAndDestroy(4, sourceFile); // sourceFile, targetFile, fs, fileManager
			break;
			}
		default:
			{
			PanicClient(aMessage, EPanicIllegalFunction);
			break;
			}
		}
	
	if(EAddSoftwareType == f || EDeleteSoftwareType == f)
		{
		// create transaction on SCR for mutable operations
		Server().ScrSession().CommitTransactionL();
		
		TPckg<TInt> timePckg(actualTestDuration);
		TPckg<RScrAccessor::TAccessorOperationResult> opResultPckg(opResult);
			
		if(EDeleteSoftwareType == f)
			{
			aMessage.WriteL(0, opResultPckg);
			aMessage.WriteL(1, timePckg);
			}
		else if (EAddSoftwareType == f)
			{
			aMessage.WriteL(1, opResultPckg);
			aMessage.WriteL(2, timePckg);
			}
		}

	aMessage.Complete(err);
	}

void CScrAccessSession::ServiceError(const RMessage2& aMessage, TInt aError)
	{
	if (aError == KErrBadDescriptor)
		PanicClient(aMessage, EPanicBadDescriptor);
	CSession2::ServiceError(aMessage,aError);
	}




/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file contains the various test-cases for testing  CCafSisDataProvider, CDesDataProvider and CFileSisDataProvider class.
*
*/


/**
 @file tdataproviderstep.cpp
 @internalTechnology
*/

#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include "tdataproviderstep.h"
#include <e32def.h> 
#include <utf.h>
#include "sisinstallerrors.h"


using namespace Swi;


CDataProviderStep::~CDataProviderStep()
	{
	}

CDataProviderStep::CDataProviderStep()
	{
	}

TVerdict CDataProviderStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("DataProviderStep Read Test Step Preamble"));
	SetTestStepResult(EPass);

	return TestStepResult();
	}

/**
Description: Creates the dataprovider object at runtime and does the operations
exported by the dataprovider classes.  
@return:TVerdict - returns EFail if there is a failure else EPass
@internalTechnology:
@test:
*/
TVerdict CDataProviderStep::doTestStepL()
	{

	//Get the data provider type from the ini file
	__UHEAP_MARK;
	SetTestStepResult(EFail);

	TInt providerType;
	if(!GetIntFromConfig(ConfigSection(),_L("DataProviderType"),providerType))
		{
		ERR_PRINTF1(_L("DataProviderType not available in the ini file"));
		return TestStepResult();
		}

	TPtrC data;
	if(!GetStringFromConfig(ConfigSection(),_L("Data"),data))
		{
		ERR_PRINTF1(_L("DataProviderType not available in the ini file"));
		return TestStepResult();
		}
	
	TInt opType;	
	if(!GetIntFromConfig(ConfigSection(),_L("Operation"),opType))
		{
		ERR_PRINTF1(_L("Operation Type not available in the ini file"));
		return TestStepResult();
		}
		

	MSisDataProvider *sisDataProvider = NULL ;
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession); 
	switch(providerType)
		{	
		case EDesSisDataProvider:
			{
			HBufC8 *inData  = HBufC8::NewLC(data.Length());
			inData->Des().Copy(data);
			//creates the DesSisdataprovider obj
			sisDataProvider = CDesDataProvider::NewL(*inData);
			DoReadL(sisDataProvider, opType ); 
			CleanupStack::PopAndDestroy();
			break;
			}
		case EFileSisDataProvider:
			{
			TInt inputDataType;
			if(!GetIntFromConfig(ConfigSection(),_L("inputdatatype"),inputDataType))
				{
				ERR_PRINTF1(_L("Input Data type value not available in the ini file"));
				return TestStepResult();
				}
			switch(inputDataType)
				{
				case EFilename:
					{
					//creates the FileSisDataProvider object
					sisDataProvider = CFileSisDataProvider::NewL(fsSession,data);
					DoReadL(sisDataProvider, opType ); 
					break;
					}
				case EFileHandle:
					{			
					RFile file;
	    				User::LeaveIfError(file.Open(fsSession,data, EFileRead | EFileStream | EFileShareReadersOnly));
	    				CleanupClosePushL(file);
	    				//creates the FileSisDataProvider object
	    				sisDataProvider = CFileSisDataProvider::NewL(file);
	    	    			DoReadL(sisDataProvider, opType ); 
	    				CleanupStack::PopAndDestroy();		
					break;
					}				
				default:
					{
						ERR_PRINTF1(_L("Unknown Step type"));
						break;
					}				
				}				
			break;
			}
		case ECafSisDataProvider:
			{
			TInt inputStepType;
			if(!GetIntFromConfig(ConfigSection(),_L("inputsteptype"),inputStepType))
				{
				ERR_PRINTF1(_L("Input Step type value not available in the ini file"));
				return TestStepResult();
				}
			switch(inputStepType)
				{
				case EReadStep:
					{
					HBufC8 *inData  = HBufC8::NewLC(data.Length());
					inData->Des().Copy(data);
					TPtr8 pData(inData->Des());
					//creates the CafSisDataProvider object
					sisDataProvider = CCafSisDataProvider::NewL(data);
					sisDataProvider->Read(pData);
					SetTestStepResult(EPass);
					CleanupStack::PopAndDestroy();
					break;
					}
				case EOpenStep:
					{			
					TInt err;
					//creates the CafSisDataProvider object
					sisDataProvider = CCafSisDataProvider::NewL(data);
					sisDataProvider->OpenDrmContentL(ContentAccess::EUnknown);
					TRAP(err,sisDataProvider->OpenDrmContentL(ContentAccess::EUnknown)); 
					if(err == KErrNone)
						{
						SetTestStepResult(EPass);
						}
					break;
					}
				case ESeekStep:
					{
					//creates the CafSisDataProvider object
					sisDataProvider = CCafSisDataProvider::NewL(data);
					sisDataProvider->OpenDrmContentL(ContentAccess::EUnknown);
					DoReadL(sisDataProvider,opType);
					}
				default:
					{
					ERR_PRINTF1(_L("Unknown Data type"));
					break;
					}
				}
			break;
			}
		default:
			{
			ERR_PRINTF1(_L("Unknown Provider type"));
			break;
			}
		}
	if(sisDataProvider)
		{
		delete sisDataProvider ;
		}
	CleanupStack::PopAndDestroy();
	__UHEAP_MARKEND;
	return TestStepResult();
	}


/**
Description: Generic testing function for all three dataprovider types,exported by the dataprovider classes.  
@return:TVerdict - returns EFail if there is a failure else EPass
@internalTechnology:
@test:
*/
void CDataProviderStep::DoReadL(MSisDataProvider *aSisDataProvider,	const TInt& aOpType ) 
	{
	TInt seek;
	TPtrC pPosition;
	TInt64 position = 0;
	
	SetTestStepResult(EFail);
	
	if(aOpType == EReadAndSeek || aOpType == EReadSpecificLength)	
		{
		if(!GetIntFromConfig(ConfigSection(),_L("seek"),seek))
			{
			ERR_PRINTF1(_L("Seek value not available in the ini file"));
			return;
			}
		if(!GetStringFromConfig(ConfigSection(),_L("position"),pPosition))
			{
			ERR_PRINTF1(_L("Position value not available in the ini file"));
			return;
			}
		else
			{
			TLex16 lex(pPosition);
			lex.Val(position);
			}					
		}
		
	TInt length = 0;
	if(aOpType == EReadSpecificLength)	
		{
		if(!GetIntFromConfig(ConfigSection(),_L("length"),length))
			{
			ERR_PRINTF1(_L("Length value not available in the ini file"));
			return;
			}		
		}
				
	TInt bufSize = 0;
	if(!GetIntFromConfig(ConfigSection(),_L("bufsize"),bufSize))
		{
		ERR_PRINTF1(_L("buf size not available in the ini file"));
		return;
		}	
			
	TPtrC expectedSisData;
	if(!GetStringFromConfig(ConfigSection(),_L("expecteddata"),expectedSisData))
		{
		ERR_PRINTF1(_L("expecteddata not available in the ini file"));
		return;
		}

	HBufC8 *outData  = HBufC8::NewLC(bufSize);
	TPtr8 pdata(outData->Des());
	pdata.SetLength(bufSize);
	TInt err = 0;
	switch(aOpType)
		{
		case EReadAndSeek:
			{
			//this case will carry on into EReadOnly case
			TInt64 pos = (TInt64)position; 
			err = aSisDataProvider->Seek(static_cast<TSeek>(seek),pos);
			if (err !=  KErrNone )
				{
				ERR_PRINTF2(_L("Seek returns Error=%d"),err);
				_LIT8(KERR , "%d");
				pdata.Format(KERR() , err);
				VerifyResult(pdata ,expectedSisData);
				break;
				}
			}
		case EReadOnly:
			{
			err = aSisDataProvider->Read(pdata);
			if(err == KErrSISFieldBufferTooShort)
				{
				ERR_PRINTF1(_L("Seek returns  KErrSISFieldBufferTooShort"));
				_LIT8(KERR , "%d");
				pdata.Format(KERR() , err);
				VerifyResult(pdata ,expectedSisData);
				break;	
				}
			VerifyResult(pdata ,expectedSisData);
			break;	
			}
		case EReadSpecificLength:
			{
			TInt64 pos = (TInt64)position; 
			err = aSisDataProvider->Seek(static_cast<TSeek>(seek),pos);
			if (err !=  KErrNone )
				{
				ERR_PRINTF2(_L("Seek returns Error=%d"),err);
				_LIT8(KERR , "%d");
				pdata.Format(KERR() , err);
				VerifyResult(pdata ,expectedSisData);
				break;
				}
			err = aSisDataProvider->Read(pdata , length);
			if(err == KErrSISFieldBufferTooShort)
				{
				ERR_PRINTF1(_L("Seek returns  KErrSISFieldBufferTooShort"));
				_LIT8(KERR , "%d");
				pdata.Format(KERR() , err);
				VerifyResult(pdata ,expectedSisData);
				break;
				}
			VerifyResult(pdata ,expectedSisData);
			break;	
			}
		default:
			{
			ERR_PRINTF1(_L("Unknown Operation type in the ini "));
			SetTestStepResult(EFail);	
			break;
			}
		}	
		CleanupStack::PopAndDestroy();		
	}

TVerdict CDataProviderStep::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("DataProviderStep Read Test Step Postamble"));	
	return TestStepResult();
	}
	
void  CDataProviderStep::VerifyResult (const TPtr8& aData , const TPtrC16& aExpectedSisData)
	{
	TBuf16<KMAXBUF> temp ;	
	temp.Copy(aData);
	if( aExpectedSisData.Compare(temp) == 0)
		{
		INFO_PRINTF1(_L("Verify Result Passed"));	
		SetTestStepResult(EPass);	
		}
	else
		{
		INFO_PRINTF1(_L("Verify Result failed"));	
		SetTestStepResult(EFail);
		}
	}
	
	

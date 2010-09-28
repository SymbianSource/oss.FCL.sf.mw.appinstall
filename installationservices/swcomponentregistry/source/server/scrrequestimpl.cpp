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
* Defines the class which implements SCR's service requests.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#include "scrrequestimpl.h"
#include "scrdatabase.h"
#include "screntries.h"
#include "scrserver.h"
#include "scrcommon.h"
#include "scrsubsession.h"
#include "usiflog.h"
#include "usiferror.h"
#include "scrrepository.h"
#include <s32mem.h>
#include <bautils.h>
#include <scs/streamingarray.h>
#include <scs/ipcstream.h>
#include <scs/ipcstream.inl>
#include <scs/cleanuputils.h>
#include <e32hashtab.h>
#include <usif/sts/sts.h>

using namespace Usif;

_LIT(KComponentIdColumnName, "ComponentId"); 
_LIT(KCompFileIdColumnName, "CmpFileId"); 
_LIT(KAppIdColumnName, "AppUid");
_LIT(KComponentPropertiesTable, "ComponentProperties");
_LIT(KFilePropertiesTable, "FileProperties");
_LIT(KFileOwnershipInfoTable, "FileOwnershipInfo");
_LIT(KServiceInfoTable, "ServiceInfo");
_LIT(KLocalizableAppInfoTable, "LocalizableAppInfo");
_LIT(KViewDataTable, "ViewData");
_LIT(KAppRegistrationInfoTable, "AppRegistrationInfo");
_LIT(KAppPropertiesTable, "AppProperties");


/** Maximum number of log records could be recorded by SCR. */
// It is estimated that a thousand log entries create 40K-100K log file.
const TInt KMaxScrLogEntries = 1000;
/** The file path of SCR log file. */
_LIT(KScrLogFileName, "!:\\private\\10285bc0\\scr.log");
/** The file path of SCR temporary log file. */
_LIT(KScrTempLogFileName, "!:\\private\\10285bc0\\scr_tmp.log");
	
const TUint KDbInterfaceMajorVersion = 1;
const TUint KDbInterfaceMinorVersion = 1;
#ifdef _DEBUG
const TUint KDbInterfaceBuildNumber = 1;
#endif

enum TScrPanicId
	{
	KScrIllegalCallSequence = 1,
	KScrIllegalParameter = 2
	};

static void PanicClient(const RMessagePtr2& aMessage, TScrPanicId aPanic)
	{
	_LIT(KPanicMessage, "ScrServer");
    aMessage.Panic(KPanicMessage, aPanic);
    }

HBufC* UpdateFilePathDriveLC(const TDesC& aFilePath, TChar aDrive)
	{
	HBufC *fileName = HBufC::NewLC(aFilePath.Length());
	TPtr ptrFileName(fileName->Des());
	ptrFileName.Copy(aFilePath);
	ptrFileName[0] = aDrive;
	return fileName; 
	}	
	
////////////////////////
// CScrRequestImpl
////////////////////////

CScrRequestImpl::CScrRequestImpl(RFs& aFs)
	: iFs(aFs)
	{
	}

CScrRequestImpl::~CScrRequestImpl()
	{
	delete iDbHandle;
	delete iComponentEntry;
	iProperties.ResetAndDestroy();
	delete iSingleProperty;
	iFileComponents.Close();
	iVerCompIdList.ResetAndDestroy();
	iDeletedMimeTypes.ResetAndDestroy();
	iLogEntries.ResetAndDestroy();
	iMatchingSupportedLanguageList.Close();
	
    //Delete the instance of CScrRepository, if present.
    CScrRepository::DeleteRepositoryInstance();
	}

CScrRequestImpl* CScrRequestImpl::CScrRequestImpl::NewL(RFs& aFs, RFile& aDatabaseFile, RFile& aJournalFile)
	{
	CScrRequestImpl *self = new(ELeave) CScrRequestImpl(aFs);
	CleanupStack::PushL(self);
	self->ConstructL(aDatabaseFile, aJournalFile);
	CleanupStack::Pop(self);
	return self;
	}

void CScrRequestImpl::ConstructL(RFile& aDatabaseFile, RFile& aJournalFile)
	{
	iDbHandle = CDatabase::NewL(aDatabaseFile, aJournalFile);
	InitializeDbVersionL();
	VerifyDbVersionCompatibilityL();
	// Make sure that private directory exists - required for temporary and log files
	_LIT(KPrivateDirPath, "!:\\private\\10285bc0\\");
	HBufC* privateDirPath = UpdateFilePathDriveLC(KPrivateDirPath, iFs.GetSystemDriveChar());
	TInt res = iFs.MkDirAll(*privateDirPath);
	__ASSERT_ALWAYS(res == KErrNone || res == KErrAlreadyExists, User::Leave(res));
	CleanupStack::PopAndDestroy(privateDirPath);
	}

HBufC* CScrRequestImpl::ReadDescLC(const RMessage2& aMessage, TInt aSlot)
	{
	TInt len = aMessage.GetDesLengthL(aSlot);
	HBufC* desc = HBufC::NewLC(len);
	TPtr ptrDesc(desc->Des());
	aMessage.ReadL(aSlot, ptrDesc);
	return desc;
	}

HBufC8* CScrRequestImpl::ReadDesc8LC(const RMessage2& aMessage, TInt aSlot)
	{
	TInt len = aMessage.GetDesLengthL(aSlot);
	HBufC8* desc = HBufC8::NewLC(len);
	TPtr8 ptrDesc(desc->Des());
	aMessage.ReadL(aSlot, ptrDesc);
	return desc;
	}

HBufC* CScrRequestImpl::FormatStatementLC(const TDesC& aStatement, TInt aFormattedLength,...) const
	{
	VA_LIST list;
	VA_START(list, aFormattedLength);
	HBufC* statementStr = HBufC::NewLC(aStatement.Length() + aFormattedLength + 1);
	TPtr statementStrPtr(statementStr->Des()); 
	statementStrPtr.FormatList(aStatement, list);
	VA_END(list);
	
	// SQLite requires the statement string to end with NULL char.
	statementStrPtr.Append('\0');
	return statementStr;
	}

void CScrRequestImpl::CreateTransactionL()
	{
	DEBUG_PRINTF(_L8("Create Transaction request received."));
	_LIT(KBeginTransaction, "BEGIN IMMEDIATE;");
	ExecuteStatementL(KBeginTransaction());	
	DEBUG_PRINTF(_L8("Transaction has begun!"));
	}

void CScrRequestImpl::RollbackTransactionL()
	{
	DEBUG_PRINTF(_L8("Rollback Transaction request received."));
	_LIT(KRollbackTransaction, "ROLLBACK;");
	ExecuteStatementL(KRollbackTransaction());
	DEBUG_PRINTF(_L8("Transaction has been rolled back successfuly!"));
	}

void CScrRequestImpl::CommitTransactionL()
	{
	DEBUG_PRINTF(_L8("Commit Transaction request received."));
	_LIT(KCommitTransaction, "COMMIT;");
	ExecuteStatementL(KCommitTransaction());
	DEBUG_PRINTF(_L8("Transaction has been committed successfully!"));
	}

HBufC* CScrRequestImpl::GenerateGlobalIdL(const TDesC& aUniqueSwTypeName, const TDesC& aGlobalId) const
	{ 
	DEBUG_PRINTF(_L8("Generating Global ID."));
	HBufC *globalIdBuf = HBufC::NewL(aUniqueSwTypeName.Length() + aGlobalId.Length() + 1); // 1 is extra memory to put NULL char
	TPtr globalIdDes(globalIdBuf->Des());
	
	globalIdDes.Copy(aUniqueSwTypeName);
	globalIdDes.Append('\0');
	globalIdDes.Append(aGlobalId);
	
	return globalIdBuf;
	}

TComponentId CScrRequestImpl::CommonAddComponentL(const TDesC& aUniqueSwTypeName, const TDesC& aGlobalId)
	{
	DEBUG_PRINTF(_L8("Adding a base component."));
	// Get the current time
	TTime time;
	time.HomeTime();
	TDateTime datetime = time.DateTime();
	TBuf<16> installTime;
	// We cannot use TTime::FormatL, since it starts months and days with 01, which is not accepted by TTime::Set() which starts them from 0
	installTime.Format(_L("%04d%02d%02d:%02d%02d%02d"), datetime.Year(), datetime.Month(), datetime.Day(), datetime.Hour(), datetime.Minute(), datetime.Second());
	
	// SoftwareTypeId is the hash of unique software type name. For more information, see CScrRequestImpl::AddSoftwareTypeL.
	// In Components table, both SoftwareTypeId and unique SoftwareTypeName is stored so that the software type name of a component
	// can be returned even if the corresponding installer (software type) has been uninstalled. The unique sofware type name
	// might be used by the client to identify the installer required to uninstall the component.
	TUint32 swTypeId = HashCaseSensitiveL(aUniqueSwTypeName);
	
	if(aGlobalId.CompareF(KNullDesC()))
		{ // Global Id is supplied by client
		HBufC *globalId = GenerateGlobalIdL(aUniqueSwTypeName, aGlobalId);
		CleanupStack::PushL(globalId);
		TUint32 globalIdHash = HashCaseSensitiveL(*globalId);
		
		_LIT(KInsertComponent,"INSERT INTO Components(SoftwareTypeId,SoftwareTypeName,GlobalIdHash,GlobalId,InstallTime) VALUES(?,?,?,?,?);");
		TInt numberOfValues = 5;
		ExecuteStatementL(KInsertComponent(), numberOfValues, EValueInteger, swTypeId, EValueString, &aUniqueSwTypeName, EValueInteger, globalIdHash, EValueString, globalId, EValueString, &installTime);
		CleanupStack::PopAndDestroy(globalId);
		}
	else
		{// Global Id is NOT supplied by client
		_LIT(KInsertComponent,"INSERT INTO Components(SoftwareTypeId,SoftwareTypeName,InstallTime) VALUES(?,?,?);");
		TInt numberOfValues = 3;
		ExecuteStatementL(KInsertComponent(), numberOfValues, EValueInteger, swTypeId, EValueString, &aUniqueSwTypeName, EValueString, &installTime);
		}
	
	DEBUG_PRINTF(_L8("The new component has been added to Components table successfully."));
	return iDbHandle->LastInsertedIdL();
	}

void CScrRequestImpl::AddComponentLocalizablesL(TComponentId aCompId, TLanguage aLocale, const TDesC& aName, const TDesC& aVendor)
	{
	DEBUG_PRINTF4(_L("Adding the component(%d) localizables: name(%S) and vendor(%S)."), aCompId, &aName, &aVendor);
	
	if(!aName.CompareF(KNullDesC()))
		{// Component name cannot be empty
		DEBUG_PRINTF(_L8("Component name cannot be NULL string."));
		User::Leave(KErrArgument);
		}
	
	_LIT(KInsertComponentLocalizables, "INSERT INTO ComponentLocalizables(ComponentId,Locale,Name,Vendor) VALUES(?,?,?,?);");
	TInt numberOfValues = 4;
	ExecuteStatementL(KInsertComponentLocalizables(), numberOfValues, EValueInteger, aCompId, EValueInteger, aLocale, EValueString, &aName, EValueString, &aVendor);
	
	DEBUG_PRINTF(_L8("The component localizables have been added successfully."));
	}

void CScrRequestImpl::ComponentRollback(TAny* aParam)
	{
	TRollbackParams *param = static_cast<TRollbackParams*>(aParam);
	_LIT(KDeleteComponents, "DELETE FROM Components WHERE ComponentId=?;");
	TInt numberOfValues = 1;
	TRAP_IGNORE(param->iReqImplHandle.ExecuteStatementL(KDeleteComponents, numberOfValues, EValueInteger, param->iIdColumnVal););
	}
	
void CScrRequestImpl::ComponentLocalizablesRollback(TAny* aParam)
	{
	TRollbackParams *param = static_cast<TRollbackParams*>(aParam);
	_LIT(KDeleteComponentLocalizables, "DELETE FROM ComponentLocalizables WHERE ComponentId=?;");
	TInt numberOfValues = 1;
	TRAP_IGNORE(param->iReqImplHandle.ExecuteStatementL(KDeleteComponentLocalizables, numberOfValues, EValueInteger, param->iIdColumnVal););
	}

void CScrRequestImpl::AddComponentL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Adding a new component."));
	// First check if the supplied operation type is valid
	TScrComponentOperationType compOpType;
	TPckg<TScrComponentOperationType> opTypePckg(compOpType);
	aMessage.ReadL(3, opTypePckg, 0);
		
	if(compOpType != EScrCompInstall && compOpType != EScrCompUpgrade && compOpType != EScrCompHidden)
		{
		DEBUG_PRINTF2(_L("Unexpected component operation type (%d) was provided!"), static_cast<TInt>(compOpType));
		User::Leave(KErrArgument);
		}
	
	RIpcReadStream componentInfoReader;
	CleanupClosePushL(componentInfoReader);
	componentInfoReader.Open(aMessage, 1);
	
	RPointerArray<CLocalizableComponentInfo> componentInfoArray;
	CleanupResetAndDestroyPushL(componentInfoArray);
	InternalizePointersArrayL(componentInfoArray, componentInfoReader);
	TInt arrayCount = componentInfoArray.Count();
	// This check is done at the client side - if we get this condition here, this means that a rogue client-server call bypassed the R-class
	__ASSERT_ALWAYS(arrayCount > 0, PanicClient(aMessage, KScrIllegalParameter));

	HBufC *uniqueSwTypeName = GetSoftwareTypeNameFromMsgLC(aMessage);
	HBufC *globalId = ReadDescLC(aMessage, 2);
	TComponentId newComponentId = CommonAddComponentL(*uniqueSwTypeName, *globalId);
	
	// Add the newly added component into the cleanupstack so that it can be deleted in case of leaving.
	TRollbackParams cleanupParam(newComponentId, *this);
	CleanupStack::PushL(TCleanupItem(CScrRequestImpl::ComponentRollback, &cleanupParam));
	// Add the component localizables cleanup into the cleanupstack so that they can be deleted in case of leaving.
	CleanupStack::PushL(TCleanupItem(CScrRequestImpl::ComponentLocalizablesRollback, &cleanupParam));
	
	// For a log record, the component name is chosen for the current locale. If it doesn't exist, 
	// then the non-localized component name is picked. If none of them exist, the first name in the names 
	// array is used as default.
	TInt componentNameIndexForLog (0);
	TInt indexForCurrentLocale = KErrNotFound;
	TInt indexForNonLocalized = KErrNotFound;
	for(TInt i=0; i<arrayCount; ++i)
		{
		TLanguage locale(componentInfoArray[i]->Locale());
		const TDesC& name = componentInfoArray[i]->NameL();
		const TDesC& vendor = componentInfoArray[i]->VendorL();
		AddComponentLocalizablesL(newComponentId, locale, name, vendor);
		if(locale == User::Language())
			indexForCurrentLocale = i;
		else if(locale == KNonLocalized)
			indexForNonLocalized = i;
		}
	CleanupStack::Pop(2, &cleanupParam); // cleanupitem for ComponentRollback, cleanupitem for ComponentLocalizablesRollback
	
	if(indexForCurrentLocale != KErrNotFound)
		componentNameIndexForLog = indexForCurrentLocale;
	else if(indexForNonLocalized != KErrNotFound)
		componentNameIndexForLog = indexForNonLocalized;
	
	if(EScrCompHidden != compOpType)
		{
		CScrLogEntry *logRecord = CScrLogEntry::NewLC(componentInfoArray[componentNameIndexForLog]->NameL(), *uniqueSwTypeName, *globalId, KNullDesC, compOpType);
		logRecord->iComponentId = newComponentId;
		iLogEntries.AppendL(logRecord);
		CleanupStack::Pop(logRecord); // Ownershipd is transferred
		}
	CleanupStack::PopAndDestroy(4, &componentInfoReader); // componentInfoReader, componentInfoArray, uniqueSwTypeName, globalId
	
	TPckg<TComponentId> componentIdDes(newComponentId);
	aMessage.WriteL(3, componentIdDes);
	
	DEBUG_PRINTF(_L8("New component added successfully."));
	}

HBufC* CScrRequestImpl::ReadAndGetGlobalIdLC(const RMessage2& aMessage, TInt aSlot) const
	{
	// Read global id object from the message
	CGlobalComponentId *globalId = ReadObjectFromMessageLC<CGlobalComponentId>(aMessage, aSlot);
	HBufC *globalIdBuf = GenerateGlobalIdL(globalId->SoftwareTypeName(), globalId->GlobalIdName());
	CleanupStack::PopAndDestroy(globalId);
	CleanupStack::PushL(globalIdBuf);
	return globalIdBuf;
	}

TUint32 CScrRequestImpl::HashGlobalIdsL(const TDesC& aDependantId, const TDesC& aSupplierId) const
	{
	RBuf concatGlobalIds;
	concatGlobalIds.CreateL(aDependantId.Length() + aSupplierId.Length());
	concatGlobalIds.CleanupClosePushL();
	concatGlobalIds.Copy(aDependantId);
	concatGlobalIds.Append(aSupplierId);
	TUint32 globalIdsHash = HashCaseSensitiveL(concatGlobalIds);
	CleanupStack::PopAndDestroy(&concatGlobalIds);
	return globalIdsHash;
	}

void CScrRequestImpl::AddComponentDependencyL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Adding a new component dependency."));
	
	CVersionedComponentId *verCompId = ReadObjectFromMessageLC<CVersionedComponentId>(aMessage, 1);
	HBufC *suppGlobalId = GenerateGlobalIdL(verCompId->GlobalId().SoftwareTypeName(), verCompId->GlobalId().GlobalIdName());
	CleanupStack::PushL(suppGlobalId);
	TUint32 suppGlobalIdHash = HashCaseSensitiveL(*suppGlobalId);
	
	HBufC *depGlobalId = ReadAndGetGlobalIdLC(aMessage, 2);
	TUint32 depGlobalIdHash = HashCaseSensitiveL(*depGlobalId);
	
	TUint32 globalIdsHash = HashGlobalIdsL(*depGlobalId, *suppGlobalId);
	
	const TDesC* versionFrom = verCompId->VersionFrom();
	const TDesC* versionTo = verCompId->VersionTo();
	
	_LIT(KInsertDependencyFront, "INSERT INTO ComponentDependencies(GlobalIdHash,DependantGlobalIdHash,SupplierGlobalIdHash,DependantGlobalId,SupplierGlobalId");
	_LIT(KInsertDependencyEnd, ") VALUES(?,?,?,?,?");

	const TInt KMaxInsertDependencyLength = 180;     ///< Maximum length of a possible dependency insert statement (KInsertDependencyFront + KInsertDependencyEnd + some space for optional columns)
	const TInt KMaxInsertDependencyValueLength = 30; ///< Maximum length of the end part of a possible dependency insert statement (KInsertDependencyEnd + some space for optional values).

	TInt numberOfValues = 5; // 5->(GlobalIdHash, DependantGlobalIdHash, SupplierGlobalIdHash, DependantGlobalId, SupplierGlobalId)
	RBuf insertStmtStr;
	insertStmtStr.CreateL(KMaxInsertDependencyLength);
	insertStmtStr.CleanupClosePushL();
	insertStmtStr.Copy(KInsertDependencyFront());
	
	RBuf insertStmtEndStr;
	insertStmtEndStr.CreateL(KMaxInsertDependencyValueLength);
	insertStmtEndStr.CleanupClosePushL();
	insertStmtEndStr.Copy(KInsertDependencyEnd());
	
	// VersionFrom and VersionTo values are optional. Hence, a dynamic insert statement will be created.
	// insertStmtStr is initialized with the statement including mandatory columns. Then versionFrom/To
	// values are checked. If any of them is provided, then the corresponding column name is added to the statement.
	// insertStmtEndStr is constructed with a number of value characters(?) as new value chars will be added
	// if new columns are added to the statement
	// In the end, insertStmtEndStr is added to insertStmtStr in order to complete the dependency insert statement.
	
	const TDesC* firstVersionParam(0);
	const TDesC* secondVersionParam(0);
	
	_LIT(KInsertValue, ",?");
	if(versionFrom)
		{
		_LIT(KVersionFrom, ",VersionFrom");
		insertStmtStr.Append(KVersionFrom());
		insertStmtEndStr.Append(KInsertValue());
		firstVersionParam = versionFrom;
		++numberOfValues;
		}
	
	if(versionTo)
		{
		_LIT(KVersionTo, ",VersionTo");
		insertStmtStr.Append(KVersionTo());
		insertStmtEndStr.Append(KInsertValue());
		if(firstVersionParam)
			{
			secondVersionParam = versionTo;
			}
		else
			{
			firstVersionParam = versionTo;
			}
		++numberOfValues;
		}
	
	// Append the end part of the statement to the front part.
	insertStmtStr.Append(insertStmtEndStr);
	// Close the statement
	_LIT(KInsertClose,");");
	insertStmtStr.Append(KInsertClose());
	// SQLite requires the statement string to end with NULL char.
	insertStmtStr.Append('\0');
	// All parameters are supplied, but numberOfValues of them will be taken into account.
	ExecuteStatementL(insertStmtStr, numberOfValues, EValueInteger, globalIdsHash, EValueInteger, depGlobalIdHash, EValueInteger, suppGlobalIdHash, EValueString, depGlobalId, EValueString, suppGlobalId, EValueString, firstVersionParam, EValueString, secondVersionParam);
	
	CleanupStack::PopAndDestroy(5, verCompId); // verCompId, suppGlobalId, depGlobalId, insertStmtStr, insertStmtEndStr
	
	DEBUG_PRINTF(_L8("New component dependency added successfully."));
	}

CStatement* CScrRequestImpl::CreateGeneralPropertyStatementWithLocaleL(const TDesC& aStmtStr, TInt aIdColumnValue, TLanguage aLocale, const TDesC& aPropName, TBool aDoLocaleResolving) const
	{
	CStatement *stmt(0);
	TInt numberOfValues = 3;
	
	if(KUnspecifiedLocale == aLocale)
		{
		// No locale is specified. Client wants SCR to find the property automatically.
		// So, first look up the properties for the current locale and its downgraded languages.
		ASSERT(aDoLocaleResolving); // We cannot get a situation where KUnspecifiedLocale is given with locale resolving disabled: it is an undefined state.
		stmt = CreateStatementObjectWithLocaleL(aStmtStr, User::Language(), numberOfValues, EValueString, &aPropName, EValueInteger, aIdColumnValue, EValueLanguage);
		
		if(!stmt)
			{
			// No property is defined with the current locale. Look up the non-localized properties.
			stmt = CreateStatementObjectWithLocaleL(aStmtStr, KNonLocalized, numberOfValues, EValueString, &aPropName, EValueInteger, aIdColumnValue, EValueLanguage);
			}
		}
	else if(KNonLocalized == aLocale)
		{
		// Non-localized properties are requested. Look up the non-localized properties.
		stmt = CreateStatementObjectWithLocaleL(aStmtStr, KNonLocalized, numberOfValues, EValueString, &aPropName, EValueInteger, aIdColumnValue, EValueLanguage);
		}
	else
		{
		// A particular language is specified. Look up the properties with for this particular language
		// and its downgraded languages.
		stmt = aDoLocaleResolving ? 
			CreateStatementObjectWithLocaleL(aStmtStr, aLocale, numberOfValues, EValueString, &aPropName, EValueInteger, aIdColumnValue, EValueLanguage, aDoLocaleResolving) :
			CreateStatementObjectWithLocaleNoDowngradeL(aStmtStr, aLocale, numberOfValues, EValueString, &aPropName, EValueInteger, aIdColumnValue, EValueLanguage, aDoLocaleResolving);
		}
	return stmt;
	}

TInt CScrRequestImpl::FindGeneralPropertyNoLocaleDowngradeL(const TDesC& aTableName, const TDesC& aIdColumnName, TInt aIdColumnValue, const TDesC& aPropertyName, TLanguage aLocale, TPropertyType& aPropertyType) const
	{
	_LIT(KFindProperty, "SELECT PropertyId,IntValue,StrValue,IsStr8Bit FROM %S WHERE Name=? AND %S=? AND Locale=?;");
	TInt formattedLen = aTableName.Length() + aIdColumnName.Length();
	HBufC *statementStr = FormatStatementLC(KFindProperty(), formattedLen, &aTableName, &aIdColumnName);

	// As we do not require downgrading locales, the last parameter is EFalse
	CStatement *stmtFind = CreateGeneralPropertyStatementWithLocaleL(*statementStr, aIdColumnValue, aLocale, aPropertyName, EFalse);
	if(!stmtFind)
		{ // the property does not exist, return
		CleanupStack::PopAndDestroy(statementStr);
		return KErrNotFound;
		}
	CleanupStack::PushL(stmtFind);
	
	// the property in question exists in the properties table 
	TInt propertyId = stmtFind->IntColumnL(0);					// 0 -> PropertyId		
	TBool isIntPropertyNull = stmtFind->IsFieldNullL(1); 	// 2 -> IntValue
	TBool isStrPropertyNull = stmtFind->IsFieldNullL(2); 	// 3 -> StrValue
			
	if(!isIntPropertyNull)
		{
		aPropertyType = CScrRequestImpl::EPropertyInteger;
		}
	else if(!isStrPropertyNull)
		{
		TBool is8BitString = (stmtFind->IntColumnL(3) == 1);
		aPropertyType = is8BitString ? CScrRequestImpl::EPropertyBinary : CScrRequestImpl::EPropertyLocalizable;
		}
	else // None of the columns is defined - corrupt DB 
		{
		DEBUG_PRINTF(_L8("Unexpected situation! Neither int value nor str value are defined for a property"));
		User::Leave(KErrCorrupt);
		}
	CleanupStack::PopAndDestroy(2, statementStr); // stmtFind, statementStr
	return propertyId;
	}

void CScrRequestImpl::BindStatementValuesL(CStatement& aStatement, TLanguage aLanguage, TInt aValuesNum, VA_LIST aList) const
	{
	TInt bindIdx(0);
	while(aValuesNum--)
		{
		CScrRequestImpl::TValueType t = static_cast<CScrRequestImpl::TValueType>(VA_ARG(aList, TInt));
		switch(t)
			{
			case EValueString:
				{
				const TDesC* strVal = VA_ARG(aList, const TDesC*);
				aStatement.BindStrL(++bindIdx, *strVal);
				break;
				}
			case EValueInteger:
				{
				TInt intVal = VA_ARG(aList, TInt);
				aStatement.BindIntL(++bindIdx, intVal);
				break;
				}
			case EValueInteger64:
				{
				TInt64 intVal64 = VA_ARG(aList, TInt64);
				aStatement.BindInt64L(++bindIdx, intVal64);
				break;
				}
			case EValueLanguage:
				{
				aStatement.BindIntL(++bindIdx, aLanguage);
				break;
				}
			case EValueBinary:
				{
				const TDesC8* binaryVal = VA_ARG(aList, const TDesC8*);
				aStatement.BindBinaryL(++bindIdx, *binaryVal);
				break;
				}				
			default:
				DEBUG_PRINTF2(_L8("Encountered unexpected value type (%d)!"), t);
				ASSERT(0);
			} // switch
		} // while
	} // End of the function

void CScrRequestImpl::ExecuteStatementL(TRefByValue<const TDesC> aStatement, TInt aValuesNum,...)
	{// TRefByValue is used to suppress rcvt compile warning
	VA_LIST argList;
	VA_START(argList, aValuesNum);
	
	CStatement *stmt = iDbHandle->PrepareStatementLC(aStatement);
	BindStatementValuesL(*stmt, KLangNone, aValuesNum, argList);
	stmt->ExecuteStatementL();
	CleanupStack::PopAndDestroy(stmt);
	
	VA_END(argList);
	}

void CScrRequestImpl::SetGeneralLocalizablePropertyL(CScrRequestImpl::TPropertyType aPropType, TInt aPropertyId, const TDesC& aIdColumnName, TInt aIdColumnValue, const TDesC& aPropName, TLanguage aLocale, const TDesC& aPropValue)
	{
	switch(aPropType)
		{
		case CScrRequestImpl::EPropertyUndefined:
			{// the property does NOT exist, insert a new one
			_LIT(KInsertProperty, "INSERT INTO ComponentProperties(Name,Locale,%S,StrValue) VALUES(?,?,?,?);");
			TInt formattedLen = aIdColumnName.Length();
			HBufC *statementStr = FormatStatementLC(KInsertProperty(), formattedLen, &aIdColumnName );
			TInt numberOfValues = 4;
			ExecuteStatementL(*statementStr, numberOfValues, EValueString, &aPropName, EValueInteger, aLocale, EValueInteger, aIdColumnValue, EValueString, &aPropValue);
			CleanupStack::PopAndDestroy(statementStr);
			break;
			}
		case CScrRequestImpl::EPropertyLocalizable: 
			{
			// the property exists, update it
			_LIT(KUpdateProperty, "UPDATE ComponentProperties SET StrValue=? WHERE PropertyId=?;");
			TInt numberOfValues = 2;
			ExecuteStatementL(KUpdateProperty(), numberOfValues, EValueString, &aPropValue, EValueInteger, aPropertyId);
			break;
			}
		default:
			DEBUG_PRINTF(_L8("The property type isn't localizable string and cannot be updated with this API."))
			User::Leave(KErrAbort);
		}
	}

void CScrRequestImpl::SetGeneralBinaryPropertyL(CScrRequestImpl::TPropertyType aPropType, TInt aPropertyId, const TDesC& aTableName, const TDesC& aIdColumnName, TInt aIdColumnValue, const TDesC& aPropName, const TDesC8& aPropValue)
	{
	switch(aPropType)
		{
		case CScrRequestImpl::EPropertyUndefined:
			{// the property does NOT exist, insert a new one
			_LIT(KInsertProperty, "INSERT INTO %S(Name,%S,StrValue,IsStr8Bit) VALUES(?,?,?,1);");
			TInt formattedLen = aTableName.Length() + aIdColumnName.Length();
			HBufC *statementStr = FormatStatementLC(KInsertProperty(), formattedLen, &aTableName, &aIdColumnName );
			TInt numberOfValues = 3;
			ExecuteStatementL(*statementStr, numberOfValues, EValueString, &aPropName, EValueInteger, aIdColumnValue, EValueBinary, &aPropValue);
			CleanupStack::PopAndDestroy(statementStr);
			break;
			}
		case CScrRequestImpl::EPropertyBinary: 
			{
			// the property exists, update it
			_LIT(KUpdateProperty, "UPDATE %S SET StrValue=? WHERE PropertyId=?;");
			HBufC *statementStr = FormatStatementLC(KUpdateProperty(), aTableName.Length(), &aTableName);
			TInt numberOfValues = 2;
			ExecuteStatementL(*statementStr, numberOfValues, EValueBinary, &aPropValue, EValueInteger, aPropertyId);
			CleanupStack::PopAndDestroy(statementStr);
			break;
			}
		default:
			DEBUG_PRINTF(_L8("The property type isn't an 8-bit string and cannot be updated with this API."))
			User::Leave(KErrAbort);
		}
	}

void CScrRequestImpl::SetGeneralIntPropertyL(TPropertyType aPropType, TInt aPropertyId, const TDesC& aTableName, const TDesC& aIdColumnName , TInt aIdColumnValue, const TDesC& aPropName, TInt64 aPropValue)
	{
	switch(aPropType)
		{
		case CScrRequestImpl::EPropertyUndefined:
			{// the property does NOT exist, insert a new one
			_LIT(KInsertProperty, "INSERT INTO %S(Name,%S,IntValue) VALUES(?,?,?)");
			TInt formattedLen = aTableName.Length() + aIdColumnName.Length();
			HBufC* statementStr = FormatStatementLC(KInsertProperty(), formattedLen, &aTableName, &aIdColumnName );
			TInt numberOfValues = 3;
			ExecuteStatementL(*statementStr, numberOfValues,EValueString, &aPropName, EValueInteger, aIdColumnValue, EValueInteger64, aPropValue);
			CleanupStack::PopAndDestroy(statementStr);
			break;
			}
		case CScrRequestImpl::EPropertyInteger:
			{// the property exists, update it
			_LIT(KUpdateProperty, "UPDATE %S SET IntValue=? WHERE PropertyId=?;");
			HBufC *statementStr = FormatStatementLC(KUpdateProperty(), aTableName.Length(), &aTableName);
			TInt numberOfValues = 2;
			ExecuteStatementL(*statementStr, numberOfValues, EValueInteger64, aPropValue, EValueInteger, aPropertyId);
			CleanupStack::PopAndDestroy(statementStr);
			break;
			}
		default:
			DEBUG_PRINTF(_L8("The property type isn't integer and cannot be updated with this API."));
			User::Leave(KErrAbort);
		}
	}

template <class A> void VerifySetPropertyParamsL(HBufC* aPropName, A* aPropValue)
	{
	if (!aPropName || !aPropValue)
		User::Leave(KErrArgument);	
	}
	
void CScrRequestImpl::SetComponentLocalizablePropertyL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Setting component string property."));
	// N.B. Don't change the order of IPC parameter reading, otherwise fails in UREL mode.
	HBufC *propName = ReadDescLC(aMessage, 1);
	HBufC *propValue = ReadDescLC(aMessage, 2);
	VerifySetPropertyParamsL(propName, propValue);

	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	TLanguage locale = TLanguage(aMessage.Int3());
	
	CScrRequestImpl::TPropertyType propType(CScrRequestImpl::EPropertyUndefined);
	// The function name states that we don't want automatic locale resolving. If, e.g. we set a property for ELangAmerican, we don't want FindGeneralProperty to match existing value for ELangEnglish
	TInt propertyId = FindGeneralPropertyNoLocaleDowngradeL(KComponentPropertiesTable(), KComponentIdColumnName(), componentId, *propName, locale, propType);
	SetGeneralLocalizablePropertyL(propType, propertyId, KComponentIdColumnName(), componentId, *propName, locale, *propValue);
	CleanupStack::PopAndDestroy(2, propName);
	}

void CScrRequestImpl::SetComponentBinaryPropertyL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Setting component binary property."));
	// N.B. Don't change the order of IPC parameter reading, otherwise fails in UREL mode.
	HBufC *propName = ReadDescLC(aMessage, 1);
	HBufC8 *propValue = ReadDesc8LC(aMessage, 2);
	
	VerifySetPropertyParamsL(propName, propValue);
	
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	
	CScrRequestImpl::TPropertyType propType(CScrRequestImpl::EPropertyUndefined);
	TInt propertyId = FindGeneralPropertyNoLocaleDowngradeL(KComponentPropertiesTable(), KComponentIdColumnName(), componentId, *propName, KLangNone, propType);
	SetGeneralBinaryPropertyL(propType, propertyId, KComponentPropertiesTable(), KComponentIdColumnName(), componentId, *propName, *propValue);
	CleanupStack::PopAndDestroy(2, propName);
	}

void CScrRequestImpl::SetComponentIntPropertyL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Setting component integer property."));
	// N.B. Don't change the order of IPC parameter reading, otherwise fails in UREL mode.
	HBufC* propName = ReadDescLC(aMessage, 1);
	
	if (!propName)
		User::Leave(KErrArgument);
	
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	TInt64 propValue = MAKE_TINT64(aMessage.Int2(), aMessage.Int3());	
		
	TPropertyType propType(CScrRequestImpl::EPropertyUndefined);
	TInt propertyId = FindGeneralPropertyNoLocaleDowngradeL(KComponentPropertiesTable(), KComponentIdColumnName(), componentId, *propName, KLangNone, propType);
	SetGeneralIntPropertyL(propType, propertyId, KComponentPropertiesTable(), KComponentIdColumnName(), componentId, *propName, propValue);
	CleanupStack::PopAndDestroy(propName);
	}

TUint32 CScrRequestImpl::HashCaseInsensitiveL(const TDesC& aName)
	{
	RBuf temp;
	temp.CreateMaxL(aName.Length());
	temp.CleanupClosePushL();
	temp.Copy(aName);
	temp.LowerCase(); // The hashed column is case-insensitive. So, the hashed value must be
					  // case-insensitive too. To achieve that we need to set to lower-case.	
	TUint32 hashVal = HashCaseSensitiveL(temp);
	CleanupStack::PopAndDestroy(&temp);
	return hashVal;
	}

TUint32 CScrRequestImpl::HashCaseSensitiveL(const TDesC& aName)
	{
	TUint32 hashVal = 0;
	Mem::Crc32(hashVal,aName.Ptr(),aName.Size());
	return hashVal;
	}

TInt CScrRequestImpl::GetComponentFileIdL(const TDesC& aFileName, TComponentId aComponentId) const
	{
	DEBUG_PRINTF3(_L("Looking up a component(%d) file(%S)"), aComponentId, &aFileName);
	
	TUint32 hash = HashCaseInsensitiveL(aFileName);
	_LIT(KFindComponentFile, "SELECT CmpFileId FROM ComponentsFiles WHERE ComponentId=? AND LocationHash=?;");
	CStatement *stmtFind = iDbHandle->PrepareStatementLC(KFindComponentFile());
	stmtFind->BindIntL(1, aComponentId);
	stmtFind->BindIntL(2, hash);
	
	if(!stmtFind->ProcessNextRowL())
		{
		DEBUG_PRINTF3(_L("Component %d does not have File=%S!"), aComponentId, &aFileName);
		CleanupStack::PopAndDestroy(stmtFind);
		return KErrNotFound;
		}
	TInt componentFileId = stmtFind->IntColumnL(0);
	CleanupStack::PopAndDestroy(stmtFind);
	return componentFileId;
	}

TInt CScrRequestImpl::FindComponentFileL(const TDesC& aFileName, TComponentId aComponentId) const
	{
	TInt cmpFileId = GetComponentFileIdL(aFileName, aComponentId);
	if(KErrNotFound == cmpFileId)
		{
		User::Leave(KErrNotFound);
		}
	return cmpFileId;
	}

TInt CScrRequestImpl::GetDriveFromFilePath(const TDesC& aFilePath, TDriveUnit& aDriveUnit) const
	{
	if(!iFs.IsValidName(aFilePath))
		{// even if the file name validity is checked before registering the file, do double check
		 // to be on the safe side. Because, if the file format is invalid, the rest of the code may panic.
		DEBUG_PRINTF2(_L("The file (%S) doesn't have a valid name format!"), &aFilePath);
		return KErrArgument;
		}
	
	TParsePtrC fileParser(aFilePath);
	TPtrC driveLetter(fileParser.Drive());
	if(driveLetter == KNullDesC())
		{
		DEBUG_PRINTF2(_L("The file (%S) doesn't contain any drive letter!"), &aFilePath);
		return KErrArgument;
		}
	aDriveUnit = driveLetter;
	return KErrNone;
	}

TInt CScrRequestImpl::InstalledDrivesToBitmaskL(const TDriveList& aDriveList) const
	{
	TInt installedDrives(0);
	for(TInt driveNum=EDriveA; driveNum<=EDriveZ; ++driveNum)
		{
		if(aDriveList[driveNum])
			{
			installedDrives |= 1<<driveNum;
			}
		}
	return installedDrives;
	}

TInt CScrRequestImpl::GetInstalledDrivesBitmaskL(TComponentId aComponentId) const
	{
	// Get installed drives list from database
	_LIT(KSelectInstalledDrivesBitmask, "SELECT InstalledDrives FROM Components WHERE ComponentId=?;");
	CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectInstalledDrivesBitmask);
	stmt->BindIntL(1, aComponentId);
	if(!stmt->ProcessNextRowL())
		{
		DEBUG_PRINTF2(_L8("Component (%d) couldn't be found in the SCR database."), aComponentId);
		User::Leave(KErrNotFound);
		}
	TInt res = stmt->IntColumnL(0);
	CleanupStack::PopAndDestroy(stmt);
	return res;
	}

TBool CScrRequestImpl::HasFilesOnDriveL(TDriveUnit aDrive, TComponentId aComponentId)
	{
	CStatement *stmt = OpenFileListStatementL(aComponentId);
	CleanupStack::PushL(stmt);
	HBufC *nextFilePath(NULL);
	TBool returnValue(EFalse);
	
	while ( returnValue == EFalse && (nextFilePath = GetNextFilePathL(*stmt)) != NULL)
		{
		TDriveUnit fileDrive;
		TInt res = GetDriveFromFilePath(*nextFilePath, fileDrive);
		// Files with invalid paths are ignored
		if (res == KErrNone && fileDrive == aDrive)
			{
			returnValue = ETrue;
			}
		delete nextFilePath;			
		}
		
	CleanupStack::PopAndDestroy(stmt);
	return returnValue;
	}
	
void CScrRequestImpl::UpdateInstalledDrivesL(TComponentId aComponentId, const TDesC& aFilePath, TFileOperationType aType)
	{
	TDriveUnit drive;
	User::LeaveIfError(GetDriveFromFilePath(aFilePath, drive));
	
	TInt driveNum = static_cast<TInt>(drive);	
	TInt oldDrivesBitmask = GetInstalledDrivesBitmaskL(aComponentId);

	TInt newDrivesBitmask = oldDrivesBitmask;
	// Update the list of installed drives
	if(aType == EFileRegistered)
		{
		newDrivesBitmask |= 0x1 << driveNum;
		}
	else // aType == EFileUnregistered
		{
		TBool hasOtherFilesOnTheDrive = HasFilesOnDriveL(drive, aComponentId);
		if (!hasOtherFilesOnTheDrive)
			{
			// Do a logical AND with a bitmask which looks like 111110111111
			TInt andBitmask = ~(0x1 << driveNum);
			newDrivesBitmask &= andBitmask;
			}
		}

	if (newDrivesBitmask != oldDrivesBitmask)
		{
		// Write back the updated installed drives and list 
		_LIT(KUpdateInstalledDrives, "UPDATE Components SET InstalledDrives=? WHERE ComponentId=?;");
		TInt numberOfValues = 2;
		ExecuteStatementL(KUpdateInstalledDrives, numberOfValues, EValueInteger, newDrivesBitmask, EValueInteger, aComponentId);
		}
	}

void CScrRequestImpl::RegisterComponentFileL(const RMessage2& aMessage)
   	{
	DEBUG_PRINTF(_L8("Registering component file."));
   	
   	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
   	HBufC* fileName = ReadFileNameFromMsgLC(aMessage);
 	TBool considerFileInDrivesList = aMessage.Int2();	
   	TUint32 hash = HashCaseInsensitiveL(*fileName);
	TDriveUnit drive(*fileName);
   	TVolumeInfo volInfo;
   	
   	_LIT(KInsertFile, "INSERT INTO ComponentsFiles(ComponentId,LocationHash,Location) VALUES(?,?,?);");
   	TInt numberOfValues = 3;
   	ExecuteStatementL(KInsertFile(), numberOfValues, EValueInteger, componentId, EValueInteger, hash, EValueString, fileName);
   	// Reflect this file addition in the installed drives field of the components table
	// Or the drive list is updated in case of a FILENULL operation on a removable drive
   	// If this step fails (for example, this file name is incorrect) then we cannot register the file
 	if (KErrNone == iFs.Volume(volInfo, drive) || considerFileInDrivesList)
 		UpdateInstalledDrivesL(componentId, *fileName, EFileRegistered);
 	
   	CleanupStack::PopAndDestroy(fileName);
   	}

void CScrRequestImpl::SetFileStrPropertyL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Setting file string property."));
	
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);	
	HBufC* fileName  = ReadFileNameFromMsgLC(aMessage);
	
	TInt compFileId = FindComponentFileL(*fileName, componentId);
		
	HBufC* propName  = ReadDescLC(aMessage, 2);
	TPropertyType propType(CScrRequestImpl::EPropertyUndefined);
	TInt propertyId = FindGeneralPropertyNoLocaleDowngradeL(KFilePropertiesTable(), KCompFileIdColumnName(), compFileId, *propName, KLangNone, propType);
	
	HBufC8* propValue = ReadDesc8LC(aMessage, 3);
	SetGeneralBinaryPropertyL(propType, propertyId, KFilePropertiesTable(), KCompFileIdColumnName(), compFileId, *propName, *propValue);
	CleanupStack::PopAndDestroy(3, fileName);
	}

void CScrRequestImpl::SetFileIntPropertyL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Setting integer file property."));
		
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	HBufC* fileName  = ReadFileNameFromMsgLC(aMessage);
	TInt compFileId = FindComponentFileL(*fileName, componentId);
	
	HBufC* propName  = ReadDescLC(aMessage, 2);
	TPropertyType propType(CScrRequestImpl::EPropertyUndefined);
	TInt propertyId = FindGeneralPropertyNoLocaleDowngradeL(KFilePropertiesTable(), KCompFileIdColumnName(), compFileId, *propName, KLangNone, propType);
	
	TInt propValue   = aMessage.Int3();
	SetGeneralIntPropertyL(propType, propertyId, KFilePropertiesTable(), KCompFileIdColumnName(), compFileId, *propName, propValue);
	CleanupStack::PopAndDestroy(2, fileName);
	}

void CScrRequestImpl::SetComponentLocalizableL(TComponentId aComponentId, TLanguage aLocale, const TDesC& aColumnName, const TDesC& aName, const TDesC& aVendor)
	{
	DEBUG_PRINTF(_L8("Setting component Localizable."));
	
	_LIT(KFindComponentLocalizable, "SELECT CompLocalId FROM ComponentLocalizables WHERE ComponentId=? AND Locale=?;");
	TInt numberOfValues = 2;
	CStatement *stmt = CreateStatementObjectWithLocaleL(KFindComponentLocalizable, aLocale, numberOfValues, EValueInteger, aComponentId, EValueLanguage);	
	
	if(!stmt)
		{// Add a new name for this component
		AddComponentLocalizablesL(aComponentId, aLocale, aName, aVendor);
		}
	else
		{// Exists, update the localizable field of the component
		CleanupStack::PushL(stmt);
		TInt compLocId = stmt->IntColumnL(0);
		_LIT(KUpdateComponentName, "UPDATE ComponentLocalizables SET %S=? WHERE CompLocalId=?;");
		HBufC *statementStr = FormatStatementLC(KUpdateComponentName, aColumnName.Length(), &aColumnName);
		const TDesC *value = aName.Length() ? &aName : &aVendor;
		TInt numberOfValues = 2;
		ExecuteStatementL(*statementStr, numberOfValues, EValueString, value, EValueInteger, compLocId);
		CleanupStack::PopAndDestroy(2, stmt); // stmt, statementStr
		}
	}

void CScrRequestImpl::SetComponentNameL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Setting component name."));
	
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	HBufC *componentName = ReadDescLC(aMessage, 1);
	if (!componentName->CompareF(KNullDesC()))
		{// Component name cannot be empty
		DEBUG_PRINTF(_L8("Component name cannot be set or modified to a NULL string."));
		User::Leave(KErrArgument);
		}	
		
	TLanguage locale = TLanguage(aMessage.Int2());
	_LIT(KComponentNameField,"Name");
	SetComponentLocalizableL(componentId, locale, KComponentNameField, *componentName, KNullDesC());
	CleanupStack::PopAndDestroy(componentName);
	}

void CScrRequestImpl::SetVendorNameL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Setting vendor name."));
	
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	HBufC *vendorName = ReadDescLC(aMessage, 1);
	TLanguage locale = TLanguage(aMessage.Int2());
	_LIT(KComponentVendorField,"Vendor");
	SetComponentLocalizableL(componentId, locale, KComponentVendorField, KNullDesC(), *vendorName);
	CleanupStack::PopAndDestroy(vendorName);
	}

void CScrRequestImpl::ReadAndSetCommonComponentPropertyL(const RMessage2& aMessage, const TDesC& aPropertyColumn)
	{
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	TInt propertyValue = aMessage.Int1();
	
	_LIT(KUpdateCommonProperty, "UPDATE Components SET %S=? WHERE ComponentId=?;");
	TInt formattedLen = aPropertyColumn.Length();
	HBufC *statementStr = FormatStatementLC(KUpdateCommonProperty(), formattedLen, &aPropertyColumn );
	TInt numberOfValues = 2;
	ExecuteStatementL(*statementStr, numberOfValues, EValueInteger, propertyValue, EValueInteger, componentId);
	CleanupStack::PopAndDestroy(statementStr);
	}

void CScrRequestImpl::SetIsComponentRemovableL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Setting the component's removable attribute."));
	_LIT(KColumnNameRemovable, "Removable");
	ReadAndSetCommonComponentPropertyL(aMessage, KColumnNameRemovable);
	}

void CScrRequestImpl::SetIsComponentDrmProtectedL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Setting the component's DRM protected attribute."));
	_LIT(KColumnNameDrmProtected, "DRMProtected");
	ReadAndSetCommonComponentPropertyL(aMessage, KColumnNameDrmProtected);	
	}

void CScrRequestImpl::SetIsComponentHiddenL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Setting the component's hidden attribute."));
	_LIT(KColumnNameHidden, "Hidden");
	ReadAndSetCommonComponentPropertyL(aMessage, KColumnNameHidden);		
	}

void CScrRequestImpl::SetIsComponentKnownRevokedL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Setting the component's known-revoked attribute."));
	_LIT(KColumnNameKnownRevoked, "KnownRevoked");
	ReadAndSetCommonComponentPropertyL(aMessage, KColumnNameKnownRevoked);	
	}

void CScrRequestImpl::SetIsComponentOriginVerifiedL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Setting the component's removable attribute."));
	_LIT(KColumnNameOriginVerified, "OriginVerified");
	ReadAndSetCommonComponentPropertyL(aMessage, KColumnNameOriginVerified);	
	}
		
void CScrRequestImpl::SetComponentSizeL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Setting the component's install-time size."));
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	TInt64 componentSize = MAKE_TINT64(aMessage.Int1(), aMessage.Int2());
	if (componentSize < 0)
		{
		DEBUG_PRINTF2(_L8("Received incorrect component size: %d."), componentSize);
		User::Leave(KErrArgument);
		}
	_LIT(KUpdateComponentSize, "UPDATE Components SET Size=? WHERE ComponentId=?;");
	TInt numberOfValues = 2;
	ExecuteStatementL(KUpdateComponentSize, numberOfValues, EValueInteger64, componentSize, EValueInteger, componentId);
	}

TBool FindLogEntryWithComponentId(const TComponentId *aKey, const CScrLogEntry& aEntry)
	{
	return (*aKey == aEntry.ComponentId()); 
	}

void CScrRequestImpl::SetComponentVersionL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Setting component version."));
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	HBufC *version = ReadDescLC(aMessage, 1);
	
	_LIT(KUpdateVersion, "UPDATE Components SET Version=? WHERE ComponentId=?;");
	TInt numberOfValues = 2;
	ExecuteStatementL(KUpdateVersion, numberOfValues, EValueString, version, EValueInteger, componentId);
	
	TInt logIdx = iLogEntries.Find(componentId, FindLogEntryWithComponentId);
	if(KErrNotFound == logIdx)
		{
		CleanupStack::PopAndDestroy(version);
		return;
		}
	DeleteObjectZ(iLogEntries[logIdx]->iVersion);
	iLogEntries[logIdx]->iVersion = version; // Ownership is transferred
	CleanupStack::Pop(version);
	}

void CScrRequestImpl::DeleteComponentPropertyL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Deleting component property."));
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	HBufC *name = ReadDescLC(aMessage, 1);
	
	_LIT(KDeleteComponentProperty, "DELETE FROM ComponentProperties WHERE Name=? AND ComponentId=?;");
	TInt numberOfValues = 2;
	ExecuteStatementL(KDeleteComponentProperty, numberOfValues, EValueString, name, EValueInteger, componentId);
	CleanupStack::PopAndDestroy(name);
	}

void CScrRequestImpl::DeleteFilePropertyL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Deleting file property."));
	
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	HBufC* fileName  = ReadFileNameFromMsgLC(aMessage);
	
	TInt compFileId = FindComponentFileL(*fileName, componentId);
	HBufC *propName = ReadDescLC(aMessage, 2);
	
	_LIT(KDeleteFileProperty, "DELETE FROM FileProperties WHERE CmpFileId=? AND Name=?;");
	TInt numberOfValues = 2;
	ExecuteStatementL(KDeleteFileProperty, numberOfValues, EValueInteger, compFileId, EValueString, propName);
	CleanupStack::PopAndDestroy(2, fileName);
	}

void CScrRequestImpl::UnregisterComponentFileL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Unregistering a component file."));
	
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	HBufC* fileName  = ReadFileNameFromMsgLC(aMessage);
	
	TInt compFileId = GetComponentFileIdL(*fileName, componentId);
	if (compFileId == KErrNotFound)
		{
		CleanupStack::PopAndDestroy(fileName);
		return; // If the file is not registered, we do not return an error - this sort of condition is better enforced at upper layers 
		}
		
	_LIT(KDeleteComponentFileProperties, "DELETE from FileProperties WHERE CmpFileId=?;");
	TInt numberOfValues = 1;
	ExecuteStatementL(KDeleteComponentFileProperties, numberOfValues, EValueInteger, compFileId);
	DEBUG_PRINTF3(_L("Properties of the file(%S) of the component(%d) have been deleted."), fileName, componentId);

	_LIT(KUnregisterComponentFile, "DELETE FROM ComponentsFiles WHERE CmpFileId=?;");
	ExecuteStatementL(KUnregisterComponentFile, numberOfValues, EValueInteger, compFileId);

	// Reflect this file deletion in the installed drives field of the components table
	UpdateInstalledDrivesL(componentId, *fileName, EFileUnregistered);
	CleanupStack::PopAndDestroy(fileName);
	}

void ReadNullableStringFromStatementL(CStatement& aStmt, TPtrC& aValue, TInt aFieldNum)
	{
	aValue.Set(KNullDesC());
	if(!aStmt.IsFieldNullL(aFieldNum))
		{ // If the value is not NULL, set it.
		aValue.Set(aStmt.StrColumnL(aFieldNum));
		}
	}

void CScrRequestImpl::DeleteComponentL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Deleting a component."));
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	
	// first delete the applications associated with this id if there are any
	DeleteAllAppsWithinPackageInternalL(componentId);
	
	// Create the log record before deleting the component
	_LIT(KSelectComponentInfo, "SELECT SoftwareTypeName,GlobalId,Version FROM Components WHERE ComponentId=?;");
	CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectComponentInfo);
	stmt->BindIntL(1, componentId);
	
	// if the component doesn't exist, the code shouldn't come here. It should have left with KErrNotFound at security check level.
	__ASSERT_ALWAYS(stmt->ProcessNextRowL(), User::Leave(KErrAbort));
	
	CComponentEntry *entry2bDeleted = CComponentEntry::NewLC();
	entry2bDeleted->iSwType = stmt->StrColumnL(0).AllocL();
	
	TPtrC globalId;
	ReadNullableStringFromStatementL(*stmt, globalId, 1);
	if(globalId.Length() > 0)
		{
		CGlobalComponentId *globalIdObj = ParseGlobalComponendIdLC(globalId);
		entry2bDeleted->iGlobalId = globalIdObj->GlobalIdName().AllocL();
		CleanupStack::PopAndDestroy(globalIdObj);
		}
	else
		{
		entry2bDeleted->iGlobalId = globalId.AllocL();
		}
	TPtrC version;
	ReadNullableStringFromStatementL(*stmt, version, 2);
	entry2bDeleted->iVersion = version.AllocL();
	AddComponentEntryLocalizablesL(componentId, *entry2bDeleted, KUnspecifiedLocale);

	CScrLogEntry *logRecord = CScrLogEntry::NewL(entry2bDeleted->Name(), entry2bDeleted->SoftwareType(), entry2bDeleted->GlobalId(), entry2bDeleted->Version(), EScrCompUnInstall);
	logRecord->iComponentId = componentId;
	CleanupStack::PopAndDestroy(2, stmt); // stmt, entry2bDeleted
	CleanupStack::PushL(logRecord);
	
	// Now begin the component deletion
	TInt numberOfValues = 1;
	_LIT(KDeleteComponentProperties,"DELETE FROM ComponentProperties WHERE ComponentId=?;");
	ExecuteStatementL(KDeleteComponentProperties, numberOfValues, EValueInteger, componentId);
	DEBUG_PRINTF2(_L8("Properties of component(%d) have been deleted."), componentId);
	
	_LIT(KDeleteComponentFileProperties, "DELETE from FileProperties WHERE CmpFileId IN \
				(SELECT CmpFileId FROM ComponentsFiles WHERE ComponentId=?);");
	ExecuteStatementL(KDeleteComponentFileProperties, numberOfValues, EValueInteger, componentId);
	DEBUG_PRINTF2(_L8("Properties of files of component(%d) have been deleted."), componentId);

	_LIT(KDeleteComponentFiles, "DELETE FROM ComponentsFiles WHERE ComponentId=?;");
	ExecuteStatementL(KDeleteComponentFiles, numberOfValues, EValueInteger, componentId);
	DEBUG_PRINTF2(_L8("File registrations of component(%d) have been deleted."), componentId);

	_LIT(KDeleteComponentLocalizables, "DELETE FROM ComponentLocalizables WHERE ComponentId=?;");
	ExecuteStatementL(KDeleteComponentLocalizables, numberOfValues, EValueInteger, componentId);
	DEBUG_PRINTF2(_L8("Localizables of component(%d) have been deleted."), componentId);
	
	// Delete all dependencies where this component is dependant
	_LIT(KDeleteComponentDependencies, "DELETE FROM ComponentDependencies WHERE DependantGlobalIdHash IN \
				(SELECT GlobalIdHash FROM Components WHERE ComponentId=?);");
	ExecuteStatementL(KDeleteComponentDependencies, numberOfValues, EValueInteger, componentId);
	DEBUG_PRINTF2(_L8("Dependencies of component(%d) have been deleted."), componentId);
	
	_LIT(KDeleteComponents, "DELETE FROM Components WHERE ComponentId=?;");
	ExecuteStatementL(KDeleteComponents, numberOfValues, EValueInteger, componentId);
	DEBUG_PRINTF2(_L8("Finally component(%d) has been deleted."), componentId);
	
	// Add log record
	iLogEntries.Append(logRecord);
	CleanupStack::Pop(logRecord); // Ownership is transferred
	}

void CScrRequestImpl::DeleteComponentDependencyL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Deleting a component dependency."));
	
	HBufC *suppGlobalId = ReadAndGetGlobalIdLC(aMessage, 1);
	HBufC *depGlobalId = ReadAndGetGlobalIdLC(aMessage, 2);
	TUint32 globalIdsHash = HashGlobalIdsL(*depGlobalId, *suppGlobalId);
	CleanupStack::PopAndDestroy(2, suppGlobalId); // suppGlobalId,depGlobalId
	
	_LIT(KDeleteDependency, "DELETE FROM ComponentDependencies WHERE GlobalIdHash=?;");
	TInt numberOfValues = 1;
	ExecuteStatementL(KDeleteDependency(), numberOfValues, EValueInteger, globalIdsHash);
	}

TBool CScrRequestImpl::BindAndProcessStatementObjectL(CStatement& aStatementObj, TLanguage aLanguage, TInt aValuesNum, VA_LIST aList) const
	{
	aStatementObj.ResetL();
	BindStatementValuesL(aStatementObj, aLanguage, aValuesNum, aList);
	return aStatementObj.ProcessNextRowL();
	}

CStatement* CScrRequestImpl::CreateStatementObjectWithoutLocaleL(const TDesC& aStatement, TInt aValuesNum,...) const	
	{
	VA_LIST argList;
	VA_START(argList, aValuesNum);
	
	CStatement *stmt = iDbHandle->PrepareStatementLC(aStatement);
	
	// The language parameter is irrelevant. The BindAndProcessStatementObjectL is reused here to avoid code duplication. 
	TBool success = BindAndProcessStatementObjectL(*stmt, ELangNone, aValuesNum, argList);
	
	VA_END(argList);
	
	if(!success)
		{ // if the code reaches here, it means there is no record for the given criteria
		CleanupStack::PopAndDestroy(stmt);
		return NULL;
		}
	CleanupStack::Pop(stmt);
	return stmt;	
	}

CStatement* CScrRequestImpl::CreateStatementObjectWithLocaleNoDowngradeL(const TDesC& aStatement, TLanguage aLocale, TInt aValuesNum,...) const	
	{
	VA_LIST argList;
	VA_START(argList, aValuesNum);	
	
	CStatement *stmt = iDbHandle->PrepareStatementLC(aStatement);	
	TBool success = BindAndProcessStatementObjectL(*stmt, aLocale, aValuesNum, argList);
		
	if(!success)
		{ // if the code reaches here, it means there is no record for the given criteria		
		CleanupStack::PopAndDestroy(stmt);			
		return NULL;
		}
		
	CleanupStack::Pop(stmt);	
	return stmt;
	
	}

// This function tries automatically downgrading languages to the nearest locale	
CStatement* CScrRequestImpl::CreateStatementObjectWithLocaleL(const TDesC& aStatement, TLanguage aLocale, TInt aValuesNum,...) const
	{
	VA_LIST argList;
	VA_START(argList, aValuesNum);
	
	CStatement *stmt = iDbHandle->PrepareStatementLC(aStatement);
	//Avoiding call to BAFL for the current locale. If succeeds, BAFL will not be invoked
	TBool success = BindAndProcessStatementObjectL(*stmt, aLocale, aValuesNum, argList);	
    VA_START(argList, aValuesNum);
	
	if ( !success )
	   {
       // Run the given statement for the passed or its equivalent languages
       // First, get the Equivalent Language path for the given locale. The first element
       // in the array is the given language.		
       TLanguagePath equivalentLanguages;
       BaflUtils::GetEquivalentLanguageList(aLocale, equivalentLanguages);
    
       TInt i = 1; //skipping the current locale
       while ( equivalentLanguages[i] != ELangNone && !success)	
           {
           success = BindAndProcessStatementObjectL(*stmt, equivalentLanguages[i], aValuesNum, argList);
           VA_START(argList, aValuesNum);
           i++;
           }
       VA_END(argList);
        
       if(!success)
           { // if the code reaches here, it means there is no record for the given criteria       
           CleanupStack::PopAndDestroy(stmt);          
           return NULL;
           }
	    }
	else
		{
		VA_END(argList);
		}

	CleanupStack::Pop(stmt);	
	return stmt;
	}

CComponentEntry* CScrRequestImpl::CreateComponentEntryFromStatementHandleL(CStatement& aStmt) const
	{
	TComponentId componentId = aStmt.IntColumnL(0);
	TInt removable = aStmt.IntColumnL(3);
	TInt64 size = aStmt.Int64ColumnL(4);
	TScomoState scomoState = TScomoState(aStmt.IntColumnL(5));
	
	TInt drmProtected = aStmt.IntColumnL(6);
	TInt hidden = aStmt.IntColumnL(7);
	TInt knownRevoked = aStmt.IntColumnL(8);
	TInt originVerified = aStmt.IntColumnL(9);
	
	TPtrC globalIdDes;
	ReadNullableStringFromStatementL(aStmt, globalIdDes, 10);
	HBufC *globalId = NULL;
	if(globalIdDes.Length())
		{ // if global id is not NULL
		CGlobalComponentId *globalIdObj = ParseGlobalComponendIdLC(globalIdDes);
		globalId = globalIdObj->GlobalIdName().AllocL();
		CleanupStack::PopAndDestroy(globalIdObj);
		CleanupStack::PushL(globalId);
		}
	else
		{ // Global Id is NULL
		globalId = KNullDesC().AllocLC();
		}
	
	TInt installedDrivesBitmask = aStmt.IntColumnL(11);
	// Convert bitmask to TDriveList
	TDriveList installedDriveList;
	installedDriveList.FillZ(KMaxDrives);
	for (TInt i = 0; i < KMaxDrives && installedDrivesBitmask > 0; ++i, installedDrivesBitmask >>= 1)
		{
		installedDriveList[i] = installedDrivesBitmask & 0x1;
		}
	
	TPtrC version(KNullDesC());
	if(!aStmt.IsFieldNullL(12))
		version.Set(aStmt.StrColumnL(12));
	
	TPtrC installTime = aStmt.StrColumnL(13);
		
	CComponentEntry *entry = CComponentEntry::NewL(componentId, KNullDesC, KNullDesC, KNullDesC, *globalId, removable, size, scomoState, installedDriveList, version, installTime, drmProtected, hidden, knownRevoked, originVerified);
	CleanupStack::PopAndDestroy(globalId);
	return entry;
	}

//
// aLocalizableStmtStr = A SELECT statement which queries a table for a specific locale. In other words, the statement has got a locale condition.
// aAnyValueStmtStr = A SELECT statement which doesn't contain a locale condition. The result row set will include records with any locale.
// aConditionIntValue = The value of the integer condition of the statements given.
// aConditionLocale = The value of the locale condition of the localizable statement (first parameter).
//
CStatement* CScrRequestImpl::ExecuteLocalizableStatementL(const TDesC& aLocalizableStmtStr, const TDesC& aAnyValueStmtStr, TInt aConditionIntValue, TLanguage aConditionLocale) const
	{
	TInt numberOfValues = 2;
	CStatement *stmtLoc(0);
	
	if(KUnspecifiedLocale == aConditionLocale)
		{
		// The locale is not specified. So, first try to run the statement for the current locale and its downgraded languages.
		stmtLoc = CreateStatementObjectWithLocaleL(aLocalizableStmtStr, User::Language(), numberOfValues, EValueInteger, aConditionIntValue, EValueLanguage);
		if (!stmtLoc)
			{
			// No result for the current locale at all. Try to find non-localized values.
			stmtLoc = CreateStatementObjectWithLocaleL(aLocalizableStmtStr, KNonLocalized, numberOfValues, EValueInteger, aConditionIntValue, EValueLanguage);			
			}

		if (!stmtLoc)
			{
			// No result for the current locale or the non-localized, "neutral" locale. Try finding any locale
			stmtLoc = CreateStatementObjectWithoutLocaleL(aAnyValueStmtStr, 1, EValueInteger, aConditionIntValue);		
			}
		}
		else if(KNonLocalized == aConditionLocale)
			{
			// Only non-localized names are requested.
			stmtLoc = CreateStatementObjectWithLocaleL(aLocalizableStmtStr, KNonLocalized, numberOfValues, EValueInteger, aConditionIntValue, EValueLanguage);
			}
		else
			{
			// Names are requested for a particular locale.
			stmtLoc = CreateStatementObjectWithLocaleL(aLocalizableStmtStr, aConditionLocale, numberOfValues, EValueInteger, aConditionIntValue, EValueLanguage);
			}
	
	return stmtLoc;
	}

void CScrRequestImpl::AddComponentEntryLocalizablesL(TComponentId aComponentId, CComponentEntry& aEntry, TLanguage aLocale) const
	{
	DEBUG_PRINTF(_L8("Adding Component Entry Localizables."));
	_LIT(KSelectLocalizableNames, "SELECT Name,Vendor FROM ComponentLocalizables WHERE ComponentId=? AND Locale=?;");
	_LIT(KSelectAnyNames, "SELECT Name,Vendor FROM ComponentLocalizables WHERE ComponentId=?;");
	
	CStatement *stmtLoc = ExecuteLocalizableStatementL(KSelectLocalizableNames, KSelectAnyNames, aComponentId, aLocale);	
	if(!stmtLoc)
		{
		DEBUG_PRINTF3(_L8("Name and Vendor couldn't be found for the component (%d) and locale(%d)."), aComponentId, aLocale);
		User::Leave(KErrScrUnsupportedLocale);
		}

	CleanupStack::PushL(stmtLoc);	
		
	TPtrC name(stmtLoc->StrColumnL(0));
	TPtrC vendor(stmtLoc->StrColumnL(1));
	DeleteObjectZ(aEntry.iName);
	aEntry.iName = name.AllocL(); // Ownership is transferred to the entry object
	DeleteObjectZ(aEntry.iVendor);
	if ( 0 != vendor.Ptr())
		aEntry.iVendor = vendor.AllocL(); // Ownership is transferred to the entry object
	else
		aEntry.iVendor = HBufC::NewL(1);
	
	CleanupStack::PopAndDestroy(stmtLoc);
	}

void CScrRequestImpl::AddSoftwareTypeNameToComponentEntryL(CStatement& aStmt, CComponentEntry& aEntry, TLanguage aLocale) const
	{
	DEBUG_PRINTF(_L8("Adding SoftwareType Name To Component Entry."));
	TInt softwareTypeId = aStmt.IntColumnL(1);
	_LIT(KSelectLocalizableSwTypeName, "SELECT Name FROM SoftwareTypeNames WHERE SoftwareTypeId=? AND Locale=?;");
	CStatement *stmtLoc = ExecuteLocalizableStatementL(KSelectLocalizableSwTypeName, KSelectLocalizableSwTypeName, softwareTypeId, aLocale);
	if(stmtLoc)
		{
		CleanupStack::PushL(stmtLoc);
		TPtrC name(stmtLoc->StrColumnL(0));
		DeleteObjectZ(aEntry.iSwType);
		aEntry.iSwType = name.AllocL(); // Ownership is transferred to the entry object
		CleanupStack::PopAndDestroy(stmtLoc);
		return;
		}
	
	// The software type doesn't exist or there is no localized name of the software type, add the unique software type name
	TPtrC uniqueSwTypeName = aStmt.StrColumnL(2);
	DeleteObjectZ(aEntry.iSwType);
	aEntry.iSwType = uniqueSwTypeName.AllocL(); // Ownership is transferred to the entry object
	}

void CScrRequestImpl::GetGeneralComponentEntrySizeL(const RMessage2& aMessage, const TDesC& aConditionColumn, TUint32 aConditionValue, TInt aReturnSizeSlot, TLanguage aLocale, CComponentEntry*& aComponentEntry) const
	{
	_LIT(KSelectComponents, "SELECT ComponentId,SoftwareTypeId,SoftwareTypeName,Removable,Size,ScomoState,DRMProtected,Hidden,KnownRevoked,OriginVerified,GlobalId,InstalledDrives,Version,InstallTime FROM Components WHERE %S=?;");
	
	TInt formattedLen = aConditionColumn.Length();
	HBufC *statementStr = FormatStatementLC(KSelectComponents(), formattedLen, &aConditionColumn);
	CStatement *stmt = iDbHandle->PrepareStatementLC(*statementStr);
	stmt->BindIntL(1, aConditionValue);
		
	if(!stmt->ProcessNextRowL())
		{
		DEBUG_PRINTF2(_L8("The component(%d) couldn't be found. The size returned is zero."), aConditionValue);
		WriteIntValueL(aMessage, aReturnSizeSlot, 0);
		CleanupStack::PopAndDestroy(2, statementStr); // statementStr, stmt
		return;
		}
		
	DeleteObjectZ(aComponentEntry);
	aComponentEntry = CreateComponentEntryFromStatementHandleL(*stmt);
	AddComponentEntryLocalizablesL(aComponentEntry->ComponentId(), *aComponentEntry, aLocale);
	AddSoftwareTypeNameToComponentEntryL(*stmt, *aComponentEntry, aLocale);
	
	WriteObjectSizeL(aMessage, aReturnSizeSlot, aComponentEntry);
	CleanupStack::PopAndDestroy(2, statementStr); // statementStr, stmt
	}

void CScrRequestImpl::GetComponentEntrySizeL(const RMessage2& aMessage) const
	{
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	DEBUG_PRINTF2(_L8("Returning the entry size of component(%d)."), componentId);
	TLanguage locale = TLanguage(aMessage.Int1());
	
	_LIT(KConditionColumn, "ComponentId");
	TInt returnSizeSlot=2;
	GetGeneralComponentEntrySizeL(aMessage, KConditionColumn(), componentId, returnSizeSlot, locale, iComponentEntry);
	}
	
void CScrRequestImpl::GetComponentEntryDataL(const RMessage2& aMessage) const
	{
	DEBUG_PRINTF(_L8("Returning the component entry data."));
	WriteObjectDataL(aMessage, 0, iComponentEntry);
	DeleteObjectZ(iComponentEntry); // Delete the object to prevent it to be resent.
	}

void CScrRequestImpl::GetComponentLocalizedEntrySizeL(const RMessage2& aMessage) const
    {
    TComponentId componentId = GetComponentIdFromMsgL(aMessage);
    DEBUG_PRINTF2(_L8("Returning the localized information entry size of component(%d)."), componentId);
    _LIT(KSelectMatchingSupportedLocales, "SELECT Locale, Name, Vendor FROM ComponentLocalizables WHERE ComponentId=?;");
    CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectMatchingSupportedLocales);
    stmt->BindIntL(1, componentId);
    while(stmt->ProcessNextRowL())
       {
        TPtrC name(stmt->StrColumnL(1));
        TPtrC vendor(stmt->StrColumnL(2));
        TInt locale(stmt->IntColumnL(0)); 
        CLocalizableComponentInfo *compLocalizedInfo = Usif::CLocalizableComponentInfo::NewLC( name, vendor,(TLanguage)locale);
        iCompLocalizedInfoArray.Insert(compLocalizedInfo,iCompLocalizedInfoArray.Count());
        CleanupStack::Pop(compLocalizedInfo);
        }
    // Release allocated memories
    CleanupStack::PopAndDestroy(1, stmt); // stmt
    WriteArraySizeL(aMessage, 1, iCompLocalizedInfoArray);
    }

void CScrRequestImpl::GetComponentLocalizedEntryDataL(const RMessage2& aMessage) const
    {
    DEBUG_PRINTF(_L8("Returning the localized information entry data."));
    WriteArrayDataL(aMessage, 0, iCompLocalizedInfoArray);
    iCompLocalizedInfoArray.ResetAndDestroy(); 
    }

TBool CompareProperties(const CPropertyEntry& aRhs, const CPropertyEntry& aLhs)
	{
	return ((aRhs.PropertyType() == aLhs.PropertyType()) && 
		   (aRhs.PropertyName() == aLhs.PropertyName()));		   
	}

void CScrRequestImpl::AddGeneralPropertiesArrayWithLocaleL(const TDesC& aStmtStr, TLanguage aLocale, TComponentId aIdColumnValue, RPointerArray<CPropertyEntry>& aProperties) const
	{
	DEBUG_PRINTF(_L8("Adding General Properties Array With Locale."));
	
	TInt numberOfValues = 2;
	CStatement *stmt = CreateStatementObjectWithLocaleL(aStmtStr, aLocale, numberOfValues, EValueInteger, aIdColumnValue, EValueLanguage);
	if(!stmt)
		{
		return;
		}
	CleanupStack::PushL(stmt);	
	do
		{
		TPtrC name(stmt->StrColumnL(0));
		CPropertyEntry *entry = GetPropertyEntryL(*stmt, name, 1); // aStartingIndex=1
		CleanupStack::PushL(entry);
		if(KErrNotFound == aProperties.Find(entry, TIdentityRelation<CPropertyEntry>(CompareProperties)))
			{
			aProperties.AppendL(entry);
			CleanupStack::Pop(entry);	// because array is now owner
			}
		else
			{
			CleanupStack::PopAndDestroy(entry);
			}
		} while(stmt->ProcessNextRowL());
	CleanupStack::PopAndDestroy(stmt);
	}

void CScrRequestImpl::GetGeneralPropertiesArrayL(const TDesC& aTableName, const TDesC& aIdColumnName , TComponentId aIdColumnValue, TLanguage aLocale, RPointerArray<CPropertyEntry>& aProperties) const
	{
	DEBUG_PRINTF(_L8("Returning General Properties Array."));
	
	_LIT(KSelectGeneralLocalizableProperties, "SELECT Name,IntValue,StrValue,Locale,IsStr8Bit FROM %S WHERE %S=? AND Locale=?;");
	TInt formattedLen = aTableName.Length() + aIdColumnName.Length();
	HBufC *statementStr = FormatStatementLC(KSelectGeneralLocalizableProperties, formattedLen, &aTableName, &aIdColumnName );
	
	if(KUnspecifiedLocale == aLocale)
		{
		// Locale is not specified. First get non-localized properties
		AddGeneralPropertiesArrayWithLocaleL(*statementStr, KNonLocalized, aIdColumnValue, aProperties);		
		// Then get the localizable properties with the current locale
		AddGeneralPropertiesArrayWithLocaleL(*statementStr, User::Language(), aIdColumnValue, aProperties);
		}
	else if(KNonLocalized == aLocale)
		{
		// Only non-localized properties are requested.
		AddGeneralPropertiesArrayWithLocaleL(*statementStr, KNonLocalized, aIdColumnValue, aProperties);
		}
	else
		{
		// Locale is specified. It means that only properties with this locale are wanted
		AddGeneralPropertiesArrayWithLocaleL(*statementStr, aLocale, aIdColumnValue, aProperties);
		}
	
	CleanupStack::PopAndDestroy(statementStr);
#ifdef _DEBUG		
	if(!aProperties.Count())
		DEBUG_PRINTF4(_L("Property couldn't be found for column name=(%S), column id=%d and locale=%d."), &aIdColumnName, aIdColumnValue, aLocale);
#endif
	}

void CScrRequestImpl::GetFilePropertiesSizeL(const RMessage2& aMessage) const
	{
	DEBUG_PRINTF(_L8("Returning the size of file properties."));
	
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	HBufC* fileName  = ReadFileNameFromMsgLC(aMessage);
	
	TInt compFileId = FindComponentFileL(*fileName, componentId);
	
	iProperties.ResetAndDestroy();
	GetGeneralPropertiesArrayL(KFilePropertiesTable(), KCompFileIdColumnName, compFileId, KNonLocalized, iProperties);
	CleanupStack::PopAndDestroy(fileName);
	WriteArraySizeL(aMessage, 2, iProperties);
	}

void CScrRequestImpl::GetFilePropertiesDataL(const RMessage2& aMessage) const
	{
	DEBUG_PRINTF(_L8("Returning the file properties data."));
	WriteArrayDataL(aMessage, 0, iProperties);	
	}

CPropertyEntry* CScrRequestImpl::GetPropertyEntryL(CStatement& aStmt, const TDesC& aPropName, TInt aStartingIndex) const
	{
	DEBUG_PRINTF(_L8("Returning the file properties entry."));
	
	CPropertyEntry *entry(0);
		
	if(!aStmt.IsFieldNullL(aStartingIndex))
		{
		TInt64 intVal = aStmt.Int64ColumnL(aStartingIndex);
		entry = CIntPropertyEntry::NewL(aPropName, intVal);
		}
	else if(!aStmt.IsFieldNullL(aStartingIndex+1))
		{
		TBool is8BitString = (aStmt.IntColumnL(aStartingIndex + 3) != 0);
		if (is8BitString)
			{
			TPtrC8 binaryVal(aStmt.BinaryColumnL(aStartingIndex+1));
			entry = CBinaryPropertyEntry::NewL(aPropName, binaryVal);			
			}
		else
			{
			TPtrC strVal(aStmt.StrColumnL(aStartingIndex+1));
			TLanguage locale = TLanguage(aStmt.IntColumnL(aStartingIndex + 2));
			entry = CLocalizablePropertyEntry::NewL(aPropName, strVal, locale);			
			}
		}
	else
		{// Should never come here - shows DB inconsistency
		DEBUG_PRINTF(_L("Both integer and string values are NULL. Inconsistency problem!"));
		User::Leave(KErrCorrupt);
		}	
	return entry;
	}

CPropertyEntry* CScrRequestImpl::GetGeneralSinglePropertyL(const TDesC& aTableName, const TDesC& aIdColumnName , TInt aIdColumnValue, const TDesC& aPropName,  TLanguage aLocale) const
	{
	DEBUG_PRINTF5(_L("Getting general single property for %S. PropName=%S, Id=%d, Locale=%d"), &aIdColumnName , &aPropName, aIdColumnValue, aLocale);
	
	_LIT(KSelectSingleGeneralProperty, "SELECT IntValue,StrValue,Locale,IsStr8Bit FROM %S WHERE Name=? AND %S=? AND Locale=?;");
	TInt formattedLen = aTableName.Length() + aIdColumnName.Length();
	HBufC *statementStr = FormatStatementLC(KSelectSingleGeneralProperty, formattedLen, &aTableName, &aIdColumnName );
	
	CStatement *stmt = CreateGeneralPropertyStatementWithLocaleL(*statementStr, aIdColumnValue, aLocale, aPropName);
	if(!stmt)
		{
		DEBUG_PRINTF4(_L("Property couldn't be found for column name=(%S), column id=%d and locale=%d."), &aIdColumnName, aIdColumnValue, aLocale);
		CleanupStack::PopAndDestroy(statementStr);
		return NULL;
		}
	CleanupStack::PushL(stmt);
	
	CPropertyEntry *entry = GetPropertyEntryL(*stmt, aPropName, 0); // aStartingIndex=0 
	CleanupStack::PopAndDestroy(2, statementStr); // statementStr, stmt
	return entry;
	}

void CScrRequestImpl::GetSingleFilePropertySizeL(const RMessage2& aMessage) const
	{
	DEBUG_PRINTF(_L8("Returning the size of single file property."));
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	HBufC* fileName  = ReadFileNameFromMsgLC(aMessage);
	HBufC *propName = ReadDescLC(aMessage, 2);
	
	TInt compFileId = FindComponentFileL(*fileName, componentId);
	
	DeleteObjectZ(iSingleProperty);
	iSingleProperty = GetGeneralSinglePropertyL(KFilePropertiesTable(), KCompFileIdColumnName, compFileId, *propName, KNonLocalized);
	CleanupStack::PopAndDestroy(2, fileName); // fileName, propName
	WriteObjectSizeL(aMessage, 3, iSingleProperty);
	}

void CScrRequestImpl::GetSingleFilePropertyDataL(const RMessage2& aMessage) const
	{
	DEBUG_PRINTF(_L8("Returning the single file property data."));
	WriteObjectDataL(aMessage, 0, iSingleProperty);	
	DeleteObjectZ(iSingleProperty); // Delete the object to prevent it to be resent.
	}

void CScrRequestImpl::GetComponentFilesCountL(const RMessage2& aMessage) const
	{
	DEBUG_PRINTF(_L8("Returning the Component Files Count."));
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);

	DEBUG_PRINTF2(_L8("Returning the files count for component %d."), componentId);
	
	_LIT(KGetComponentFilesCount, "SELECT COUNT(CmpFileId) FROM ComponentsFiles WHERE ComponentId=?;");
	CStatement *stmt = iDbHandle->PrepareStatementLC(KGetComponentFilesCount);
	stmt->BindIntL(1, componentId);

	TBool res = stmt->ProcessNextRowL();
	// DB Query did not return a single row for component files count query - internal error!
	__ASSERT_ALWAYS(res, User::Leave(KErrAbort));

	TInt filesCountTmp = stmt->IntColumnL(0);
	__ASSERT_DEBUG(filesCountTmp >= 0, User::Leave(KErrAbort));
	TUint filesCount = static_cast<TUint>(filesCountTmp);
	CleanupStack::PopAndDestroy(stmt);
	
	TPckg<TUint> filesCountPkg(filesCount);
	aMessage.WriteL(1, filesCountPkg);
	}

void CScrRequestImpl::GetFileComponentsL(const TDesC& aFileName, RArray<TComponentId>& aComponents) const
	{
	DEBUG_PRINTF(_L8("Returning the File Components."));
	_LIT(KSelectFileComponents, "SELECT ComponentId FROM ComponentsFiles WHERE LocationHash=?;");
	CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectFileComponents);
	TUint32 hash = HashCaseInsensitiveL(aFileName);
	stmt->BindIntL(1, hash);
		
	while(stmt->ProcessNextRowL())
		{
		User::LeaveIfError(aComponents.InsertInOrder(stmt->IntColumnL(0)));
		}
	CleanupStack::PopAndDestroy(stmt);
	}

void CScrRequestImpl::GetFileComponentsSizeL(const RMessage2& aMessage) const
	{
	HBufC *fileName = ReadDescLC(aMessage, 0);
	
	iFileComponents.Reset();
	GetFileComponentsL(*fileName, iFileComponents);
	CleanupStack::PopAndDestroy(fileName);
	
	WriteArraySizeL(aMessage, 1, iFileComponents);
	}

void CScrRequestImpl::GetFileComponentsDataL(const RMessage2& aMessage) const
	{
	DEBUG_PRINTF(_L8("Returning the File Components Data."));
	WriteArrayDataL(aMessage, 0, iFileComponents);
	}
	
void CScrRequestImpl::GetComponentPropertiesSizeL(const RMessage2& aMessage) const
	{
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	TLanguage locale = TLanguage(aMessage.Int1());
	
	iProperties.ResetAndDestroy();
	GetGeneralPropertiesArrayL(KComponentPropertiesTable(), KComponentIdColumnName, componentId, locale, iProperties);
	WriteArraySizeL(aMessage, 2, iProperties);
	}

void CScrRequestImpl::GetComponentPropertiesDataL(const RMessage2& aMessage) const
	{
	WriteArrayDataL(aMessage, 0, iProperties);
	}

void CScrRequestImpl::GetComponentSinglePropertySizeL(const RMessage2& aMessage) const
	{ 
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	HBufC *propName = ReadDescLC(aMessage, 1);
	TLanguage locale = TLanguage(aMessage.Int2());
	
	DeleteObjectZ(iSingleProperty);
	iSingleProperty = GetGeneralSinglePropertyL(KComponentPropertiesTable(), KComponentIdColumnName, componentId, *propName, locale);
	CleanupStack::PopAndDestroy(propName);
	WriteObjectSizeL(aMessage, 3, iSingleProperty);
	}

void CScrRequestImpl::GetComponentSinglePropertyDataL(const RMessage2& aMessage) const
	{
	WriteObjectDataL(aMessage, 0, iSingleProperty);
	DeleteObjectZ(iSingleProperty); // Delete the object to prevent it to be resent.
	}

TUint32 CScrRequestImpl::ReadAndHashGlobalIdL(const RMessage2& aMessage, TInt aGlobalIdNameSlot, TInt aSwTypeNameSlot) const
	{
	HBufC *globalIdName = ReadDescLC(aMessage, aGlobalIdNameSlot);
	HBufC *swTypeName = ReadDescLC(aMessage, aSwTypeNameSlot);
	
	// Concatenate the software type id with the global id name
	HBufC *globalId = GenerateGlobalIdL(*swTypeName, *globalIdName);
	CleanupStack::PushL(globalId);
	// Hash the concatenated value
	TUint32 globalIdHash = HashCaseSensitiveL(*globalId);
	CleanupStack::PopAndDestroy(3, globalIdName); // globalIdName, swTypeName, concatGlobalId
	return globalIdHash;
	}

void CScrRequestImpl::GetComponentIdL(const RMessage2& aMessage) const
	{
	DEBUG_PRINTF(_L8("Returning the local component id of a received global id."));
	
	TUint32 globalIdHash = ReadAndHashGlobalIdL(aMessage, 0, 1);
	
	_LIT(KSelectComponentId, "SELECT ComponentId FROM Components WHERE GlobalIdHash=?;");
	CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectComponentId);
	stmt->BindIntL(1, globalIdHash);
			
	if(!stmt->ProcessNextRowL())
		{
		DEBUG_PRINTF2(_L8("There is no local component id corresponding to the supplied global id (hash=%d)."), globalIdHash);
		User::Leave(KErrNotFound);
		}
	
	TComponentId compId = stmt->IntColumnL(0);
	CleanupStack::PopAndDestroy(stmt);
	
	TPckg<TComponentId> componentIdDes(compId);
	aMessage.WriteL(2, componentIdDes);
	}

void CScrRequestImpl::GetComponentWithGlobalIdSizeL(const RMessage2& aMessage) const
	{
	TUint32 globalIdHash = ReadAndHashGlobalIdL(aMessage, 0, 1);
	TLanguage locale = TLanguage(aMessage.Int2());
	
	_LIT(KConditionColumn, "GlobalIdHash");
	TInt returnSizeSlot=3;
	GetGeneralComponentEntrySizeL(aMessage, KConditionColumn(), globalIdHash, returnSizeSlot, locale, iComponentEntry);
	}

void CScrRequestImpl::GetComponentWithGlobalIdDataL(const RMessage2& aMessage) const
	{
	WriteObjectDataL(aMessage, 0, iComponentEntry);
	DeleteObjectZ(iComponentEntry); // Delete the object to prevent the usage of this function
								    // without calling GetComponentWithGlobalIdSizeL.
	}

CGlobalComponentId* CScrRequestImpl::ParseGlobalComponendIdLC(const TDesC& aGlobalId) const
	{
	TChar nullCr = '\0';
	TInt pos = aGlobalId.Locate(nullCr);
	__ASSERT_ALWAYS(pos != KErrNotFound, User::Leave(KErrCorrupt));
	
	TPtrC swTypeName = aGlobalId.Left(pos);
	TPtrC globalIdName = aGlobalId.Right(aGlobalId.Length() - pos - 1);
	
	return CGlobalComponentId::NewLC(globalIdName, swTypeName);
	}

void CScrRequestImpl::GetGeneralDependencyListL(const TDesC& aSelectColumn, const TDesC& aConditionColumn, const TDesC& aConditionValue, RPointerArray<CVersionedComponentId> &aVerCompIdList) const
	{
	DEBUG_PRINTF(_L8("Returning General Dependency List."));
	_LIT(KSelectDependencies, "SELECT %S,VersionFrom,VersionTo FROM ComponentDependencies WHERE %S=?;");
	TInt formattedLen = aSelectColumn.Length() + aConditionColumn.Length();
	HBufC *stmtStr = FormatStatementLC(KSelectDependencies(), formattedLen, &aSelectColumn, &aConditionColumn);
	
	CStatement *stmt = iDbHandle->PrepareStatementLC(*stmtStr);
	TUint32 conditionValueHash = HashCaseSensitiveL(aConditionValue);
	stmt->BindIntL(1, conditionValueHash);
	
	while(stmt->ProcessNextRowL())
		{
		TPtrC selectedGlobalId(stmt->StrColumnL(0));
		CGlobalComponentId *globalId = ParseGlobalComponendIdLC(selectedGlobalId);
		
		TPtrC versionFrom;
		ReadNullableStringFromStatementL(*stmt, versionFrom, 1);
		TPtrC versionTo;
		ReadNullableStringFromStatementL(*stmt, versionTo, 2);
		
		CVersionedComponentId *verCompId = CVersionedComponentId::NewLC(*globalId, &versionFrom, &versionTo);
		aVerCompIdList.AppendL(verCompId);
		
		CleanupStack::Pop(verCompId); // Ownership passed to the array
		CleanupStack::PopAndDestroy(globalId);
		}
	
	CleanupStack::PopAndDestroy(2, stmtStr); // stmtStr, stmt
	}

void CScrRequestImpl::GetSupplierComponentsSizeL(const RMessage2& aMessage) const
	{
	HBufC *dependantGlobalId = ReadAndGetGlobalIdLC(aMessage, 0);
	
	_LIT(KSelectColumn, "SupplierGlobalId");
	_LIT(KConditionColumn, "DependantGlobalIdHash");
	iVerCompIdList.ResetAndDestroy();
	
	GetGeneralDependencyListL(KSelectColumn(), KConditionColumn(), *dependantGlobalId, iVerCompIdList);
	CleanupStack::PopAndDestroy(dependantGlobalId);
	WriteArraySizeL(aMessage, 1, iVerCompIdList);
	}

void CScrRequestImpl::GetSupplierComponentsDataL(const RMessage2& aMessage) const
	{
	WriteArrayDataL(aMessage, 0, iVerCompIdList);
	}

void CScrRequestImpl::GetDependantComponentsSizeL(const RMessage2& aMessage) const
	{

	HBufC *supplierGlobalId = ReadAndGetGlobalIdLC(aMessage, 0);
		
	_LIT(KSelectColumn, "DependantGlobalId");
	_LIT(KConditionColumn, "SupplierGlobalIdHash");
	iVerCompIdList.ResetAndDestroy();
		
	GetGeneralDependencyListL(KSelectColumn(), KConditionColumn(), *supplierGlobalId, iVerCompIdList);
	CleanupStack::PopAndDestroy(supplierGlobalId);
	WriteArraySizeL(aMessage, 1, iVerCompIdList);
	}

void CScrRequestImpl::GetDependantComponentsDataL(const RMessage2& aMessage) const
	{
	DEBUG_PRINTF(_L8("Returning Dependant Components Data."));
	WriteArrayDataL(aMessage, 0, iVerCompIdList);
	}
		
void CScrRequestImpl::GetIsMediaPresentL(const RMessage2& aMessage) const
	{
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	DEBUG_PRINTF2(_L8("Retrieving media presence of component(%d)."), componentId);

	TBool result = CheckForMediaPresenceL(componentId);
	
	TPckg<TBool> resultPkg(result);
	aMessage.WriteL(1, resultPkg);	
	}

TBool CScrRequestImpl::CheckForMediaPresenceL(TComponentId aComponentId) const
	{
	DEBUG_PRINTF(_L8("Check For Media Presence."));
	TInt componentDrivesBitmask = GetInstalledDrivesBitmaskL(aComponentId);	
	
	TDriveList presentDriveSet;
	User::LeaveIfError(iFs.DriveList(presentDriveSet));	
	
	// Check for each drive whether it is present in the system
	for (TInt drive=0; componentDrivesBitmask; componentDrivesBitmask >>= 1, ++drive)
		{
		if ((componentDrivesBitmask & 0x1) && (presentDriveSet.Length() <= drive || !presentDriveSet[drive]))
			{
			DEBUG_PRINTF3(_L8("Component %d registers a file at drive %c which is not present. Returning EFalse for IsMediaPresent)."), aComponentId, drive + 'a');
			return EFalse;			
			}
		}
	
	return ETrue;
	}

void CScrRequestImpl::SetScomoStateL(const RMessage2& aMessage)
	{
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	TScomoState state = static_cast<TScomoState>(aMessage.Int1());
	DEBUG_PRINTF3(_L8("Updating the scomo state of component(%d). New value=%d."), componentId, state);
	
	_LIT(KUpdateScomoState, "UPDATE Components SET ScomoState=? WHERE ComponentId=?;");
	TInt numberOfValues = 2;
	ExecuteStatementL(KUpdateScomoState, numberOfValues, EValueInteger, state, EValueInteger, componentId);
	}

void CScrRequestImpl::GetPluginUidWithMimeTypeL(const RMessage2& aMessage) const
	{
	DEBUG_PRINTF(_L8("Returning plugin for MIME type."));
	HBufC *mimeType = ReadDescLC(aMessage, 0);
	
	_LIT(KSelectPluginsWithMimeType, "SELECT SifPluginUid FROM SoftwareTypes WHERE SoftwareTypeId IN (SELECT SoftwareTypeId FROM MimeTypes WHERE MimeType=?);");
	CStatement* stmt = iDbHandle->PrepareStatementLC(KSelectPluginsWithMimeType);
	stmt->BindStrL(1, *mimeType);
	if(!stmt->ProcessNextRowL())
		{
		DEBUG_PRINTF2(_L("Sif Plugin Uid couldn't be found for Mime Type (%S)!"), mimeType);
		User::Leave(KErrSifUnsupportedSoftwareType);
		}
	TUint32 uid = stmt->IntColumnL(0);
	CleanupStack::PopAndDestroy(2, mimeType); // mimeType, stmt
	
	TPckg<TUint32> uidDes(uid);
	aMessage.WriteL(1, uidDes);
	}

TBool CScrRequestImpl::GetSifPluginUidIInternalL(TInt aSoftwareTypeId, TInt& aValue) const
	{
	DEBUG_PRINTF(_L8("Returning Sif Plugin UidI Internal."));
	TBool found = EFalse;	
		
	_LIT(KSelectSifPluginUid, "SELECT SifPluginUid FROM SoftwareTypes WHERE SoftwareTypeId=?;");
	CStatement* stmt = iDbHandle->PrepareStatementLC(KSelectSifPluginUid);
	stmt->BindIntL(1, aSoftwareTypeId);
	
	if(stmt->ProcessNextRowL())
		{
		aValue = stmt->IntColumnL(0);
		found = ETrue;
		}
	CleanupStack::PopAndDestroy(stmt);
	return found;
	}

TBool CScrRequestImpl::GetSidsForSoftwareTypeIdL(TInt aSoftwareTypeId, RArray<TSecureId>& aSids) const
    {
    TBool found = EFalse;
       
    _LIT(KSelectSecureIds, "SELECT SecureId FROM CustomAccessList WHERE SoftwareTypeId=?;");
    CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectSecureIds);
    stmt->BindIntL(1, aSoftwareTypeId);
    
    aSids.Reset();
    while(stmt->ProcessNextRowL())
        {
        aSids.AppendL(stmt->IntColumnL(0));
        found = ETrue;
        }
    
    CleanupStack::PopAndDestroy(stmt);
    return found;
    }

TBool CScrRequestImpl::GetInstallerOrExecutionEnvSidsForComponentL(TComponentId aComponentId, RArray<TSecureId>& aSids) const
    {
    // Get the software type id for the component
    TInt swTypeId = GetSoftwareTypeForComponentL(aComponentId);
    // Retrieve the Sids for the type id
    return GetSidsForSoftwareTypeIdL(swTypeId, aSids);
    }

TBool CScrRequestImpl::IsInstallerOrExecutionEnvSidL(TSecureId& aSid) const
	{
	_LIT(KSelectStatement, "SELECT SecureId FROM CustomAccessList WHERE SecureId=? AND AccessMode=?;");
	CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectStatement);
	stmt->BindIntL(1, aSid);
	stmt->BindIntL(2, (TInt)ETransactionalSid);	
	TBool res = ETrue;
	if(!stmt->ProcessNextRowL())
		{
		DEBUG_PRINTF2(_L("%d is not an installer or execution environment SID"), TUint32(aSid));
		res = EFalse;
		}
	CleanupStack::PopAndDestroy(stmt);
	return res;	
	}

void CScrRequestImpl::GetPluginUidWithComponentIdL(const RMessage2& aMessage) const
	{
	DEBUG_PRINTF(_L8("Returning Plugin Uid With ComponentId."));
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	DEBUG_PRINTF2(_L8("Returning the plugin of component(%d)."), componentId);
	
	// Get the software type id for the component
	TInt swTypeId = GetSoftwareTypeForComponentL(componentId);
	    
	TInt uid (0);
	TBool found = GetSifPluginUidIInternalL(swTypeId, uid);
	__ASSERT_ALWAYS(found, User::Leave(KErrNotFound));
	
	TPckg<TUint32> uidDes(uid);
	aMessage.WriteL(1, uidDes);
	}

// This function returns whether the SID looked up has been found in the software types table.
TBool CScrRequestImpl::GetSidsForSoftwareTypeL(const HBufC* aSoftwareTypeName, RArray<TSecureId>& aSids) const
	{
	DEBUG_PRINTF2(_L("Returning SIDs for software type(%S)."), aSoftwareTypeName);
	
	TUint32 swTypeId = HashCaseSensitiveL(*aSoftwareTypeName);	
	return GetSidsForSoftwareTypeIdL(swTypeId, aSids);
	}

TBool CScrRequestImpl::CheckIfAppUidExistsL(const TUid aAppUid) const
    {
    DEBUG_PRINTF(_L8("Check if the given application UID exists."));
    TUid retrievedAppUid;
    _LIT(KSelectColumn, "AppUid");
    _LIT(KTable, "AppRegistrationInfo");
    _LIT(KConditionColumn, "AppUid");
    if(GetIntforConditionL(KSelectColumn, KTable, KConditionColumn, aAppUid.iUid, (TInt&)retrievedAppUid.iUid))
        {    
       	return ETrue;
    	}
    return EFalse ; 
    }
	
void CScrRequestImpl::SetLocaleForRegInfoForApplicationSubsessionContextL(const RMessage2& aMessage, CRegInfoForApplicationSubsessionContext *aSubsessionContext)
    {
    TLanguage appLocale= (TLanguage)aMessage.Int1();
    TUid appUid;
    appUid.iUid=aMessage.Int0();
    if(!GetNearestAppLanguageL(appLocale,appUid, aSubsessionContext->iAppLanguageForCurrentLocale))
    	{
      	if (KUnspecifiedLocale!=appLocale)
      		{
        	DEBUG_PRINTF2(_L8("Given Locale %d is not supported by application"),appLocale);
        	User::Leave(KErrNotFound);
      		}
    	}
    }
	
void CScrRequestImpl::GetServiceUidSizeL(const RMessage2& aMessage,TUid aAppUid, CRegInfoForApplicationSubsessionContext *aSubsessionContext) const
    {    
    TUid uid;
    _LIT(KSelectServiceUidWithAppUid, "SELECT Uid FROM ServiceInfo WHERE AppUid =?;");
    CStatement* stmt = iDbHandle->PrepareStatementLC(KSelectServiceUidWithAppUid);
    stmt->BindIntL(1, aAppUid.iUid);
    aSubsessionContext->iServiceUidList.Close();
    while(stmt->ProcessNextRowL())
    {    
        uid.iUid = stmt->IntColumnL(0);       
        aSubsessionContext->iServiceUidList.Append(uid);       
    }
    if(aSubsessionContext->iServiceUidList.Count() == 0)
    {
        DEBUG_PRINTF2(_L8("No service ids correspond to the given AppUid,%d"),aAppUid);
    }    
    CleanupStack::PopAndDestroy(stmt);
    WriteArraySizeL(aMessage, 1, aSubsessionContext->iServiceUidList);
    }

void CScrRequestImpl::GetServiceUidDataL(const RMessage2& aMessage, CRegInfoForApplicationSubsessionContext *aSubsessionContext) const
{
    WriteArrayDataL(aMessage, 0, aSubsessionContext->iServiceUidList);
    aSubsessionContext->iServiceUidList.Close();
}

void CScrRequestImpl::GetApplicationLanguageL(const RMessage2& aMessage, CRegInfoForApplicationSubsessionContext *aSubsessionContext) const
    {
    DEBUG_PRINTF(_L8("Returning the Application Language ."));
    TPckg<TLanguage> applicationLanguage (aSubsessionContext->iAppLanguageForCurrentLocale);
    aMessage.WriteL(0, applicationLanguage);
    }

void CScrRequestImpl::GetDefaultScreenNumberL(const RMessage2& aMessage,TUid aAppUid) const
    {
    DEBUG_PRINTF(_L8("Returning the default screen number."));             
    
    TInt screenNumber=0;
    _LIT(KSelectColumn, "DefaultScreenNumber");
    _LIT(KTable, "AppRegistrationInfo");
    _LIT(KConditionColumn, "AppUid");
        
    GetIntforConditionL(KSelectColumn(),KTable(),KConditionColumn(),aAppUid.iUid, screenNumber);
        {
        TPckg<TInt> screenNumberDes(screenNumber);
        aMessage.WriteL(0, screenNumberDes);
        }
    }

void CScrRequestImpl::GetNumberOfOwnDefinedIconsL(const RMessage2& aMessage,TUid aAppUid , CRegInfoForApplicationSubsessionContext *aSubsessionContext) const
    {
    DEBUG_PRINTF(_L8("Returning the no of own defined Icons count "));
    TInt numberOfIcons;
    _LIT(KSelectNoOfIcones, "SELECT NumberOfIcons FROM CaptionAndIconInfo WHERE  CaptionAndIconId IN (SELECT CaptionAndIconId FROM LocalizableAppInfo WHERE AppUid= ? AND Locale = ?);");
    CStatement* stmt = iDbHandle->PrepareStatementLC(KSelectNoOfIcones);
    stmt->BindIntL(1, aAppUid.iUid);
    stmt->BindIntL(2, aSubsessionContext->iAppLanguageForCurrentLocale);
    if(stmt->ProcessNextRowL())
       {
       numberOfIcons = stmt->IntColumnL(0);
       TPckg<TInt> numberOfIconsDes(numberOfIcons);
       aMessage.WriteL(0, numberOfIconsDes);
       }
    CleanupStack::PopAndDestroy(stmt);
    }

void CScrRequestImpl::GetAppForDataTypeAndServiceL(const RMessage2& aMessage) const
    {
    DEBUG_PRINTF(_L8("Returns the App Uid for a given Service Uid that handles the specified datatype with the highest priority."));
    HBufC *dataType = ReadDescLC(aMessage,0);
    TUid inputServiceUid = TUid::Uid(aMessage.Int1());

    _LIT(KSelectAppUidsForGivenUidAndDataType, "SELECT AppUid FROM (ServiceInfo JOIN DataType ON ServiceInfo.ServiceId = DataType.ServiceId) WHERE Uid=? AND Type=? ORDER BY Priority DESC;");
    CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectAppUidsForGivenUidAndDataType);
    stmt->BindIntL(1, inputServiceUid.iUid);
    stmt->BindStrL(2, *dataType);

    if(stmt->ProcessNextRowL())
        {
        TPckg<TInt> appUidDes(stmt->IntColumnL(0));
        aMessage.WriteL(2, appUidDes); 
        CleanupStack::PopAndDestroy(2, dataType); //stmt, dataType
        }
    else
        {
        DEBUG_PRINTF(_L("No AppUid for given datatype and Service Uid"));
        CleanupStack::PopAndDestroy(2, dataType); //stmt, dataType        
        User::Leave(KErrNotFound);
        }
    }

void CScrRequestImpl::GetAppForDataTypeL(const RMessage2& aMessage) const
    {
    DEBUG_PRINTF(_L8("Returning the app UID."));
    
    TUid appUid = TUid::Null();
    HBufC *type = ReadDescLC(aMessage, 0);
    TInt serviceId = GetServiceIdForDataTypeL(*type);
    
    if(serviceId == KErrNotFound)
        {
        DEBUG_PRINTF(_L("No Service Id for the given datatype"));
        User::Leave(KErrNotFound);
        }
               
    CleanupStack::PopAndDestroy(type);
    if(!GetAppUidForServiceIdL(serviceId, appUid))
        {
        DEBUG_PRINTF(_L("No AppUid for given datatype"));
        }
    TPckg<TUid> appUidDes(appUid);
    aMessage.WriteL(1, appUidDes);       
    }

TInt CScrRequestImpl::GetServiceIdForDataTypeL(const TDesC& aType) const
    {
	DEBUG_PRINTF(_L8("Returning ServiceId For DataType."));
    TInt serviceId = 0;
    _LIT(KSelectAppForDataTypeAndService, "SELECT ServiceId FROM DataType WHERE Type=? ORDER BY Priority DESC;");    
    CStatement* stmt = iDbHandle->PrepareStatementLC(KSelectAppForDataTypeAndService);
    stmt->BindStrL(1, aType);
                
    if(!stmt->ProcessNextRowL())
        {
        DEBUG_PRINTF(_L("No Service for given datatype"));
        }
    else
        {
        serviceId = stmt->IntColumnL(0);
        }
    CleanupStack::PopAndDestroy(stmt);
    return serviceId;
    }
    
TBool CScrRequestImpl::GetAppUidForServiceIdL(const TInt ServiceId, TUid& aAppUid) const
    {
	DEBUG_PRINTF(_L8("Returning App Uid for ServiceId."));
    _LIT(KSelectColumn, "AppUid");
    _LIT(KTable, "ServiceInfo");
    _LIT(KConditionColumn, "ServiceId");
    if(GetIntforConditionL(KSelectColumn(),KTable(),KConditionColumn(),ServiceId, (TInt&)aAppUid.iUid))
        {
        return ETrue;
        }
    else
        {
        DEBUG_PRINTF(_L("No AppUid for given datatype"));
        return EFalse;
        }
    }

void CScrRequestImpl::GetNearestAppLanguageForOpaqueDataL(TLanguage aRequiredLocale, TUid aAppUid, TUid aServiceUid, TLanguage& aFinalAppLocale) const
    {
	DEBUG_PRINTF(_L8("Returning Nearest App Language For OpaqueData."));
    // Set the default language
    aFinalAppLocale = TLanguage(0);
    
    if (aRequiredLocale == KUnspecifiedLocale)
        {
        aRequiredLocale = User::Language();
        }
        
    // Get the list of locales supported by the application
    RArray<TInt> appLocales;
    CleanupClosePushL(appLocales);
    GetLocalesForAppIdL(appLocales, aAppUid);
    
    // Return the default language if the app doesn't have localized info
    if (!appLocales.Count())
        {
        CleanupStack::PopAndDestroy(&appLocales);
        return;            
        }
  
    // Check if current language is supported by application
    if (KErrNotFound != appLocales.Find((TInt)aRequiredLocale))
        {
        aFinalAppLocale = aRequiredLocale;
        }
    else // Get the nearest languages corresponding to the required language
        {    
        TLanguagePath equivalentLanguages;
        BaflUtils::GetEquivalentLanguageList(aRequiredLocale, equivalentLanguages);
            
        // Identify the application locale corresponding to the nearest required locale
        TInt index = 0;
        while (1)
            {
            if (equivalentLanguages[index] == ELangNone)
                {
                break;
                }
            
            if (appLocales.FindInOrder((TInt)equivalentLanguages[index]) != KErrNotFound)
                {
                aFinalAppLocale = equivalentLanguages[index];
                break;
                }           
    
            index++;
            }
        }
    
    // Check the opaque data exists in the selected language
    if (aFinalAppLocale != TLanguage(0))
        {
        _LIT(KOpaqueData, "SELECT StrValue FROM AppProperties where Name = ? AND ServiceUid = ?  AND AppUid = ? AND Locale = ?");
        CStatement *stmt = iDbHandle->PrepareStatementLC(KOpaqueData);        
        stmt->BindStrL(1, _L("OpaqueData"));
        stmt->BindIntL(2, aServiceUid.iUid);
        stmt->BindIntL(3, aAppUid.iUid);
        stmt->BindIntL(4, (TInt)aFinalAppLocale);
        
        if (!stmt->ProcessNextRowL())
            {
            // No opaque data for given locale, hence set to the default locale
            aFinalAppLocale = TLanguage(0);
            }
        CleanupStack::PopAndDestroy(stmt);
        }
       
    CleanupStack::PopAndDestroy(&appLocales);
    }

TBool CScrRequestImpl::GetNearestAppLanguageL(TLanguage aRequiredLocale, TUid aAppUid, TLanguage& aFinalAppLocale) const
    {
	DEBUG_PRINTF(_L8("Returning Nearest App Language."));
    TLanguagePath equivalentLanguages;
    TInt index = 0;
    RArray<TInt> appLocales;
    TBool isLocalizedInfoPresent = EFalse;
    aFinalAppLocale = KUnspecifiedLocale;
    
    //If required language is not specified, take the current user language. 
    if (aRequiredLocale == KUnspecifiedLocale)
        {
        aRequiredLocale = User::Language();
        }
        
    //Get the app language list.
    CleanupClosePushL(appLocales);
    GetLocalesForAppIdL(appLocales,aAppUid);
  
    //Check if current language is supported by application
    if (KErrNotFound != appLocales.Find((TInt)aRequiredLocale))
        {
        aFinalAppLocale = aRequiredLocale;
        isLocalizedInfoPresent = ETrue;
        CleanupStack::PopAndDestroy(&appLocales);
        return isLocalizedInfoPresent;
        }
    
    //Get the nearest languages corresponding to the required language.
    BaflUtils::GetEquivalentLanguageList(aRequiredLocale, equivalentLanguages);
        
    //Identify the app language corresponding to the nearest required language.
    if(0 != appLocales.Count()) 
        {
        while(1)
            {
            if(equivalentLanguages[index] == ELangNone)
                {
                break;
                }
            
            if(appLocales.FindInOrder((TInt)equivalentLanguages[index]) != KErrNotFound)
                {
                aFinalAppLocale = equivalentLanguages[index];
                isLocalizedInfoPresent = ETrue;
                break;
                }           

            index++;
            }
        
        // If a matching language is not found in the list of equivalent languages,
        // we check if a default locale (KNonLocalized) is present.
        if(!isLocalizedInfoPresent && appLocales[0] == (TInt)KNonLocalized)
            {
            isLocalizedInfoPresent = ETrue;
            aFinalAppLocale = KNonLocalized;
            }        
        }
    
    CleanupStack::PopAndDestroy(&appLocales);
    return isLocalizedInfoPresent;
    }

TBool CScrRequestImpl::GetIntforConditionL(const TDesC& aSelectColumn,const TDesC& aTableInfo,const TDesC& aConditionColumn,TInt aConditionValue,TInt& aRetrievedValue) const 
    {     
    TBool isIntValFound = 0;
    _LIT(KSelectDependencies, "SELECT %S FROM %S WHERE %S=?;");
    TInt formattedLen = aSelectColumn.Length() + aTableInfo.Length()+ aConditionColumn.Length();
    HBufC *stmtStr = FormatStatementLC(KSelectDependencies(), formattedLen, &aSelectColumn, &aTableInfo,&aConditionColumn);
               
    CStatement* stmt = iDbHandle->PrepareStatementLC(*stmtStr);
    stmt->BindIntL(1, aConditionValue);
    if(!stmt->ProcessNextRowL())
       {
       DEBUG_PRINTF3(_L("%S with value %d not found!"), &aSelectColumn, aConditionValue);           
       }
    else
       {
       aRetrievedValue = stmt->IntColumnL(0);
       isIntValFound = 1;
       }
    CleanupStack::PopAndDestroy(2,stmtStr);
    
    return isIntValFound;
    }
    
void IntersectSortedArraysL(RArray<TComponentId>& aLhs, RArray<TComponentId> aRhs)
	{
	RArray<TComponentId> tmp;
	CleanupClosePushL(tmp);
	TInt idxLhs(0);
	TInt idxRhs(0);
	TInt countLhs = aLhs.Count();
	TInt countRhs = aRhs.Count();
	
	while(idxLhs < countLhs && idxRhs < countRhs)
		{
		if(aLhs[idxLhs] < aRhs[idxRhs])
			++idxLhs;
		else if(aLhs[idxLhs] > aRhs[idxRhs])
			++idxRhs;
		else
			{
			tmp.AppendL(aRhs[idxRhs]);
			++idxRhs;
			++idxLhs;
			}
		}
	
	aLhs.Reset();
	CopyFixedLengthArrayL(aLhs, tmp);
	CleanupStack::PopAndDestroy(&tmp);
	}

void CScrRequestImpl::GetOperatorStringL(CComponentFilter::TDbOperator aOperator, HBufC*& aOperatorString) const
    {
    _LIT(KEqualOperator, "=");
    _LIT(KLikeOperator, "LIKE");
    
    switch(aOperator)
        {
        case CComponentFilter::EEqual:
            aOperatorString = KEqualOperator().AllocL();
            break;
            
        case CComponentFilter::ELike:
            aOperatorString = KLikeOperator().AllocL();
            break;   
            
        default:
            User::Leave(KErrArgument);
			        
        }      
    }

void CScrRequestImpl::GetComponentIdsHavingThePropertiesL(RArray<TComponentId>& aComponentIds, RPointerArray<CPropertyEntry>& aProperties, 
                                                              RArray<CComponentFilter::TPropertyOperator>& aPropertyOperatorList, TBool aDoIntersect) const
    {
    // GROUP BY is added at the end to avoid fetching twice components which have the same localizable values for different locales
    
    _LIT(KFindComponentsFromProperties, "SELECT ComponentId FROM ComponentProperties WHERE NAME %S ? AND %S GROUP BY ComponentId;");

    _LIT(KPropertyIntValue, "IntValue %S ?");
    _LIT(KPropertyStrValue, "StrValue %S ?");
    _LIT(KPropertyLocalizedValue, "StrValue %S ? AND Locale = ?");
            
    TInt propCount = aProperties.Count();

    RArray<TComponentId> propCompIds;
    CleanupClosePushL(propCompIds);
    
    for(TInt i=0; i<propCount; ++i)
        {
        //Retrieve the operators to be used to form this query.
        CComponentFilter::TPropertyOperator propOperator = aPropertyOperatorList[i];
       
	   // Get the name operator.
	    HBufC* nameOperator(0);
        GetOperatorStringL(aPropertyOperatorList[i].NameOperator(), nameOperator);
        CleanupStack::PushL(nameOperator);     
        
        // Get the value operator.
        HBufC* valueOperator(0);
        GetOperatorStringL(aPropertyOperatorList[i].ValueOperator(), valueOperator);
        CleanupStack::PushL(valueOperator);
        
        // Create the value string based on the property type.
        HBufC* valueString(0);
        switch(aProperties[i]->PropertyType())
            {            
            case CPropertyEntry::EIntProperty:
                {
                valueString = FormatStatementLC(KPropertyIntValue, valueOperator->Length(), valueOperator);               
                }

                break;
                
            case CPropertyEntry::EBinaryProperty:
                {
                valueString = FormatStatementLC(KPropertyStrValue, valueOperator->Length(), valueOperator);            
                }

                break;
            case CPropertyEntry::ELocalizedProperty:
                {
                CLocalizablePropertyEntry *localizedProp = static_cast<CLocalizablePropertyEntry *>(aProperties[i]);
                if (localizedProp->LocaleL() == KUnspecifiedLocale) // If the locale was not specified, then we match across all locales  
                    {
                    valueString = FormatStatementLC(KPropertyStrValue, valueOperator->Length(), valueOperator);        
                    }
                else
                    {
                    valueString = FormatStatementLC(KPropertyLocalizedValue, valueOperator->Length(), valueOperator);                
                    }
                }
                break;              
            default:
                DEBUG_PRINTF(_L8("The property type couldn't be recognized."));
                User::Leave(KErrAbort); 
            }
        
        // Prepare full statement using the name operator and the value string.
        HBufC *statementStr = FormatStatementLC(KFindComponentsFromProperties, nameOperator->Length()+ valueString->Length(), 
                                                    nameOperator, valueString); 
        
        // Create Sql statement.
        CStatement *stmt = iDbHandle->PrepareStatementLC(*statementStr);
        stmt->BindStrL(1, aProperties[i]->PropertyName());
                
        switch(aProperties[i]->PropertyType())
            {
            case CPropertyEntry::EIntProperty:
                {
                CIntPropertyEntry *intProp = static_cast<CIntPropertyEntry *>(aProperties[i]);
                stmt->BindInt64L(2, intProp->Int64Value());
                break;
                }
            case CPropertyEntry::EBinaryProperty:
                {
                CBinaryPropertyEntry *binaryProp = static_cast<CBinaryPropertyEntry *>(aProperties[i]);
                stmt->BindBinaryL(2, binaryProp->BinaryValue());
                break;
                }
            case CPropertyEntry::ELocalizedProperty:
                {
                CLocalizablePropertyEntry *localizedProp = static_cast<CLocalizablePropertyEntry *>(aProperties[i]);
                stmt->BindStrL(2, localizedProp->StrValue());
                if (localizedProp->LocaleL() != KUnspecifiedLocale)
                    stmt->BindIntL(3, localizedProp->LocaleL());
                break;
                }
            default:
                DEBUG_PRINTF(_L8("The property type couldn't be recognized."));
                User::Leave(KErrAbort); 
            }
        
        while(stmt->ProcessNextRowL())
            {
            User::LeaveIfError(propCompIds.InsertInOrder(stmt->IntColumnL(0)));
            }
        
        CleanupStack::PopAndDestroy(5, nameOperator);
        
        if (aDoIntersect)
            {
            IntersectSortedArraysL(aComponentIds, propCompIds);
            }
        else
            {
            CopyFixedLengthArrayL(aComponentIds, propCompIds);
            aDoIntersect = ETrue; // If many properties are present in the filter, we need to intersect the component Ids on further iterations
            }
        
        propCompIds.Reset();
        }
    CleanupStack::PopAndDestroy(&propCompIds);
    }


void ReallocStatementIfNeededL(RBuf& aStatementStr, TInt aAddedLength)
	{
	TInt freeSpace = aStatementStr.MaxLength() - aStatementStr.Length();
	TInt extraSpace = aAddedLength - freeSpace;
	if (extraSpace > 0)
		{
		aStatementStr.ReAllocL(aStatementStr.MaxLength() +  extraSpace);
		}	
	}
	
void AppendConditionToStatementL(RBuf& aStatementStr, const TDesC& aConditionStr, TBool& aIsFirstCondition)
	{
	TInt addedLength = 2 + aConditionStr.Length(); // 2 chars used by spaces
	
	_LIT(KConditionAnd, " AND");
	_LIT(KConditionWhere, " WHERE");
	
	if(aIsFirstCondition)
		{
		addedLength += KConditionWhere().Length();
		}
	else
		{
		addedLength += KConditionAnd().Length();
		}
	
	ReallocStatementIfNeededL(aStatementStr, addedLength);
	
	if(aIsFirstCondition)
		{
		aStatementStr.Append(KConditionWhere);
		aIsFirstCondition = EFalse;
		}
	else
		{
		aStatementStr.Append(KConditionAnd);
		}
	aStatementStr.Append(aConditionStr);
	}

CStatement* CScrRequestImpl::CreateStatementObjectForComponentLocalizablesLC(const TDesC& aName, const TDesC& aVendor, TUint aSetFlag, TComponentId aComponentId /* = 0*/ ) const
	{
	_LIT(KSelectComponentLocalizables, "SELECT ComponentId,Name,Vendor FROM ComponentLocalizables");
	RBuf stmtStr;
	stmtStr.CreateL(100); // 100 should be enough for this statement
	stmtStr.CleanupClosePushL();
	stmtStr.Copy(KSelectComponentLocalizables);
	TBool isFirstCondition = ETrue;
	
	if(aComponentId > 0)
		{
		_LIT(KConditionComponentId, " ComponentId=?");
		AppendConditionToStatementL(stmtStr, KConditionComponentId, isFirstCondition);	
		}
	
	if (aSetFlag & CComponentFilter::EName)
		{
		_LIT(KConditionName, " Name=?");
		AppendConditionToStatementL(stmtStr, KConditionName, isFirstCondition);	
		}
		
	if (aSetFlag & CComponentFilter::EVendor)
		{
		_LIT(KConditionVendor, " Vendor=?");
		AppendConditionToStatementL(stmtStr, KConditionVendor, isFirstCondition);
		}		
	// Take care not to return the same component id twice in case we have the same name or vendor for two different locales
	_LIT(KComponentIdGroupBy, " GROUP BY ComponentId;");
	ReallocStatementIfNeededL(stmtStr, KComponentIdGroupBy().Length() + 1); // one space for the NULL char
	stmtStr.Append(KComponentIdGroupBy());
	// SQLite requires the statement string to end with NULL char.
	stmtStr.Append('\0');
	
	CStatement *stmt = iDbHandle->PrepareStatementLC(stmtStr);
	
	TInt bindIdx = 1;
	if(aComponentId > 0)
		{
		stmt->BindIntL(bindIdx++, aComponentId);
		}
	if (aSetFlag & CComponentFilter::EName)
		{
		stmt->BindStrL(bindIdx++, aName);
		}
	if (aSetFlag & CComponentFilter::EVendor)
		{
		stmt->BindStrL(bindIdx, aVendor);
		}
	CleanupStack::Pop(stmt);
	CleanupStack::PopAndDestroy(&stmtStr);
	CleanupStack::PushL(stmt);
	return stmt;
	}

void CScrRequestImpl::GetComponentsHavingNameVendorL(RArray<TComponentId>& aComponentIds, const TDesC& aName, const TDesC& aVendor, TUint16 aSetFlag, TBool aDoIntersect) const
	{
	CStatement *stmt = CreateStatementObjectForComponentLocalizablesLC(aName, aVendor, aSetFlag); 
	
	RArray<TComponentId> foundCompIds;
	CleanupClosePushL(foundCompIds);
	
	while(stmt->ProcessNextRowL())
		{
		User::LeaveIfError(foundCompIds.InsertInOrder(stmt->IntColumnL(0)));
		}
	
	if (aDoIntersect)
		IntersectSortedArraysL(aComponentIds, foundCompIds);
	else
		CopyFixedLengthArrayL(aComponentIds, foundCompIds);

	CleanupStack::PopAndDestroy(2, stmt); // stmt, foundCompIds
	}

CComponentFilter* CScrRequestImpl::ReadComponentFilterL(const RMessage2& aMessage) const
	{
	CComponentFilter *filter = ReadObjectFromMessageLC<CComponentFilter>(aMessage, 0);
	CleanupStack::Pop(filter);
	return filter;
	}

void AppendFilterConditionToStatementL(TUint16 aSetFlag, TUint16 aConditionFlag, RBuf& aStatementStr, const TDesC& aConditionStr, TBool& aIsFirstCondition)
	{
	if(aSetFlag & aConditionFlag)
		{
		AppendConditionToStatementL(aStatementStr, aConditionStr, aIsFirstCondition);
		}
	}

void BindIntegerFilterToStatementL(CStatement& aStatement, TUint16 aSetFlag, TUint16 aConditionFlag, TInt aValue, TInt& aStmtIdx)
	{
	if(aSetFlag & aConditionFlag)
		{
		aStatement.BindIntL(aStmtIdx++, aValue);
		}
	}

CStatement* CScrRequestImpl::OpenComponentViewL(CComponentFilter& aFilter, RArray<TComponentId>& aComponentFilterSuperset, TBool& aFilterSupersetInUse) const
	{
	DEBUG_PRINTF(_L8("Opening Component View."));
	aComponentFilterSuperset.Reset();
	aFilterSupersetInUse = EFalse;

	TBool doIntersect = EFalse; // We should intersect component ids iff one of the previous filters on component ids was invoked
	if(aFilter.iSetFlag & CComponentFilter::EFile)
		{
		GetFileComponentsL(*aFilter.iFile, aComponentFilterSuperset);
		// The component Ids which own the given file is copied into componentIds array.
		doIntersect = ETrue;
		aFilterSupersetInUse = ETrue;
		}
	
	if(aFilter.iSetFlag & CComponentFilter::EProperty)
		{
		GetComponentIdsHavingThePropertiesL(aComponentFilterSuperset, aFilter.iPropertyList, aFilter.iPropertyOperatorList, doIntersect);
		// Inside the function, componentIds array is intersected with the components Ids which have the given properties.
		doIntersect = ETrue;
		aFilterSupersetInUse = ETrue;
		}
	
	if(aFilter.iSetFlag & (CComponentFilter::EName | CComponentFilter::EVendor))
		{
		GetComponentsHavingNameVendorL(aComponentFilterSuperset, *aFilter.iName, *aFilter.iVendor, aFilter.iSetFlag, doIntersect);
		// Inside the function, componentIds array is intersected with the components Ids which have the given name/vendor.
		aFilterSupersetInUse = ETrue;
		}
	
	_LIT(KSelectComponents, "SELECT ComponentId,SoftwareTypeId,SoftwareTypeName,Removable,Size,ScomoState,DRMProtected,Hidden,KnownRevoked,OriginVerified,GlobalId,InstalledDrives,Version,InstallTime FROM Components");
	
	RBuf statementStr;
	statementStr.CreateL(KSelectComponents().Length()+ 50); // A reasonable starting buffer length
	statementStr.CleanupClosePushL();
	statementStr.Copy(KSelectComponents);
	
	TBool isFirstCondition = ETrue;
	
	_LIT(KConditionSoftwareType, " SoftwareTypeId=?"); // as only unique sw type name can be defined in the filter.
	AppendFilterConditionToStatementL(aFilter.iSetFlag, CComponentFilter::ESoftwareType, statementStr, KConditionSoftwareType, isFirstCondition);
	
	_LIT(KConditionRemovable, " Removable=?");
	AppendFilterConditionToStatementL(aFilter.iSetFlag, CComponentFilter::ERemovable, statementStr, KConditionRemovable, isFirstCondition);
	
	_LIT(KConditionScomoState, " ScomoState=?");
	AppendFilterConditionToStatementL(aFilter.iSetFlag, CComponentFilter::EScomoState, statementStr, KConditionScomoState, isFirstCondition);
	
	_LIT(KConditionInstalledDrives, " InstalledDrives&?"); // & -> bitwise AND operator
	AppendFilterConditionToStatementL(aFilter.iSetFlag, CComponentFilter::EInstalledDrive, statementStr, KConditionInstalledDrives, isFirstCondition);
		
	_LIT(KConditionDrmProtected, " DRMProtected=?");
	AppendFilterConditionToStatementL(aFilter.iSetFlag, CComponentFilter::EDrmProtected, statementStr, KConditionDrmProtected, isFirstCondition);
	
	_LIT(KConditionHidden, " Hidden=?");
	AppendFilterConditionToStatementL(aFilter.iSetFlag, CComponentFilter::EHidden, statementStr, KConditionHidden, isFirstCondition);
	
	_LIT(KConditionKnownRevoked, " KnownRevoked=?");
	AppendFilterConditionToStatementL(aFilter.iSetFlag, CComponentFilter::EKnownRevoked, statementStr, KConditionKnownRevoked, isFirstCondition);

	_LIT(KConditionOriginVerified, " OriginVerified=?");
	AppendFilterConditionToStatementL(aFilter.iSetFlag, CComponentFilter::EOriginVerified, statementStr, KConditionOriginVerified, isFirstCondition);
		
	statementStr.Append(';');
	// SQLite requires the statement string to end with NULL char.
	statementStr.Append('\0');
	
	CStatement *stmt = iDbHandle->PrepareStatementLC(statementStr);
	TInt stmtIdx = 1;
	
	if(aFilter.iSetFlag & CComponentFilter::ESoftwareType)
		{
		TUint32 swTypeId = HashCaseSensitiveL(*aFilter.iSwType);
		stmt->BindIntL(stmtIdx++, swTypeId);
		}
	
	if(aFilter.iSetFlag & CComponentFilter::EInstalledDrive)
		{
		if (aFilter.iInstalledDrives.Length() != KMaxDrives)
			{
			DEBUG_PRINTF2(_L("Incorrect size of drive list supplied in filter, the string supplied was %S"), &aFilter.iInstalledDrives);
			User::Leave(KErrArgument);
			}
		TInt installedDrivesBitmask = InstalledDrivesToBitmaskL(aFilter.iInstalledDrives);
		stmt->BindIntL(stmtIdx++, installedDrivesBitmask);
		}
	
	BindIntegerFilterToStatementL(*stmt, aFilter.iSetFlag, CComponentFilter::ERemovable, aFilter.iRemovable, stmtIdx);
	BindIntegerFilterToStatementL(*stmt, aFilter.iSetFlag, CComponentFilter::EScomoState, aFilter.iScomoState, stmtIdx);
	BindIntegerFilterToStatementL(*stmt, aFilter.iSetFlag, CComponentFilter::EDrmProtected, aFilter.iDrmProtected, stmtIdx);
	BindIntegerFilterToStatementL(*stmt, aFilter.iSetFlag, CComponentFilter::EHidden, aFilter.iHidden, stmtIdx);
	BindIntegerFilterToStatementL(*stmt, aFilter.iSetFlag, CComponentFilter::EKnownRevoked, aFilter.iKnownRevoked, stmtIdx);
	BindIntegerFilterToStatementL(*stmt, aFilter.iSetFlag, CComponentFilter::EOriginVerified, aFilter.iOriginVerified, stmtIdx);
	
	CleanupStack::Pop(stmt);
	CleanupStack::PopAndDestroy(&statementStr);
	return stmt;
	}

void CScrRequestImpl::AddComponentEntryLocalizablesL(TComponentId aComponentId, CComponentEntry& aEntry, TLanguage aLocale, const CComponentFilter& aFilter) const
	{
	DEBUG_PRINTF(_L8("Adding  Component Entry Localizables."));
	if(aFilter.iSetFlag & (CComponentFilter::EName | CComponentFilter::EVendor))
		{ // If a name or vendor is specified in the filter, the locale is ignored
		  // and the provided names are retrieved from the ComponentLocalizables table.
		CStatement *stmt = CreateStatementObjectForComponentLocalizablesLC(*aFilter.iName, *aFilter.iVendor, aFilter.iSetFlag, aComponentId);
		TBool res = stmt->ProcessNextRowL();
		// If the name and the vendor are not found, leave with KErrNotFound.
		__ASSERT_ALWAYS(res, User::Leave(KErrNotFound)); 		
		DeleteObjectZ(aEntry.iName);
		aEntry.iName = stmt->StrColumnL(1).AllocL(); // Ownership is transferred to the entry object
		DeleteObjectZ(aEntry.iVendor);
		aEntry.iVendor = stmt->StrColumnL(2).AllocL(); // Ownership is transferred to the entry object
		CleanupStack::PopAndDestroy(stmt);
		}
	else
		{// if name and/or vendor are not defined in the filter, try to find them by using the nearest language algorithm. 
		AddComponentEntryLocalizablesL(aComponentId, aEntry, aLocale);
		}
	}


TBool CScrRequestImpl::IsSoftwareTypeExistingL(TInt aSoftwareTypeId) const
	{
	DEBUG_PRINTF(_L8("verify Software Type Exists."));
	TBool result = EFalse;
	_LIT(KComponentSoftwareType, "SELECT SoftwareTypeId FROM SoftwareTypes WHERE SoftwareTypeId=?;");	
		
	CStatement *stmt = iDbHandle->PrepareStatementLC(KComponentSoftwareType);
	stmt->BindIntL(1, aSoftwareTypeId);
			
	if(stmt->ProcessNextRowL())
		{ // The software type exists. 
		result = ETrue;
		}
	CleanupStack::PopAndDestroy(stmt);
	return result;
	}

void CScrRequestImpl::SubsessionAddLocalizableSoftwareTypeNameL(CStatement& aStmt, CComponentEntry& aEntry, TLanguage aLocale, CCompViewSubsessionContext* aSubsessionContext) const	
	{
	TInt softwareTypeId = aStmt.IntColumnL(1);
	// Check if we've already cached this software type name - caching is important here as fetching the same software type name for all components in the sub-session is expensive
	if (softwareTypeId == aSubsessionContext->iLastSoftwareTypeId)
		{
		DeleteObjectZ(aEntry.iSwType);
		aEntry.iSwType = aSubsessionContext->iLastSoftwareTypeName->AllocL();
		}
	else
		{
		AddSoftwareTypeNameToComponentEntryL(aStmt, aEntry, aLocale);
		// Save the last results - if the next component in the filter has the same software type id, we won't have to recalculate the name
		aSubsessionContext->iLastSoftwareTypeId = softwareTypeId;
		DeleteObjectZ(aSubsessionContext->iLastSoftwareTypeName);
		aSubsessionContext->iLastSoftwareTypeName = aEntry.iSwType->AllocL();
		}
	}

CComponentEntry* CScrRequestImpl::GetNextComponentEntryL(CStatement& aStmt, CComponentFilter& aFilter, TLanguage aLocale, CCompViewSubsessionContext* aSubsessionContext) const
	{
	// Since the query does not account for filter's name, vendor, property and file conditions,
	// we intersect the resut of the query with components which answer these conditions, i.e. iComponentFilterSuperset 
	while (1)
		{
		if(!aStmt.ProcessNextRowL())
			{
			return NULL;
			}
		TComponentId queryComponentId = aStmt.IntColumnL(0); 
		if (!aSubsessionContext->iFilterSupersetInUse || aSubsessionContext->iComponentFilterSuperset.FindInOrder(queryComponentId) >= 0) 
			break; // The component id exists in both parts of the filter - return it.
		}
		
	CComponentEntry *component = CreateComponentEntryFromStatementHandleL(aStmt);
	CleanupStack::PushL(component);
	AddComponentEntryLocalizablesL(component->ComponentId(), *component, aLocale, aFilter);
	SubsessionAddLocalizableSoftwareTypeNameL(aStmt, *component, aLocale, aSubsessionContext);	
	CleanupStack::Pop(component);
	return component;
	}

void CScrRequestImpl::NextComponentSizeL(const RMessage2& aMessage, CStatement* aStmt, CComponentFilter* aFilter, CComponentEntry*& aEntry, CCompViewSubsessionContext* aSubsessionContext) const
	{
	__ASSERT_ALWAYS(aStmt && aFilter, PanicClient(aMessage, KScrIllegalCallSequence));
	
	TLanguage locale = TLanguage(aMessage.Int0());
	
	DeleteObjectZ(aEntry);
	
	TInt err(0);
	do {
       TRAP(err, aEntry = GetNextComponentEntryL(*aStmt, *aFilter, locale, aSubsessionContext));
       }while(err == KErrNotFound);
	
	if(KErrNone != err)
	    {
        User::Leave(err);
	    }
	
	TInt sizeSlot = 1;
	if(!aEntry)
		{
		DEBUG_PRINTF(_L8("Reached the end of the view."));
		WriteIntValueL(aMessage, sizeSlot, 0);
		return;
		}
	WriteObjectSizeL(aMessage, sizeSlot, aEntry);
	}

void CScrRequestImpl::NextComponentDataL(const RMessage2& aMessage, CComponentEntry*& aEntry) const
	{
	DEBUG_PRINTF(_L8("Returning the component entry data."));
	WriteObjectDataL(aMessage, 0, aEntry);
	DeleteObjectZ(aEntry); // Delete the object to prevent it to be resent.
	}

void CScrRequestImpl::NextComponentSetSizeL(const RMessage2& aMessage, CStatement* aStmt, CComponentFilter* aFilter, RPointerArray<CComponentEntry>& aEntryList, CCompViewSubsessionContext* aSubsessionContext) const
	{
	DEBUG_PRINTF(_L8("Returning the size of the next component entry set."));
	__ASSERT_ALWAYS(aStmt && aFilter, PanicClient(aMessage, KScrIllegalCallSequence));	

	TInt maxArraySize = aMessage.Int0();
	TLanguage locale = TLanguage(aMessage.Int1());
	
	aEntryList.ResetAndDestroy();
	CComponentEntry *entry(0);
	TInt err(0);
	for(TInt i=0; i<maxArraySize; ++i)
		{
        do {
           TRAP(err, entry = GetNextComponentEntryL(*aStmt, *aFilter, locale, aSubsessionContext));
           }while(err == KErrNotFound);
        
        if(KErrNone != err)
            {
            User::Leave(err);
            }		
        
		if(!entry)
			{
			break;
			}
		CleanupStack::PushL(entry);
		aEntryList.AppendL(entry);
		CleanupStack::Pop(entry); // Ownership is transferred
		}
	WriteArraySizeL(aMessage, 2, aEntryList);
	}


void CScrRequestImpl::NextComponentSetDataL(const RMessage2& aMessage, RPointerArray<CComponentEntry>& aEntryList) const
	{
	DEBUG_PRINTF(_L8("Returning the next component entry set."));
	WriteArrayDataL(aMessage, 0, aEntryList);
	}

void CScrRequestImpl::GetComponentIdListSizeL(const RMessage2& aMessage) const
	{
	DEBUG_PRINTF(_L8("Returning the required size to copy all components IDs from the components view."));
	// Get filter from the client
	CComponentFilter *filter = ReadComponentFilterL(aMessage);
	CleanupStack::PushL(filter);
	
	RArray<TComponentId> componentFilterSuperset;
	CleanupClosePushL(componentFilterSuperset);
	TBool filterSupersetInUse;
	// Create a statement based on the filter
	CStatement *stmt = OpenComponentViewL(*filter, componentFilterSuperset, filterSupersetInUse);
	CleanupStack::PushL(stmt);
	iComponentIdList.Reset();
	// Fetch all components from the row set and put them into the component list
	while(stmt->ProcessNextRowL())
		{
		TComponentId componentId = stmt->IntColumnL(0);
		if (filterSupersetInUse && componentFilterSuperset.FindInOrder(componentId) < 0) 
			continue; // The component id does not match the name/vendor/file/property filter - do not add it to the result		
		iComponentIdList.InsertInOrder(componentId);
		}
	// Release allocated memories
	CleanupStack::PopAndDestroy(3, filter); // stmt, componentFilterSuperset, filter
	WriteArraySizeL(aMessage, 1, iComponentIdList);
	}

void CScrRequestImpl::GetComponentIdListDataL(const RMessage2& aMessage) const
	{
	DEBUG_PRINTF(_L8("Returning all components IDs from the components view."));
	WriteArrayDataL(aMessage, 0, iComponentIdList);
	}

CStatement* CScrRequestImpl::OpenFileListStatementL(TComponentId aComponentId) const
	{
	_LIT(KSelectFilesOfComponent, "SELECT Location FROM ComponentsFiles WHERE ComponentId=?;");
	
	CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectFilesOfComponent);
	stmt->BindIntL(1, aComponentId);
	CleanupStack::Pop(stmt); // Ownership is transferred to the caller
	
	return stmt;
	}	
	
CStatement* CScrRequestImpl::OpenFileListL(const RMessage2& aMessage) const
	{
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	DEBUG_PRINTF2(_L8("Creating the requested file list for component(%d)."), componentId);
	return OpenFileListStatementL(componentId);
	}

HBufC* CScrRequestImpl::GetNextFilePathL(CStatement& aStmt) const
	{
	if(!aStmt.ProcessNextRowL())
		{
		return NULL;
		}
	return aStmt.StrColumnL(0).AllocL();
	}

void CScrRequestImpl::NextFileSizeL(const RMessage2& aMessage, CStatement* aStmt, HBufC*& aFilePath) const
	{
	__ASSERT_ALWAYS(aStmt, PanicClient(aMessage, KScrIllegalCallSequence));
	DeleteObjectZ(aFilePath);
	aFilePath = GetNextFilePathL(*aStmt);
	WriteObjectSizeL(aMessage, 0, aFilePath);
	}

void CScrRequestImpl::NextFileDataL(const RMessage2& aMessage, HBufC*& aFilePath) const
	{
	WriteObjectDataL(aMessage, 0, aFilePath);
	DeleteObjectZ(aFilePath); // Delete the file path to prevent it to be resent.
	}

void CScrRequestImpl::NextFileSetSizeL(const RMessage2& aMessage, CStatement* aStmt, RPointerArray<HBufC>& aFileList) const
	{
	__ASSERT_ALWAYS(aStmt, PanicClient(aMessage, KScrIllegalCallSequence));
	TInt maxArraySize = aMessage.Int0();
	aFileList.ResetAndDestroy();
	for(TInt i=0; i<maxArraySize; ++i)
		{
		HBufC *filePath = GetNextFilePathL(*aStmt);
		if(!filePath)
			{
			break;
			}
		CleanupStack::PushL(filePath);
		aFileList.AppendL(filePath);
		CleanupStack::Pop(filePath); // Ownership is transferred
		}
	WriteArraySizeL(aMessage, 1, aFileList);
	}

void CScrRequestImpl::NextFileSetDataL(const RMessage2& aMessage, RPointerArray<HBufC>& aFileList) const
	{
	DEBUG_PRINTF(_L8("Returning the next file path set."));
	WriteArrayDataL(aMessage, 0, aFileList);
	}

TComponentId CScrRequestImpl::GetComponentIdFromMsgL(const RMessage2& aMessage)
	{	
	return aMessage.Int0();
	}

HBufC* CScrRequestImpl::GetSoftwareTypeNameFromMsgLC(const RMessage2& aMessage)
	{	
	switch (CScsServer::StripScsFunctionMask(aMessage.Function()))
		{
		case EAddComponent:
		case EAddComponentDependency:
		case EDeleteComponentDependency:
			{
			return ReadDescLC(aMessage, 0);
			}
		default:
			__ASSERT_ALWAYS(0, User::Invariant());
		}
	return NULL;
	}

HBufC* CScrRequestImpl::ReadFileNameFromMsgLC(const RMessage2& aMessage)
	{
	return ReadDescLC(aMessage, 1);
	}

void CScrRequestImpl::InitializeDbVersionL()
	{
	_LIT(KSelectStatement, "SELECT MajorVersion, MinorVersion, BuildNumber FROM ScrVersion;");
	CStatement* stmt = iDbHandle->PrepareStatementLC(KSelectStatement);
	if(!stmt->ProcessNextRowL())
		{
		DEBUG_PRINTF(_L("Could not find entries in the version table - internal problem"));
		User::Leave(KErrCorrupt);
		}
	
	iDbVersion.iMajor = stmt->IntColumnL(0);
	iDbVersion.iMinor = stmt->IntColumnL(1);
	iDbVersion.iBuild = stmt->IntColumnL(2);	
	CleanupStack::PopAndDestroy(stmt);
	DEBUG_PRINTF4(_L8("Opened SCR database with major version %d, minor version %d, and build number %d."), iDbVersion.iMajor,
			iDbVersion.iMinor, iDbVersion.iBuild);		
	}

void CScrRequestImpl::VerifyDbVersionCompatibilityL() const
	{
	if (iDbVersion.iMajor != KDbInterfaceMajorVersion)
		{
		DEBUG_PRINTF3(_L8("The SCR DB is incompatible! Supported major version is %d, and the DB major version is %d"), KDbInterfaceMajorVersion, iDbVersion.iMajor);
		User::Leave(KErrCorrupt);
		}
	
	if (iDbVersion.iMinor > KDbInterfaceMinorVersion)
		{
		DEBUG_PRINTF3(_L8("The SCR DB supports newer features which are not available in this software version. The DB minor version is %d, while the supported minor version is %d"), iDbVersion.iMinor, KDbInterfaceMinorVersion);
		return;
		}	
	
	if (iDbVersion.iMinor < KDbInterfaceMinorVersion)
		{
		DEBUG_PRINTF3(_L8("The SCR DB does not support some newer features available in this software version. The DB minor version is %d, while the supported minor version is %d"), iDbVersion.iMinor, KDbInterfaceMinorVersion); 
		return;
		}	
	
	DEBUG_PRINTF4(_L8("The SCR DB has matching version with the supported interface. Supported major version is %d, minor version %d and build number %d"), KDbInterfaceMajorVersion, KDbInterfaceMinorVersion, KDbInterfaceBuildNumber);	
	}

void ExtractNumberFromHexStringL(TInt& aPosition, const TDesC8& aString, TUint32& aIntValue)
	{
	TLex8 l(aString.Mid(aPosition, KUidStringLen));
	l.Val(aIntValue, EHex);
	aPosition += KUidStringLen;
	}

void ParseUidHexStringL(const TDesC8& aUidString, TUint32& aSifPluginUid, TUint32& aInstallerSid, TUint32& aExecutionLayerSid)
	{
	__ASSERT_ALWAYS(aUidString.Length() >= 3*KUidStringLen, User::Leave(KErrArgument));
	TInt pos (0); // The starting position of aUidString
	ExtractNumberFromHexStringL(pos, aUidString, aSifPluginUid);
	ExtractNumberFromHexStringL(pos, aUidString, aInstallerSid);
	ExtractNumberFromHexStringL(pos, aUidString, aExecutionLayerSid);
	}

TBool CompareLocalizedSoftwareTypeName(const CLocalizedSoftwareTypeName& aLeft, const CLocalizedSoftwareTypeName& aRight)
	{
	TBool result = EFalse;
	
	TRAPD(err, result = (aLeft.Locale() == aRight.Locale() && !aLeft.NameL().CompareF(aRight.NameL())));
	if (err != KErrNone)
		{
		DEBUG_PRINTF2(_L("CompareLocalizedSoftwareTypeName has left with error %d"), err);
		return EFalse;
		}
	else
		{
		return result;
		}
	}

TBool CompareHBufDescs(const HBufC& aLeft, const HBufC& aRight)
	{
	return !aLeft.CompareF(aRight);
	}

TBool CScrRequestImpl::IsSoftwareTypeExistingL(TUint32 aSwTypeId, TUint32 aSifPluginUid, RArray<TCustomAccessInfo>& aSidArray, const RPointerArray<HBufC>& aMimeTypesArray, const RPointerArray<CLocalizedSoftwareTypeName>& aLocalizedNamesArray, const TDesC& aLauncherExecutable)
	{
	//Check if software type id exists
    if(!IsSoftwareTypeExistingL(aSwTypeId))
        {
        DEBUG_PRINTF2(_L8("IsSoftwareTypeExistingL: Software Type Id (%d) doesn't exist in the SCR."), aSwTypeId);
        return EFalse;
        }
    
    //Check if sif plugin uid for the software type id is the same 
    TInt pluginUid(0);
    if(!GetSifPluginUidIInternalL(aSwTypeId, pluginUid))
        {
        DEBUG_PRINTF2(_L8("IsSoftwareTypeExistingL: SIF Plugin Uid doesn't exist in the SCR for TypeId %d."), aSwTypeId);
        return EFalse;
        }
    
    if(aSifPluginUid != pluginUid)
        {
        DEBUG_PRINTF2(_L8("IsSoftwareTypeExistingL: SIF Plugin Uid doesn't match with the one in the SCR."), pluginUid);
        return EFalse;
        }
    
    //Check if launcher executable for the software type id is the same
    HBufC *launcherExe;
    _LIT(KSelectLauncherExecutable, "SELECT LauncherExecutable FROM SoftwareTypes WHERE SoftwareTypeId=?;");
    CStatement* stmt = iDbHandle->PrepareStatementLC(KSelectLauncherExecutable);
    stmt->BindIntL(1, aSwTypeId);

    if(stmt->ProcessNextRowL())
        {
        launcherExe = stmt->StrColumnL(0).AllocLC();
        if(launcherExe->Compare(aLauncherExecutable) != 0)
            {
            DEBUG_PRINTF(_L8("IsSoftwareTypeExistingL: Launcher Executable doesn't match with the one in the SCR."));
            CleanupStack::PopAndDestroy(2, stmt);
            return EFalse;
            }
        CleanupStack::PopAndDestroy(2, stmt);
        }
    else
        {
        DEBUG_PRINTF2(_L8("IsSoftwareTypeExistingL: Launcher Executable doesn't exist in the SCR for TypeId %d."), aSwTypeId);
        CleanupStack::PopAndDestroy(stmt);
        return EFalse;
        }
            
    //Check if associated installer sid's for the software type id is the same
    
    RArray<TSecureId> installerSids;
    CleanupClosePushL(installerSids);
    if(GetSidsForSoftwareTypeIdL(aSwTypeId, installerSids))
        {
        for(TInt i=0; i<aSidArray.Count(); ++i)
            {
            if(KErrNotFound == installerSids.Find(aSidArray[i].SecureId()))
                {
                DEBUG_PRINTF(_L8("IsSoftwareTypeExistingL: One of the Sid doesn't match with the one in the SCR."));
                CleanupStack::PopAndDestroy(&installerSids);
                return EFalse;
                }
            }
        }
    CleanupStack::PopAndDestroy(&installerSids);
    
	//Check if localized software type name for the software type id is the same
	_LIT(KSelectSwTypeNames, "SELECT Locale,Name FROM SoftwareTypeNames WHERE SoftwareTypeId=? AND Locale!=?;");
	CStatement* stmtNames = iDbHandle->PrepareStatementLC(KSelectSwTypeNames);
	stmtNames->BindIntL(1, aSwTypeId);
	stmtNames->BindIntL(2, KNonLocalized);
	TInt numOfFoundNames (0);
	while(stmtNames->ProcessNextRowL())
		{
		++numOfFoundNames;
		const TDesC& swTypeNameValue = stmtNames->StrColumnL(1);
		CLocalizedSoftwareTypeName *swTypeName = CLocalizedSoftwareTypeName::NewLC(swTypeNameValue, TLanguage(stmtNames->IntColumnL(0)));
		TInt ret = aLocalizedNamesArray.Find(swTypeName, TIdentityRelation<CLocalizedSoftwareTypeName>(CompareLocalizedSoftwareTypeName));
		CleanupStack::PopAndDestroy(swTypeName);
		if(KErrNotFound == ret)
			{
			DEBUG_PRINTF2(_L("IsSoftwareTypeExistingL: %S doesn't exist in the provided sofwtare type names list."), &swTypeNameValue);
			CleanupStack::PopAndDestroy(stmtNames);
			return EFalse;
			}
		}
	CleanupStack::PopAndDestroy(stmtNames);
	if(numOfFoundNames != aLocalizedNamesArray.Count())
		{
		DEBUG_PRINTF3(_L("IsSoftwareTypeExistingL: the number of provided software type names (%d) is different than the number of existing software type names (%d) in the SCR."), numOfFoundNames, aLocalizedNamesArray.Count());
		return EFalse;
		}
	
	//Check if mime type for the software type id is the same
	_LIT(KSelectMimeTypes, "SELECT MimeType FROM MimeTypes WHERE SoftwareTypeId=?;");
	CStatement* stmtMimes = iDbHandle->PrepareStatementLC(KSelectMimeTypes);
	stmtMimes->BindIntL(1, aSwTypeId);
	TInt numOfFoundMimes (0);
	while(stmtMimes->ProcessNextRowL())
		{
		++numOfFoundMimes;
		HBufC *mimeType = stmtMimes->StrColumnL(0).AllocLC();
		TInt ret = aMimeTypesArray.Find(mimeType, TIdentityRelation<HBufC>(CompareHBufDescs));
		if(KErrNotFound == ret)
			{
			DEBUG_PRINTF2(_L("IsSoftwareTypeExistingL: %S doesn't exist in the provided MIME types list."), mimeType);
			CleanupStack::PopAndDestroy(2, stmtMimes); // stmtMimes, mimeType
			return EFalse;
			}
		CleanupStack::PopAndDestroy(mimeType);
		}
	CleanupStack::PopAndDestroy(stmtMimes);
	if(numOfFoundMimes != aMimeTypesArray.Count())
		{
		DEBUG_PRINTF3(_L("IsSoftwareTypeExistingL: the number of provided MIME types (%d) is different than the number of existing MIME types (%d) in the SCR."), numOfFoundMimes, aMimeTypesArray.Count());
		return EFalse;
		}
	// All software type details are identical
	DEBUG_PRINTF(_L8("IsSoftwareTypeExistingL: All software type details are identical. This is a software type update."));
	return ETrue;
	}

void CScrRequestImpl::AddSoftwareTypeL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Adding a new software type."));
	// NB SoftwareTypeId is the hash of unique software type name. Since this name is unique, it is expected
	// that there won't be any conflicts with software type ids. If an installer is uninstalled and then re-installed,
	// the same software type id will be assigned for it as its unique name is not changed. The advantage is that
	// the orphaned components will become non-orphaned again automatically when their installer is re-installed.
	// Otherwise, we would have to find a complex solution to associate the software type id of orphaned components
	// with the corresponding installer which had been re-installed.
	// Another advantage of this approach is that it is not mandatory to query SoftwareTypes table in order to get
	// the software type id of an installer. If the uniqe software type name of the installer is known, its hash
	// is simply calculated to obtain its software type id.
	
	CSoftwareTypeRegInfo *regInfo = ReadObjectFromMessageLC<CSoftwareTypeRegInfo>(aMessage, 0);
	
	HBufC* uniqueSwTypeName = HBufC::NewLC(regInfo->UniqueSoftwareTypeName().Length());
	uniqueSwTypeName->Des().Copy(regInfo->UniqueSoftwareTypeName());

	TUint32 swTypeId = HashCaseSensitiveL(regInfo->UniqueSoftwareTypeName());
	    
	TUint32 sifPluginUid (0);
	sifPluginUid = regInfo->SifPluginUid().iUid;
	
	RArray<TCustomAccessInfo> sidArray;
	sidArray = regInfo->CustomAccessList();
	
	RPointerArray<HBufC> mimeTypesArray = regInfo->MimeTypes();
	
	RPointerArray<CLocalizedSoftwareTypeName> localizedNamesArray = regInfo->LocalizedSoftwareTypeNames();
	
	HBufC* launcherExecutable = HBufC::NewLC(regInfo->LauncherExecutable().Length());
	launcherExecutable->Des().Copy(regInfo->LauncherExecutable());

	if (IsSoftwareTypeExistingL(swTypeId, sifPluginUid, sidArray, mimeTypesArray, localizedNamesArray, *launcherExecutable))
		{ // If the software type exists, do nothing and return;	
		CleanupStack::PopAndDestroy(3, regInfo); // uniqueSwTypeName, launcherExecutable, regInfo
		return; 
		}
	
	// First, insert the main record to SoftwareTypes table
	_LIT(KInsertSwType, "INSERT INTO SoftwareTypes(SoftwareTypeId,SifPluginUid,LauncherExecutable) VALUES(?,?,?);");
	TInt numberOfValuesSwType = 3;
	ExecuteStatementL(KInsertSwType(), numberOfValuesSwType, EValueInteger, swTypeId, EValueInteger, sifPluginUid, EValueString, launcherExecutable);
	
	_LIT(KInsertCustomAccess, "INSERT INTO CustomAccessList(SoftwareTypeId,SecureId,AccessMode) VALUES(?,?,?);");
	TInt numberOfValuesCustomAccess = 3;
	for(TInt i=0; i<sidArray.Count(); ++i)
		{
		TUint32 sid = sidArray[i].SecureId();
		TAccessMode accessMode = sidArray[i].AccessMode();
		ExecuteStatementL(KInsertCustomAccess(), numberOfValuesCustomAccess, EValueInteger, swTypeId, EValueInteger, sid, EValueInteger, accessMode);
		}

	// Then, insert MIME types of this software type into MimeTypes table
	_LIT(KInsertMimeType, "INSERT INTO MimeTypes(SoftwareTypeId,MimeType) VALUES(?,?);");
	TInt numberOfValuesMimeType = 2;
	TInt countMimeTypes = mimeTypesArray.Count();
	for(TInt i=0; i<countMimeTypes; ++i)
		{
		ExecuteStatementL(KInsertMimeType(), numberOfValuesMimeType, EValueInteger, swTypeId, EValueString, mimeTypesArray[i]);
		}
		
	// Finally, insert unique and localized names into SoftwareTypeNames table
	_LIT(KInsertSwTypeName, "INSERT INTO SoftwareTypeNames(SoftwareTypeId,Locale,Name) VALUES(?,?,?);");
	TInt numberOfValuesSwTypeName = 3;
	ExecuteStatementL(KInsertSwTypeName(), numberOfValuesSwTypeName, EValueInteger, swTypeId, EValueInteger, KNonLocalized, EValueString, uniqueSwTypeName);
		
	TInt countNames = localizedNamesArray.Count();
	for(TInt i=0; i<countNames; ++i)
		{
		TLanguage locale = localizedNamesArray[i]->Locale();
		const TDesC& name = localizedNamesArray[i]->NameL();
		ExecuteStatementL(KInsertSwTypeName(), numberOfValuesSwTypeName, EValueInteger, swTypeId, EValueInteger, locale, EValueString, &name);
		}	
	CleanupStack::PopAndDestroy(3, regInfo); // uniqueSwTypeName, launcherExecutable, regInfo
	}

void CScrRequestImpl::DeleteSoftwareTypeL(const RMessage2& aMessage)
	{
	HBufC *uniqueSwTypeName = ReadDescLC(aMessage, 0);
	DEBUG_PRINTF2(_L("Deleting Software type (%S)."), uniqueSwTypeName);
	
	TUint32 swTypeId = HashCaseSensitiveL(*uniqueSwTypeName);
	CleanupStack::PopAndDestroy(uniqueSwTypeName);
	
	// First, delete software type names
	_LIT(KDeleteSoftwareTypeNames, "DELETE FROM SoftwareTypeNames WHERE SoftwareTypeId=?;");
	TInt numberOfValues = 1;
	ExecuteStatementL(KDeleteSoftwareTypeNames(), numberOfValues, EValueInteger, swTypeId);
	
	// Secondly, delete the actual software type record
	_LIT(KDeleteSoftwareType, "DELETE FROM SoftwareTypes WHERE SoftwareTypeId=?;");
	ExecuteStatementL(KDeleteSoftwareType(), numberOfValues, EValueInteger, swTypeId);
	
	_LIT(KDeleteCustomAccess, "DELETE FROM CustomAccessList WHERE SoftwareTypeId=?;");
	ExecuteStatementL(KDeleteCustomAccess(), numberOfValues, EValueInteger, swTypeId);
	
	// Thirdly, get the list of MIME types belong to the deleted software type. 
	// This list will be returned to the client with another request (GetDeletedMimeTypesL)
	_LIT(KSelectMimeTypes, "SELECT MimeType FROM MimeTypes WHERE SoftwareTypeId=?;");
	CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectMimeTypes);
	stmt->BindIntL(1, swTypeId);
	
	iDeletedMimeTypes.ResetAndDestroy();
	while(stmt->ProcessNextRowL())
		{
		iDeletedMimeTypes.AppendL(stmt->StrColumnL(0).AllocL());	
		}
	CleanupStack::PopAndDestroy(stmt);
	
	// Finally, delete the MIME types belong to the deleted software type name
	_LIT(KDeleteMimeTypes, "DELETE FROM MimeTypes WHERE SoftwareTypeId=?;");
	ExecuteStatementL(KDeleteMimeTypes(), numberOfValues, EValueInteger, swTypeId);
	WriteArraySizeL(aMessage, 1, iDeletedMimeTypes);
	}

void CScrRequestImpl::GetDeletedMimeTypesL(const RMessage2& aMessage) const
	{
	WriteArrayDataL(aMessage, 0, iDeletedMimeTypes);
	iDeletedMimeTypes.ResetAndDestroy();
	}

TInt CScrRequestImpl::GetSoftwareTypeForComponentL(TComponentId aComponentId) const
    {
    _LIT(KSelectComponentSoftwareTypeId, "SELECT SoftwareTypeId FROM Components WHERE ComponentId=?;");   
    
    TInt swTypeId = 0;
    CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectComponentSoftwareTypeId);
    stmt->BindIntL(1, aComponentId);
        
    if(!stmt->ProcessNextRowL())
        { 
        DEBUG_PRINTF2(_L("Component Id (%d) couldn't be found!"), aComponentId);
        User::Leave(KErrNotFound);
        }
    
    swTypeId = stmt->IntColumnL(0);
    CleanupStack::PopAndDestroy(stmt);
    
    return swTypeId;
    }

TBool CScrRequestImpl::GetIsComponentOrphanedL(TComponentId aComponentId) const	
	{
	// Get the software type for component
	TUint32 swTypeId = GetSoftwareTypeForComponentL(aComponentId);
		
	// The component is orphaned iff the software type does not exist.
	return !IsSoftwareTypeExistingL(swTypeId);
	}
	
void CScrRequestImpl::GetIsComponentOrphanedL(const RMessage2& aMessage) const
	{
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	TBool result = GetIsComponentOrphanedL(componentId);
	DEBUG_PRINTF2(_L8("Retrieving whether the component(%d) is orphaned."), componentId);

	TPckg<TBool> resultPkg(result);
	
	aMessage.WriteL(1, resultPkg);	
	}

void CheckAndCreateLogFileL(RFs& aFs, const TDesC& aFileName)
	{
	TEntry entry;
	TInt err = aFs.Entry(aFileName, entry);
	
	if(KErrNone == err)
		return;

	if (KErrPathNotFound == err)
		{
		User::LeaveIfError(aFs.MkDirAll(aFileName));
		}		
	else if(KErrNotFound != err)
		{
		DEBUG_PRINTF2(_L("The log file couldn't be opened. Error=%d."), err);
		User::Leave(err);	
		}
	
	RFileWriteStream stream;
	User::LeaveIfError(stream.Create(aFs, aFileName, EFileWrite | EFileShareAny | EFileStream));
	stream.PushL();
	stream.WriteInt32L(1); // The major version of the log file
	stream.WriteInt32L(0); // The minor version of the log file
	// Please note that the log file version is not used currently. It is implemented for a possible future need.
	// If the version format is changed, then the end of FlushLogEntriesArrayL function where the final log count is written must be updated.
	// FlushLogEntriesArrayL assumes that first 8 bytes store version information.
	stream.WriteInt32L(0); // The number of log records in the log file
	stream.CommitL();
	CleanupStack::PopAndDestroy(&stream);			
	}

void CScrRequestImpl::FlushLogEntriesArrayL()
	{
	TInt logCount2bFlushed = iLogEntries.Count();
	if(!logCount2bFlushed)
		return; // No record to write into the log file.
	
	// Get the log file name
	HBufC *logFileName = UpdateFilePathDriveLC(KScrLogFileName, iFs.GetSystemDriveChar());
	// Check whether the log file exists. If not, create one.
	CheckAndCreateLogFileL(iFs, *logFileName);
	
	RFileReadStream readStream;
	User::LeaveIfError(readStream.Open(iFs, *logFileName, EFileRead|EFileShareExclusive|EFileStream));
	readStream.PushL();
	(void)readStream.ReadInt32L(); // skip the major version of the log file
	(void)readStream.ReadInt32L(); // skip the minor version of the log file
	TInt currentLogCount = readStream.ReadInt32L();
		
	if(KMaxScrLogEntries <= currentLogCount + logCount2bFlushed)
		{
		//Create a temporary file and read the log file in that
		//oldest entries will be removed in the temporary log file
		HBufC *tempLogName = UpdateFilePathDriveLC(KScrTempLogFileName, iFs.GetSystemDriveChar());
		TInt err = iFs.Delete(*tempLogName);
		__ASSERT_ALWAYS(err == KErrNotFound || err == KErrNone, User::Leave(err));
		CheckAndCreateLogFileL(iFs, *tempLogName);
		
		RFileWriteStream writeStream;
		User::LeaveIfError(writeStream.Open(iFs, *tempLogName, EFileWrite | EFileStream));
		writeStream.PushL();
		writeStream.Sink()->SeekL(MStreamBuf::EWrite,EStreamEnd); // Skip the version info and log count
			
		TInt tmpLogCount (0);
		CScrLogEntry *log = NULL;
			
		for (TInt i = logCount2bFlushed; i < currentLogCount; ++i, ++tmpLogCount)
			{
			log = CScrLogEntry::NewLC(readStream);		
			writeStream << *log;
			CleanupStack::PopAndDestroy(log); 					
			} // for
			
		currentLogCount = tmpLogCount; 
		writeStream.CommitL();
		CleanupStack::PopAndDestroy(&writeStream); 
		readStream.Release();
			
		RStsSession stsSession; // transaction service
		CleanupClosePushL(stsSession);
		stsSession.CreateTransactionL();				
			
		stsSession.RemoveL(*logFileName);
		stsSession.RegisterNewL(*logFileName);
		stsSession.RegisterTemporaryL(*tempLogName);
			
		TInt renameErr = iFs.Rename(*tempLogName,*logFileName);							
		if (KErrNone == renameErr)
			{
			// commit file changes.
			stsSession.CommitL();
			}
		else
			{
			// rollback file changes and leave with the error code.
			stsSession.RollBackL();
			DEBUG_PRINTF2(_L("The log file couldn't be renamed. Error=%d."), renameErr);
			User::Leave(renameErr);
			}
		CleanupStack::PopAndDestroy(2, tempLogName); // tempLogName, stsSession
		} // endif KMaxScrLogEntries <=
	
	CleanupStack::PopAndDestroy(&readStream);
	
	RFile file;
	User::LeaveIfError(file.Open(iFs, *logFileName, EFileRead|EFileWrite| EFileShareExclusive| EFileStream));
	CleanupClosePushL(file);
	TInt pos (0);
	User::LeaveIfError(file.Seek(ESeekEnd, pos));	
	RFileWriteStream stream(file, pos);
	stream.PushL();
	
	TInt startIdx = (logCount2bFlushed - KMaxScrLogEntries)>0 ? (logCount2bFlushed - KMaxScrLogEntries) : 0;
	TInt logCountFlushed (0);
	for(TInt i=startIdx; i<logCount2bFlushed; ++i, ++logCountFlushed)
		{
		iLogEntries[i]->ExternalizeL(stream);
		}
	iLogEntries.ResetAndDestroy();
	stream.CommitL();
	stream.Release();
	file.Close();
		
	// Update the log count
	User::LeaveIfError(stream.Open(iFs, *logFileName, EFileRead|EFileWrite|EFileShareExclusive|EFileStream));
	stream.Sink()->SeekL(MStreamBuf::EWrite,EStreamBeginning,8); // Skip the version info
	stream.WriteInt32L(logCountFlushed + currentLogCount);
	stream.CommitL();
	
	CleanupStack::PopAndDestroy(3, logFileName); // logFileName, file, stream
	}

void CScrRequestImpl::GetLogFileHandleL(const RMessage2& aMessage) const
	{
	// Get the log file name
	HBufC *logFileName = UpdateFilePathDriveLC(KScrLogFileName, iFs.GetSystemDriveChar());
	// Check whether the log file exists. If not, create one.
	CheckAndCreateLogFileL(iFs, *logFileName);
	
	RFile file;
	User::LeaveIfError(file.Open(iFs, *logFileName, EFileRead | EFileShareAny | EFileStream));
	CleanupClosePushL(file);
		
	// Store the RFile handle into the package buffer in slot 0 and complete the message with the RFs handle
	User::LeaveIfError(file.TransferToClient(aMessage, 0));
	ASSERT(aMessage.IsNull());  // The message should have been completed
	CleanupStack::PopAndDestroy(2, logFileName); // logFileName, file
	}

void CScrRequestImpl::GetIsComponentOnReadOnlyDriveL(const RMessage2& aMessage) const
	{
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	DEBUG_PRINTF2(_L8("Checking if the component(%d) is on read-only drive."), componentId);
	
	// Get the drives on which the component files' are registered
	TInt driveBitmask = GetInstalledDrivesBitmaskL(componentId);	
			
	TBool result = EFalse;
	
	for (TInt index = 0; index < KMaxDrives; ++index, driveBitmask >>= 1)
		{
		if ((driveBitmask & 0x1) && IsDriveReadOnlyL(index))
			{
			DEBUG_PRINTF3(_L8("Component %d has registered a file at read-only drive %c ."), componentId, index + 'a');
			result = ETrue;
			break;
			}
		}
	
	TPckg<TBool> resPkg(result);	
	aMessage.WriteL(1, resPkg);	
	}

TBool CScrRequestImpl::IsDriveReadOnlyL(TInt driveIndex) const
	{
	TDriveInfo driveInfo;
	User::LeaveIfError(iFs.Drive(driveInfo,driveIndex));
		
	TBool result = EFalse;
	if ((driveInfo.iDriveAtt&KDriveAttRom) || (driveInfo.iType==EMediaRom))
		result = ETrue;
	
	return result;
	}

void CScrRequestImpl::GetIsComponentPresentL(const RMessage2& aMessage) const
	{
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	TBool result = IsComponentPresentL(componentId);
				
	TPckg<TBool> isCompPresent(result);	
	aMessage.WriteL(1, isCompPresent);	
	}


TBool CScrRequestImpl::IsComponentPresentL(TComponentId componentId) const
    {
    DEBUG_PRINTF2(_L8("Checking if the component(%d) is available."), componentId);
    
    _LIT(KSelectCompPresent, "SELECT CompPresence FROM Components WHERE ComponentId=?;");
    CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectCompPresent);
    stmt->BindIntL(1, componentId);
    if(!stmt->ProcessNextRowL())
        {
        DEBUG_PRINTF2(_L8("Component (%d) couldn't be found in the SCR database."), componentId);
        User::Leave(KErrNotFound);
        }
    TBool result = (stmt->IntColumnL(0) == 1);
    CleanupStack::PopAndDestroy(stmt);
    
    // The default value for CompPresence is ETrue. So when we find that the SCR DB contains the 
    // default property value we check if the drives registered by the component are present.   
    if (result && !CheckForMediaPresenceL(componentId))
        {
        result = EFalse;
        }
    return result;
    }

void CScrRequestImpl::SetIsComponentPresentL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Setting the component as present."));
	_LIT(KColumnNameCompPresence, "CompPresence");
	ReadAndSetCommonComponentPropertyL(aMessage, KColumnNameCompPresence);	
	}

void CScrRequestImpl::GetComponentSupportedLocalesListSizeL(const RMessage2& aMessage) const
	{
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	DEBUG_PRINTF2(_L8("Returning the required size to copy all matching supported language IDs for component =(%d)"), componentId);

	_LIT(KSelectMatchingSupportedLocales, "SELECT Locale FROM ComponentLocalizables WHERE ComponentId=?;");
	CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectMatchingSupportedLocales);
	stmt->BindIntL(1, componentId);
	iMatchingSupportedLanguageList.Close();
	while(stmt->ProcessNextRowL())
		{
		TLanguage languageId = (TLanguage)stmt->IntColumnL(0);
		iMatchingSupportedLanguageList.Insert(languageId,iMatchingSupportedLanguageList.Count());
		}
	if (iMatchingSupportedLanguageList.Count() == 1)
	    {
	    iMatchingSupportedLanguageList.Close();
	    }
	
	// Release allocated memories
	CleanupStack::PopAndDestroy(1, stmt); // stmt
	WriteArraySizeL(aMessage, 1, iMatchingSupportedLanguageList);
	
	}

void CScrRequestImpl::GetComponentSupportedLocalesListDataL(const RMessage2& aMessage) const
	{
	DEBUG_PRINTF(_L8("Getting the Data of matching supported languages"));
	WriteArrayDataL(aMessage, 0, iMatchingSupportedLanguageList);
	iMatchingSupportedLanguageList.Close();
	}

TBool CScrRequestImpl::DoesAppWithScreenModeExistL(TUid aAppUid, TInt aScreenMode, TLanguage aLocale) const
    {
    _LIT(KSelectAppUidFromLocalizableAppInfo,"SELECT COUNT(*)FROM (LocalizableAppInfo JOIN ViewData ON LocalizableAppInfo.LocalAppInfoId = ViewData.LocalAppInfoId) WHERE AppUid = ? AND ScreenMode = ? AND Locale = ?;");
    CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectAppUidFromLocalizableAppInfo);
    stmt->BindIntL(1, aAppUid.iUid);            
    stmt->BindIntL(2, aScreenMode);
    stmt->BindIntL(3, aLocale);
    stmt->ProcessNextRowL();
    TInt count = stmt->IntColumnL(0);
    CleanupStack::PopAndDestroy(stmt);
    if(count!=0)
        return ETrue;
    else 
        return EFalse;
    }

void CScrRequestImpl::GetAppUidsL(CAppInfoViewSubsessionContext* aSubsessionContext, TBool aScreenModePresent) const
    {
	DEBUG_PRINTF(_L8("Returning the App Uid."));
    CStatement* stmt;
    _LIT(KAllAppIds,"SELECT AppUid from AppRegistrationInfo"); 
    stmt = iDbHandle->PrepareStatementLC(KAllAppIds);
    aSubsessionContext->iApps.Close();
    while(stmt->ProcessNextRowL())
        {
        TAppUidWithLocaleInfo appUidWithLocaleInfo;
        appUidWithLocaleInfo.iAppUid = TUid::Uid(stmt->IntColumnL(0));
        appUidWithLocaleInfo.iLocale = KUnspecifiedLocale;
        GetNearestAppLanguageL(aSubsessionContext->iLocale, appUidWithLocaleInfo.iAppUid, appUidWithLocaleInfo.iLocale);
        if(aScreenModePresent)
        	{
            if(DoesAppWithScreenModeExistL(appUidWithLocaleInfo.iAppUid, aSubsessionContext->iScreenMode, appUidWithLocaleInfo.iLocale))
                {
                aSubsessionContext->iApps.AppendL(appUidWithLocaleInfo);
                }
        	}
        
        else
        	{
            aSubsessionContext->iApps.AppendL(appUidWithLocaleInfo);
        	}
        }
    CleanupStack::PopAndDestroy(stmt);  
    }
        
void CScrRequestImpl::GetEmbeddableAppUidsL(CAppInfoViewSubsessionContext* aSubsessionContext, TBool aScreenModePresent) const
    {
	DEBUG_PRINTF(_L8("Returning the Embeddable App Uids."));
    CStatement *stmt1;
    _LIT (KGetAppIdWithEmbeddability, "SELECT DISTINCT AppUid from AppRegistrationInfo where Embeddable IN(1,2)"); 
    stmt1 = iDbHandle->PrepareStatementLC(KGetAppIdWithEmbeddability);
    aSubsessionContext->iApps.Close();
    while(stmt1->ProcessNextRowL())
    	{
        TAppUidWithLocaleInfo appUidWithLocaleInfo;
        appUidWithLocaleInfo.iAppUid = TUid::Uid(stmt1->IntColumnL(0));
        appUidWithLocaleInfo.iLocale = KUnspecifiedLocale;
        GetNearestAppLanguageL(aSubsessionContext->iLocale, appUidWithLocaleInfo.iAppUid, appUidWithLocaleInfo.iLocale);
        if(aScreenModePresent)
        	{
            if(DoesAppWithScreenModeExistL(appUidWithLocaleInfo.iAppUid, aSubsessionContext->iScreenMode, appUidWithLocaleInfo.iLocale))
                {
                aSubsessionContext->iApps.AppendL(appUidWithLocaleInfo);
                }
        	}
        else
        	{
            aSubsessionContext->iApps.AppendL(appUidWithLocaleInfo);
        	}
    	}
    CleanupStack::PopAndDestroy(stmt1); 
    }


void CScrRequestImpl::GetServerAppUidsL(CAppInfoViewSubsessionContext* aSubsessionContext, TUid aServiceUid, TBool aScreenModePresent) const
    {
    CStatement *stmt1;
    _LIT(KSelectAppUidForServiceId,"SELECT DISTINCT AppUid from ServiceInfo where Uid = ?"); 
    stmt1 = iDbHandle->PrepareStatementLC(KSelectAppUidForServiceId);
    stmt1->BindIntL(1, aServiceUid.iUid);
    aSubsessionContext->iApps.Close();
    while(stmt1->ProcessNextRowL())
    	{
        TAppUidWithLocaleInfo appUidWithLocaleInfo;
        appUidWithLocaleInfo.iAppUid = TUid::Uid(stmt1->IntColumnL(0));
        appUidWithLocaleInfo.iLocale = KUnspecifiedLocale;
        GetNearestAppLanguageL(aSubsessionContext->iLocale, appUidWithLocaleInfo.iAppUid, appUidWithLocaleInfo.iLocale);
        if(aScreenModePresent)
        	{
            if(DoesAppWithScreenModeExistL(appUidWithLocaleInfo.iAppUid, aSubsessionContext->iScreenMode, appUidWithLocaleInfo.iLocale))
                {
                aSubsessionContext->iApps.AppendL(appUidWithLocaleInfo);
                }
        	}
        else
        	{
            aSubsessionContext->iApps.AppendL(appUidWithLocaleInfo);
        	}
    	}
    CleanupStack::PopAndDestroy(stmt1); 
    }

void CScrRequestImpl::GetAppUidsWithEmbeddabilityFilterL(CAppInfoViewSubsessionContext* aSubsessionContext, TEmbeddableFilter& aFilter, TBool aScreenModePresent) const
    {
    CStatement *stmt1;
    TApplicationCharacteristics::TAppEmbeddability embeddability;
    _LIT(KSelectAppUidForServiceId,"SELECT DISTINCT AppUid,Embeddable from AppRegistrationInfo"); 
    stmt1 = iDbHandle->PrepareStatementLC(KSelectAppUidForServiceId);
    aSubsessionContext->iApps.Close();  
    while(stmt1->ProcessNextRowL())
            {   
            embeddability  = (TApplicationCharacteristics::TAppEmbeddability)stmt1->IntColumnL(1);
            if(aFilter.MatchesEmbeddability(embeddability))
            	{
                TAppUidWithLocaleInfo appUidWithLocaleInfo;
                appUidWithLocaleInfo.iAppUid = TUid::Uid(stmt1->IntColumnL(0));
                appUidWithLocaleInfo.iLocale = KUnspecifiedLocale;
                GetNearestAppLanguageL(aSubsessionContext->iLocale, appUidWithLocaleInfo.iAppUid, appUidWithLocaleInfo.iLocale);
                if(aScreenModePresent)
                	{
                    if(DoesAppWithScreenModeExistL(appUidWithLocaleInfo.iAppUid, aSubsessionContext->iScreenMode, appUidWithLocaleInfo.iLocale))
                        {
                        aSubsessionContext->iApps.AppendL(appUidWithLocaleInfo);
                        }
                	}
                else
                	{
                    aSubsessionContext->iApps.AppendL(appUidWithLocaleInfo);
                	}
            	}
            else
            	continue;
            }
    CleanupStack::PopAndDestroy(stmt1);
    }

void CScrRequestImpl::GetAppUidsWithCapabilityMaskAndValueL(CAppInfoViewSubsessionContext* aSubsessionContext,TUint aCapabilityAttrFilterMask, TUint aCapabilityAttrFilterValue, TBool aScreenModePresent) const
    {
    CStatement *stmt1;
    TUint attributes;
    _LIT(KSelectAppUidForServiceId,"SELECT DISTINCT AppUid,Attributes from AppRegistrationInfo"); 
    stmt1 = iDbHandle->PrepareStatementLC(KSelectAppUidForServiceId);            
    aSubsessionContext->iApps.Close(); 
    while(stmt1->ProcessNextRowL())
    	{
    	attributes = stmt1->IntColumnL(1);
        if(((attributes & aCapabilityAttrFilterMask) == (aCapabilityAttrFilterValue & aCapabilityAttrFilterMask)))
        	{
            TAppUidWithLocaleInfo appUidWithLocaleInfo;
            appUidWithLocaleInfo.iAppUid = TUid::Uid(stmt1->IntColumnL(0));
            appUidWithLocaleInfo.iLocale = KUnspecifiedLocale;
            GetNearestAppLanguageL(aSubsessionContext->iLocale, appUidWithLocaleInfo.iAppUid, appUidWithLocaleInfo.iLocale);
            if(aScreenModePresent)
            	{
                if(DoesAppWithScreenModeExistL(appUidWithLocaleInfo.iAppUid, aSubsessionContext->iScreenMode, appUidWithLocaleInfo.iLocale))
                    {
                    aSubsessionContext->iApps.AppendL(appUidWithLocaleInfo);
                    }
            	}
            else
            	{
                aSubsessionContext->iApps.AppendL(appUidWithLocaleInfo);
            	}
        	}
        else
        	continue;
    	}
    CleanupStack::PopAndDestroy(stmt1);
    }

void CScrRequestImpl::GetLocalesForAppIdL(RArray<TInt>& aLocales, TUid aAppUid) const
    {     
    _LIT(KAllLocales," SELECT Locale from LocalizableAppInfo where AppUid = ? ORDER BY Locale");               
    CStatement* stmt = iDbHandle->PrepareStatementLC(KAllLocales); 
    stmt->BindIntL(1, aAppUid.iUid);
    aLocales.Close();
    while(stmt->ProcessNextRowL())
         {
         aLocales.Append(stmt->IntColumnL(0));                  
         }
    CleanupStack::PopAndDestroy(1,stmt);                    
    }

CStatement* CScrRequestImpl::CreateStatementForAppInfoL(const TDesC& aStatement, TLanguage aLocale, TInt aValuesNum,...) const 
    {
    VA_LIST argList;
    VA_START(argList, aValuesNum);  
    
    CStatement *stmt = iDbHandle->PrepareStatementLC(aStatement);   
    BindStatementValuesL(*stmt, aLocale,aValuesNum, argList); 
    CleanupStack::Pop(stmt);
    return stmt;    
    }

CAppInfoFilter* CScrRequestImpl::ReadAppInfoFilterL(const RMessage2& aMessage) const
    {
    CAppInfoFilter *filter = ReadObjectFromMessageLC<CAppInfoFilter>(aMessage, 0);
    CleanupStack::Pop(filter);
    return filter;
    }

void CScrRequestImpl::OpenAppInfoViewL(CAppInfoFilter& aFilter, CAppInfoViewSubsessionContext* aSubsessionContext)
    {
	DEBUG_PRINTF(_L8("Opening App Info View."));			
    switch(aFilter.iSetFlag)
        {
        
        case CAppInfoFilter::EAllApps:
            {
            GetAppUidsL(aSubsessionContext);                    
            break;
            }
        case CAppInfoFilter::EAllAppsWithScreenMode:
            {            
            aSubsessionContext->iScreenMode = aFilter.iScreenMode;
            GetAppUidsL(aSubsessionContext, ETrue);
            break;
            }
            
        case CAppInfoFilter::EGetEmbeddableApps:
            {            
            GetEmbeddableAppUidsL(aSubsessionContext);
            break;
            }
            
        case CAppInfoFilter::EGetEmbeddableAppsWithSreenMode:
            {
            aSubsessionContext->iScreenMode = aFilter.iScreenMode;
            GetEmbeddableAppUidsL(aSubsessionContext, ETrue);
            break;
            }
        
        case CAppInfoFilter::EGetFilteredAppsWithEmbeddabilityFilter:
            {
            GetAppUidsWithEmbeddabilityFilterL(aSubsessionContext, aFilter.iEmbeddabilityFilter);
            break;
            }
            
        case CAppInfoFilter::EGetFilteredAppsWithEmbeddabilityFilterWithScreenMode:
            {
            aSubsessionContext->iScreenMode = aFilter.iScreenMode;
            GetAppUidsWithEmbeddabilityFilterL(aSubsessionContext, aFilter.iEmbeddabilityFilter, ETrue);
            break;
            }
            
        case CAppInfoFilter::EGetFilteredAppsWithCapabilityMaskAndValue:
            {
            GetAppUidsWithCapabilityMaskAndValueL(aSubsessionContext,aFilter.iCapabilityAttributeMask,aFilter.iCapabilityAttributeValue);
            break;
            }
            
        case CAppInfoFilter::EGetFilteredAppsWithCapabilityMaskAndValueWithScreenMode:
            {
            aSubsessionContext->iScreenMode = aFilter.iScreenMode;
            GetAppUidsWithCapabilityMaskAndValueL(aSubsessionContext,aFilter.iCapabilityAttributeMask,aFilter.iCapabilityAttributeValue, ETrue);
            break;
            }
        case CAppInfoFilter::EGetServerApps:
            {
            GetServerAppUidsL(aSubsessionContext, aFilter.iServiceUid);
            break;
            }
            
        case CAppInfoFilter::EGetServerAppsWithScreenMode:
            {
            aSubsessionContext->iScreenMode = aFilter.iScreenMode;
            GetServerAppUidsL(aSubsessionContext, aFilter.iServiceUid, ETrue);
            break;
            }
            
        default:
            User::Leave(KErrArgument);
        }
    
    }

void CScrRequestImpl::NextAppInfoSizeL(const RMessage2& aMessage, TAppRegInfo*& aAppInfo, CAppInfoViewSubsessionContext* aSubsessionContext)
    {
    while(1)
        {
        if(aSubsessionContext->iAppInfoIndex < aSubsessionContext->iApps.Count())
            {
            TBool isPresent = EFalse;
            aAppInfo = new(ELeave) TAppRegInfo;               
            aAppInfo->iUid = (aSubsessionContext->iApps[aSubsessionContext->iAppInfoIndex]).iAppUid;
    
            _LIT(KSelectAppFilename, "SELECT AppFile FROM AppRegistrationInfo WHERE AppUid=?;");
            CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectAppFilename);
            stmt->BindIntL(1, aAppInfo->iUid.iUid);
            if(stmt->ProcessNextRowL())
                {
                isPresent = ETrue;
                aAppInfo->iFullName = stmt->StrColumnL(0);
                }                   
            CleanupStack::PopAndDestroy(stmt);
            if(isPresent)
                {
                if((aSubsessionContext->iApps[aSubsessionContext->iAppInfoIndex]).iLocale != KUnspecifiedLocale)
                    {
                    GetCaptionAndShortCaptionInfoForLocaleL(aAppInfo->iUid, aSubsessionContext->iApps[aSubsessionContext->iAppInfoIndex].iLocale, aAppInfo->iShortCaption, aAppInfo->iCaption);
                    }
                aSubsessionContext->iAppInfoIndex++;
                WriteObjectSizeL(aMessage, 1, aAppInfo);
                break;
                }
            else
                {
                DeleteObjectZ(aAppInfo);
                aSubsessionContext->iAppInfoIndex++;
                }        
            }
        else
            {
            break;
            }
        }        
    }

void CScrRequestImpl::NextAppInfoDataL(const RMessage2& aMessage, TAppRegInfo*& aAppInfo)
    {
    DEBUG_PRINTF(_L8("Returning the AppInfo data."));
    WriteObjectDataL(aMessage, 0, aAppInfo);
    DeleteObjectZ(aAppInfo); // Delete the object to prevent it to be resent.
    aAppInfo = NULL;
    }

void CScrRequestImpl::AddApplicationEntryL(const RMessage2& aMessage)
    {
    DEBUG_PRINTF(_L8("Adding the application details into SCR"));
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
    CApplicationRegistrationData *regInfo = ReadObjectFromMessageLC<CApplicationRegistrationData>(aMessage, 1);
        
    TSecureId clientSid = aMessage.SecureId();
    TUint32 swTypeId = 0;
    if (componentId == 0 && clientSid == KSisRegistryServerSid)
        {
        swTypeId = HashCaseSensitiveL(Usif::KSoftwareTypeNative);
        }
    else
        {
        // Applicaiton is always associated with a component. We use the SoftwareTypeId of the 
        // component as the ApplicationTypeId.
        swTypeId = GetSoftwareTypeForComponentL(componentId);
        }
    
        
    _LIT(KInsertAppRegistrationInfo,"INSERT INTO AppRegistrationInfo(AppUid, ComponentId, AppFile, TypeId, Attributes, Hidden, Embeddable, NewFile, Launch, GroupName, DefaultScreenNumber) VALUES(?,?,?,?,?,?,?,?,?,?,?);");
    TInt numberOfValues = 11;    
    ExecuteStatementL(KInsertAppRegistrationInfo(), numberOfValues, EValueInteger, regInfo->AppUid(), EValueInteger, componentId, EValueString, &(regInfo->AppFile()), EValueInteger, swTypeId, EValueInteger, regInfo->Attributes(), EValueInteger, regInfo->Hidden(), EValueInteger, regInfo->Embeddability(), EValueInteger, regInfo->NewFile(), EValueInteger, regInfo->Launch(), EValueString, &(regInfo->GroupName()),  EValueInteger, regInfo->DefaultScreenNumber());
    
    RPointerArray<HBufC> ownedFileArray = regInfo->OwnedFileArray();    
    for(TInt i=0;i<ownedFileArray.Count();++i)
    	{
        AddFileOwnershipInfoL(regInfo->AppUid(),*(ownedFileArray[i]));
        }

	RPointerArray<CServiceInfo> serviceArray = regInfo->ServiceArray();
    for(TInt i=0;i<serviceArray.Count();++i)
        {
        AddServiceInfoL(regInfo->AppUid(),serviceArray[i]);
        }
    
	RPointerArray<CLocalizableAppInfo> localizableAppInfo = regInfo->LocalizableAppInfoList();
    for(TInt i=0;i<localizableAppInfo.Count();++i)
       	{       	
       	AddLocalizableAppInfoL(regInfo->AppUid(), localizableAppInfo[i]);
       	}
    
	RPointerArray<CPropertyEntry> appPropertiesList = regInfo->AppProperties();
    for(TInt i=0;i<appPropertiesList.Count();++i)
       	{
        AddPropertyL(regInfo->AppUid(), appPropertiesList[i]);
		}
     
    RPointerArray<COpaqueData> opaqueDataList = regInfo->AppOpaqueData();
    for (TInt i = 0; i < opaqueDataList.Count(); ++i)
        {
        AddOpaqueDataL(regInfo->AppUid(), opaqueDataList[i]);
        }
          
    CleanupStack::PopAndDestroy(regInfo);
    DEBUG_PRINTF(_L8("Added the application details into SCR successfully "));
    }

void CScrRequestImpl::AddFileOwnershipInfoL(TUid aAppUid, const TDesC& aFileName)
    {
	DEBUG_PRINTF(_L8("Adding the File Ownership Info details into SCR"));
    if(aAppUid == KNullUid || !aFileName.CompareF(KNullDesC()))
    	{
    	DEBUG_PRINTF(_L8("Mandatory values not provided."));
    	User::Leave(KErrArgument);
    	}
    
    _LIT(KInsertFileOwnershipInfo,"INSERT INTO FileOwnershipInfo(AppUid, FileName) VALUES(?,?);");
    TInt numberOfValues = 2;
    ExecuteStatementL(KInsertFileOwnershipInfo(), numberOfValues, EValueInteger, aAppUid, EValueString, &aFileName);
    }

void CScrRequestImpl::AddLocalizableAppInfoL(TUid aAppUid, Usif::CLocalizableAppInfo* aLocalizableAppInfoEntry)
    {
	DEBUG_PRINTF(_L8("Adding the Localizable App Info details into SCR"));
	TInt captionAndIconInfoId = 0;
	if(NULL != aLocalizableAppInfoEntry->iCaptionAndIconInfo)
	    {
	    captionAndIconInfoId = AddCaptionAndIconInfoL(aLocalizableAppInfoEntry->iCaptionAndIconInfo);
	    }
	_LIT(KInsertLocalizableAppInfo,"INSERT INTO LocalizableAppInfo(AppUid, ShortCaption, GroupName, Locale, CaptionAndIconId) VALUES(?,?,?,?,?);");
    TInt numberOfValues = 5;
    ExecuteStatementL(KInsertLocalizableAppInfo(), numberOfValues, EValueInteger, aAppUid, EValueString, &(aLocalizableAppInfoEntry->ShortCaption()), EValueString, &(aLocalizableAppInfoEntry->GroupName()), EValueInteger, aLocalizableAppInfoEntry->ApplicationLanguage(), EValueInteger, captionAndIconInfoId);
    TInt localAppInfoId = iDbHandle->LastInsertedIdL();
	RPointerArray<CAppViewData> viewDataList = aLocalizableAppInfoEntry->ViewDataList();
	for (TInt i=0;i<viewDataList.Count();i++)
		{
		AddViewDataL(localAppInfoId, viewDataList[i]);
		}
	}

void CScrRequestImpl::AddViewDataL(TInt aLocalAppInfoId, Usif::CAppViewData* aViewDataEntry)
    {
	DEBUG_PRINTF(_L8("Adding the ViewData details into SCR"));
	if(aLocalAppInfoId == 0 || aViewDataEntry->Uid() == KNullUid )
		{
		DEBUG_PRINTF(_L8("Mandatory values not provided."));
        User::Leave(KErrArgument);
		}

	TInt captionAndIconInfoId = 0;
	if(NULL != aViewDataEntry->iCaptionAndIconInfo)
	   {
	   captionAndIconInfoId = AddCaptionAndIconInfoL(aViewDataEntry->iCaptionAndIconInfo);
	   }
	
	_LIT(KInsertViewData,"INSERT INTO ViewData(LocalAppInfoId, Uid, ScreenMode, CaptionAndIconId) VALUES(?,?,?,?);");
    TInt numberOfValues = 4;
    ExecuteStatementL(KInsertViewData(), numberOfValues, EValueInteger, aLocalAppInfoId, EValueInteger, aViewDataEntry->Uid(), EValueInteger, aViewDataEntry->ScreenMode(), EValueInteger, captionAndIconInfoId);
    }

TInt CScrRequestImpl::AddCaptionAndIconInfoL(Usif::CCaptionAndIconInfo* aCaptionAndIconEntry)
    {
	DEBUG_PRINTF(_L8("Adding the Caption And Icon Info details into SCR"));
	_LIT(KCaptionAndIconInfo,"INSERT INTO CaptionAndIconInfo(Caption, NumberOfIcons, IconFile) VALUES(?,?,?);");
    TInt numberOfValues = 3;
    ExecuteStatementL(KCaptionAndIconInfo(), numberOfValues, EValueString, &(aCaptionAndIconEntry->Caption()), EValueInteger, aCaptionAndIconEntry->NumOfAppIcons(), EValueString, &(aCaptionAndIconEntry->IconFileName()));
    return iDbHandle->LastInsertedIdL();
	}

void CScrRequestImpl::AddServiceInfoL(TUid aAppUid, Usif::CServiceInfo* aAppServiceInfoEntry)
    {
	DEBUG_PRINTF(_L8("Adding the Service Info details into SCR"));
    if(aAppUid == KNullUid)
    	{
    	DEBUG_PRINTF(_L8("Values for app uid is absent"));
    	User::Leave(KErrArgument);
    	}
    _LIT(KInsertServiceInfo,"INSERT INTO ServiceInfo(AppUid, Uid) VALUES(?,?);");
    TInt numberOfValues = 2;
    
    ExecuteStatementL(KInsertServiceInfo(), numberOfValues, EValueInteger, aAppUid, EValueInteger, aAppServiceInfoEntry->Uid());
    TInt serviceId = iDbHandle->LastInsertedIdL();

    RPointerArray<Usif::COpaqueData> opaqueData = aAppServiceInfoEntry->OpaqueData();
    for(TInt i=0;i<opaqueData.Count();i++)
        {
        AddOpaqueDataL(aAppUid, opaqueData[i], aAppServiceInfoEntry->Uid());
        }
    
    RPointerArray<Usif::CDataType> dataTypes = aAppServiceInfoEntry->DataTypes();
	for (TInt i=0;i<dataTypes.Count();i++)
		{
		AddServiceDataTypeL(serviceId, dataTypes[i]);
		}
    }

void CScrRequestImpl::AddServiceDataTypeL(TInt aServiceUid, Usif::CDataType* aDataTypeEntry)
	{
	DEBUG_PRINTF(_L8("Adding the Service Data Type details into SCR"));
    if(!((aDataTypeEntry->Type()).CompareF(KNullDesC())))
    	{
    	DEBUG_PRINTF(_L8("Values for service uid or type is absent"));
    	User::Leave(KErrArgument);
    	}
    _LIT(KInsertServiceDataTypeInfo,"INSERT INTO DataType(ServiceId, Priority, Type) VALUES(?,?,?);");
    TInt numberOfValues = 3;
    ExecuteStatementL(KInsertServiceDataTypeInfo(), numberOfValues, EValueInteger, aServiceUid, EValueInteger, aDataTypeEntry->Priority() , EValueString, &(aDataTypeEntry->Type()));
	}

void CScrRequestImpl::AddPropertyL(TUid aAppUid, Usif::CPropertyEntry* aAppPropertiesEntry)
    {
	DEBUG_PRINTF(_L8("Adding the Property details into SCR"));
    if(aAppUid == KNullUid || !((aAppPropertiesEntry->PropertyName().CompareF(KNullDesC()))))
    	{
    	DEBUG_PRINTF(_L8("Property name is absent and hence cannot be entered into the DB."));
    	User::Leave(KErrArgument);
    	}
	_LIT(KInsertAppProperties, "INSERT INTO AppProperties(AppUid, %S;");
	_LIT(KPropertyIntValue," Name, IntValue) VALUES(?,?,?)");
	_LIT(KPropertyStrValue," Locale, Name, StrValue) VALUES(?,?,?,?)");
	_LIT(KPropertyBinaryValue," Name, StrValue, IsStr8Bit) VALUES(?,?,?,1)");
	
	HBufC *statementStr(0);
	CStatement *stmt ;
	
    switch(aAppPropertiesEntry->PropertyType())
		{
		case CPropertyEntry::EIntProperty:
			{
			statementStr = FormatStatementLC(KInsertAppProperties, KPropertyIntValue().Length(), &KPropertyIntValue());
			stmt = iDbHandle->PrepareStatementLC(*statementStr);
			CIntPropertyEntry *intProp = static_cast<CIntPropertyEntry *>(aAppPropertiesEntry);
			stmt->BindIntL(1, aAppUid.iUid);
			stmt->BindStrL(2, intProp->PropertyName());
			stmt->BindInt64L(3, intProp->Int64Value());
			stmt->ExecuteStatementL();
			CleanupStack::PopAndDestroy(2,statementStr);
			}
			break;
		case CPropertyEntry::ELocalizedProperty:
			{
			statementStr = FormatStatementLC(KInsertAppProperties, KPropertyStrValue().Length(), &KPropertyStrValue());
			stmt = iDbHandle->PrepareStatementLC(*statementStr);
			CLocalizablePropertyEntry *localizedProp = static_cast<CLocalizablePropertyEntry *>(aAppPropertiesEntry);
			stmt->BindIntL(1, aAppUid.iUid);
			stmt->BindIntL(2, localizedProp->LocaleL());
			stmt->BindStrL(3, localizedProp->PropertyName());
			stmt->BindStrL(4, localizedProp->StrValue());
			stmt->ExecuteStatementL();
			CleanupStack::PopAndDestroy(2,statementStr);
			}
			break;	
		case CPropertyEntry::EBinaryProperty:
		    {
		    statementStr = FormatStatementLC(KInsertAppProperties, KPropertyBinaryValue().Length(), &KPropertyBinaryValue());
            stmt = iDbHandle->PrepareStatementLC(*statementStr);
            CBinaryPropertyEntry *binaryProp = static_cast<CBinaryPropertyEntry *>(aAppPropertiesEntry);
            stmt->BindIntL(1, aAppUid.iUid);
            stmt->BindStrL(2, binaryProp->PropertyName());
            stmt->BindBinaryL(3, binaryProp->BinaryValue());
            stmt->ExecuteStatementL();
            CleanupStack::PopAndDestroy(2,statementStr);
            }
		    break;
		
		default:
			DEBUG_PRINTF(_L8("The property type couldn't be recognized."));
			User::Leave(KErrAbort);	
		}
    }

void CScrRequestImpl::AddOpaqueDataL(TUid aAppUid, Usif::COpaqueData* aOpaqueDataEntry, TUid aServiceUid)
    {
	DEBUG_PRINTF(_L8("Adding the Opaque Data details into SCR"));
    const TInt KMaxOpaqueDataLength = 4096;
    /* AppUid cannot be NULL since this function is invoked from AddApplicationEntryL */ 
    __ASSERT_DEBUG(aAppUid != TUid::Null(), User::Leave(KErrArgument));
    
    _LIT(KOpaqueDataEntry, "INSERT INTO AppProperties(AppUid, Name, Locale, ServiceUid, StrValue, IsStr8Bit) VALUES(?,?,?,?,?,1)");
    
    CStatement *stmt = iDbHandle->PrepareStatementLC(KOpaqueDataEntry);
    
    stmt->BindIntL(1, aAppUid.iUid);
    stmt->BindStrL(2, _L("OpaqueData"));
    stmt->BindIntL(3, (TInt)aOpaqueDataEntry->iLanguage);
    stmt->BindIntL(4, aServiceUid.iUid);
    stmt->BindBinaryL(5, *(aOpaqueDataEntry->iOpaqueData), KMaxOpaqueDataLength);
    stmt->ExecuteStatementL();
    CleanupStack::PopAndDestroy(stmt);
    }

void CScrRequestImpl::DeleteApplicationEntryInternalL(const TInt aAppUid)
	{
	TInt numberOfValues = 1;

	DeleteFromTableL(KFileOwnershipInfoTable,KAppIdColumnName,aAppUid);
            
    _LIT(KDeleteDataType, "DELETE FROM DataType WHERE ServiceId IN \
                    (SELECT ServiceId FROM ServiceInfo WHERE AppUid=?);");
    ExecuteStatementL(KDeleteDataType, numberOfValues, EValueInteger, aAppUid);
    DEBUG_PRINTF2(_L8("Service datatype details associated with application(%d) have been deleted."), aAppUid);
        
    DeleteFromTableL(KServiceInfoTable,KAppIdColumnName,aAppUid);
        
    RArray<TInt> viewId;
    CleanupClosePushL(viewId);
    DEBUG_PRINTF2(_L8("Deleting the LocalizableAppInfo details associated with application (%d)"), aAppUid);
    _LIT(KSelectViewId, "SELECT ViewId FROM ViewData WHERE LocalAppInfoId IN(SELECT LocalAppInfoId FROM LocalizableAppInfo WHERE AppUid = ?);");
    CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectViewId);
    stmt->BindIntL(1, aAppUid);
    viewId.Close();
    while(stmt->ProcessNextRowL())
        {
        viewId.AppendL(stmt->IntColumnL(0));
        }
    
    _LIT(KViewId,"ViewId");
    _LIT(KDeleteCaptionAndIconInfoAssociatedToViewData, "DELETE FROM CaptionAndIconInfo WHERE CaptionAndIconId = (SELECT CaptionAndIconId FROM ViewData WHERE ViewId=?);");
               
    for(TInt i=0; i< viewId.Count();i++)   
        {
        ExecuteStatementL(KDeleteCaptionAndIconInfoAssociatedToViewData, numberOfValues, EValueInteger, viewId[i]);
        DeleteFromTableL(KViewDataTable,KViewId,viewId[i]);
        }
    CleanupStack::PopAndDestroy(2, &viewId);
    
    _LIT(KDeleteCaptionAndIconInfo, "DELETE FROM CaptionAndIconInfo WHERE CaptionAndIconId IN \
                        (SELECT CaptionAndIconId FROM LocalizableAppInfo WHERE AppUid=?);");
    ExecuteStatementL(KDeleteCaptionAndIconInfo, numberOfValues, EValueInteger, aAppUid);
        
    DeleteFromTableL(KLocalizableAppInfoTable,KAppIdColumnName,aAppUid);
        
    DeleteFromTableL(KAppPropertiesTable,KAppIdColumnName,aAppUid);
    DeleteFromTableL(KAppRegistrationInfoTable,KAppIdColumnName,aAppUid);
                
   }

void CScrRequestImpl::DeleteApplicationEntryL(const RMessage2& aMessage)
    {
	DEBUG_PRINTF(_L8("Deleting Application Entry details from SCR"));
    TInt applicationUid = aMessage.Int0();
    DeleteApplicationEntryInternalL(applicationUid);
    }

void CScrRequestImpl::DeleteAllAppsWithinPackageL(const RMessage2& aMessage)
    {
    TComponentId componentId = GetComponentIdFromMsgL(aMessage);
    TSecureId clientSid = aMessage.SecureId();
    if(componentId == 0 && clientSid != KSisRegistryServerSid)
        {
        DEBUG_PRINTF(_L8("ComponentId 0 corresponds to In-Rom Applications that cannot be deleted."));
        User::Leave(KErrNotSupported);
        }
    
    DeleteAllAppsWithinPackageInternalL(componentId);
    }

void CScrRequestImpl::DeleteAllAppsWithinPackageInternalL(const TComponentId aComponentId)
    {
    DEBUG_PRINTF2(_L8("Deleting all the applications associated with component (%d) from SCR."), aComponentId);
    
    // Fetching all the applications associated with the component
    _LIT(KSelectAssociatedAppIds, "SELECT AppUid FROM AppRegistrationInfo WHERE ComponentId=?;");
    CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectAssociatedAppIds);
    stmt->BindIntL(1, aComponentId);
        
    while(stmt->ProcessNextRowL())
        {
        TInt appId = stmt->IntColumnL(0);
        DeleteApplicationEntryInternalL(appId);
        }
        
    // Release allocated memories
    CleanupStack::PopAndDestroy(1, stmt); // stmt
    }

void CScrRequestImpl::DeleteFromTableL(const TDesC& aTableName, const TDesC& aAttribute, const TInt aValue) 
    {
    _LIT(KDeleteFromTable,"DELETE FROM %S WHERE %S=?;");
    TInt formattedLen = aTableName.Length() + aAttribute.Length();
    HBufC *statementStr = FormatStatementLC(KDeleteFromTable(), formattedLen, &aTableName, &aAttribute );
    TInt numberOfValues = 1;
    ExecuteStatementL(*statementStr, numberOfValues, EValueInteger, aValue);
    DEBUG_PRINTF4(_L8("%S info where %S = %d has been deleted."), &aTableName, &aAttribute, aValue);
    CleanupStack::PopAndDestroy(statementStr);
	}

CCaptionAndIconInfo* CScrRequestImpl::GetCaptionAndIconInfoL(TInt aCaptionAndIconId) const
    {
	DEBUG_PRINTF2(_L8("Returning the Caption And Icon Info associated with CaptionAndIconId (%d) from SCR."), aCaptionAndIconId);
    _LIT(KGetLocalizedCaptionAndIconInfo, "Select Caption,NumberOfIcons,Iconfile from CaptionAndIconInfo where CaptionAndIconId = ?");
    CStatement *stmt = iDbHandle->PrepareStatementLC(KGetLocalizedCaptionAndIconInfo);
    stmt->BindIntL(1, aCaptionAndIconId);
    if(stmt->ProcessNextRowL())
        {
        TPtrC caption(stmt->StrColumnL(0));
        TInt noOfAppIcons(stmt->IntColumnL(1));
        TPtrC iconFilename(stmt->StrColumnL(2));
        CCaptionAndIconInfo* captionandIconInfo = CCaptionAndIconInfo::NewL(caption, iconFilename, noOfAppIcons);
        DEBUG_PRINTF2(_L("The Caption for this App is %S "), captionandIconInfo->iCaption);
        DEBUG_PRINTF2(_L("The Number of AppIcons for this App is %d "), captionandIconInfo->iNumOfAppIcons);
        DEBUG_PRINTF2(_L("The Icon File Name this App is %S "), captionandIconInfo->iIconFileName);
        CleanupStack::PopAndDestroy(stmt);
        return captionandIconInfo;
        }
    else
        {
        CleanupStack::PopAndDestroy(stmt);
        return NULL;
        }
    }

void CScrRequestImpl::GetViewsL(RPointerArray<Usif::CAppViewData>& aViewInfoArray, TUid aAppUid, TLanguage aLanguage) const
    {                   
        _LIT(KSelectViewDetailsWithAppUid, "SELECT Uid, ScreenMode, CaptionAndIconId FROM ViewData WHERE LocalAppInfoId IN(SELECT LocalAppInfoId FROM LocalizableAppInfo WHERE AppUid = ? AND Locale = ?);");
        CStatement *stmt1 = iDbHandle->PrepareStatementLC(KSelectViewDetailsWithAppUid);
        stmt1->BindIntL(1, aAppUid.iUid);
        stmt1->BindIntL(2, aLanguage);
        aViewInfoArray.ResetAndDestroy();
        while(stmt1->ProcessNextRowL())
            {
            TUid uid;
            uid.iUid = stmt1->IntColumnL(0); 
            TInt screenMode(stmt1->IntColumnL(1)); 
            TInt captionAndIconId(stmt1->IntColumnL(2));           
            DEBUG_PRINTF2(_L("The view Uid for this App is 0x%x "),uid.iUid);            
            DEBUG_PRINTF2(_L("The view Screen Mode for this App is %d "), screenMode);
            CCaptionAndIconInfo *captionAndIconInfo = GetCaptionAndIconInfoL(captionAndIconId);                       
            CAppViewData *viewdataInfo =  CAppViewData::NewLC(uid, screenMode, captionAndIconInfo);              
            aViewInfoArray.AppendL(viewdataInfo);
            CleanupStack::Pop(viewdataInfo); // viewdataInfo
            }
        CleanupStack::PopAndDestroy(1, stmt1); // stmt1           
    }

void CScrRequestImpl::GetViewSizeL(const RMessage2& aMessage, TUid aAppUid, CRegInfoForApplicationSubsessionContext *aSubsessionContext) const
    {    
    GetViewsL(aSubsessionContext->iViewInfoArray,aAppUid,aSubsessionContext->iAppLanguageForCurrentLocale);
    if(aSubsessionContext->iViewInfoArray.Count() == 0)
      {
      DEBUG_PRINTF2(_L8("No view details associated with the given AppUid,%d"),aAppUid);
      }  
    DEBUG_PRINTF2(_L8("Returning the view details' entry size of application(0x%x) for the current locale."), aAppUid);
    WriteArraySizeL(aMessage, 1, aSubsessionContext->iViewInfoArray);
    }

void CScrRequestImpl::GetViewDataL(const RMessage2& aMessage, CRegInfoForApplicationSubsessionContext *aSubsessionContext) const
    {
    DEBUG_PRINTF(_L8("Returning the localized information entry data."));
    WriteArrayDataL(aMessage, 0, aSubsessionContext->iViewInfoArray);
    aSubsessionContext->iViewInfoArray.ResetAndDestroy(); 
    }

void CScrRequestImpl::OpenApplicationRegistrationViewL(const RMessage2& aMessage, CAppRegistrySubsessionContext*  aSubsessionContext) 
    {
    TLanguage requiredLanguage = TLanguage(aMessage.Int0());
    if(requiredLanguage == KUnspecifiedLocale)
        {
        requiredLanguage = User::Language();
        }
    aSubsessionContext->iLanguage = requiredLanguage;
   
    CStatement* stmt;
    _LIT(KAllAppIds," SELECT AppUid from AppRegistrationInfo"); 
    stmt = iDbHandle->PrepareStatementLC(KAllAppIds);
    aSubsessionContext->iAppUids.Close();
    while(stmt->ProcessNextRowL())
        {
        TUid appUid = TUid::Uid(stmt->IntColumnL(0));
        TComponentId componentId(0);
        if(GetComponentIdForAppInternalL(appUid, componentId))
	    	{
	    	if(!componentId || IsComponentPresentL(componentId))
            	{
            	aSubsessionContext->iAppUids.AppendL(appUid);    
            	}
	    	}
        }
    CleanupStack::PopAndDestroy(stmt);  
    }

void CScrRequestImpl::OpenApplicationRegistrationForAppUidsViewL(const RMessage2& aMessage, CAppRegistrySubsessionContext*  aSubsessionContext) 
    {
    TLanguage requiredLanguage = TLanguage(aMessage.Int0());
    if(requiredLanguage == KUnspecifiedLocale)
        {
        requiredLanguage = User::Language();
        }
    aSubsessionContext->iLanguage = requiredLanguage;
    
    //Read languages need to pass
    TInt bufSize=0;
    bufSize = aMessage.GetDesMaxLength(1);
    HBufC8* bufToHoldAppUids = HBufC8::NewLC(bufSize);
    TPtr8 bufPtrDscToHoldAppUids = bufToHoldAppUids->Des();
    aMessage.ReadL(1, bufPtrDscToHoldAppUids);
    RDesReadStream inStream(bufPtrDscToHoldAppUids);
    CleanupClosePushL(inStream);
    TInt size = inStream.ReadInt32L();
       
    aSubsessionContext->iAppUids.Close();
    for (TInt i =0; i<size ;i++)
        {
        TUid appUid = TUid::Uid(inStream.ReadInt32L());
		TComponentId componentId(0);
        if(GetComponentIdForAppInternalL(appUid, componentId)) // Check for application presence and fetch the corresponding ComponentId
	    	{
	    	if(!componentId || IsComponentPresentL(componentId)) // Check if the component is present
            	{
            	aSubsessionContext->iAppUids.AppendL(appUid);    
            	}
	    	}
        }
    
    CleanupStack::PopAndDestroy(2,bufToHoldAppUids); //bufToHoldAppUids, inStream
    }

TBool CScrRequestImpl::GetApplicationRegistrationInfoL(CApplicationRegistrationData& aApplicationRegistration,TUid aAppUid) const
    {
    _LIT(KSelectAppRegInfo, "SELECT AppUid, ComponentId, AppFile, TypeId, Attributes, Hidden, Embeddable, NewFile, Launch, GroupName, DefaultScreenNumber  FROM AppRegistrationInfo WHERE AppUid = ?");
           
    CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectAppRegInfo);
    stmt->BindIntL(1, aAppUid.iUid);
    if(stmt->ProcessNextRowL())
      {
      aApplicationRegistration.iAppUid = TUid::Uid(stmt->IntColumnL(0)); 
      HBufC* appFile = stmt->StrColumnL(2).AllocL();
      DeleteObjectZ(aApplicationRegistration.iAppFile);
      aApplicationRegistration.iAppFile = appFile;
      aApplicationRegistration.iTypeId = stmt->IntColumnL(3);
      aApplicationRegistration.iCharacteristics.iAttributes = stmt->IntColumnL(4);
      aApplicationRegistration.iCharacteristics.iAppIsHidden = stmt->IntColumnL(5);
      aApplicationRegistration.iCharacteristics.iEmbeddability = (TApplicationCharacteristics::TAppEmbeddability)stmt->IntColumnL(6);
      aApplicationRegistration.iCharacteristics.iSupportsNewFile = stmt->IntColumnL(7);
      aApplicationRegistration.iCharacteristics.iLaunchInBackground = stmt->IntColumnL(8);
      aApplicationRegistration.iCharacteristics.iGroupName = stmt->StrColumnL(9);
      aApplicationRegistration.iDefaultScreenNumber = stmt->IntColumnL(10);
     
      DEBUG_PRINTF2(_L("The Uid for this App is 0x%x "),aApplicationRegistration.iAppUid);
      DEBUG_PRINTF2(_L("The App File for this App is %S "), aApplicationRegistration.iAppFile);
      DEBUG_PRINTF2(_L("The Attribute for this App is %d "), aApplicationRegistration.iCharacteristics.iAttributes);
      DEBUG_PRINTF2(_L("The Hidden for this App is %d "), (aApplicationRegistration.iCharacteristics.iAppIsHidden));
      DEBUG_PRINTF2(_L("The Embeddability for this App is %d "), aApplicationRegistration.iCharacteristics.iEmbeddability);
      DEBUG_PRINTF2(_L("The New File for this App is %d "), aApplicationRegistration.iCharacteristics.iSupportsNewFile);
      DEBUG_PRINTF2(_L("The Launch for this App is %d "), aApplicationRegistration.iCharacteristics.iLaunchInBackground);
      DEBUG_PRINTF2(_L("The Group Name for this App is %S "),  &(aApplicationRegistration.iCharacteristics.iGroupName));
      DEBUG_PRINTF2(_L("The Default screen number for this App is %d "), aApplicationRegistration.iDefaultScreenNumber);
                       
      }
    else
      {
      DEBUG_PRINTF2(_L8("AppUid %d Not Found in the SCR"),aAppUid);       
      CleanupStack::PopAndDestroy(stmt);
      return EFalse;
      }
    CleanupStack::PopAndDestroy(stmt);
    return ETrue;
    }

void CScrRequestImpl::GetFileOwnershipInfoL(CApplicationRegistrationData& aApplicationRegistration,TUid aAppUid) const
    {
    _LIT(KGetFileOwnershipInfo, "SELECT FileName FROM FileOwnershipInfo WHERE AppUid = ?");
    CStatement *stmt = iDbHandle->PrepareStatementLC(KGetFileOwnershipInfo);
    stmt->BindIntL(1, aAppUid.iUid);
    aApplicationRegistration.iOwnedFileArray.ResetAndDestroy();
    while(stmt->ProcessNextRowL())
		{
		HBufC *fileName = stmt->StrColumnL(0).AllocLC();
        aApplicationRegistration.iOwnedFileArray.AppendL(fileName);        
        DEBUG_PRINTF2(_L("The File Name for owned Files for this App is %S "), fileName);
        CleanupStack::Pop();
        }    
    CleanupStack::PopAndDestroy(stmt);
    }


void CScrRequestImpl::GetDataTypesL(RPointerArray<Usif::CDataType>& aDataTypes,TInt aServiceId)const
    {
    _LIT(KGetDataType, "SELECT Priority, Type FROM DataType where ServiceId = ?");
    CStatement *stmt = iDbHandle->PrepareStatementLC(KGetDataType);
    stmt->BindIntL(1, aServiceId);
    aDataTypes.ResetAndDestroy();
    while(stmt->ProcessNextRowL())
         {
         Usif::CDataType* dataType = CDataType::NewLC();
         dataType->iPriority = stmt->IntColumnL(0);
         DeleteObjectZ(dataType->iType);
         dataType->iType = stmt->StrColumnL(1).AllocLC();
         DEBUG_PRINTF2(_L("The Service Info Priority for this App is %d "), dataType->iPriority);
         DEBUG_PRINTF2(_L("The Service Type for this App is %S "), dataType->iType);
         aDataTypes.AppendL(dataType);
         CleanupStack::Pop(2,dataType); //for  iType and dataType 
         }
    CleanupStack::PopAndDestroy(stmt);
    }

void CScrRequestImpl::GetServiceInfoL(CApplicationRegistrationData& aApplicationRegistration, TUid aAppUid, TLanguage aLanguage) const
    {
    Usif::CServiceInfo* serviceInfo = NULL;     
    TInt serviceId = 0;
    _LIT(KGetServiceInfo, "SELECT ServiceId, Uid FROM ServiceInfo where AppUid = ?");
    CStatement *stmt = iDbHandle->PrepareStatementLC(KGetServiceInfo);
    stmt->BindIntL(1, aAppUid.iUid);
    aApplicationRegistration.iServiceArray.ResetAndDestroy();
    while(stmt->ProcessNextRowL())
        {        
        serviceInfo = Usif::CServiceInfo::NewLC();
        serviceId = stmt->IntColumnL(0);
        serviceInfo->iUid = TUid::Uid(stmt->IntColumnL(1));

        DEBUG_PRINTF2(_L("The Service Uid for this App is 0x%x "), serviceInfo->iUid);
        if(serviceInfo->iUid.iUid)
            {
            GetOpaqueDataArrayL(aAppUid, serviceInfo->iUid, serviceInfo->iOpaqueDataArray, aLanguage);
            }

        //populate the data types for a service Id 
        GetDataTypesL(serviceInfo->iDataTypes,serviceId);      
               
        aApplicationRegistration.iServiceArray.AppendL(serviceInfo);        
        CleanupStack::Pop(serviceInfo); // serviceInfo 
        }    
    CleanupStack::PopAndDestroy(stmt);        
    }

void CScrRequestImpl::GetLocalizableAppInfoL(CApplicationRegistrationData& aApplicationRegistration,TUid aAppUid, TLanguage aLanguage)
    {    
    TLanguage storedLanguage;
    if(GetNearestAppLanguageL(aLanguage, aAppUid, storedLanguage))
        {
        Usif::CLocalizableAppInfo* localizedAppInfo = NULL;        
        _LIT(KGetServiceInfo, "Select ShortCaption,GroupName,Locale,CaptionAndIconId from LocalizableAppInfo where AppUid = ? and Locale = ?");
        CStatement *stmt = iDbHandle->PrepareStatementLC(KGetServiceInfo);
        stmt->BindIntL(1, aAppUid.iUid);
        stmt->BindIntL(2, (TInt)storedLanguage);
        if(stmt->ProcessNextRowL())
           {
           localizedAppInfo = Usif::CLocalizableAppInfo::NewLC();
           DeleteObjectZ(localizedAppInfo->iShortCaption);
           localizedAppInfo->iShortCaption = stmt->StrColumnL(0).AllocL();
           DeleteObjectZ(localizedAppInfo->iGroupName);
           localizedAppInfo->iGroupName = stmt->StrColumnL(1).AllocL();
           localizedAppInfo->iApplicationLanguage = (TLanguage)stmt->IntColumnL(2);           
          
           DEBUG_PRINTF2(_L("The Short Caption for this App is %S "), localizedAppInfo->iShortCaption);
           DEBUG_PRINTF2(_L("The Group name this App is %S "), localizedAppInfo->iGroupName);
           DEBUG_PRINTF2(_L("The application language this App is %d "), localizedAppInfo->iApplicationLanguage);
          
           //populate localized caption and icon info
           TInt captionAndIconID = stmt->IntColumnL(3);
           localizedAppInfo->iCaptionAndIconInfo = GetCaptionAndIconInfoL(captionAndIconID);
           //populate view data           
           GetViewsL(localizedAppInfo->iViewDataList, aAppUid, storedLanguage);
           
           aApplicationRegistration.iLocalizableAppInfoList.AppendL(localizedAppInfo);
           CleanupStack::Pop(localizedAppInfo); 
           }
        CleanupStack::PopAndDestroy(stmt);
        }
        else
        {
        DEBUG_PRINTF2(_L("No Nearest locale found for AppUid 0x%x in the SCR"), aAppUid);
        }    
    }

void CScrRequestImpl::GetAppRegOpaqueDataL(CApplicationRegistrationData& aApplicationRegistration, TUid aAppUid, TLanguage aLanguage) const
    {
    GetOpaqueDataArrayL(aAppUid, TUid::Null(), aApplicationRegistration.iOpaqueDataArray, aLanguage);
    }

void CScrRequestImpl::NextApplicationRegistrationInfoSizeL(const RMessage2& aMessage, CApplicationRegistrationData*& aApplicationRegistration, CAppRegistrySubsessionContext*  aSubsessionContext)
    {
    while(1)
        {
        DeleteObjectZ(aApplicationRegistration);
        aApplicationRegistration = CApplicationRegistrationData::NewL();
        if((aSubsessionContext->iAppRegIndex < aSubsessionContext->iAppUids.Count()))
            {
            TBool dataFound = EFalse;
            TUid appUid = aSubsessionContext->iAppUids[aSubsessionContext->iAppRegIndex];
              
			TInt err = KErrNone; //Initialised to avoid compiler warning 
            //Populate the Application Registration Info
            TRAP(err, 
                    {
                    TBool retVal = EFalse;
                    retVal = GetApplicationRegistrationInfoL(*aApplicationRegistration, appUid);

                    //Check if we have a valid application
                    if(retVal)
                        {
                        //Populate File ownership info 
                        GetFileOwnershipInfoL(*aApplicationRegistration, appUid);
                        //Populate service info
                        GetServiceInfoL(*aApplicationRegistration, appUid, aSubsessionContext->iLanguage);
                        //Populate localizable appinfo including caption and icon info 
                        //and view data and its caption and icon info. 
                        GetLocalizableAppInfoL(*aApplicationRegistration, appUid, aSubsessionContext->iLanguage);

                        GetAppRegOpaqueDataL(*aApplicationRegistration, appUid, aSubsessionContext->iLanguage);
    
                        GetAppPropertiesInfoL(*aApplicationRegistration, appUid, aSubsessionContext->iLanguage);
                        
                        dataFound = ETrue;
                        }
                    });
            
            //Incrementing the index
            aSubsessionContext->iAppRegIndex++;
                        
            if(dataFound)
                {
                WriteObjectSizeL(aMessage, 1, aApplicationRegistration);
                break;
                }
            else
                {
                DEBUG_PRINTF3(_L8("Error %d while reading the app registration info 0x%x. Ignoring current application details."), err, appUid);
                }
                           
            }
        else
            {
            DEBUG_PRINTF(_L8("Reached the end of the view."));
            WriteIntValueL(aMessage, 1, 0);                
            DeleteObjectZ(aApplicationRegistration);
            break;
            }
        }
    }

void CScrRequestImpl::NextApplicationRegistrationInfoDataL(const RMessage2& aMessage, CApplicationRegistrationData*& aApplicationRegistration)
    {
    DEBUG_PRINTF(_L8("Returning the Application Registration data."));
    WriteObjectDataL(aMessage, 0, aApplicationRegistration);
    DeleteObjectZ(aApplicationRegistration); // Delete the object to prevent it to be resent.
    }

void CScrRequestImpl::GetAppOwnedFilesSizeL(const RMessage2& aMessage, TUid aAppUid, CRegInfoForApplicationSubsessionContext *aSubsessionContext) const
    {
    DEBUG_PRINTF2(_L8("Returning the Application Owned files size for application 0X%x."), aAppUid);
    _LIT(KSelectFileOwnershipInfoWithAppUid, "SELECT FileName FROM FileOwnershipInfo WHERE AppUid = ? ;");
    CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectFileOwnershipInfoWithAppUid);
    stmt->BindIntL(1, aAppUid.iUid);
    aSubsessionContext->iAppOwnedFiles.ResetAndDestroy();
    while(stmt->ProcessNextRowL())
       {
       HBufC* fileName=stmt->StrColumnL(0).AllocLC();
       aSubsessionContext->iAppOwnedFiles.AppendL(fileName);
       CleanupStack::Pop(1, fileName);
       }
    CleanupStack::PopAndDestroy(1, stmt); 
    if(0 == aSubsessionContext->iAppOwnedFiles.Count() )
       {
       DEBUG_PRINTF2(_L8("Application with AppUid :0X%x does not own any file "),aAppUid);
       }  
    WriteArraySizeL(aMessage, 1, aSubsessionContext->iAppOwnedFiles);
    }

void CScrRequestImpl::GetAppOwnedFilesDataL(const RMessage2& aMessage, CRegInfoForApplicationSubsessionContext *aSubsessionContext) const
    {
    DEBUG_PRINTF(_L8("Returning the Application Owned files ."));
    WriteArrayDataL(aMessage, 0, aSubsessionContext->iAppOwnedFiles);
    aSubsessionContext->iAppOwnedFiles.ResetAndDestroy(); 
    }

void CScrRequestImpl::GetAppCharacteristicsL(const RMessage2& aMessage, TUid aAppUid) const
    {
    DEBUG_PRINTF2(_L8("Returning the characteristics of application 0X%x."), aAppUid);
    _LIT(KSelectApplicationCapability, "SELECT Attributes, Hidden, Embeddable, NewFile, Launch, GroupName FROM AppRegistrationInfo WHERE AppUid = ? ;");
    CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectApplicationCapability);
    stmt->BindIntL(1, aAppUid.iUid);
    TApplicationCharacteristics appCharacteristics;
    if(stmt->ProcessNextRowL())
       {
       appCharacteristics.iAttributes = stmt->IntColumnL(0);
       appCharacteristics.iAppIsHidden  = stmt->IntColumnL(1);
       appCharacteristics.iEmbeddability = (TApplicationCharacteristics::TAppEmbeddability)stmt->IntColumnL(2);
       appCharacteristics.iSupportsNewFile  = stmt->IntColumnL(3); 
       appCharacteristics.iLaunchInBackground  = stmt->IntColumnL(4);
       appCharacteristics.iGroupName = stmt->StrColumnL(5);
       TPckgC<TApplicationCharacteristics> infoPk(appCharacteristics);  
       aMessage.WriteL(0, infoPk);
       }
    else
       {
       DEBUG_PRINTF2(_L8("No Data found for Application capability with AppUid :0X%x "),aAppUid);
       }
    CleanupStack::PopAndDestroy(1, stmt);
    }

void CScrRequestImpl::GetAppIconForFileNameL(const RMessage2& aMessage, TUid aAppUid, CRegInfoForApplicationSubsessionContext *aSubsessionContext) const
    {
    DEBUG_PRINTF2(_L8("Returning the Icon File Name for application 0X%x."), aAppUid);
    _LIT(KSelectIconFileNameForApplication, "SELECT IconFile FROM CaptionAndIconInfo WHERE CaptionAndIconId IN (SELECT CaptionAndIconId  FROM  LocalizableAppInfo WHERE AppUid = ? AND Locale = ?);");
    CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectIconFileNameForApplication);
    stmt->BindIntL(1, aAppUid.iUid);
    stmt->BindIntL(2, aSubsessionContext->iAppLanguageForCurrentLocale);
    if(stmt->ProcessNextRowL())
       {
       TFileName fileName = stmt->StrColumnL(0);
       TPckgC<TFileName> pckg(fileName);
       aMessage.WriteL(0, pckg);
       }
    else
       {
       DEBUG_PRINTF2(_L8("No Icon file found for Application with AppUid :0X%x "),aAppUid);       
       }
    CleanupStack::PopAndDestroy(1, stmt); 
    }

void CScrRequestImpl::GetAppViewIconFileNameL(const RMessage2& aMessage, TUid aAppUid, CRegInfoForApplicationSubsessionContext *aSubsessionContext) const
    {
    TUid viewUid;
    viewUid.iUid= aMessage.Int0();
    DEBUG_PRINTF2(_L8("Returning the view Icon File Name size for application 0X%x."), aAppUid);
    _LIT(KSelectIconFileNameFromViewForApplication, "SELECT IconFile FROM CaptionAndIconInfo WHERE CaptionAndIconId IN (SELECT CaptionAndIconId  FROM ViewData WHERE Uid = ? AND LocalAppInfoId IN ( SELECT LocalAppInfoId  FROM LocalizableAppInfo WHERE AppUid = ? AND Locale = ?));");
    CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectIconFileNameFromViewForApplication);
    stmt->BindIntL(1, viewUid.iUid);
    stmt->BindIntL(2, aAppUid.iUid);
    stmt->BindIntL(3, aSubsessionContext->iAppLanguageForCurrentLocale);
    if(stmt->ProcessNextRowL())
       {
       TFileName fileName = stmt->StrColumnL(0);
       TPckgC<TFileName> pckg(fileName);
       aMessage.WriteL(1, pckg);
       }
    else
       {
       DEBUG_PRINTF3(_L8("No view Icon file found for Application with AppUid :0X%x and View ID :0X%x "),aAppUid,viewUid);
       }
    CleanupStack::PopAndDestroy(1, stmt); 
    }

void CScrRequestImpl::GetAppServiceInfoSizeL(const RMessage2& aMessage, CApplicationRegInfoSubsessionContext *aSubsessionContext ) const
    {
    CAppServiceInfoFilter *filter = ReadObjectFromMessageLC<CAppServiceInfoFilter>(aMessage, 0);
    TLanguage locale = (TLanguage)aMessage.Int1();  

    switch(filter->iSetFlag)
        {
        case CAppServiceInfoFilter::EGetServiceInfoForApp:
            {
            GetAppServicesL(filter->iAppUid, aSubsessionContext->iServiceInfoArray, locale);                    
            break;
            }
        case CAppServiceInfoFilter::EGetServiceImplementationForServiceUid:
            {            
            GetServiceImplementationsL(filter->iServiceUid, aSubsessionContext->iServiceInfoArray, locale);
            break;
            }
        case CAppServiceInfoFilter::EGetServiceImplementationForServiceUidAndDatatType:
            {            
            GetServiceImplementationsL(filter->iServiceUid, *(filter->iDataType), aSubsessionContext->iServiceInfoArray, locale);
            break;
            }
        case CAppServiceInfoFilter::EGetOpaqueDataForAppWithServiceUid:
            {      
            GetAppServiceOpaqueDataL(filter->iAppUid, filter->iServiceUid, aSubsessionContext->iServiceInfoArray, locale);
            break;
            }
        default:
            {
            DEBUG_PRINTF(_L8("No match found for the query requested."));
            User::Leave(KErrArgument);
            }
        }
    if(aSubsessionContext->iServiceInfoArray.Count()== 0)
        {    
        DEBUG_PRINTF(_L8("No service info associated with the given parameters found"));
        User::Leave(KErrNotFound);
        }
    WriteArraySizeL(aMessage, 2, aSubsessionContext->iServiceInfoArray);
    CleanupStack::PopAndDestroy(filter);
    }

void CScrRequestImpl::GetAppServiceInfoDataL(const RMessage2& aMessage, CApplicationRegInfoSubsessionContext *aSubsessionContext) const
    {
    DEBUG_PRINTF(_L8("Returning the service information details."));
    WriteArrayDataL(aMessage, 0, aSubsessionContext->iServiceInfoArray);
    aSubsessionContext->iServiceInfoArray.ResetAndDestroy(); 
    }

void CScrRequestImpl::GetAppServicesL(TUid aAppUid, RPointerArray<
        CServiceInfo>& aServiceInfoArray, TLanguage aLocale) const
    {
    DEBUG_PRINTF2(_L8("Returning the size of the service info details entry of application (%d)."), aAppUid.iUid);
    _LIT(KSelectMatchingServiceInfo, "SELECT ServiceId, Uid FROM ServiceInfo WHERE AppUid=?;");
    CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectMatchingServiceInfo);
    stmt->BindIntL(1, aAppUid.iUid);
    aServiceInfoArray.ResetAndDestroy();
    while (stmt->ProcessNextRowL())
        {
        /* AppProperties is being used for OpaqueData of both AppRegInfo
         * and ServiceInfo. So add to ServiceInfoArray only if serviceId is not 0
         */
        if (stmt->IntColumnL(1))
            {
            CServiceInfo* serviceInfo = CServiceInfo::NewLC();
            serviceInfo->iUid = TUid::Uid(stmt->IntColumnL(1));

            GetOpaqueDataArrayL(aAppUid, serviceInfo->iUid, serviceInfo->iOpaqueDataArray, aLocale);
            GetDataTypesL(serviceInfo->iDataTypes, stmt->IntColumnL(0));
            aServiceInfoArray.AppendL(serviceInfo);
            CleanupStack::Pop(serviceInfo);
            }
        }
    // Release allocated memories
    CleanupStack::PopAndDestroy(1, stmt); // stmt
    }

void CScrRequestImpl::GetServiceImplementationsL(TUid aServiceUid,
        RPointerArray<CServiceInfo>& aServiceInfoArray, TLanguage aLocale) const
    {
    if (aServiceUid.iUid)
        {
        DEBUG_PRINTF2(_L8("Returning the size of the service info details entry associated with service Uid (%d)."), aServiceUid.iUid);
        _LIT(KSelectMatchingServiceInfo, "SELECT ServiceId, Uid, AppUid FROM ServiceInfo WHERE Uid=?;");
        CStatement *stmt = iDbHandle->PrepareStatementLC(
                KSelectMatchingServiceInfo);
        stmt->BindIntL(1, aServiceUid.iUid);
        aServiceInfoArray.ResetAndDestroy();
        while (stmt->ProcessNextRowL())
            {
            CServiceInfo* serviceInfo = CServiceInfo::NewLC();
            
            serviceInfo->iUid = TUid::Uid(stmt->IntColumnL(1));
            TUid appUid = TUid::Uid(stmt->IntColumnL(2));
            GetOpaqueDataArrayL(appUid, serviceInfo->iUid, serviceInfo->iOpaqueDataArray, aLocale);
            GetDataTypesL(serviceInfo->iDataTypes, stmt->IntColumnL(0));
            
            aServiceInfoArray.AppendL(serviceInfo);
            CleanupStack::Pop(serviceInfo);
            }
        // Release allocated memories
        CleanupStack::PopAndDestroy(1, stmt); // stmt
        }
    }

void CScrRequestImpl::GetServiceImplementationsL(TUid aServiceUid,
        TDesC& aDataType, RPointerArray<CServiceInfo>& aServiceInfoArray, TLanguage aLocale) const
    {
    if (aServiceUid.iUid)
        {
        DEBUG_PRINTF3(_L8("Returning the size of the service info details entry associated with service Uid (%d) and datatype (%S)."), aServiceUid.iUid, &aDataType);
        _LIT(KSelectMatchingServiceInfo, "SELECT Uid, Priority, Type, AppUid FROM (ServiceInfo JOIN DataType ON ServiceInfo.ServiceId = DataType.ServiceId) WHERE Uid=? AND Type=?;");
        CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectMatchingServiceInfo);
        stmt->BindIntL(1, aServiceUid.iUid);
        stmt->BindStrL(2, aDataType);
        aServiceInfoArray.ResetAndDestroy();
        while (stmt->ProcessNextRowL())
            {
            CServiceInfo* serviceInfo = CServiceInfo::NewLC();
            
            serviceInfo->iUid = TUid::Uid(stmt->IntColumnL(0));
            
            TUid appUid = TUid::Uid(stmt->IntColumnL(3));
            GetOpaqueDataArrayL(appUid, serviceInfo->iUid, serviceInfo->iOpaqueDataArray, aLocale);
            TInt priority(stmt->IntColumnL(1));
            TPtrC datatype(stmt->StrColumnL(2));
            CDataType* dataType = CDataType::NewL(priority,datatype);
            CleanupStack::PushL(dataType);
            serviceInfo->iDataTypes.AppendL(dataType);
            CleanupStack::Pop(dataType);
            aServiceInfoArray.AppendL(serviceInfo);
            CleanupStack::Pop(serviceInfo);
            }
        // Release allocated memories
        CleanupStack::PopAndDestroy(1, stmt); // stmt
        }
    }


void CScrRequestImpl::GetAppServiceOpaqueDataL(TUid aAppUid,
        TUid aServiceUid, RPointerArray<CServiceInfo>& aServiceInfoArray, TLanguage aLocale) const
    {
    if (aServiceUid.iUid)
        {
        DEBUG_PRINTF3(_L8("Returning the size of the service info details entry associated with app Uid (%d) and service Uid (%d)."), aAppUid.iUid, aServiceUid.iUid);
        _LIT(KSelectMatchingServiceInfo, "SELECT ServiceId, Uid FROM ServiceInfo WHERE AppUid=? AND Uid=? ;");
        CStatement *stmt = iDbHandle->PrepareStatementLC(
                KSelectMatchingServiceInfo);
        stmt->BindIntL(1, aAppUid.iUid);
        stmt->BindIntL(2, aServiceUid.iUid);
        aServiceInfoArray.ResetAndDestroy();
        while (stmt->ProcessNextRowL())
            {
            CServiceInfo* serviceInfo = CServiceInfo::NewLC();
            
            serviceInfo->iUid = TUid::Uid(stmt->IntColumnL(1));
            GetOpaqueDataArrayL(aAppUid, serviceInfo->iUid, serviceInfo->iOpaqueDataArray, aLocale);
            GetDataTypesL(serviceInfo->iDataTypes, stmt->IntColumnL(0));

            aServiceInfoArray.AppendL(serviceInfo);
            CleanupStack::Pop(serviceInfo);
            }
        // Release allocated memories
        CleanupStack::PopAndDestroy(1, stmt); // stmt
        }
    }

void CScrRequestImpl::GetOpaqueDataArrayL(TUid aAppUid, TUid aServiceUid, RPointerArray<COpaqueData>& aOpaqueDataArray, TLanguage aLanguage) const
    {
    TLanguage finalAppLocale;
    GetNearestAppLanguageForOpaqueDataL(aLanguage, aAppUid, aServiceUid, finalAppLocale);
            
    _LIT(KOpaqueData, "SELECT StrValue FROM AppProperties where Name = ? AND ServiceUid = ?  AND AppUid = ? AND Locale = ?");
    CStatement *stmt = iDbHandle->PrepareStatementLC(KOpaqueData);
    
    stmt->BindStrL(1, _L("OpaqueData"));
    stmt->BindIntL(2, aServiceUid.iUid);
    stmt->BindIntL(3, aAppUid.iUid);
    stmt->BindIntL(4, (TInt)finalAppLocale);
    
    aOpaqueDataArray.ResetAndDestroy();
    while (stmt->ProcessNextRowL())
        {
        Usif::COpaqueData* opaqueData = Usif::COpaqueData::NewLC();
        opaqueData->iLanguage = finalAppLocale;
        DeleteObjectZ(opaqueData->iOpaqueData);
        opaqueData->iOpaqueData = stmt->BinaryColumnL(0).AllocL();
        DEBUG_PRINTF2(_L("Locale for opaque entry for this App is %d "), opaqueData->iLanguage);
        aOpaqueDataArray.AppendL(opaqueData);
        CleanupStack::Pop(opaqueData);
        }
    CleanupStack::PopAndDestroy(stmt);
    }

TBool CScrRequestImpl::GetComponentIdForAppInternalL(TUid aAppUid, TComponentId& aComponentId) const
    {
    return GetIntforConditionL(KComponentIdColumnName, KAppRegistrationInfoTable, KAppIdColumnName, aAppUid.iUid, aComponentId);
    }

void CScrRequestImpl::GetComponentIdForAppL(const RMessage2& aMessage) const
    {
    DEBUG_PRINTF(_L8("Returning the componentId that the given appUid is associated with."));
    TUid appUid;
    TPckg<TUid> appUidPckg(appUid);
    aMessage.ReadL(0,appUidPckg);
    TComponentId compId;
        
    if(GetComponentIdForAppInternalL(appUid, compId))
        {
        TPckg<TComponentId> compIdPckg(compId);
        aMessage.WriteL(1, compIdPckg);
        }
    else
        {
        DEBUG_PRINTF(_L8("The given app doesnot exist"));
        User::Leave(KErrNotFound);
        }
    }

void CScrRequestImpl::GetAppUidsForComponentSizeL(const RMessage2& aMessage) const
    {
    DEBUG_PRINTF(_L8("Returning the size of the list of AppUids associated with the component."));
    TComponentId compId = aMessage.Int0();

    _LIT(KSelectAppUids, "SELECT AppUid FROM AppRegistrationInfo WHERE ComponentId=?;");
    CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectAppUids);
    stmt->BindIntL(1, compId);
    iComponentAppUids.Close();
    while(stmt->ProcessNextRowL())
        {
        TInt appUid(stmt->IntColumnL(0));
        iComponentAppUids.AppendL(TUid::Uid(appUid));
        }
    // Release allocated memories
    CleanupStack::PopAndDestroy(1, stmt); // stmt
    
    if(iComponentAppUids.Count()== 0)
        {    
        DEBUG_PRINTF(_L8("No apps associated with the given componentId found"));
        User::Leave(KErrNotFound);
        }

    WriteArraySizeL(aMessage, 1, iComponentAppUids);
    }

void CScrRequestImpl::GetAppUidsForComponentDataL(const RMessage2& aMessage) const
    {
    DEBUG_PRINTF(_L8("Returning the list of appUids associated with the component."));
    WriteArrayDataL(aMessage, 0, iComponentAppUids);
    iComponentAppUids.Reset(); 
    }

void CScrRequestImpl::GetApplicationInfoL(const RMessage2& aMessage) 
    {
    TUid appUid = TUid::Uid(aMessage.Int0());
    TLanguage locale = (TLanguage)aMessage.Int1();
    if(locale == KUnspecifiedLocale)
        {
        locale = User::Language();
        }
    TAppRegInfo appRegInfo;
    TPckg<TAppRegInfo> appRegInfoPckg(appRegInfo);
    aMessage.ReadL(2, appRegInfoPckg);

    _LIT(KNull,"");
    DEBUG_PRINTF2(_L8("Returning the basic information contained in TAppRegInfo for application 0x%x."), appUid.iUid);
    _LIT(KSelectAppInfo, "SELECT AppFile FROM AppRegistrationInfo WHERE AppUid = ?;");
    CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectAppInfo);
    stmt->BindIntL(1, appUid.iUid);
    if(stmt->ProcessNextRowL())
        {
        appRegInfo.iUid = appUid;
        appRegInfo.iFullName.Copy(stmt->StrColumnL(0));
        TLanguage finallocale = KUnspecifiedLocale;
        if(GetNearestAppLanguageL(locale,appRegInfo.iUid, finallocale))
            {
            GetCaptionAndShortCaptionInfoForLocaleL(appRegInfo.iUid, finallocale, appRegInfo.iShortCaption, appRegInfo.iCaption);
            }
        else
            {
            appRegInfo.iCaption.Copy(KNull);
            appRegInfo.iShortCaption.Copy(KNull);            
            }
        aMessage.WriteL(2, appRegInfoPckg);
        }
    else
        {
        appRegInfo.iUid = TUid::Null();
        DEBUG_PRINTF2(_L8("Application 0x%x not found."), appUid.iUid);
        }
    CleanupStack::PopAndDestroy(stmt); 
    }

void CScrRequestImpl::GetCaptionAndShortCaptionInfoForLocaleL(TUid aAppUid, TLanguage aLocale, TAppCaption& aShortCaption, TAppCaption& aCaption)
    {
    _LIT(KGetShortCaptionAndCaptionAndIconId,"SELECT CaptionAndIconId, ShortCaption FROM LocalizableAppInfo WHERE AppUid = ? AND Locale = ?;");                                      
    CStatement *stmt1 = iDbHandle->PrepareStatementLC(KGetShortCaptionAndCaptionAndIconId);
    stmt1->BindIntL(1, aAppUid.iUid);
    stmt1->BindIntL(2, aLocale);
    stmt1->ProcessNextRowL();
    aShortCaption.Copy(stmt1->StrColumnL(1));
    TInt captionAndIconId = stmt1->IntColumnL(0);
    if(captionAndIconId)
        {
        _LIT(KGetCaption,"SELECT Caption FROM CaptionAndIconInfo WHERE CaptionAndIconId = ?;");                                      
        CStatement *stmt2 = iDbHandle->PrepareStatementLC(KGetCaption);
        stmt2->BindIntL(1, captionAndIconId);
        stmt2->ProcessNextRowL();
        aCaption.Copy(stmt2->StrColumnL(0));
        CleanupStack::PopAndDestroy(stmt2);
        }
        CleanupStack::PopAndDestroy(stmt1);
    }

void CScrRequestImpl::GetAppPropertiesInfoL(CApplicationRegistrationData& aApplicationRegistration,TUid aAppUid, TLanguage aLanguage)
    {
	DEBUG_PRINTF2(_L8("returning App Properties Info for AppUid %d in the SCR"), aAppUid);
    TLanguage appSupportedLanguage;
    if(GetNearestAppLanguageL(aLanguage, aAppUid, appSupportedLanguage))
        {
        _LIT(KGetAppPropertiesInfo, "SELECT Name, IntValue, StrValue, Locale, IsStr8Bit FROM AppProperties WHERE (AppUid =?) AND (Locale= ? OR Locale= ?)");
        CStatement *stmt = iDbHandle->PrepareStatementLC(KGetAppPropertiesInfo);
        stmt->BindIntL(1, aAppUid.iUid);
        stmt->BindIntL(2, Usif::KNonLocalized);
        stmt->BindIntL(3, (TInt)appSupportedLanguage);
        while(stmt->ProcessNextRowL())
            {
            TPtrC name(stmt->StrColumnL(0));
            CPropertyEntry *entry = GetPropertyEntryL(*stmt, name, 1); // aStartingIndex=1
            DEBUG_PRINTF2(_L("Value read for Property %S "), &name);
            CleanupStack::PushL(entry);
            aApplicationRegistration.iAppPropertiesArray.AppendL(entry);
            CleanupStack::Pop(entry);   // because array is now owner
            } 
        CleanupStack::PopAndDestroy(stmt);    
        }
    else
        {
        DEBUG_PRINTF2(_L8("No Nearest locale found for AppUid %d in the SCR"), aAppUid);
        }    
    }

void CScrRequestImpl::GetApplicationLaunchersSizeL(const RMessage2& aMessage) const
    {
    DEBUG_PRINTF(_L8("Returning the size of the list of application launchers"));
   
    _LIT(KSelectLauncher, "SELECT SoftwareTypeId, LauncherExecutable FROM SoftwareTypes;");
    CStatement* stmt = iDbHandle->PrepareStatementLC(KSelectLauncher);
        
    while(stmt->ProcessNextRowL())
        {
        CLauncherExecutable* launcher = CLauncherExecutable::NewLC();
        launcher->iTypeId = stmt->IntColumnL(0);
        DeleteObjectZ(launcher->iLauncher);
        launcher->iLauncher = stmt->StrColumnL(1).AllocL();
        iLaunchers.AppendL(launcher);
        CleanupStack::Pop(launcher);
        }    
    CleanupStack::PopAndDestroy(stmt);
    
    WriteArraySizeL(aMessage, 1, iLaunchers);
    }

void CScrRequestImpl::GetApplicationLaunchersDataL(const RMessage2& aMessage) const
    {
    DEBUG_PRINTF(_L8("Returning the list of application launchers"));
    WriteArrayDataL(aMessage, 0, iLaunchers);    
    iLaunchers.ResetAndDestroy();
    }


void CScrRequestImpl::GenerateNonNativeAppUidL(const RMessage2& aMessage)
    {    
	DEBUG_PRINTF(_L8("Generating Non Native AppUid"));
    //Get access to the repository instance.
    CScrRepository* scrRepository = CScrRepository::GetRepositoryInstanceL();
    
    //Number of ranges defined in the cenrep file
    TInt rangeCount = scrRepository->AppUidRangeCountL();
        
    TUid rangeBegin = TUid::Null();
    TUid rangeEnd = TUid::Null();
    TUid generatedUid = TUid::Null(); // Used to store final result.
    
    for(TInt counter = 1; counter <= rangeCount; counter++)
        {
        // Retrieve the range. 
        TRAPD(err, scrRepository->GetAppUidRangeL(counter, rangeBegin, rangeEnd));
        
        if(KErrNotFound == err)
            continue;// Incorrect range, try the next one.
        if(rangeBegin.iUid >= rangeEnd.iUid)
            continue; // Incorrect range, try the next one.
        
        _LIT(KSelectAppUids, "SELECT AppUid FROM AppRegistrationInfo WHERE AppUid >= ? AND AppUid <= ? ORDER BY AppUid;");
        
        CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectAppUids);
        stmt->BindIntL(1, rangeBegin.iUid);
        stmt->BindIntL(2, rangeEnd.iUid);
        
        TInt prevUid = rangeBegin.iUid-1;
        while(stmt->ProcessNextRowL())
            {
            TInt currUid(stmt->IntColumnL(0));
            if(currUid > prevUid+1)
                break;
            prevUid = currUid;
            }
        
        if(prevUid != rangeEnd.iUid) //If the range is not full
            {
            generatedUid = TUid::Uid(prevUid+1);
            CleanupStack::PopAndDestroy(1, stmt);
            break;
            }
        
        // Release allocated memories
        CleanupStack::PopAndDestroy(1, stmt);
        }
    
    TPckg<TUid> generatedUidPckg(generatedUid); 
    aMessage.WriteL(0, generatedUidPckg);    
    }

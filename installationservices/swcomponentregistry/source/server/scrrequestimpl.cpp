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
_LIT(KComponentPropertiesTable, "ComponentProperties");
_LIT(KFilePropertiesTable, "FileProperties");

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
		iLogEntries.Append(logRecord);
		CleanupStack::Pop(logRecord); // Ownershipd is transferred
		}
	CleanupStack::PopAndDestroy(4, &componentInfoReader); // componentInfoReader, componentInfoArray, uniqueSwTypeName, globalId
	
	TPckg<TComponentId> componentIdDes(newComponentId);
	aMessage.WriteL(3, componentIdDes);
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
	DEBUG_PRINTF2(_L8("Deleting component(%d)."), componentId);
	
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

TBool CScrRequestImpl::GetIntSoftwareTypeDataForComponentLC(TComponentId aComponentId, const TDesC& aColumnName, TInt& aValue) const
	{
	_LIT(KSelectComponents, "SELECT SoftwareTypeId FROM Components WHERE ComponentId=?;");
	TBool found = EFalse;
	CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectComponents);
	stmt->BindIntL(1, aComponentId);
	if(!stmt->ProcessNextRowL())
		{
		DEBUG_PRINTF2(_L("Component Id (%d) doesn't exist in the SCR database!"), aComponentId);
		User::LeaveIfError(KErrNotFound);
		}
	TInt swTypeId = stmt->IntColumnL(0);
	CleanupStack::PopAndDestroy(stmt);
	
	_LIT(KSelectSoftwareTypes, "SELECT %S FROM SoftwareTypes WHERE SoftwareTypeId=?;");
	HBufC *statementStr = FormatStatementLC(KSelectSoftwareTypes(), aColumnName.Length(), &aColumnName);
	CStatement *stmtSwType = iDbHandle->PrepareStatementLC(*statementStr);
	stmtSwType->BindIntL(1, swTypeId);
	
	if(stmtSwType->ProcessNextRowL())
		{
		aValue = stmtSwType->IntColumnL(0);
		found = ETrue;
		}
	CleanupStack::PopAndDestroy(2, statementStr); // statementStr, stmtSwType
	return found;
	}

TBool CScrRequestImpl::GetInstallerSidForComponentL(TComponentId aComponentId, TSecureId& aSid) const	
	{
	_LIT(KInstallerSecureId, "InstallerSecureId");
	TInt secureIdInt (0);
	TBool found = GetIntSoftwareTypeDataForComponentLC(aComponentId, KInstallerSecureId, secureIdInt);	
	aSid.iId = secureIdInt;
	return found;
	}

TBool CScrRequestImpl::GetExecutionEnvSidForComponentL(TComponentId aComponentId, TSecureId& aSid) const
	{
	_LIT(KExecutionLayerSecureId, "ExecutionLayerSecureId");
	TInt secureIdInt(0);
	TBool found = GetIntSoftwareTypeDataForComponentLC(aComponentId, KExecutionLayerSecureId, secureIdInt);	
	aSid.iId = secureIdInt;
	return found;	
	}

TBool CScrRequestImpl::IsInstallerOrExecutionEnvSidL(TSecureId& aSid) const
	{
	_LIT(KSelectStatement, "SELECT InstallerSecureId FROM SoftwareTypes WHERE InstallerSecureId=? OR ExecutionLayerSecureId=?;");
	CStatement *stmt = iDbHandle->PrepareStatementLC(KSelectStatement);
	stmt->BindIntL(1, aSid);
	stmt->BindIntL(2, aSid);	
	TBool res = ETrue;
	if(!stmt->ProcessNextRowL())
		{
		DEBUG_PRINTF2(_L("%d is not an installer SID"), TUint32(aSid));
		res = EFalse;
		}
	CleanupStack::PopAndDestroy(stmt);
	return res;	
	}

void CScrRequestImpl::GetPluginUidWithComponentIdL(const RMessage2& aMessage) const
	{
	TComponentId componentId = GetComponentIdFromMsgL(aMessage);
	DEBUG_PRINTF2(_L8("Returning the plugin of component(%d)."), componentId);
	_LIT(KSifPluginUid, "SifPluginUid");
	TInt uid (0);
	TBool found = GetIntSoftwareTypeDataForComponentLC(componentId, KSifPluginUid, uid);
	__ASSERT_ALWAYS(found, User::Leave(KErrNotFound));
	
	TPckg<TUint32> uidDes(uid);
	aMessage.WriteL(1, uidDes);
	}

// This function returns whether the SID looked up has been found in the software types table.
TBool CScrRequestImpl::GetSidsForSoftwareTypeL(const HBufC* aSoftwareTypeName, TSecureId& aInstallerSid, TSecureId& aExecutableEnvSid) const
	{
	DEBUG_PRINTF2(_L("Returning SIDs for software type(%S)."), aSoftwareTypeName);
	TBool found = EFalse;
	TUint32 swTypeId = HashCaseSensitiveL(*aSoftwareTypeName);
	
	_LIT(KSelectStatement, "SELECT ExecutionLayerSecureId, InstallerSecureId FROM SoftwareTypes WHERE SoftwareTypeId =?;");
	CStatement* stmt = iDbHandle->PrepareStatementLC(KSelectStatement);
	stmt->BindIntL(1, swTypeId);
	
	if(stmt->ProcessNextRowL())
		{
		aExecutableEnvSid = stmt->IntColumnL(0);
		aInstallerSid = stmt->IntColumnL(1);
		found = ETrue;
		}
	
	CleanupStack::PopAndDestroy(stmt);
	return found;
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

void CScrRequestImpl::GetComponentIdsHavingThePropertiesL(RArray<TComponentId>& aComponentIds, RPointerArray<CPropertyEntry> aProperties, TBool aDoIntersect) const
	{
	// GROUP BY is added at the end to avoid fetching twice components which have the same localizable values for different locales
	_LIT(KFindComponentsFromProperties, "SELECT ComponentId FROM ComponentProperties WHERE Name=? AND %S GROUP BY ComponentId;");
	_LIT(KPropertyIntValue,"IntValue=?");
	_LIT(KPropertyStrValue,"StrValue=?");
	_LIT(KPropertyLocalizedValue,"StrValue=? AND Locale=?");
			
	TInt propCount = aProperties.Count();

	HBufC *statementStr(0);
	RArray<TComponentId> propCompIds;
	CleanupClosePushL(propCompIds);
	
	for(TInt i=0; i<propCount; ++i)
		{
		switch(aProperties[i]->PropertyType())
			{
			case CPropertyEntry::EIntProperty:
				statementStr = FormatStatementLC(KFindComponentsFromProperties, KPropertyIntValue().Length(), &KPropertyIntValue());
				break;
			case CPropertyEntry::EBinaryProperty:
				statementStr = FormatStatementLC(KFindComponentsFromProperties, KPropertyStrValue().Length(), &KPropertyStrValue());
				break;
			case CPropertyEntry::ELocalizedProperty:
				{
				CLocalizablePropertyEntry *localizedProp = static_cast<CLocalizablePropertyEntry *>(aProperties[i]);
				if (localizedProp->LocaleL() == KUnspecifiedLocale) // If the locale was not specified, then we match across all locales				
					statementStr = FormatStatementLC(KFindComponentsFromProperties, KPropertyStrValue().Length(), &KPropertyStrValue());
				else // Otherwise we match for a specific locale
					statementStr = FormatStatementLC(KFindComponentsFromProperties, KPropertyLocalizedValue().Length(), &KPropertyLocalizedValue());
				}
				break;				
			default:
				DEBUG_PRINTF(_L8("The property type couldn't be recognized."));
				User::Leave(KErrAbort);	
			}
				
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
		
		CleanupStack::PopAndDestroy(2, statementStr); // stmt, statementStr
		
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
		GetComponentIdsHavingThePropertiesL(aComponentFilterSuperset, aFilter.iPropertyList, doIntersect);
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
	if(aFilter.iSetFlag & (CComponentFilter::EName | CComponentFilter::EVendor))
		{ // If a name or vendor is specified in the filter, the locale is ignored
		  // and the provided names are retrieved from the ComponentLocalizables table.
		CStatement *stmt = CreateStatementObjectForComponentLocalizablesLC(*aFilter.iName, *aFilter.iVendor, aFilter.iSetFlag, aComponentId);
		TBool res = stmt->ProcessNextRowL();
		// If the name and the vendor are not found, it means that there is a defect in this class, the filter shouldn't have returned the component in the first place		
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
	aEntry = GetNextComponentEntryL(*aStmt, *aFilter, locale, aSubsessionContext);
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
	for(TInt i=0; i<maxArraySize; ++i)
		{
		CComponentEntry *entry = GetNextComponentEntryL(*aStmt, *aFilter, locale, aSubsessionContext);
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

TBool CScrRequestImpl::IsSoftwareTypeExistingL(TUint32 aSwTypeId, TUint32 aSifPluginUid, TUint32 aInstallerSecureId, TUint32 aExecutionLayerSecureId, const RPointerArray<HBufC>& aMimeTypesArray, const RPointerArray<CLocalizedSoftwareTypeName>& aLocalizedNamesArray)
	{
	_LIT(KSelectSoftwareType, "SELECT SifPluginUid,InstallerSecureId,ExecutionLayerSecureId FROM SoftwareTypes WHERE SoftwareTypeId=?;");
	CStatement* stmt = iDbHandle->PrepareStatementLC(KSelectSoftwareType);
	stmt->BindIntL(1, aSwTypeId);
	if(!stmt->ProcessNextRowL())
		{
		DEBUG_PRINTF2(_L8("IsSoftwareTypeExistingL: Software Type Id (%d) doesn't exist in the SCR."), aSwTypeId);
		CleanupStack::PopAndDestroy(stmt);
		return EFalse;
		}
	TBool uidsNotEqual = aSifPluginUid != stmt->IntColumnL(0) ||
						 aInstallerSecureId != stmt->IntColumnL(1) ||
						 aExecutionLayerSecureId != stmt->IntColumnL(2);
	if(uidsNotEqual)
		{
		DEBUG_PRINTF(_L8("IsSoftwareTypeExistingL: One of the UIDs is different from the one in the database."));
		CleanupStack::PopAndDestroy(stmt);
		return EFalse;
		}
	CleanupStack::PopAndDestroy(stmt);
	
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
	
	// Slot-0 contains Unique Software Type Name
	HBufC *uniqueSwTypeName = ReadDescLC(aMessage, 0);
	TUint32 swTypeId = HashCaseSensitiveL(*uniqueSwTypeName);
	
	// Slot-1 contains the concatenated values of SifPluginUid, InstallerSecureId and ExecutionLayerSecureId in turn.
	HBufC8 *uidString = ReadDesc8LC(aMessage, 1);
	TUint32 sifPluginUid (0);
	TUint32 installerSecureId (0);
	TUint32 executionLayerSecureId (0);
	ParseUidHexStringL(*uidString, sifPluginUid, installerSecureId, executionLayerSecureId);
	CleanupStack::PopAndDestroy(uidString);
	
	// Slot-2 contains the list of MIME types
	RIpcReadStream mimeTypesReader;
	CleanupClosePushL(mimeTypesReader);
	mimeTypesReader.Open(aMessage, 2);
				
	RPointerArray<HBufC> mimeTypesArray;
	CleanupResetAndDestroyPushL(mimeTypesArray);
	InternalizePointersArrayL(mimeTypesArray, mimeTypesReader);
	
	// Slot-3 contains Localized Software Type Names
	RIpcReadStream localizedNamesReader;
	CleanupClosePushL(localizedNamesReader);
	localizedNamesReader.Open(aMessage, 3);
				
	RPointerArray<CLocalizedSoftwareTypeName> localizedNamesArray;
	CleanupResetAndDestroyPushL(localizedNamesArray);
	InternalizePointersArrayL(localizedNamesArray, localizedNamesReader);
	
	if (IsSoftwareTypeExistingL(swTypeId, sifPluginUid, installerSecureId, executionLayerSecureId, mimeTypesArray, localizedNamesArray))
		{ // If the software type exists, do nothing and return;	
		CleanupStack::PopAndDestroy(5, uniqueSwTypeName); // uniqueSwTypeName, mimeTypesReader, mimeTypesArray, localizedNamesReader, localizedNamesArray
		return; 
		}
	
	// First, insert the main record to SoftwareTypes table
	_LIT(KInsertSwType, "INSERT INTO SoftwareTypes(SoftwareTypeId,SifPluginUid,InstallerSecureId,ExecutionLayerSecureId) VALUES(?,?,?,?);");
	TInt numberOfValuesSwType = 4;
	ExecuteStatementL(KInsertSwType(), numberOfValuesSwType, EValueInteger, swTypeId, EValueInteger, sifPluginUid, EValueInteger, installerSecureId, EValueInteger, executionLayerSecureId);
	
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
	CleanupStack::PopAndDestroy(5, uniqueSwTypeName); // uniqueSwTypeName, mimeTypesReader, mimeTypesArray, localizedNamesReader, localizedNamesArray
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

TBool CScrRequestImpl::GetIsComponentOrphanedL(TComponentId aComponentId) const	
	{
	_LIT(KComponentSoftwareTypeId, "SELECT SoftwareTypeId FROM Components WHERE ComponentId=?;");	
	
	CStatement *stmt = iDbHandle->PrepareStatementLC(KComponentSoftwareTypeId);
	stmt->BindIntL(1, aComponentId);
		
	if(!stmt->ProcessNextRowL())
		{ 
		DEBUG_PRINTF2(_L("Component Id (%d) couldn't be found!"), aComponentId);
		User::Leave(KErrNotFound);
		}
	
	TUint32 swTypeId = stmt->IntColumnL(0);
	CleanupStack::PopAndDestroy(stmt);
	
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
				
	TPckg<TBool> isCompPresent(result);	
	aMessage.WriteL(1, isCompPresent);	
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

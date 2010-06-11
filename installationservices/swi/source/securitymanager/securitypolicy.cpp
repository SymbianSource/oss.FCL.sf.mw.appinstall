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
*
*/

#define __REFERENCE_CAPABILITY_NAMES__
#define __INCLUDE_CAPABILITY_NAMES__
#include <e32capability.h>

#include "securitypolicy.h"

#include <f32file.h>
#include <s32file.h>
#include <swi/sisinstallerrors.h>
#include <caf/caf.h>

#include "log.h"

const TInt KDefaultApplicationShutdownTimeout = 10000000; //10 seconds
const TInt KDefaultRunWaitTimeout = 180000000; //180 seconds

using namespace Swi;

CPathReplaceEntry::CPathReplaceEntry(HBufC* aFindPath, HBufC* aReplacePath)
/**
	Record the supplied find and replace paths.
	This object takes ownership of these strings
	and deletes them when it is destroyed.
	
	@param	aFindPath		Drive-qualified directory to match on target LHS.
	@param	aReplacePath	Drive-qualified replacement directory.
 */
:	iFindPath(aFindPath),
	iReplacePath(aReplacePath)
	{
	// empty.
	}

CPathReplaceEntry::~CPathReplaceEntry()
/**
	Free the find and replace paths which are owned
	by this object.
 */
	{
	delete iFindPath;
	delete iReplacePath;
	}
	

EXPORT_C CSecurityPolicy* CSecurityPolicy::GetSecurityPolicyL()
	{
	CSecurityPolicy* self = static_cast<CSecurityPolicy*>(Dll::Tls());
	if (NULL == self)
		{
		self = new(ELeave)CSecurityPolicy();
		CleanupStack::PushL(self);
		self->ConstructL();
		Dll::SetTls((void*)self);
		CleanupStack::Pop(self);
		}
	return self;
	}
	
EXPORT_C CSecurityPolicy::~CSecurityPolicy()
	{
	iCustomOids.ResetAndDestroy();
	iReplEntries.ResetAndDestroy();
	iAlternativeCodeSigningOids.ResetAndDestroy();
	}
	
EXPORT_C void CSecurityPolicy::ReleaseResource()
	{
	CSecurityPolicy* secPolicy = static_cast<CSecurityPolicy*>(Dll::Tls());
	delete secPolicy;
	secPolicy = 0;
	Dll::SetTls(NULL);
	}
	
EXPORT_C TBool CSecurityPolicy::AllowUnsigned() const
	{
	return iAllowUnsigned;
	}
	
EXPORT_C TBool CSecurityPolicy::MandateCodesigningExtension() const
	{
	return iMandateCodesigningExtension;
	}
	
EXPORT_C TBool CSecurityPolicy::MandateCertificatePolicies() const
	{
	return iMandateCertificatePolicies;
	}

EXPORT_C const RPointerArray<HBufC> CSecurityPolicy::Oids() const
	{
	return iCustomOids;
	}

EXPORT_C TBool CSecurityPolicy::DrmEnabled() const
	{
	return iDrmEnabled;
	}

EXPORT_C TInt CSecurityPolicy::DrmIntent() const
	{
	return iDrmIntent;
	}

EXPORT_C TBool CSecurityPolicy::OcspEnabled() const
	{
	return iOcspEnabled;
	}
	
EXPORT_C TBool CSecurityPolicy::OcspMandatory() const
	{
	return iOcspMandatory;
	}	

	
EXPORT_C TBool CSecurityPolicy::AllowGrantUserCaps() const
	{
	return iAllowGrantUserCaps;
	}

EXPORT_C TCapabilitySet CSecurityPolicy::UserCapabilities() const
	{
	return iUserCapabilities;
	}

EXPORT_C TBool CSecurityPolicy::AllowOrphanedOverwrite() const
	{
	return iAllowOrphanedOverwrite;
	}

EXPORT_C TBool CSecurityPolicy::AllowPackagePropagate() const
	{
	return iAllowPackagePropagate;
	}

EXPORT_C TInt CSecurityPolicy::ApplicationShutdownTimeout() const
	{
	return iApplicationShutdownTimeout;
	}

EXPORT_C TInt CSecurityPolicy::RunWaitTimeout() const
	{
	return iRunWaitTimeout;
	}

EXPORT_C TBool CSecurityPolicy::SISCompatibleIfNoTargetDevices() const
	{
	return iSISCompatibleIfNoTargetDevices;
	}

EXPORT_C TBool CSecurityPolicy::AllowRunOnInstallUninstall() const
 	{
 	return iAllowRunOnInstallUninstall;
 	}
	
EXPORT_C TUint32 CSecurityPolicy::OcspHttpHeaderFilter() const
/**
	Accessor function returns which ECOM plugin should be used
	to specify the OCSP HTTP filter.
	
	@return					ECOM plugin which is used to filter
							OCSP HTTP request.  This is specified with the
							"OcspHttpHeaderFilter" (KOcspHttpHeaderFilter)
							field.  Zero means no filter was specified.
 */
	{
	return iOcspHttpHeaderFilter;
	}

	/**
	 * This function returns a boolean indicating whether the files in this
	 * package should be deleted on uninstall - this is only applicable for
	 * packages installed from a stub sis file, where the actual target files
	 * are already in place.  In other cases installed files will always be
	 * deleted on uninstall.
	 *
	 * @return ETrue if installed files should be deleted on uninstall
	 */

EXPORT_C TBool CSecurityPolicy::DeletePreinstalledFilesOnUninstall() const
	{
	return iDeletePreinstalledFilesOnUninstall;
	}

EXPORT_C const TDesC& CSecurityPolicy::PhoneTsyName() const
 	{
 	return iPhoneTsyName;
 	}
	
EXPORT_C TBool CSecurityPolicy::AllowOverwriteOnRestore() const
 	{
 	return iAllowOverwriteOnRestore;
 	}	
	
CSecurityPolicy::CSecurityPolicy()
: iDrmEnabled(ETrue), iDrmIntent(ContentAccess::EExecute),
iApplicationShutdownTimeout(KDefaultApplicationShutdownTimeout),
iRunWaitTimeout(KDefaultRunWaitTimeout),
iSISCompatibleIfNoTargetDevices(ETrue),
  iAllowRunOnInstallUninstall(ETrue),
  iAllowProtectedOrphanOverwrite(ETrue),
  iRemoveOnlyWithLastDependent(ETrue)
	{
	}

TBool CSecurityPolicy::ReadLineL(const TDesC8& aBuffer, TInt& aPos, TPtrC8& aLine) const
	{
	TBool endOfBuffer = EFalse;	
	aLine.Set(NULL, 0);

	TInt bufferLength = aBuffer.Length();	
	__ASSERT_ALWAYS(aPos >=0 && aPos <= bufferLength, User::Leave(KErrArgument));
	
	// skip blank lines
	while (aPos < bufferLength) 
		{
		TChar  c = aBuffer[aPos];
		if (c != '\r' && c != '\n')
			{
			break;
			}
		aPos++;
		}

	// find the position of the next delimter		
	TInt endPos = aPos;	
	while (endPos < bufferLength)
		{
		TChar c = aBuffer[endPos];
		
		if (c == '\n' || c == '\r') 
			{
			break;
			}	
		endPos++;
		}
		
	if (endPos != aPos)	
		{
		TInt tokenLen = endPos - aPos;
		aLine.Set(&aBuffer[aPos], tokenLen);
		}
	else 
		{
		return ETrue; // End of buffer
		}			
		
	aPos = endPos;
	return endOfBuffer;
	}
 
void CSecurityPolicy::AddUserCapability(const TDesC8& aCapabilityName)
	{
	TBool capabilityMatched = EFalse;  

	for (TInt i=0; i < ECapability_Limit && !capabilityMatched; ++i)
		{
		TInt pos=0;
	    // PREQ274
		// DRM Capability shall not be user grantable. ever. full stop.
        // PDEF110561
		// Because DRM capability is not user-grantable, and a TCB application can escalate privileges to gain DRM, TCB also cannot be user-grantable. 
		//DEF116513
		//AllFiles, CommDD, MultimediaDD, DiskAdmin, NetworkControl shall not be user grantable (Refer symbian signed website)
		switch (i)
			{
			case ECapabilityTCB:
			case ECapabilityDRM:
			case ECapabilityCommDD:
			case ECapabilityMultimediaDD:
			case ECapabilityDiskAdmin:
			case ECapabilityNetworkControl:
			case ECapabilityAllFiles: continue;
 			}
		const char* const capabilityName=CapabilityNames[i];
		// Compare descriptor to capability name.
		while (ETrue)
			{
			TInt length = aCapabilityName.Length();
			if (capabilityName[pos]==0 && pos == length)
				{
				// Reached end of both concurrently, and no mismatches found
				
				   iUserCapabilities.AddCapability(TCapability(i));
				   capabilityMatched = ETrue;
	    		   break;
				}
			else if (pos >= length || capabilityName[pos]==0)	
				{
				// Reached end of one or the other
				break;
				}
			
			// Case insensitive compare of the characters at current positions 
			TChar c(capabilityName[pos]);
			TChar c2(aCapabilityName[pos]);
			c.LowerCase();
			c2.LowerCase();
			
			if (c != c2)
				{
				break;// Different so stop the compare
				}
			
			++pos;
			}
		}
	}

void CSecurityPolicy::AddAlternativeCodeSigningOidL(const TDesC8& aAlternativeOid)
	{
	TInt count = iAlternativeCodeSigningOids.Count();
	TBool found = EFalse;
	TInt length = aAlternativeOid.Length();
	
	HBufC* oid = HBufC::NewMaxLC(length);
	TPtr oidBuf(oid->Des());
	oidBuf.Copy(aAlternativeOid);
		
	for (TInt i=0; i < count; i++)
		{
		const TDesC& currentOid = *iAlternativeCodeSigningOids[i];
		
		if(!currentOid.CompareF(oidBuf))
			{
			found = ETrue;
			}
		}
		
	if(!found)
		{
		// The oid is not in the list
		User::LeaveIfError(iAlternativeCodeSigningOids.Append(oid));
		CleanupStack::Pop(oid);	
		}
	else
		{
		CleanupStack::PopAndDestroy(oid);
		}
	}
	
void CSecurityPolicy::ReadReplacementPathsL(const TDesC8& aLine)
	{
	// line format is find-path|(replace-path|)*
	// where find-path is the source path without a drive letter
	// or a colon, but with a leading backslash.  replace-path
	// includes them both.
	// 
	// Both find-path and replace-path must have a trailing backslash,
	// to delimit the directory; and the | separator is required
	// including for the last replace path.
	
	TInt delim = aLine.Locate('|');
	__ASSERT_DEBUG(delim != KErrNotFound, Panic(ERRPFindPathNoDelim));
	
	const TPtrC8 findPath = aLine.Left(delim);
	__ASSERT_DEBUG(delim >= 3, Panic(ERRPFindTooShort));
	__ASSERT_DEBUG(findPath[0] == '\\', Panic(ERRPFindNoLeadBackslash));
	__ASSERT_DEBUG(findPath[findPath.Length() - 1] == '\\', Panic(ERRPFindNoTrailBackslash));
	
	TInt findLen = findPath.Length();
	
	// replace-path format is "a:\replace-dir1\|b:\replace-dir2\|"
	// drive letter must be lower case
	for (
		TPtrC8 remainder(aLine.Mid(delim + 1));
		remainder.Length() > 0;
		remainder.Set(remainder.Mid(delim + 1)) )
		{
		delim = remainder.Locate('|');
		// "@:\" or "@:\<rep-dir>\"
		__ASSERT_DEBUG(delim == 3 || delim >= 5, Panic(ERRPRepTooShort));
		
		// validate replace dir format
		TPtrC8 repPath = remainder.Left(delim);
		__ASSERT_DEBUG(
			repPath[0] == '!' || (repPath[0] >= 'a' && repPath[0] <= 'z') || repPath[0] == '$',
			Panic(ERRPRepInvalidDrive));
		__ASSERT_DEBUG(repPath.Mid(1,2) == _L8(":\\"), Panic(ERRPNoRepColonBackslash));
		__ASSERT_DEBUG(repPath[repPath.Length() - 1] == '\\', Panic(ERRPRepNoTrailBackslash));

		// create find path - @:find-path for matching
		HBufC* find = HBufC::NewMaxLC(2 + findLen);
		TPtr findDes = find->Des();
		findDes[0] = repPath[0];
		findDes[1] = L':';
		findDes.MidTPtr(2).Copy(findPath);
		
		// create replace path
		HBufC* replace = HBufC::NewLC(repPath.Length());
		replace->Des().Copy(repPath);

		DEBUG_PRINTF(_L8("Security Manager - Security Policy, Path Replacement Pair:"));
		DEBUG_PRINTF2(_L("Find: %S"), find);
		DEBUG_PRINTF2(_L("Replace: %S"), replace);

		// create an entry to combine them
		CPathReplaceEntry* ppre = new(ELeave) CPathReplaceEntry(find, replace);
		CleanupStack::Pop(2, find);
		CleanupStack::PushL(ppre);
		iReplEntries.AppendL(ppre);
		CleanupStack::Pop(ppre);
		}
	}

EXPORT_C HBufC* CSecurityPolicy::ResolveTargetFileNameL(
	const TDesC& aMmpName, TText aDrive) const
/**
	Returns a copy of the supplied target filename allocated
	on the heap.
	
	The returned filename resolves '!' drive letters with the
	supplied drive and resolves '$' drive letter with system drive.
	If the target filename's left side has a replacement path in the 
	security policy then the returned filename contains the substituted
    text.
	
	@param	aMmpName		Target filename, as specified in the
							MMP file.  This may specify '!' instead
							of a drive letter.
	@param	aDrive			User selected drive.  This is used as the
							destination drive if the MMP name uses '!'.
	@return					Heap-based filename where the drive letter
							is resolved and any replacement path is
							substituted.
 */
	{
	const TInt mmpLen = aMmpName.Length();
	if (mmpLen <= 1)
		return aMmpName.AllocL();
	
	TInt repCount = iReplEntries.Count();
	TText destDrive = aMmpName[0];
	if (destDrive == '!')
		destDrive = aDrive;
	
	if (destDrive == '$')
		{
		TDriveUnit sysDrive (RFs::GetSystemDrive());
		TDriveName driveName = sysDrive.Name();
		destDrive = driveName[0];
		}
	
	destDrive = User::LowerCase(destDrive);

	const TPtrC mmp1 = aMmpName.Mid(1);
	// this loop must iterate from the first to the last
	// replacement texts in order to find the first (not
	// necesarily the best) match.
	for (TInt i = 0; i < repCount; ++i)
		{
		const CPathReplaceEntry* ppre = iReplEntries[i];
		
		// does the replacement drive match the target dest drive?
		TText repDrive = (*ppre->iFindPath)[0];
		if(repDrive == '$')
			{
			TDriveUnit sysDrive (RFs::GetSystemDrive());
			TDriveName driveName = sysDrive.Name();
			repDrive = User::LowerCase(driveName[0]);
			}
			
		if (repDrive != '!' && repDrive != destDrive)
			continue;
		
		// look for match at start
		TInt findLen = ppre->iFindPath->Length();
		if (findLen > mmpLen)
			continue;
		
		TInt findPos = mmp1.Left(findLen - 1).MatchF(ppre->iFindPath->Mid(1));
		if (findPos != 0)
			continue;
		
		// there is a match so work out new length
		TInt curLen = mmpLen;
		TInt addedLen = ppre->iReplacePath->Length();
		TInt newLen = curLen + (addedLen - findLen);
		if (newLen > KMaxFileName)
			User::Leave(KErrSISStringInvalidLength);
		
		HBufC* newName = HBufC::NewL(newLen);
		TPtr newNameDes = newName->Des();
		newNameDes.Copy(*ppre->iReplacePath);
		newNameDes[0] = destDrive;	// may be ! in replace path
		newNameDes.Append(aMmpName.Mid(findLen));
		return newName;
		}
	
	// no substitutions made so return simple copy.
	// The drive name is only replaced if it is wild,
	// so it matches that in the controller.
	
	HBufC* newTarget = aMmpName.AllocL();
	if (((*newTarget)[0] == '!') || ((*newTarget)[0] == '$'))
		{
		TPtr newTargetPtr = newTarget->Des();
		newTargetPtr[0] = destDrive;
		}
		
	return newTarget;
	}

EXPORT_C HBufC* CSecurityPolicy::ResolveTargetFileNameLC(
	const TDesC& aMmpName, TText aDrive) const
	{
	HBufC* newTarget = ResolveTargetFileNameL(aMmpName, aDrive);
	CleanupStack::PushL(newTarget);
	return newTarget;
	}

EXPORT_C TBool CSecurityPolicy::AllowProtectedOrphanOverwrite() const
	{
	return iAllowProtectedOrphanOverwrite;
	}

EXPORT_C const RPointerArray<HBufC> CSecurityPolicy::AlternativeCodeSigningOid() const
	{
	return iAlternativeCodeSigningOids;
	}

EXPORT_C TBool CSecurityPolicy::RemoveOnlyWithLastDependent() const
	{
	return iRemoveOnlyWithLastDependent;
	}
	
void CSecurityPolicy::ConstructL()
	{
	DEBUG_PRINTF(_L8("Security Manager - Reading security policy."));
	
	_LIT8(KFalse, "false");
	
	// Read the policy file into a buffer
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	RFile policyFile;
	// EFileShareReadersOnly is used since SWI Daemon holds a handle on the file to prevent exclusive locks on it
	User::LeaveIfError(policyFile.Open(fs, KSwisPolicyFile, EFileShareReadersOnly | EFileRead));
		
	CleanupClosePushL(policyFile);

	TInt size;
	policyFile.Size(size);
	CleanupStack::PopAndDestroy(&policyFile);

	HBufC8* policyBuffer = HBufC8::NewLC(size);
	TPtr8 p(policyBuffer->Des());
	p.SetLength(size);

	RFileReadStream stream;
	User::LeaveIfError(stream.Open(fs, KSwisPolicyFile, EFileShareReadersOnly| EFileStream));
	CleanupClosePushL(stream);
	stream.ReadL(p, size);
	CleanupStack::PopAndDestroy(&stream);		
				
	TInt readPos = 0;
	TPtrC8 currentLine;
	while (!ReadLineL(*policyBuffer, readPos, currentLine))
		{
		TLex8 lex(currentLine);

		lex.SkipSpaceAndMark() ; 		// move to end of character token
		lex.SkipCharacters();
				
		if (lex.TokenLength() == 0) // if valid potential token
			{
			User::Leave(KErrPolicyFileCorrupt);
			}		
		TPtrC8 key = lex.MarkedToken(); 
		
		lex.SkipSpaceAndMark(); 		// move to end of character token
		lex.SkipCharacters();
		
		if (lex.TokenLength() == 0) // if valid potential token
			{
			User::Leave(KErrPolicyFileCorrupt);
			}		
		/*TPtrC8 separator =*/ lex.MarkedToken(); 

		lex.SkipSpace(); 		// move to end of character token

		// Get the value
		TPtrC8 value = lex.Remainder(); 	
			
		TBool valueBool = (value.Compare(KFalse) != 0);
				
		if (key.Compare(KAllowUnsigned) == 0)
			{
			DEBUG_PRINTF2(_L8("Security Manager - Security Policy, Allow Unsigned: %d."), valueBool);
			iAllowUnsigned = valueBool;
			}
		else if (key.Compare(KMandatePolicies) == 0)
			{
			DEBUG_PRINTF2(_L8("Security Manager - Security Policy, Mandate Certificate Policies: %d."), valueBool);
			iMandateCertificatePolicies = valueBool;
			}
		else if (key.Compare(KMandateCodeSigningExtension) == 0)
			{
			DEBUG_PRINTF2(_L8("Security Manager - Security Policy, Mandate Code Signing Extension: %d."), valueBool);
			iMandateCodesigningExtension = valueBool;
			}
		else if (key.Compare(KOcspMandatory) == 0)
			{
			DEBUG_PRINTF2(_L8("Security Manager - Security Policy, Mandate OCSP: %d."), valueBool);
			iOcspMandatory = valueBool;
			}
		else if (key.Compare(KOcspEnabled) == 0)
			{
			DEBUG_PRINTF2(_L8("Security Manager - Security Policy, Enable OCSP: %d."), valueBool);
			iOcspEnabled = valueBool;
			}
		else if (key.Compare(KAllowGrantUserCaps) == 0)
			{
			DEBUG_PRINTF2(_L8("Security Manager - Security Policy, Allow User Grantable Caps: %d."), valueBool);
			iAllowGrantUserCaps = valueBool;
			}
		else if (key.Compare(KDRMEnabled) == 0)
			{
			DEBUG_PRINTF2(_L8("Security Manager - Security Policy, CAF Enabled: %d."), valueBool);
			iDrmEnabled = valueBool;
			}
		else if (key.Compare(KDRMIntent) == 0)
			{
			TLex8 lexValue(value);
			User::LeaveIfError(lexValue.Val(iDrmIntent));
			DEBUG_PRINTF2(_L8("Security Manager - Security Policy, DRM intent: %d."), iDrmIntent);
			}
		else if (key.Compare(KAllowOrphanedOverwrite) == 0)
			{
			DEBUG_PRINTF2(_L8("Security Manager - Security Policy, Allow Orphaned Overwrite: %d."), valueBool);
			iAllowOrphanedOverwrite = valueBool;
			}
		else if (key.Compare(KUserCapabilities) == 0)
			{
			TLex8 lexer(value);
			TPtrC8 token(lexer.NextToken());
			
			while (token.Size())
				{
				DEBUG_PRINTF2(_L8("Security Manager - Security Policy, User Grantable Capability: %S."), &token);
				AddUserCapability(token);
				token.Set(lexer.NextToken());
				}
			}
		else if (key.Compare(KOid) == 0)
			{
			DEBUG_PRINTF2(_L8("Security Manager - Security Policy, Custom Mandatory Extension: %S."), &value);
			HBufC* oid = HBufC::NewMaxLC(value.Length());
			TPtr oidBuf(oid->Des());
			oidBuf.Copy(value);
			User::LeaveIfError(iCustomOids.Append(oid));
			CleanupStack::Pop(oid);
			}
		else if(key.Compare(KAllowPackagePropagate) == 0)
			{
			DEBUG_PRINTF2(_L8("Security Manager - Security Policy, Allow Package Propagation: %d."), valueBool);
			iAllowPackagePropagate = valueBool;
			}
		else if (key.Compare(KApplicationShutdownTimeout) == 0)
			{
			TLex8 lexValue(value);
			// The timeout in the policy file is in seconds; convert
			// to microseconds for usage with RTimers.
			TInt timeoutSeconds;
			User::LeaveIfError(lexValue.Val(timeoutSeconds));
			iApplicationShutdownTimeout = timeoutSeconds * 1000000;
			DEBUG_PRINTF2(_L8("Security Manager - Security Policy, Application Shutdown Timeout: %d."), timeoutSeconds);
			}
		else if (key.Compare(KRunWaitTimeout) == 0)
			{
			TLex8 lexValue(value);
			// The timeout in the policy file is in seconds; convert
			// to microseconds for usage with RTimers.
			TInt timeoutSeconds;
			User::LeaveIfError(lexValue.Val(timeoutSeconds));
			iRunWaitTimeout = timeoutSeconds * 1000000;
			DEBUG_PRINTF2(_L8("Security Manager - Security Policy, RunWait Timeout: %d."), timeoutSeconds);
			}
		else if (key.Compare(KSISCompatibleIfNoTargetDevices) == 0)
			{
			DEBUG_PRINTF2(_L8("Security Manager - Security Policy, Compatible if no target devices specified: %d."), valueBool);
			iSISCompatibleIfNoTargetDevices = valueBool;
			}
		else if (key.Compare(KAllowRunOnInstallUninstall) == 0)
 			{
 			DEBUG_PRINTF2(_L8("Security Manager - Security Policy, Allow Run On Install: %d."), valueBool);
 			iAllowRunOnInstallUninstall = valueBool;	
 			}
		else if (key == KReplacePath)
			{
			ReadReplacementPathsL(TLex8(value).Remainder());
			}
		else if (key == KOcspHttpHeaderFilter)
			{
			TUint32 uidValue;
			User::LeaveIfError(TLex8(value).Val(uidValue, EHex));
			iOcspHttpHeaderFilter = uidValue;
			DEBUG_PRINTF2(_L8("Security Manager - Security Policy, OCSP Header Filter: 0x%08x."), uidValue);
			}
		else if (key.Compare(KDeletePreinstalledFilesOnUninstall) == 0)
			{
			DEBUG_PRINTF2(_L8("Security Manager - Security Policy, Preinstalled Delete: %d."), valueBool);
			iDeletePreinstalledFilesOnUninstall = valueBool;	
			}
		else if (key.Compare(KAllowProtectedOrphanOverwrite) == 0)
			{
			iAllowProtectedOrphanOverwrite = valueBool;
			}
		else if (key.Compare(KAlternativeCodeSigningOid)==0)
			{
			TLex8 lexer(value);
			TPtrC8 token(lexer.NextToken());
			
			while (token.Size())
				{
				DEBUG_PRINTF2(_L8("Security Manager - Security Policy, Alternative Code Signing OID: %S."), &token);
				AddAlternativeCodeSigningOidL(token);
				token.Set(lexer.NextToken());
				}		
			}
		else if (key.Compare(KRemoveOnlyWithLastDependent) == 0)
			{
			DEBUG_PRINTF2(_L8("Security Manager - Security Policy, Remove Dependency Only Last Dependent: %d."), valueBool);
			iRemoveOnlyWithLastDependent = valueBool;
			}		
		else if (key.Compare(KPhoneTsyName)==0)
			{
			DEBUG_PRINTF2(_L8("Security Manager - Security Policy, PhoneTsyName: %S."), &value);
			iPhoneTsyName.Copy(value);
			}
		else if (key.Compare(KAllowOverwriteOnRestore) == 0)
			{
			DEBUG_PRINTF2(_L8("Security Manager - Security Policy, AllowOverwriteOnRestore: %d."), valueBool);
			iAllowOverwriteOnRestore = valueBool;
			}			
		else if (key.Compare(KMaxNumOfLogEntries) == 0)
			{
			TLex8 lexValue(value);
			User::LeaveIfError(lexValue.Val(iMaxNumOfLogEntries));
			DEBUG_PRINTF2(_L8("Security Manager - Security Policy, MaxNoOfLogEntries : %d."), iMaxNumOfLogEntries);
			}	
		}
	
	CleanupStack::PopAndDestroy(2, &fs); // policyBuffer, fs
	}

#ifdef _DEBUG

void CSecurityPolicy::Panic(CSecurityPolicy::TPanic aPanic)
/**
	Halt the current thread with panic category "SWIPOLICY"
	and the supplied reason.
	
	@param	aPanic			Panic reason.
 */
	{
	_LIT(KPanicCat, "SWIPOLICY");
	User::Panic(KPanicCat, aPanic);
	}

#endif

// -------- thread-static instance --------

EXPORT_C RSecPolHandle::RSecPolHandle()
/**
	Initializes this object as unopened.
 */
:	iSecPol(0)
	{
	// empty.
	}

EXPORT_C void RSecPolHandle::OpenL()
/**
	Attempts to open this object by getting access to
	an instance of CSecurityPolicy.

	@see OpenLC
 */
	{
	__ASSERT_DEBUG(iSecPol == 0, Panic(EOpnAlreadyOpen));

	iSecPol = static_cast<const CSecurityPolicy*>(Dll::Tls());
	if (iSecPol == 0)
		{
		iSecPol = CSecurityPolicy::GetSecurityPolicyL();
		Dll::SetTls((void*)iSecPol);
		}
	
	++iSecPol->iRefCount;
	}

EXPORT_C void RSecPolHandle::OpenLC()
/**
	Same as OpenL, except this handle object is placed
	on the cleanup stack.

	@see OpenL
 */
	{
	OpenL();
	CleanupClosePushL(*this);
	}

EXPORT_C void RSecPolHandle::Close()
/**
	Decrements the reference count on this handle's
	CSecurityPolicy object.  OpenL or OpenLC must be
	called before this object can be used again.

	It is safe to call this function if the handle was
	not successfully opened.
 */
	{
	if (iSecPol == 0)
		return;
	
	if (--iSecPol->iRefCount == 0)
		{
		delete iSecPol;
		Dll::SetTls(0);
		}
	
	iSecPol = 0;
	}

#ifdef _DEBUG

void RSecPolHandle::Panic(RSecPolHandle::TPanic aPanic)
/**
	Halt the current thread with panic category "SWIPOLHND"
	and the supplied reason.
	
	@param	aPanic			Panic reason.
 */
	{
	_LIT(KPanicCat, "SWIPOLHND");
	User::Panic(KPanicCat, aPanic);
	}

#endif

EXPORT_C TInt CSecurityPolicy::MaxNumOfLogEntries() const
	{
	return iMaxNumOfLogEntries;
	}
	

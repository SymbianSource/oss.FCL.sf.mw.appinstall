/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Definition of the installation error codes
* The code in SisRegistryUtil::RomBasedPackageL() relies on the range: 
* KErrSISNotEnoughSpaceToInstall < x < KErrSISFieldIdMissing.  Error codes 
* should not be moved from this range without also updating the relevant part 
* of this method. By the same token any new SIS parsing related errors that 
* are introduced should be added to the method.
*
*/


/**
 @file 
 @released
 @publishedPartner
*/

#ifndef __SISINSTALLERRORS_H__
#define __SISINSTALLERRORS_H__

#include <e32std.h>

/**
*
* Field Id was not found during SIS file parsing.
*
*/
const TInt KErrSISFieldIdMissing = -10100;
/**
*
* Field Length was not found during SIS file parsing.
*
*/
const TInt KErrSISFieldLengthMissing = -10101;
/**
* An incorrect Field Length was encountered during field parsing in a SIS file.
* This might happen in both corrupted SIS files, and potentially malicious SIS files 
* trying to cause panic conditions via large allocations.
*
*/
const TInt KErrSISFieldLengthInvalid = -10102;
/**
* A specific case of KErrSISFieldLengthInvalid for SISString. 
* In addition to the cases documented for KErrSISFieldLengthMissing, 
* it can happen whenever a string with an odd length is specified (which is invalid on a UCS2 field).
*/
const TInt KErrSISStringInvalidLength = -10103;
/**
* SIS Controller was not found in a signed SIS file.
*
*/
const TInt KErrSISSignedControllerSISControllerMissing = -10104;
/**
* SISInfo field was missing in a SIS controller.
*
*/
const TInt KErrSISControllerSISInfoMissing = -10105;
/**
* SISUid field was missing in a SISInfo structure.
*
*/
const TInt KErrSISInfoSISUidMissing = -10106;
/**
*
* SISNames field was not found in a SISInfo structure. This might happen if a name was not found for a specific language 
* (i.e. the number of languages specified is higher than the number of names specified in a SISInfo).
*
*/
const TInt KErrSISInfoSISNamesMissing = -10107;
/**
*
* An insufficient buffer length was specified while parsing a SIS file.
*
*/
const TInt KErrSISFieldBufferTooShort = -10108;
/**
* An invalid element was found in a SISStringArray.
*
*/
const TInt KErrSISStringArrayInvalidElement = -10109;
/**
*
* SISVendorNames field was not found in a SISInfo structure. This might happen if a vendor name was not found for a specific language 
* (i.e. the number of languages specified is higher than the number of vendor names specified in a SISInfo).
*
*/
const TInt KErrSISInfoSISVendorNamesMissing = -10110;
/**
*
* SISVersion field was not found in a SISInfo structure.
*
*/
const TInt KErrSISInfoSISVersionMissing = -10111;
/**
*
* SISSupportedLanguages field was missing in a SIS controller.
*
*/
const TInt KErrSISControllerSISSupportedLanguagesMissing = -10112;
/**
*
* An incorrectly constructed SISSupportedLanguages element was found during SIS file installation.
*
*/
const TInt KErrSISSupportedLanguagesInvalidElement = -10113;
/**
*
* Invalid length was specified in a SISLanguage element.
*
*/
const TInt KErrSISLanguageInvalidLength = -10114;
/**
*
* Invalid length was specified in a SISLanguage element.
*
*/
const TInt KErrSISContentsSISSignedControllerMissing = -10115;
/**
*
* SISData was missing in a SIS file.
*
*/
const TInt KErrSISContentsSISDataMissing = -10116;
/**
*
* SISFileData was missing in a SISDataUnit structure.
*
*/
const TInt KErrSISDataSISFileDataUnitMissing = -10117;
/**
*
* Target was missing for a SISFile data unit.
*
*/
const TInt KErrSISFileDataUnitTargetMissing = -10118;
/**
*
* SISFileOptions field was missing.
*
*/
const TInt KErrSISFileOptionsMissing = -10119;
/**
*
* SISFileDataUnit descriptor was missing.
*
*/
const TInt KErrSISFileDataUnitDescriptorMissing = -10120;
/**
*
* SISFileData description was missing.
*
*/
const TInt KErrSISFileDataDescriptionMissing = -10121;
/**
*
* SISContents field was missing in a SIS file.
*
*/
const TInt KErrSISContentsMissing = -10122;
/**
*
* An embedded controller was missing in a SISController structure.
*
*/
const TInt KErrSISEmbeddedControllersMissing = -10123;
/**
*
* An embedded SISDataUnit was missing.
*
*/
const TInt KErrSISEmbeddedDataUnitsMissing = -10124;
/**
*
* SISupportedOptions field was missing in a SIS controller.
*
*/
const TInt KErrSISControllerOptionsMissing = -10125;
/**
*
* A SISExpression was missing.
*
*/
const TInt KErrSISExpressionMissing = -10126;
/**
*
* A StringValue was missing in a SISExpression.
*
*/
const TInt KErrSISExpressionStringValueMissing = -10127;
/**
*
* A SISOptionString was missing.
*
*/
const TInt KErrSISOptionsStringMissing = -10128;
/**
*
* A SISFileOptionsExpression was missing.
*
*/
const TInt KErrSISFileOptionsExpressionMissing = -10129;
/**
*
* A SISExpression integer value was missing.
*
*/
const TInt KErrSISExpressionHeadValueMissing = -10130;
/**
*
* SISEmbeddedSISOptions were missing.
*
*/
const TInt KErrSISEmbeddedSISOptionsMissing = -10131;
/**
*
* SISUpgradeRange was missing in a SISInfo.
*
*/
const TInt KErrSISInfoSISUpgradeRangeMissing = -10132;
/**
*
* UID was missing in a SISDependency.
*
*/
const TInt KErrSISDependencyMissingUid = -10133;
/**
*
* Version was missing in a SISDependency.
*
*/
const TInt KErrSISDependencyMissingVersion = -10134;
/**
*
* Names were missing in a SISDependency.
*
*/
const TInt KErrSISDependencyMissingNames = -10135;
/**
*
* A prerequisite was not found for a SIS file. Please note that all prerequisites should be found either in the SIS file itself
* or pre-installed on the device. 
*
*/
const TInt KErrSISPrerequisitesMissingDependency = -10136;
/**
*
* SISPrerequisites field was missing in a SIS controller.
*
*/
const TInt KErrSISControllerMissingPrerequisites = -10137;
/**
*
* Version field was missing in a SISUpgradeRange.
*
*/
const TInt KErrSISUpgradeRangeMissingVersion = -10138;

/**
*
* An unexpected field type was found while parsing a SIS file. This is most likely to happen when a certain field type
* is expected in a certain structure, and another one is detected instead.
*
*/
const TInt KErrSISUnexpectedFieldType = -10139;
/**
*
* An unknown operator was encountered in a SISExpression.
*
*/
const TInt KErrSISExpressionUnknownOperator = -10140;
/**
*
* The length of a SISArray does not match the sum of the length of its items and the internal field type size.
*
*/
const TInt KErrSISArrayReadError = -10141;
/**
*
* Internal element type in a SISArray does not match the expected element type for that array. E.g., an array of strings was expected,
* but an array of another type was found.
*
*/
const TInt KErrSISArrayTypeMismatch = -10142;
/**
*
* A specific case of KErrSISFieldLengthInvalid for SISString. 
* In addition to the cases documented for KErrSISFieldLengthMissing, 
* it can happen whenever a string with an odd length is specified (which is invalid on a UCS2 field).
*/
const TInt KErrSISInvalidStringLength = -10143;
/**
*
* An unknown compression algorithm was encountered in a SIS file.
*
*/
const TInt KErrSISCompressionNotSupported = -10144;
/**
*
* A too deep level of embedding was found in a SIS file (a default maximal depth is 8).
*
*/
const TInt KErrSISTooDeeplyEmbedded = -10145;
/**
*
* An invalid target file specification was found in a SIS file. Typical examples are:
*	- A wrong drive has been specified.
*   - An update package attempted to change files out of the scope of the original package. 
*   - Non-executable or a DLL was being installed to "/sys/bin". 
*   - An attempt was made to overwrite a file by an existing package. 
*
*   Other sorts of incorrect target file path specification in a SIS file can also lead to the same error. 
*   This code is usually generated in conjunction with other error codes, for example EUiInvalidFileName or EUiDiskNotPresent.
*
*/
const TInt KErrSISInvalidTargetFile = -10146;
/**
*
* A file being installed would overwrite an existing file, for instance because it has already been installed.
*
*/
const TInt KErrSISWouldOverWrite = -10147;
/**
*
* RemoveDirectories were missing in a SISInfo.
*
*/
const TInt KErrSISInfoMissingRemoveDirectories = -10148;
/**
*
* Not enough space to install a SIS file.
*
*/
const TInt KErrSISNotEnoughSpaceToInstall = -10149;

/**
*
* A generic error was encountered in the SIS installer.
*
*/
const TInt KErrInstallerLeave = -10200;

// Security Manager errors

/**
*
* A corrupt policy file was found on the device.
*
*/
const TInt KErrPolicyFileCorrupt = -10250;
/**
*
* Security policy file declared a signature scheme which is not supported by the Symbian OS.
*
*/
const TInt KErrSignatureSchemeNotSupported = -10251;
/**
*
* An unsupported digest function was used in a signed SIS file.
*
*/
const TInt KErrDigestNotSupported = -10252;
/**
*
* A bad digest was found in a signed file. Usually it happens when the signed file has been tampered with.
*
*/
const TInt KErrBadHash = -10253;

/** 
*
*	A general-purpose error code method after installation was aborted due to a security error which had 
	already been reported via a callback dialog (see MUiHandler interface).
*   Typical examples which can lead to KErrSecurityError are: 
*   	- The SIS file is not signed but the device is set up to require they be so.
*		- The SIS file's content contains a protected SID/VID which isn't signed for.
*		- System capabilities are requested but not signed for.
*/
const TInt KErrSecurityError = -10254;

/** 
* A general-purpose error code for misuse of an interface.
*/
const static TInt KErrBadUsage = -10263;


// Expression evaluator errors
/** 
* Invalid type while evaluating an expression.
*/
const static TInt KErrInvalidType = -10260; 
/** 
* Invalid expression format has been encountered.
*/
const static TInt KErrInvalidExpression = -10261; 
/** 
* Expression was too complex to process (e.g. had too many nesting levels).
*/
const static TInt KErrExpressionToComplex = -10262; 


// Installer errors
/** 
* An installation of an augmentation or partial upgrade failed, since the base package is not present on the device.
*/
const TInt KErrMissingBasePackage = -10255; 
/** 
* An upgrade failed because the package being installed is not a valid upgrade of the package on the device.
*/
const TInt KErrInvalidUpgrade = -10256; 
/** 
* Detected an attempt to perform an illegal eclipsing of a file during installation or restore.
*/
const TInt KErrInvalidEclipsing = -10257;
/** 
* An incorrect executable format was detected by the software installer.
*/
const TInt KErrWrongHeaderFormat = -10258; // The h/w executable doesn't have the format KImageHdrFmt_V. 
/** 
* Mismatch between capabilities declared in the executable, and those found in the SIS file controller section.
*/
const TInt KErrCapabilitiesMismatch = -10259; 

/** 
* A legacy SIS file has been detected.
*/
const TInt KErrLegacySisFile = -10270;

/** 
* An installation failed because the package being installed contains an invalid software type registration file.
*/
const TInt KErrInvalidSoftwareTypeRegistrationFile = -10271; 

#endif

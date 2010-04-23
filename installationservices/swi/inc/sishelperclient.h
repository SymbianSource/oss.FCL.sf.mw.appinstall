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
* Common definitions for sishelper server and client
*
*/


/**
 @file
 @internalTechnology
*/
 
#ifndef __SISHELPERCLIENT_H__
#define __SISHELPERCLIENT_H__
 
#include <e32std.h>
#include <f32file.h>
#include <caf/caf.h>

#include "swi/siscontroller.h" 
#include "writestream.h"

namespace Swi
{

_LIT(KSisHelperServerName, "!SISHELPER");
const TUid KSisHelperServerUid3={0x101F7293};
const TInt KSisHelperVersionMajor=1;
const TInt KSisHelperVersionMinor=0;
const TInt KSisHelperVersionBuild=0;

class CSisHelperStream;
class RUiHandler;
class MSisDataProvider;

/**
Panic code for panicking a badly written client
@internalTechnology
@released
*/
enum TSisHelperPanic
	{
	EPanicBadDescriptor,
	EPanicIllegalFunction,
	EPanicAlreadyReceiving,
	/**
	Unknown startup data type (see TSisHelperStartParams::TType)
	*/
	EPanicBadStartupDataType,
	/**
	Bad interface pointer passed in startup data
	*/
	EPanicBadDataProvider
	};

/**
Start params can be either a file name or a data provider interface pointer.
This struct encapsulates SisHelper startup params, supporting both types of params.
@internalTechnology
@released
*/
struct TSisHelperStartParams
	{
	/**
	Type of contained pointer
	*/
	enum TType 
		{
		ETypeFileName,    ///< Pointer to a SISX file name
		ETypeDataProvider, ///< Pointer to a data provider interface
		ETypeFileHandle, ///< Pointer to a file handle.
		ETypeNull, ///< No Pointer to any handle .
		};
	TType iType; ///< determines what kind of pointer we have
	union
		{
		const TFileName*  iFileName;     ///< Pointer to a file name
		MSisDataProvider* iDataProvider; ///< Data provider interface pointer
		RFile* iFileHandle;
		};
	
	/**
	Constructs parameters structure with a file name
	@param aFileName File name to pass to SISHelper
	*/
	inline TSisHelperStartParams(const TFileName& aFileName);

	/**
	Constructs parameters structure with a file handle
	@param aFileHandle File handle to pass to SISHelper
	*/
	inline TSisHelperStartParams(RFile& aFileHandle);
	
	/**
	Constructs parameters structure with a data provider interface
	@param aDataProvider Data provider interface implementation pointer
	*/
	inline TSisHelperStartParams(MSisDataProvider& aDataProvider);
	
	/**
	Constructs parameters for ETypeNull
	*/
	inline TSisHelperStartParams();
	};

inline TSisHelperStartParams::TSisHelperStartParams(const TFileName& aFileName)
:	iType(ETypeFileName),
	iFileName(&aFileName)
	{
	}

inline TSisHelperStartParams::TSisHelperStartParams(RFile& aFileHandle)
:	iType(ETypeFileHandle),
	iFileHandle(&aFileHandle)
	{
	}
	
inline TSisHelperStartParams::TSisHelperStartParams(
	MSisDataProvider& aDataProvider)
:	iType(ETypeDataProvider),
	iDataProvider(&aDataProvider)
	{
	}

inline TSisHelperStartParams::TSisHelperStartParams()
:	iType(ETypeNull)
	{
	}

/**
SISHelper server messages
@released
@internalTechnology
*/
enum TSisHelperMessage
	{
	ESisHelperGetController, ///< Retrieve SISController
	ESisHelperExtractFile,   ///< Copy / decompress a file
	ESisHelperFillDrivesAndSpaces, ///< Return available disk drives and spaces
	ESisHelperOpenDrmContent, ///< Evaluate the intent for this installation
	ESisHelperExecuteDrmIntent, ///< Execute the intent for this installation
	ESisHelperSetupAsyncExtraction, ///< Begin copy / decompress part of a file
	ESisHelperAsyncExtraction, ///< Continue copy / decompress part of a file
	ESisHelperEndAsyncExtraction, ///< End copy / decompress part of a file
	ESisHelperCreateSisStub,		///< Create a .SIS stub file
	ESisHelperIsStub,			///< Query whether the SIS file contains any data, if not it's a stub (preinstalled)
	ESisHelperGetSisFileDrive,  ///< Find the drive letter the SIS is installed from. (-1) if unknown
	ESisHelperIsSisFileReadOnly, ///< Check if the SIS file is read only (affects whether to delete on uninstall of preinstalled package.)
	ESisHelperGetSisFileName,     ///< Get name with path of SIS file.
	ESisHelperGetControllerFromSis, ///< Retrieve SISController from the specified SISX file
	ESisHelperGetEquivalentLanguages, ///< Retrieve fetching equivalent languages
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	ESisHelperIsDrmProtected		// Says whether the SIS controller is DRM protected or not
	#endif
	};

/**
 * RClass which is used to make calls to the SIS Helper server using
 * the client-server framework. The ExecuteL() method is passed a
 * command in order to perform the appropriate action
 *
 * @internalTechnology
 * @released
 */
class RSisHelper : public RSessionBase
	{
public:
	/**
	 * Open the connection to the SIS Helper Server
	 *
	 * @return KErrNone or an error code
	*/
	IMPORT_C TInt Connect();

	/**
	 * Close the connection to the SIS Helper Server
	 *
	 * @return KErrNone or an error code
	*/
	IMPORT_C void Close();
	
	/**
	 * Retrieves a SISController from SISHelper
	 *
	 * @return Pointer to a new SISController (caller owns it)
	*/
	IMPORT_C HBufC8* SisControllerLC();
	
	/**
	 * Extracts the specified file from SISX file
	 *
	 * @param aRfs		 The RFs used by aFile
	 * @param aFile		 An already opened file handle
	 * @param aFileIndex Index of the file in the SISX
	 * @param aDataUnit  Data unit index
	 * @param aUiHandler UiHandler
	 *
	 * @return KErrNone on success, or an error code
	 */
	IMPORT_C TInt ExtractFileL(RFs& aFs, RFile& aFile, TInt aFileIndex, 
		TInt aDataUnit, RUiHandler& aUiHandler);
		
	/**
	 * Fills arrays with drive letter and free space information. Takes into 
	 * account drive availability and suitability for installation.
	 *
	 * @param aDriveLetters Array of available drive letters
	 * @param aDriveSpaces  Array of free disk space values for the drives
	 */
	IMPORT_C void FillDrivesAndSpacesL(RArray<TChar>& aDriveLetters, 
		RArray<TInt64>& aDriveSpaces);
	
	/**
	 * This function is used to open drm content, provided by the MSisDataProvider implementation.
	 * SisHelper server owns an implementation of the MSisDataProvider interface, and acting as 
	 * a proxy, forwards this call to the implementation.
	 *
	 * @param aIntent the intent to evaluate
	 */
	IMPORT_C void OpenDrmContentL(ContentAccess::TIntent aIntent);
	
	/**
	 * This function is used to execute DRM intent on data provided by the MSisDataProvider implementation.
	 * SisHelper server owns an implementation of the MSisDataProvider interface, and acting as 
	 * a proxy, forwards this call to the implementation.
	 *
	 * @param aIntent the intent to execute
	 */
	IMPORT_C void ExecuteDrmIntentL(ContentAccess::TIntent aIntent);

	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/** 
	* Find out whether the SIS file is DRM protected one. 	
	* @return ETrue if the SIS file is a DRM protected.
	*/
	TBool IsDrmProtectedL();
	#endif
	/** 
	* Find out the equivalent languages for a given language ID. 	
	* @return list of the equivalent languages.
	*/
	void GetEquivalentLanguageListL(TLanguage aLangId,RArray<TLanguage>& aEquivLangs);
	
	/**
	* Create a .SIS stub file from the existing SIS file.
	* In reality this just saves the SIS header followed by the controller
	* in order to create a stub.
	*/
	TInt CreateSisStub(RFile& aFile);

	/** Find out whether the SIS file handled by SisHelperServer is a stub
	This means it is either a removable media stub, a PreInstalledApp or a PreInstalledPatch
	@return ETrue if the SIS file is a stub
	*/
	TBool IsStubL();

	/** Find out which drive the SIS file used for this installation is coming from
	@return The drive where the SIS file is located or -1 if the drive is unknown
	*/
	TChar GetSisFileDriveL();

	/**
	* Find out whether the SIS file handled by SisHelperServer is read only
	* @return ETrue if the SIS file is read only
	*/
	TBool IsSisFileReadOnlyL();

	/**
	* Get the name of the SIS file handled by SisHelperServer.
	*
	* @param aFileName - A writable descriptor which should be large enough to
	*                    hold the filename.
	*/
	void GetSisFileNameL(TDes& aFileName);

	/**
	 * Retrieves a SISController from the specified SISX file
	 *
	 * @param aFileName	A controller file to read the row controller data.
	 * @return Pointer to a new SISController (caller owns it)
	 */
	IMPORT_C HBufC8* GetControllerFromSisFileLC(const TDesC& aFileName);

// Async Extraction Functions

	void SetupAsyncExtractionL(RFs& aFs, RFile& aFile, TInt aFileIndex,TInt aDataUnit);
	void AsyncExtractionL(TInt64 length, TRequestStatus& aStatus);
	void EndAsyncExtractionL();


private:
	TBuf8<32> iBuffer;	// enough space for all the arguments to ExtractPartialFileL 
	};

} // namespace Swi
 
#endif // __SISHELPERCLIENT_H__

// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Secure Symbian OS porting layer - multi-threaded implementation!!!
// 
//

#ifdef  OS_SYMBIAN

#ifdef __cplusplus
extern "C" {
#endif

#include "sqliteInt.h"
#include "os.h"
#include "os_common.h"

#ifdef __cplusplus
}  /* End of the 'extern "C"' block */
#endif

#include <s32file.h>
#include <e32math.h>
#include <e32svr.h>
#include <f32fsys.h>
#include "os_symbian_panic.h"
#include "os_symbian_mutex.h"

//

//Const buffer used for generating temporary file names.
//No lower case letters in the buffer because the OS won't make difference between lower and upper case in file names.
const char TheChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

const ThreadData KZeroThreadData = {0, 0, 0, NULL, 0, NULL};

_LIT(KCwd, ".\\");

//Only one global TSqlite3SymbianMutex instance
TSqlite3SymbianMutex TheMutex;

#ifdef SQLITE_TEST

	//Maximum allowed size of a file (TCL tests).  Disabled if 0.
	extern "C" TInt sqlite3_max_file_size = 0;

	// The following variable, if set to a non-zero value, becomes the result
	// returned from sqlite3OsCurrentTime().  This is used for testing.
	extern "C" int sqlite3_current_time = 0;

	// Remember the number of thread-specific-data blocks allocated.
	// Use this to verify that we are not leaking thread-specific-data.
	// Ticket #1601
	extern "C" int sqlite3_tsd_count = 0;
	#define TSD_COUNTER_INCR() User::LockedInc(sqlite3_tsd_count)
	#define TSD_COUNTER_DECR() User::LockedDec(sqlite3_tsd_count)
	
#else

	#define TSD_COUNTER_INCR()
	#define TSD_COUNTER_DECR()
	
#endif

//Unique TLS handle (used for both builds: SQLITE3.DLL and TCLSQLITE3SECURE.EXE)
static TInt TlsHandle()
	{
	return reinterpret_cast <TInt> (&TlsHandle);
	}

//
//                    TSqlitePanicUtil                                 //
//

/**
Panic category - used by asserts in the SQLite OS porting layer and SqliteSecure.cpp (the TSqliteSecure implementation file).
@internalComponent
*/
_LIT(KSqlitePanic, "Sqlite");

/**
Panics the caller with aPanicCode panic code.
The call will terminate the thread where it is called from.

@param aPanicCode Panic code.

@internalComponent
*/
static void SqlitePanic(TSqlitePanicCode aPanicCode)
	{
	User::Panic(KSqlitePanic, aPanicCode);
	}

#ifdef _SQLITE_ASSERTIONS

/**
The function prints out a "Sqlite panic" message to the console and panics the thread where it is called from.
It gives a useful information about the found error together with the source file name and line number where
it occurred.

The function is used when _SQLITE_ASSERTIONS is defined.

@param aFile Source file name
@param aLine Source line number
@param aPanicCode Panic code

@return KErrNone

@internalComponent
*/	
TInt TSqlitePanicUtil::Assert(const TText* aFile, TInt aLine, TInt aPanicCode)
	{
	TBuf<16> tbuf;
	TSqlitePanicUtil::GetTimeStr(tbuf);
	TBuf<80> buf;
	_LIT(KFormat,"**%S* SQLite panic %d, at %S(%d)");
	TPtrC fname(Filename(aFile));
	buf.Format(KFormat, &tbuf, aPanicCode, &fname, aLine);
	RDebug::Print(buf);
	::SqlitePanic(static_cast <TSqlitePanicCode> (aPanicCode));
	return KErrNone;
	}
	
/**
Formats the current time into aWhere descriptor.

The function is used when _SQLITE_ASSERTIONS is defined.

@param aWhere Output parameter. The current time will be formatted there. The buffer length should be at least 16 characters.
*/
void TSqlitePanicUtil::GetTimeStr(TDes& aWhere)
	{
	TTime time;
	time.HomeTime();
	TDateTime dt = time.DateTime();
	TBuf<16> tbuf;
	aWhere.Format(_L("%02d:%02d:%02d.%06d"), dt.Hour(), dt.Minute(), dt.Second(), dt.MicroSecond());
	};
	
/**
The function creates and returns TPtrC object which points to aFile parameter.

The function is used when _SQLITE_ASSERTIONS is defined.

@param aFile File name
@return TPtrC object pointing to aFile parameter.

@internalComponent
*/	
TPtrC TSqlitePanicUtil::Filename(const TText* aFile)
	{
	TPtrC p(aFile);
	TInt ix = p.LocateReverse('\\');
	if(ix<0)
		ix=p.LocateReverse('/');
	if(ix>=0)
		p.Set(p.Mid(1+ix));
	return p;
	}
	
#else //_SQLITE_ASSERTIONS

/**
The function panics the thread where it is called from.
The function is used when _SQLITE_ASSERTIONS is not defined.

@param Not used
@param Not used
@param aPanicCode Panic code

@return KErrNone

@internalComponent
*/	
TInt TSqlitePanicUtil::Assert(const TText*, TInt, TInt aPanicCode)
	{
	::SqlitePanic(static_cast <TSqlitePanicCode> (aPanicCode));
	return KErrNone;
	}

#endif//_SQLITE_ASSERTIONS

//
//       File handles API                                 //
//

const char KFhSeparator = '|';	//The symbol, which when used in the file name means that the string does not contain a real file name but file handles
const TInt KFhMainDbPos = 1;	//Main Db RFile*
const TInt KFhJournalPos = 9;	//Journal RFile*
const TInt KFhAddrLen = 8;		//RFile address length

const char KJournalSuffix[] = "-journal";
const TInt KJournalSuffixLen = 8;//The length of KJournalSuffix

const char KMasterJournalSuffix[] = "-mj";
const TInt KMasterJournalSuffixLen = 3 + 8;//The length of KMasterJournalSuffix + 8, where 8 is the max possible length
										   //of the hex string presentation of a unsigned 32-bit number

/**
Database file name types:
@code
	- EFhNone 			- The database file name does not contain file handles;
	- EFhJournal		- The database file name contains file handles, and is a journal file name;
	- EFhMasterJournal	- The database file name contains file handles, and is a master journal file name;
	- EFhMainDb			- The database file name contains file handles and is the main db file;
@endcode

@see FhStringProps()

@internalComponent
*/
enum TFhType
	{
	EFhNone,			//The database file name does not contain file handles
	EFhJournal,			//The database file name contains file handles, and is a journal file name
	EFhMasterJournal,	//The database file name contains file handles, and is a master journal file name
	EFhMainDb			//The database file name contains file handles and is the main db file
	};

/**
The OS porting layer functions use this function to determine the type of the file name, which can be 
one of the TFhType enum item values.

@param aFileName Zero-terminated, UTF8 encoded file name.
@return The file name type, one of the TFhType enum item values.

@see TFhType

@internalComponent
*/
static TFhType FhStringProps(const char* aFileName)
	{
	if(!aFileName || aFileName[0] != KFhSeparator)	//The "|" character should be at position 0
		{
		return EFhNone;
		}
		
	TInt len = strlen(aFileName);	
	
	if(len > KJournalSuffixLen && strcmp(aFileName + len - KJournalSuffixLen, KJournalSuffix) == 0)
		{
		return EFhJournal;	
		}
		
	if(len > KMasterJournalSuffixLen && strstr(aFileName + len - KMasterJournalSuffixLen, KMasterJournalSuffix) != NULL)
		{
		return EFhMasterJournal;	
		}
		
	return EFhMainDb;
	}

/**
Extracts from the aDbFileName string the requested RFile object's address.

The expectation is that FhStringProps() was called before the FhExtract() call and it is known that aDbFileName
contains file handles

@param aDbFileName Input parameter. The file name. 
				   The aDbFileName format is:
@code
      				"|<Main Db RFile*><Journal RFile*><db file name>"
@endcode

@see TFhType
@see FhStringProps()

@internalComponent

@panic Sqlite 10 In _DEBUG mode - invalid aPos value.
@panic Sqlite 12 In _DEBUG mode - invalid position of the "|" character in the file name.
*/
static TInt FhExtract(const TDesC& aDbFileName, TInt aPos, RFile*& aFile)
	{
	__SQLITEASSERT(aPos >= 0 && aPos < (aDbFileName.Length() - KFhAddrLen), ESqlitePanicFhInvalidPos);
	__SQLITEASSERT(aDbFileName.Locate(TChar(KFhSeparator)) == 0, ESqlitePanicFhInvalidStr);
	//Extract from the aDbFileName string the requested RFile object's address
	TLex lex;
	lex.Assign(aDbFileName.Mid(aPos, KFhAddrLen));
	TUint32 addr;
	TInt err = lex.Val(addr, EHex);
	if(err != KErrNone)
		{
		return err;	
		}
	aFile = reinterpret_cast <RFile*> (addr);
	if(!aFile)
		{
		return KErrArgument;	
		}
	return KErrNone;
	}

//Extracts the file handle from aDbFileName, specified by the aFhType parameter, and stores the result in
//the aFile output parameter.
//The function returns KErrNone if the file handle is extracted successfully or returns a system-wide error code otherwise.
static TInt FhExtractFile(const TDesC& aDbFileName, TFhType aFhType, RFile& aFile)
	{
	__SQLITEASSERT(aFhType == EFhJournal || aFhType == EFhMainDb, ESqlitePanicFhInvalidType);
	RFile* file = NULL;
	TInt err = FhExtract(aDbFileName, aFhType == EFhJournal ? KFhJournalPos : KFhMainDbPos, file);
	if(err == KErrNone)
		{
		__SQLITEASSERT(file != NULL, ESqlitePanicFhInvalidData);
		aFile = *file;	
		}
	return err;
	}

//Extracts the file handle from aDbFileName, specified by the aFhType parameter, and duplicates the file handle,
//storing the duplicated handle in the aFile output parameter.
//The function returns KErrNone if the file handle is extracted successfully or returns a system-wide error code otherwise.
static TInt FhDuplicateFile(const TDesC& aDbFileName, TFhType aFhType, RFile& aFile)
	{
	RFile file;
	TInt err = FhExtractFile(aDbFileName, aFhType, file);
	if(err == KErrNone)
		{
		err = aFile.Duplicate(file);
		}
	return err;
	}

//
//       COsThreadData class - declaration, definition    //
//

//COsThreadData class manages per-thread copy of the following data: 
//  - RFs instance;
//	- ThreadData instance (SQLITE thread data)
class COsThreadData
	{
public:	
	static COsThreadData* New();
	~COsThreadData();
	static COsThreadData& Instance();
	
	TInt GetFs(RFs& aFs);
	inline ThreadData& SqliteData();
	
private:
	COsThreadData();
	
private:
	RFs			iFs;
	ThreadData	iSqliteThreadData;
	
	};

//Creates and returns per-thread COsThreadData instance.
COsThreadData* COsThreadData::New()
	{
	COsThreadData* osThreadData = static_cast <COsThreadData*> (UserSvr::DllTls(TlsHandle()));
	__SQLITEASSERT(!osThreadData, ESqlitePanicInternalError);
	if(!osThreadData)
		{
		osThreadData = new COsThreadData;
		if(!osThreadData)
			{
			return 0;
			}
		if(UserSvr::DllSetTls(TlsHandle(), osThreadData) != KErrNone)
			{
			delete osThreadData;
			return 0;
			}
		}
	return osThreadData;
	}

//Closes the file session.
COsThreadData::~COsThreadData()
	{
	(void)UserSvr::DllSetTls(TlsHandle(), 0);
	iFs.Close();	
	}

//Returns a reference to the per-thread COsThreadData instance.	
COsThreadData& COsThreadData::Instance()
	{
	COsThreadData* osThreadData = static_cast <COsThreadData*> (UserSvr::DllTls(TlsHandle()));
	__SQLITEASSERT_ALWAYS(osThreadData != NULL, ESqlitePanicNullOsLayerDataPtr);
	return *osThreadData;
	}

//If the iFs instance is not initialized yet, the function calls RFs::Connect().
//Upon a successful GetFs() call, the aFs parameter will be initialized with the iFs properties.
TInt COsThreadData::GetFs(RFs& aFs)
	{
	if(iFs.Handle() == KNullHandle)
		{
 		TInt err = iFs.Connect();
		if(err != KErrNone)
			{
			return err;	
			}
		}
	aFs = iFs;
	return KErrNone;
	}

//Returns a reference to the SQLITE (per-thread) specific data.
inline ThreadData& COsThreadData::SqliteData()
	{
	return iSqliteThreadData;	
	}

//
COsThreadData::COsThreadData()
	{
	Mem::FillZ(&iSqliteThreadData, sizeof(iSqliteThreadData));
	}

//Common code used every time when the calling function needs to get an access to the RFs instance.
//The macro defines a local "fs_obj" object of RFs type, initializes the "fs_obj" object and returns
//"failure_rc" code if the initialization fails.
#define GET_FS(fs_obj, failure_rc)\
	RFs fs_obj;\
	if(COsThreadData::Instance().GetFs(fs_obj) != KErrNone)\
		{\
		return failure_rc;\
		}

//
//       OS porting layer static global functions      //
//

//aFileName argument is expected to point to UTF8 encoded, zero terminated string.
//The function converts aFileName to UTF16 encoded file name, and stores the UTF16 encoded file name
//to the place pointed by aFileNameDestBuf argument.
//If the UTF16 conversion of the file name failed because the file name is too long or NULL, 
//the function returns EFalse. 
//Max allowed aFileName length is KMaxFileName (excluding terminating 0 character).
//aFileNameDestBuf max length must be at least KMaxFileName characters.
//
//aFileNameDestBuf will hold UTF16, non-zero-terminated string
static TBool ConvertToUnicode(const char *aFileName, TDes& aFileNameDestBuf)
	{
	if(aFileName)
		{
		wchar_t* dest = reinterpret_cast <wchar_t*> (const_cast <TUint16*> (aFileNameDestBuf.Ptr()));
		TInt len = mbstowcs(dest, aFileName, KMaxFileName);
		//Check the file name length. If it is longer than KMaxFileName characters, then the file name is not valid.
		if(len > 0 && len <= KMaxFileName)
			{
			aFileNameDestBuf.SetLength(len);
			return ETrue;
			}
		}
	return EFalse;
	}

//aFileName argument is expected to point to UTF16 encoded, zero terminated string.
//The function converts aFileName to UTF8 encoded file name, and stores the UTF8 encoded file name
//to the place pointed by aFileNameDestBuf argument.
//If the UTF8 conversion of the file name failed because the file name is too long or NULL, 
//the function returns EFalse. 
//Max allowed aFileName length is KMaxFileName (excluding terminating 0 character).
//aFileNameDestBuf max length must be at least KMaxFileName characters.
//
//aFileNameDestBuf will hold UTF8, non-zero-terminated string
static TBool ConvertFromUnicode(const TDesC& aFileName, TDes8& aFileNameDestBuf)
	{
	char* dest = reinterpret_cast <char*> (const_cast <TUint8*> (aFileNameDestBuf.Ptr()));
	const wchar_t* src = reinterpret_cast <const wchar_t*> (aFileName.Ptr());
	TInt len = wcstombs(dest, src, KMaxFileName);
	//Check the file name length. If it is longer than KMaxFileName characters, then the file name is not valid.
	if(len > 0 && len <= KMaxFileName)
		{
		aFileNameDestBuf.SetLength(len);
		return ETrue;
		}
	return EFalse;
	}

//Retrieves the sector size of aDriveNo.
//The call never fails.
//
//Algorithm:
// Define a variable: sectorSize;
// The sectorSize is initialized with a default value: SQLITE_DEFAULT_SECTOR_SIZE;
// If aDriveNo is a removable drive, get the cluster size for that drive
//     and initialize sectorSize with it (the cluster size). The reason for that is: the atomic write operation
//     for removable media drives is "write cluster". That means that the atomic file unit to be stored in the rollback journal
//     is cluster, not sector. If SQLITE stores in the rollback journal sectors, not clusters, then the database consistency cannot
//     be guaranteed after a power failure.
static TInt GetSectorSize(RFs& aFs, TInt aDriveNo, const TDriveInfo& aDriveInfo)
	{
	__SQLITEASSERT(aDriveNo >= EDriveA && aDriveNo <= EDriveZ, ESqlitePanicInvalidArg);
	TInt sectorSize = SQLITE_DEFAULT_SECTOR_SIZE;//no reason to initialize aSectorSize with a smaller value,
												 //because, the smallest possible page size is SQLITE_DEFAULT_SECTOR_SIZE bytes.
	//if aDriveNo is a removable drive, query always the cluster size
	if(aDriveInfo.iDriveAtt & KDriveAttRemovable)
		{
		TVolumeIOParamInfo volIoParams;
		TInt err = aFs.VolumeIOParam(aDriveNo, volIoParams);
		//VolumeIOParam() demonstrates the following behaviour:
		//==Emulator==
		// Drive C: - err = 0, volIoParams.iBlockSize = KErrNotReady;
		// Drive Z: - err = 0, volIoParams.iBlockSize = KErrNotReady;
		//==Hardware==
		// Drive C: - err = 0, volIoParams.iBlockSize = 1;
		// Drive Z: - err = 0, volIoParams.iBlockSize = KErrNotReady;
		//
		//Initialize the aSectorSize variable only if: 
		// - VolumeIOParam() call didn't fail;
		// - volIoParams.iBlockSize > SQLITE_DEFAULT_SECTOR_SIZE;
		// - volIoParams.iBlockSize is power of 2;
		if(err == KErrNone && volIoParams.iBlockSize > SQLITE_DEFAULT_SECTOR_SIZE && 
		   (volIoParams.iBlockSize & (volIoParams.iBlockSize - 1)) == 0)
			{
			sectorSize = volIoParams.iBlockSize;
			}
		}
	__SQLITEASSERT(sectorSize > 0 && (sectorSize & (sectorSize - 1)) == 0, ESqlitePanicInternalError);
	return sectorSize;
	}

//The function copies into "aDefaultPath" parameter file session's default path.
//The "aDefaultPath" max length should be at least KMaxFileName characters.
//The function returns KErrNone if completes successfully or some system-wide error code.
static TInt GetDefaultPath(RFs& aFs, TDes& aDefaultPath)
  	{
	return aFs.SessionPath(aDefaultPath);
	}

//Some of the calls of the OS porting layer functions may fail with KErrNoMemory.
//But the OS porting layer API spec does not allow in some cases SQLITE_NOMEM to be returned 
//(for example,  the "file write" API is expected to return SQLITE_FULL in a case of a failure)
//That's the reason, after each OS call, NotifySqliteIfNoMemoryErr() shall be called with the Symbian OS err,
//to notify SQLite in a case of an "out of memory" problem.
static void NotifySqliteIfNoMemoryErr(TInt aErr)
	{
	if(aErr == KErrNoMemory)
		{
		sqlite3FailedMalloc();			
		}
	}

//
//       CDbFile class - declaration, definition    //
//

//CDbFile::iFileBuf - file buffer size in bytes
const TInt KFileBufSize = 8192;	

// CDbFile class has a set of methods and data members used for the file I/O operations performed by SQLITE.
// Following the recomendations made in the comments of OsFile structure in os.h file, CDbFile was declared
// as a class derived from OsFile.
// The class consists of two sets of methods:
// - Non-static methods, which implement specific file operation.
// - Static methods. Used for the initialization of OsFile::pMethod data member (IoMethod structure).
// The class also holds information about the current file lock type and a flag indicating should the
// file be deleted after closing it.
class CDbFile : public OsFile
	{
public:	
	static CDbFile* New();
	~CDbFile();
	TInt Create(RFs& aFs, const TDesC& aFileName);
	TInt CreateExclusive(RFs& aFs, const TDesC& aFileName, TBool aDeleteOnClose);
	TInt Open(RFs& aFs, const TDesC& aFileName);
	TInt OpenReadOnly(RFs& aFs, const TDesC& aFileName);
	void OpenFromHandle(RFile& aFile);
	void RetrieveSectorSize();

	//"IoMethod" methods
	static int Close(OsFile** aOsFile);
	static int OpenDirectory(OsFile* aOsFile, const char* aDirName);
	static int Read(OsFile* aOsFile, void*, int amt);
	static int Write(OsFile* aOsFile, const void*, int amt);
	static int Seek(OsFile* aOsFile, i64 offset);
	static int Truncate(OsFile* aOsFile, i64 size);
	static int Sync(OsFile* aOsFile, int);
	static void SetFullSync(OsFile* aOsFile, int setting);
	static int FileHandle(OsFile* aOsFile);
	static int FileSize(OsFile* aOsFile, i64 *pSize);
	static int Lock(OsFile* aOsFile, int aLockType);
	static int Unlock(OsFile* aOsFile, int);
	static int LockState(OsFile* aOsFile);
	static int CheckReservedLock(OsFile* aOsFile);
	static int SectorSize(OsFile* aOsFile);

private:
	CDbFile();	
	static CDbFile& Instance(void* aDbFile);
	TInt ReadL(TUint8* aDest, TInt aLength);
	void WriteL(const TUint8* aData, TInt aLength);
	inline TInt SizeL() const;
	void SetSizeL(TInt aSize);
	void FlushL();
	inline void Seek(TInt aOffset);
	inline TInt SectorSize() const;
	inline TInt Lock(TInt aPos, TInt aLength);
	inline TInt Unlock(TInt aPos, TInt aLength);
	TInt GetReadLock();
	inline TInt UnlockReadLock();
	inline TInt Handle() const;
	inline void SetLockType(TInt aLockType);
	inline TInt LockType() const;

private:	
	RFs			iFs;
	RFileBuf	iFileBuf;
	TBool		iDeleteOnClose; //True, if CDbFile is a temporary file and will be deleted when closed
	TInt		iLockType;		//Type of lock currently held on this file: NO_LOCK, SHARED_LOCK, 
								//RESERVED_LOCK, PENDING_LOCK, EXCLUSIVE_LOCK
	TInt16 		iSharedLockByte;//Randomly chosen byte used as a shared lock
	TInt		iFilePos;
	TBool		iReadOnly;
	TInt		iSectorSize;	//The sector size of the drive where the file is
	
	};

#define EXCLUSIVE_FILE_MODE()	(EFileRead | EFileWrite | EFileShareExclusive)
#define SHARED_FILE_MODE() 		(EFileRead | EFileWrite | EFileShareAny)
#define READONLY_FILE_MODE() 	(EFileRead | EFileShareReadersOnly)
	
//  CDbFile - method implementations   //
	
//Creates non-initializad CDbFile instance.
CDbFile* CDbFile::New()
	{
	return new CDbFile;
	}

//Closes the file. If iFileToBeDeleted is not NULL, the file will be deleted.
CDbFile::~CDbFile()
	{
	TFileName fname;
	TInt err = KErrGeneral;
	if(iDeleteOnClose)
		{
		if(iFileBuf.File().SubSessionHandle() != KNullHandle)
			{
			err = iFileBuf.File().FullName(fname);
			}
		}
	iFileBuf.Close();
	if(iDeleteOnClose && err == KErrNone)
		{
		(void)iFs.Delete(fname);
		}
	}

//Creates a file with aFileName. The file will be created for shared reading/writing.
//This call initializes CDbFile instance.
TInt CDbFile::Create(RFs& aFs, const TDesC& aFileName)
	{
	iFs = aFs;
	return iFileBuf.Create(iFs, aFileName, SHARED_FILE_MODE());
	}
	
//Creates a file with aFileName. The file will be created for exclusive reading/writing.
//This call initializes CDbFile instance.
//The function may return KErrNoMemory.
TInt CDbFile::CreateExclusive(RFs& aFs, const TDesC& aFileName, TBool aDeleteOnClose)
	{
	iFs = aFs;
	iDeleteOnClose = aDeleteOnClose;
	return iFileBuf.Create(iFs, aFileName, aDeleteOnClose ? EXCLUSIVE_FILE_MODE() : SHARED_FILE_MODE());
	}
	
//Opens a file with aFileName. The file will be opened for shared reading/writing.
//This call initializes CDbFile instance.
TInt CDbFile::Open(RFs& aFs, const TDesC& aFileName)
	{
	iFs = aFs;
	return iFileBuf.Open(iFs, aFileName, SHARED_FILE_MODE());
	}
	
//Opens a file with aFileName. The file will be opened in shared read-only mode.
//This call initializes CDbFile instance.
TInt CDbFile::OpenReadOnly(RFs& aFs, const TDesC& aFileName)
	{
	iFs = aFs;
	TInt err = iFileBuf.Open(iFs, aFileName, READONLY_FILE_MODE());
	iReadOnly = (err == KErrNone);
	return err;
	}
	
void CDbFile::OpenFromHandle(RFile& aFile)
	{
	iFileBuf.Attach(aFile);
	}

//Reads aLength bytes from the file and stores them to the place pointed by aDest argument.
//Returns count of bytes actually read.
TInt CDbFile::ReadL(TUint8* aDest, TInt aLength)
	{
	(void)iFileBuf.SeekL(MStreamBuf::ERead, EStreamBeginning, iFilePos);
	TInt cnt = iFileBuf.ReadL(aDest, aLength);
  	__SQLITEASSERT(cnt >= 0 && cnt <= aLength, ESqlitePanicInvalidRAmount);
	iFilePos += cnt;
	return cnt;
	}

//Writes aLength bytes to the file.
//If the file is read-only, the function leaves with KErrAccessDenied.
//If the write position is beyond the end of the file, the file will be extended.
void CDbFile::WriteL(const TUint8* aData, TInt aLength)
	{
	if(iReadOnly)
		{
		User::Leave(KErrAccessDenied);
		}
#ifdef SQLITE_TEST
	if(sqlite3_max_file_size > 0 && (iFilePos + aLength) > sqlite3_max_file_size )
		{
		User::Leave(KErrDiskFull);
		}
#endif
	TInt len = iFilePos - iFileBuf.SizeL();
	if(len > KFileBufSize)
		{
		iFileBuf.SetSizeL(iFilePos);
		}
	else if(len > 0)
		{
		(void)iFileBuf.SeekL(MStreamBuf::EWrite, EStreamEnd);
		const TInt KZBufSize = 512;
    	TBuf8<KZBufSize> zbuf;
    	zbuf.FillZ(KZBufSize);
    	while(len > KZBufSize)
        	{
			iFileBuf.WriteL(zbuf.Ptr(), KZBufSize);
        	len -= KZBufSize;
        	}
		iFileBuf.WriteL(zbuf.Ptr(), len);
    	}
	(void)iFileBuf.SeekL(MStreamBuf::EWrite, EStreamBeginning, iFilePos);
	iFileBuf.WriteL(aData, aLength);
	iFilePos += aLength;
	}

//Returns the file size in bytes.
inline TInt CDbFile::SizeL() const
	{
	return iFileBuf.SizeL();
	}
	
//Sets the file size. aSize - in bytes.
//Note that if SetSize() truncates the file, iFilePos may become invalid!
void CDbFile::SetSizeL(TInt aSize)
	{
	if(iReadOnly)
		{
		User::Leave(KErrAccessDenied);
		}
	iFileBuf.SetSizeL(aSize);
	}

void CDbFile::FlushL()
	{
	if(iReadOnly)
		{
		User::Leave(KErrAccessDenied);
		}
	iFileBuf.SynchL();
	}

inline void CDbFile::Seek(TInt aOffset)
	{
  	iFilePos = aOffset;
	}

//Returns the sector size of the drive where the file is
inline TInt CDbFile::SectorSize() const
	{
	__SQLITEASSERT(iSectorSize >= SQLITE_DEFAULT_SECTOR_SIZE && (iSectorSize & (iSectorSize - 1)) == 0, ESqlitePanicInvalidSectorSize);
	return iSectorSize;
	}

inline TInt CDbFile::Lock(TInt aPos, TInt aLength)
	{
	return iFileBuf.File().Lock(aPos, aLength);
	}

inline TInt CDbFile::Unlock(TInt aPos, TInt aLength)
	{
	return iFileBuf.File().UnLock(aPos, aLength);
	}

TInt CDbFile::GetReadLock()
	{
    TInt lock;
    sqlite3Randomness(sizeof(lock), &lock);
    //Explanation regarding how the file locking works can be found in os.h file, lines 279-335.
    //Shortly, in order to read pages from the database the calling thread must obtain a shared lock.
    //This is done locking a randomly chosen byte - iSharedLockByte.
    //The calculation of iSharedLockByte is done in a way that:
    // - All calculated iSharedLockByte fit on a single page, even if the page size is chosen to be the smallest one possible.
    //       That's why the "% (SHARED_SIZE - 1)" is used in the calculation;
    // - The locked byte cannot be used for storing data. That is the reason SHARED_FIRST to be set to be a position beyond the
    //       1Gb boundary;
    iSharedLockByte = (lock & 0x7fffffff) % (SHARED_SIZE - 1);
	return Lock(SHARED_FIRST + iSharedLockByte, 1);
	}
	
inline TInt CDbFile::UnlockReadLock()
	{
	return Unlock(SHARED_FIRST + iSharedLockByte, 1);
	}

//Returns the file handle. Used for debug purposes only.
inline TInt CDbFile::Handle() const
	{
	return iFileBuf.File().SubSessionHandle();	
	}

//Sets the file lock type.
inline void CDbFile::SetLockType(TInt aLockType)
	{
	iLockType = aLockType;
	}
	
//Returns the current file lock type.
inline TInt CDbFile::LockType() const
	{
	return iLockType;
	}

//  CDbFile - static "IoMethod" method implementations   //

//Used in the initialization of "TheIoMethods" structure.
//Closes the file. The file will be deleted if it has been marked for deletion (iFileToBeDeleted not NULL).
/* static */ int CDbFile::Close(OsFile** aOsFile)
	{
	if(aOsFile)
		{
		CDbFile* dbFile = static_cast <CDbFile*> (*aOsFile);
    	delete dbFile;
    	OpenCounter(-1);
    	*aOsFile = 0;
		}
	return SQLITE_OK;
	}

//Used in the initialization of "TheIoMethods" structure.
//Attempt to open a file descriptor for the directory that contains a
//file.  This file descriptor can be used to fsync() the directory
//in order to make sure the creation of a new file is actually written
//to disk.
//
//This routine is only meaningful for Unix.  It is a no-op under
//Symbian OS since Symbian OS does not support hard links.
/* static */ int CDbFile::OpenDirectory(OsFile* /*aOsFile*/, const char* /*aDirName*/)
	{
	return SQLITE_OK;
	}

//Used in the initialization of "TheIoMethods" structure.
//Read data from a file into a buffer.  Return SQLITE_OK if all
//bytes were read successfully and SQLITE_IOERR if anything goes
//wrong.
/* static */ int CDbFile::Read(OsFile *aOsFile, void *aBuf, int aAmt)
	{
  	CDbFile& dbFile = CDbFile::Instance(aOsFile);
	TInt cnt = 0;
	TRAPD(err, cnt = dbFile.ReadL(reinterpret_cast <TUint8*> (aBuf), aAmt));
  	SimulateIOError(return SQLITE_IOERR_READ);
	if((err == KErrNone && cnt < aAmt) || err == KErrEof)
		{
		__SQLITEASSERT(cnt <= aAmt, ESqlitePanicInvalidRAmount);
		memset(&((char*)aBuf)[cnt], 0, aAmt - cnt);
		return SQLITE_IOERR_SHORT_READ;
		}
	__SQLITEASSERT(err == KErrNone ? cnt == aAmt : ETrue, ESqlitePanicInvalidRAmount);
	::NotifySqliteIfNoMemoryErr(err);
	return err == KErrNone ? SQLITE_OK : SQLITE_IOERR_READ;
	}

//Used in the initialization of "TheIoMethods" structure.
//Write data from a buffer into a file.  Return SQLITE_OK on success
//or some other error code on failure.
/* static */ int CDbFile::Write(OsFile *aOsFile, const void *aBuf, int aAmt)
	{
  	CDbFile& dbFile = CDbFile::Instance(aOsFile);
	__SQLITEASSERT(aAmt > 0, ESqlitePanicInvalidWAmount);
	TRAPD(err, dbFile.WriteL(reinterpret_cast <const TUint8*> (aBuf), aAmt));
	SimulateIOError(return SQLITE_IOERR_WRITE);
	SimulateDiskfullError(return SQLITE_FULL);
  	__SQLITEASSERT(err != KErrEof, ESqlitePanicInternalError);
	::NotifySqliteIfNoMemoryErr(err);
	return err == KErrNone ? SQLITE_OK : SQLITE_FULL;
	}

//Used in the initialization of "TheIoMethods" structure.
//Move the read/write pointer in a file.
//The function does not actually move the file pointer, it only stores the requested offset in the related CDbFile object.
/* static */ int CDbFile::Seek(OsFile* aOsFile, i64 aOffset)
	{
  	//Symbian OS supports 32 bit file size only! 
	//An assert here is inappropriate, because if the database file is corrupted, the offset could be 64-bit.
  	if((TInt32)aOffset != aOffset)
  		{
  		return SQLITE_FULL;	
  		}
#ifdef SQLITE_TEST
	if(aOffset > 0) 
		{
		SimulateDiskfullError(return SQLITE_FULL);
		}
#endif
  	CDbFile::Instance(aOsFile).Seek((TInt)aOffset);
	return SQLITE_OK;
	}

//Used in the initialization of "TheIoMethods" structure.
//Make sure all writes to a particular file are committed to disk.
/* static */ int CDbFile::Sync(OsFile *aOsFile, int)
	{
  	CDbFile& dbFile = CDbFile::Instance(aOsFile);
	TRAPD(err, dbFile.FlushL());
  	SimulateIOError(return SQLITE_IOERR_FSYNC);
	::NotifySqliteIfNoMemoryErr(err);
	return err == KErrNone ? SQLITE_OK : SQLITE_IOERR;
	}

//Used in the initialization of "TheIoMethods" structure.
//Truncate an open file to a specified size
//This operation invalidates iFilePos!!!
/* static */ int CDbFile::Truncate(OsFile *aOsFile, i64 aOffset)
	{
  	CDbFile& dbFile = CDbFile::Instance(aOsFile);
  	//Symbian OS supports 32 bit file size only!
	//An assert here is inappropriate, because if the database file is corrupted, the offset could be 64-bit.
  	if((TInt32)aOffset != aOffset)
  		{
  		return SQLITE_FULL;	
  		}
	TInt32 offset32 = (TInt)aOffset;//Symbian OS supports 32 bit file size only!
	TRAPD(err, dbFile.SetSizeL(offset32));
	::NotifySqliteIfNoMemoryErr(err);
  	SimulateIOError(return SQLITE_IOERR_TRUNCATE);
	if(err == KErrNone)
		{
		dbFile.iFilePos = -1;
		return SQLITE_OK;
		}
	return SQLITE_IOERR;	
	}

//Used in the initialization of "TheIoMethods" structure.
//Determine the current size of a file in bytes
/* static */ int CDbFile::FileSize(OsFile *aOsFile, i64 *aSize)
	{
  	CDbFile& dbFile = CDbFile::Instance(aOsFile);
  	TInt size32 = 0;//Symbian OS supports 32 bit file size only!
	TRAPD(err, size32 = dbFile.SizeL());
	SimulateIOError(return SQLITE_IOERR_FSTAT);
	::NotifySqliteIfNoMemoryErr(err);
  	*aSize = size32;
	return err == KErrNone ? SQLITE_OK : SQLITE_IOERR;
	}

//
//       FILE LOCKING  - BEGIN       //
//

//Used in the initialization of "TheIoMethods" structure.
// Lock the file with the lock specified by parameter locktype - one
// of the following:
//
//     (1) SHARED_LOCK
//     (2) RESERVED_LOCK
//     (3) PENDING_LOCK
//     (4) EXCLUSIVE_LOCK
//
// Sometimes when requesting one lock state, additional lock states
// are inserted in between.  The locking might fail on one of the later
// transitions leaving the lock state different from what it started but
// still short of its goal.  The following chart shows the allowed
// transitions and the inserted intermediate states:
//
//    UNLOCKED -> SHARED
//    SHARED -> RESERVED
//    SHARED -> (PENDING) -> EXCLUSIVE
//    RESERVED -> (PENDING) -> EXCLUSIVE
//    PENDING -> EXCLUSIVE
//
// This routine will only increase a lock.  The CDbFile::Lock() routine
// erases all locks at once and returns us immediately to locking level 0.
// It is not possible to lower the locking level one step at a time.  You
// must go straight to locking level 0.
/* static */ int CDbFile::Lock(OsFile *aOsFile, int aLockType)
	{
  	CDbFile& dbFile = CDbFile::Instance(aOsFile);
	//If there is already a lock of this type or more restrictive on the OsFile, then - do nothing.
	if(dbFile.LockType() >= aLockType)
		{
		return SQLITE_OK;
		}

	//The file flushing here must be done in order to get the RFileBuf object content (iFileBuf data member))
	//synchronised with the database file content (the database file content may get modified by a different connection
	//at the same time).
	if(aLockType == SHARED_LOCK && !dbFile.iReadOnly)
		{
		TRAPD(err, dbFile.FlushL());
		::NotifySqliteIfNoMemoryErr(err);
		if(err != KErrNone)
			{
			return SQLITE_IOERR;	
			}
		}
	
	//Make sure the locking sequence is correct
	__SQLITEASSERT(dbFile.LockType() != NO_LOCK || aLockType == SHARED_LOCK, ESqlitePanicLockInvalid);
	__SQLITEASSERT(aLockType != PENDING_LOCK, ESqlitePanicLockInvalid);
	__SQLITEASSERT(aLockType != RESERVED_LOCK || dbFile.LockType() == SHARED_LOCK, ESqlitePanicLockInvalid);
		
	TInt rc = SQLITE_OK;    //Return code from subroutines
	TBool locked = ETrue;   //Result of a file lock call (the default value means: "lock accuired")
  	TInt newLockType = -1;	//Set dbFile.LockType() to this value before exiting
	TBool gotPendingLock = EFalse;//True if we acquired a PENDING lock this time

	//Lock the PENDING_LOCK byte if we need to acquire a PENDING lock or
	//SHARED lock. If we are acquiring a SHARED lock, the acquisition of
	//the PENDING_LOCK byte is temporary.
	newLockType = dbFile.LockType();
	if(dbFile.LockType() == NO_LOCK || (aLockType == EXCLUSIVE_LOCK && dbFile.LockType() == RESERVED_LOCK))
		{
		//Try 3 times to get the pending lock.  The pending lock might be
		//held by another reader process who will release it momentarily.
		const TInt KLockTryCnt = 3;
		locked = EFalse;
		for(TInt i=0;i<KLockTryCnt&&!locked;++i)
			{
			locked = (dbFile.Lock(PENDING_BYTE, 1) == KErrNone);
			if(!locked)
				{
				(void)sqlite3SymbianSleep(1);
				}
			}
		gotPendingLock = locked;
		}

	//Acquire a shared lock
	if(aLockType == SHARED_LOCK && locked)
		{
		__SQLITEASSERT(dbFile.LockType() == NO_LOCK, ESqlitePanicLockInvalid);
		locked = (dbFile.GetReadLock() == KErrNone);
		if(locked)
			{
			newLockType = SHARED_LOCK;
			}
  		}

	//Acquire a RESERVED lock
	if(aLockType == RESERVED_LOCK && locked)
		{
		__SQLITEASSERT(dbFile.LockType() == SHARED_LOCK, ESqlitePanicLockInvalid);
    	locked = (dbFile.Lock(RESERVED_BYTE, 1) == KErrNone);
		if(locked)
			{
			newLockType = RESERVED_LOCK;
			}
		}

	// Acquire a PENDING lock
	if(aLockType == EXCLUSIVE_LOCK && locked)
		{
		newLockType = PENDING_LOCK;
		gotPendingLock = EFalse;
		}

	//Acquire an EXCLUSIVE lock
	if(aLockType == EXCLUSIVE_LOCK && locked)
		{
		__SQLITEASSERT(dbFile.LockType() >= SHARED_LOCK, ESqlitePanicLockInvalid);
		(void)dbFile.UnlockReadLock();
		locked = (dbFile.Lock(SHARED_FIRST, SHARED_SIZE) == KErrNone);
		if(locked)
			{
			newLockType = EXCLUSIVE_LOCK;
			}
		}

	// If we are holding a PENDING lock that ought to be released, then
	// release it now.
	if(gotPendingLock && aLockType == SHARED_LOCK)
		{
		(void)dbFile.Unlock(PENDING_BYTE, 1);
  		}

	// Update the state of the lock has held in the file descriptor then
	// return the appropriate result code.
	rc = locked ? SQLITE_OK : SQLITE_BUSY;
	dbFile.SetLockType(newLockType);
	return rc;
	}

//Used in the initialization of "TheIoMethods" structure.
//This routine checks if there is a RESERVED lock held on the specified
//file by this or any other process. If such a lock is held, return
//non-zero, otherwise zero.
/* static */ int CDbFile::CheckReservedLock(OsFile *aOsFile)
	{
  	CDbFile& dbFile = CDbFile::Instance(aOsFile);
	TInt rc;
	if(dbFile.LockType() >= RESERVED_LOCK)
		{
		rc = 1;
  		}
	else
		{
    	rc = (dbFile.Lock(RESERVED_BYTE, 1) == KErrNone);
		if(rc) //non-zero rc means: the lock has been successful (there wasn't a reserved lock on this file)
			{
			(void)dbFile.Unlock(RESERVED_BYTE, 1);
			}
    	rc = !rc;
		}
	return rc;
	}

//Used in the initialization of "TheIoMethods" structure.
//Lower the locking level on file descriptor id to locktype.  locktype
//must be either NO_LOCK or SHARED_LOCK.
//
//If the locking level of the file descriptor is already at or below
//the requested locking level, this routine is a no-op.
//
//It is not possible for this routine to fail if the second argument
//is NO_LOCK.  If the second argument is SHARED_LOCK then this routine
//might return SQLITE_IOERR;
/* static */ int CDbFile::Unlock(OsFile *aOsFile, int aLockType)
	{
	__SQLITEASSERT(aLockType <= SHARED_LOCK, ESqlitePanicLockInvalid);
	
	TInt rc = SQLITE_OK;
  	CDbFile& dbFile = CDbFile::Instance(aOsFile);
	TInt currLockType = dbFile.LockType();
	
	if(currLockType >= EXCLUSIVE_LOCK)
		{
		(void)dbFile.Unlock(SHARED_FIRST, SHARED_SIZE);
    	if(aLockType == SHARED_LOCK && !(dbFile.GetReadLock() == KErrNone))
    		{
			//This should never happen. We should always be able to reacquire the read lock
			rc = SQLITE_IOERR;
			}
		}
	if(currLockType >= RESERVED_LOCK)
		{
    	(void)dbFile.Unlock(RESERVED_BYTE, 1);
		}
	if(aLockType == NO_LOCK && currLockType >= SHARED_LOCK)
		{
		dbFile.UnlockReadLock();
		}
	if(currLockType>= PENDING_LOCK)
		{
		(void)dbFile.Unlock(PENDING_BYTE, 1);
		}
		
	dbFile.SetLockType(aLockType);
	return rc;
	}

//Used in the initialization of "TheIoMethods" structure.
// Return an integer that indices the type of lock currently held
// by this handle.  (Used for testing and analysis only.)
/* static */ int CDbFile::LockState(OsFile *aOsFile)
	{
  	return  CDbFile::Instance(aOsFile).LockType();
	}

//
//         FILE LOCKING  - END         //
//
	
//Used in the initialization of "TheIoMethods" structure.
// The fullSync option is meaningless on Symbian. This is a no-op.
/* static */ void CDbFile::SetFullSync(OsFile*, int)
	{
	}

//Used in the initialization of "TheIoMethods" structure.
// Return the underlying file handle for an OsFile
/* static */ int CDbFile::FileHandle(OsFile *aOsFile)
	{
  	return CDbFile::Instance(aOsFile).Handle();
	}

//Used in the initialization of "TheIoMethods" structure.
//Return the sector size in bytes of the underlying block device for
//the specified file. This is almost always 512 bytes, but may be
//larger for some devices.
//
//SQLite code assumes this function cannot fail. It also assumes that
//if two files are created in the same file-system directory (i.e.
//a database and it's journal file) that the sector size will be the
//same for both.
/* static */ int CDbFile::SectorSize(OsFile *aOsFile)
	{
	return CDbFile::Instance(aOsFile).SectorSize();
	}

//TheIoMethods holds a pointers to the file functions used later for initialization of 
//OsFile::pMethod data member.
static const IoMethod TheIoMethods = 
	{
	&CDbFile::Close,
	&CDbFile::OpenDirectory,
	&CDbFile::Read,
	&CDbFile::Write,
	&CDbFile::Seek,
	&CDbFile::Truncate,
	&CDbFile::Sync,
	&CDbFile::SetFullSync,
	&CDbFile::FileHandle,
	&CDbFile::FileSize,
	&CDbFile::Lock,
	&CDbFile::Unlock,
	&CDbFile::LockState,
	&CDbFile::CheckReservedLock,
	&CDbFile::SectorSize
	};
	
CDbFile::CDbFile() :
	iFileBuf(KFileBufSize),
	iDeleteOnClose(EFalse),
	iLockType(NO_LOCK),
    iSharedLockByte(0),
	iFilePos(0),
	iReadOnly(EFalse),
	iSectorSize(0)
	{
	pMethod = &TheIoMethods;
	}

CDbFile& CDbFile::Instance(void* aDbFile)
	{
  	__ASSERT_DEBUG(aDbFile != NULL, User::Invariant());
  	return *(static_cast <CDbFile*> (aDbFile));
	}

//Initializes iSectorSize data member.
void CDbFile::RetrieveSectorSize()
	{
	__SQLITEASSERT(iSectorSize == 0, ESqlitePanicInvalidArg);
	iSectorSize = SQLITE_DEFAULT_SECTOR_SIZE;
	TInt driveNo;
	TDriveInfo driveInfo;
	if(iFileBuf.File().Drive(driveNo, driveInfo) == KErrNone)
		{
		__SQLITEASSERT(driveNo >= EDriveA && driveNo <= EDriveZ, ESqlitePanicInvalidDrive);
		iSectorSize = ::GetSectorSize(iFs, driveNo, driveInfo);
		}
	__SQLITEASSERT(iSectorSize >= SQLITE_DEFAULT_SECTOR_SIZE && (iSectorSize & (iSectorSize - 1)) == 0, ESqlitePanicInvalidSectorSize);
	}

//
//       SQLITE OS porting layer, API definitions    //
//

//Delete the named file.
//aFilename is expected to be UTF8 encoded, zero terminated string.
//
//If the file name contains handles, then:
// - if the file name is the main db or the journal - then set the file size to 0;
// - if the file name is a master journal - then return an error;
//In all other cases the file will be deleted.
int sqlite3SymbianDelete(const char *aFileName)
	{
	SimulateIOError(return SQLITE_IOERR_DELETE);
	TFhType fhType = FhStringProps(aFileName);
	TFileName fname;
	if(!ConvertToUnicode(aFileName, fname))
		{
		return SQLITE_ERROR;	
		}
	switch(fhType)
		{
		case EFhMainDb:
		case EFhJournal:
			{
			RFile file;
			TInt err = FhExtractFile(fname, fhType, file);
			if(err == KErrNone)
				{
				err = file.SetSize(0);	
				}
			::NotifySqliteIfNoMemoryErr(err);
			return err == KErrNone ? SQLITE_OK : SQLITE_IOERR;
			}
		case EFhMasterJournal:
			return SQLITE_ERROR;
		case EFhNone:
		default:
			break;
		}
	GET_FS(fs, SQLITE_IOERR);
	TInt err = fs.Delete(fname);
	::NotifySqliteIfNoMemoryErr(err);
	return err == KErrNone ? SQLITE_OK : SQLITE_IOERR;
	}

//Return TRUE if the named file exists.
//aFilename is expected to be UTF8 encoded, zero terminated string.
//Returns:
//     0 -  a file with "aFileName" name  does not exist or RFs()::Entry() call failed;
// non-0 -  a file with "aFileName" exists;
//
//If the file name contains handles, then:
// - if the file name is the main db or the journal - the file is reported as an existing one if the file size is > 0;
// - if the file name is a master journal - then return an error;
//In all other cases RFs::Entry() will be used to check whether the file exists or not.
int sqlite3SymbianFileExists(const char *aFileName)
	{
	TBool res = EFalse;
	TFhType fhType = FhStringProps(aFileName);
	TFileName fname;
	if(ConvertToUnicode(aFileName, fname))
		{
		switch(fhType)
			{
			case EFhMainDb:
			case EFhJournal:
				{
				TInt size = 0;
				RFile file;
				TInt err = FhExtractFile(fname, fhType, file);
				if(err == KErrNone)
					{
					err = file.Size(size);	
					}
				::NotifySqliteIfNoMemoryErr(err);
				if(err == KErrNone)
					{
					return size > 0;	
					}
				return EFalse;
				}
			case EFhMasterJournal:
				return EFalse;
			case EFhNone:
			default:
				break;
			}
		GET_FS(fs, EFalse);
		TEntry entry;
		res = fs.Entry(fname, entry) == KErrNone;
		}
	return res;
	}

//All possible "file open" operations
enum TOpenFileOpType {EOpenReadWrite, EOpenExclusive, EOpenReadOnly};

//File open function
//aReadOnly flag is an output parameter, indicating wheter the file was open in read-only mode or not
//It is a non-null pointer only for EOpenReadWrite operations.
//If the file name contains handles, then:
// - if the file name is the main db or the journal - the related file handle will be extracted from teh file name, 
//   duplicated and the used. The duplication is needed in order to close the file safely, without any complications;
// - if the file name is a master journal - then return an error;
static TInt DoOpenFile(TOpenFileOpType aOpType, const char *aFileName, OsFile** aOsFile, int* aReadOnly, int aDeleteOnClose)
	{
	TInt err = KErrNone;
	GET_FS(fs, SQLITE_CANTOPEN);
	TFhType fhType = FhStringProps(aFileName);
	//Convert the name from UTF8 to UTF16
	TFileName fname;
	if(!ConvertToUnicode(aFileName, fname))
		{
		return SQLITE_CANTOPEN;	
		}
	//Create new, unitialized CDbFile object
	CDbFile* dbFile = CDbFile::New();
	if(!dbFile)
		{
		return SQLITE_NOMEM;
		}
	//  FILE OPEN/CREATE CODE  BEGIN  //
	//Open from handle
	if(fhType != EFhNone)
		{
		switch(fhType)
			{
			case EFhJournal:
			case EFhMainDb:
				{
				RFile file;
				err = FhDuplicateFile(fname, fhType, file);
				if(err == KErrNone)
					{
					dbFile->OpenFromHandle(file);
					if(aOpType == EOpenReadWrite)
						*aReadOnly = 0;
					}
				}
				break;
			case EFhMasterJournal:
			default:
				err = KErrArgument;
				break;	
			}
		}
	//Open for read/write		
	else if(aOpType == EOpenReadWrite)
		{
		*aReadOnly = 0;
		//If the file exists - open it, otherwise - create it.(R/W mode)
		//The reason that "Open" and "Create" calls are packed in a "for" loop is:
		//1) Current thread calls  dbFile->Open() and the returned error code is KErrNotFound. Then the thread will try to create the file.
		//2) But another thread takes the CPU time and creates the file before the curent thread.
		//3) Current thread tries to create the file but gets KErrAlreadyExists error code.
		//4) Then the current thread has to call dbFile->Open() again to open the file if it already exists.
		TInt cnt = 0;//This is to prevent test hanging during "file I/O failure" testing
		const TInt KMaxAttemptCount = 3;//Try at most KMaxAttemptCount times to open/create the file
		for(err=KErrAlreadyExists;(err==KErrAlreadyExists)&&(cnt<KMaxAttemptCount);++cnt)
			{
			if((err = dbFile->Open(fs, fname)) == KErrNotFound)
				{
				err = dbFile->Create(fs, fname);
				}
			}
		if(err != KErrNone && err != KErrNoMemory) 
			{
			TInt prevErr = err;
			err = dbFile->OpenReadOnly(fs, fname);
			if(err == KErrNone)
				{
				*aReadOnly = 1;
				}
			else if(prevErr == KErrAccessDenied) //this is attempt to create a file on a read-only drive
				{
				err = KErrAccessDenied;
				}
			}
		}
	//Open for exclusive access
	else if(aOpType == EOpenExclusive)
		{
		err = dbFile->CreateExclusive(fs, fname, aDeleteOnClose);
		}
	//Open for read-only access
	else if(aOpType == EOpenReadOnly)
		{
		err = dbFile->OpenReadOnly(fs, fname);
		}
	else
		{
		__SQLITEASSERT(0, ESqlitePanicInvalidOpType);
		}
	//  FILE OPEN/CREATE CODE  END    //
	if(err != KErrNone)
		{
		delete dbFile;
		return err == KErrNoMemory ? SQLITE_NOMEM : SQLITE_CANTOPEN;
		}
	__SQLITEASSERT(dbFile != NULL, ESqlitePanicInvalidOpType);
	dbFile->RetrieveSectorSize();		
	*aOsFile = dbFile;
	OpenCounter(+1);
	return SQLITE_OK;
	}

// Attempt to open a file for both reading and writing.  If that
// fails, try opening it read-only.  If the file does not exist,
// try to create it.
//
// On success, a handle for the open file is written to *aOsFile
// and *aReadOnly is set to 0 if the file was opened for reading and
// writing or 1 if the file was opened read-only.  The function returns
// SQLITE_OK.
//
// On failure, the function returns SQLITE_CANTOPEN and leaves
// *aOsFile and *aReadOnly unchanged.
int sqlite3SymbianOpenReadWrite(const char *aFileName, OsFile** aOsFile, int* aReadOnly)
	{
	return DoOpenFile(EOpenReadWrite, aFileName, aOsFile, aReadOnly, 0);
	}

// Attempt to open a new file for exclusive access by this process.
// The file will be opened for both reading and writing.  To avoid
// a potential security problem, we do not allow the file to have
// previously existed.  Nor do we allow the file to be a symbolic
// link.
//
// If aDelFlag is true, then make arrangements to automatically delete
// the file when it is closed.
//
// On success, write the file handle into *aOsFile and return SQLITE_OK.
//
// On failure, return SQLITE_CANTOPEN.
int sqlite3SymbianOpenExclusive(const char *aFileName, OsFile** aOsFile, int aDelFlag)
	{
	return DoOpenFile(EOpenExclusive, aFileName, aOsFile, NULL, aDelFlag);
	}

// Attempt to open a new file for read-only access.
//
// On success, write the file handle into *aOsFile and return SQLITE_OK.
//
// On failure, return SQLITE_CANTOPEN.
int sqlite3SymbianOpenReadOnly(const char *aFileName, OsFile** aOsFile)
	{
	return DoOpenFile(EOpenReadOnly, aFileName, aOsFile, NULL, 0);
	}

//Create a temporary file name in aBuf.  aBuf must be big enough to
//hold at least SQLITE_TEMPNAME_SIZE characters.
//After the call aBuf will hold the temporary file name, UTF8 encoded, zero terminated string.
//The function does not use "sqlite3_temp_directory" global variable. All temporary files will
//be created in the process's private data cage.
int sqlite3SymbianTempFileName(char *aBuf)
	{
	GET_FS(fs, SQLITE_ERROR);
	TFileName defaultPath;
	TInt err = GetDefaultPath(fs, defaultPath);
	if(err != KErrNone)
		{
		return SQLITE_ERROR;	
		}
	TBuf<SQLITE_TEMPNAME_SIZE> tmpFileName;
	tmpFileName.Copy(defaultPath);
	const TInt KFileNamePos = tmpFileName.Length();
	TUint32 randomVal = Math::Random(); 
	TInt64 seed = (TInt64)randomVal;
	const TInt KFileNameLen = 15;
	tmpFileName.SetLength(tmpFileName.Length() + KFileNameLen);
	
	for(;;)
		{
		TInt pos = KFileNamePos;
    	for(TInt i=0;i<KFileNameLen;++i,++pos)
			{
			TInt j = Math::Rand(seed) % (sizeof(TheChars) - 1);
      		tmpFileName[pos] = TheChars[j];
    		}
  		TUint attr;
  		TInt err = fs.Att(tmpFileName, attr);
  		
  		//In case of a persistent file system I/O failure, the outer for loop never ends. 
  		//So the code was changed to check for error code & accordingly return SQL error.
    	if(err == KErrNotFound)
    		{
    		break;
    		}
    	else
    		{
    		return SQLITE_IOERR;
    		}
  		}

	//No need to convert the temporary file name to its unicode presentation: the file name contains only 
	//ASCII characters!!!
	TPtr8 dest(reinterpret_cast <TUint8*> (aBuf), SQLITE_TEMPNAME_SIZE);
	dest.Copy(tmpFileName);
	dest.Append(TChar(0));
  	
  	return SQLITE_OK; 
	}

//Sync the directory zDirname. This is a no-op on operating systems other
//than UNIX.
int sqlite3SymbianSyncDirectory(const char* /*aDirName*/)
	{
  	SimulateIOError(return SQLITE_IOERR_DIR_FSYNC);
	return SQLITE_OK;
	}
	
#ifndef SQLITE_OMIT_PAGER_PRAGMAS

//Check that a given pathname is a directory and is writable.
//aDirName is expected to be UTF8 encoded, zero terminated string.
int sqlite3SymbianIsDirWritable(char *aDirName)
	{
	GET_FS(fs, 0);
	int res = 0;
	TFileName dirName;
	if(ConvertToUnicode(aDirName, dirName))
		{
		TEntry entry;
		if(fs.Entry(dirName, entry) == KErrNone)
			{
			if(entry.IsDir() && !entry.IsReadOnly())
				{
				res = 1;	
				}
			}
		}
	return res;
	}

#endif//SQLITE_OMIT_PAGER_PRAGMAS

//Turn a relative pathname into a full pathname.  Return a pointer
//to the full pathname stored in space obtained from sqliteMalloc().
//The calling function is responsible for freeing this space once it
//is no longer needed.
//
//The input file name is expected to be UTF8, zero-terminated. The output file name will be UTF8, zero-terminated.
//If the file name contans handles, then a file name copy will be created and returned.
char *sqlite3SymbianFullPathname(const char *aRelative)
	{
	if(!aRelative)	//NULL argument
		{
		return 0;
		}
		
	TFhType fhType = FhStringProps(aRelative);
	if(fhType != EFhNone)
		{
		char* result = static_cast <char*> (sqliteMalloc(strlen(aRelative) + 1));
		if(result)
			{
			strcpy(result, aRelative);
			}
		return result;
		}
	//Convert the received file name to UTF16
	TBuf<KMaxFileName + 1> fname;
	if(!ConvertToUnicode(aRelative, fname))
		{
		return 0;
		}
	//Search if the file name begins with ".\" - current directory
	if(fname.Find(KCwd) == 0)
		{
		fname.Delete(0, KCwd().Length());
		}
	//Zero-terminate the converted file name
	fname.Append(TChar(0));
	char* result = static_cast <char*> (sqliteMalloc(KMaxFileName + 1));
	if(result)
		{
		RFs fs;
		if(COsThreadData::Instance().GetFs(fs) == KErrNone)
			{
			TFileName defaultPath;
			if(GetDefaultPath(fs, defaultPath) == KErrNone)
				{
				TParse parse;
				(void)parse.Set(fname, &defaultPath, 0);//If fname does not have a path, defaultPath will be used
				TPtr8 dest8(reinterpret_cast <TUint8*> (result), KMaxFileName + 1);	
				if(ConvertFromUnicode(parse.FullName(), dest8))
					{
					return result;
					}
				}
			}
		}
	sqliteFree(result);
	return 0;
	}

// ***************************************************************************
// ** Everything above deals with file I/O.  Everything that follows deals
// ** with other miscellanous aspects of the operating system interface
// ***************************************************************************

//Get information to seed the random number generator.  The seed
//is written into the buffer aBuf[256].  The calling function must
//supply a sufficiently large buffer.
int sqlite3SymbianRandomSeed(char *aBuf)
	{
	//We have to initialize aBuf to prevent valgrind from reporting
	//errors.  The reports issued by valgrind are incorrect - we would
	//prefer that the randomness be increased by making use of the
	//uninitialized space in aBuf - but valgrind errors tend to worry
	//some users.  Rather than argue, it seems easier just to initialize
	//the whole array and silence valgrind, even if that means less randomness
	//in the random seed.
	//
	//When testing, initializing aBuf[] to zero is all we do.  That means
	//that we always use the same random number sequence.* This makes the
	//tests repeatable.
	Mem::FillZ(aBuf, 256);
	TUint32 randomVal[2];
	randomVal[0] = Math::Random();
	randomVal[1] = Math::Random();
	Mem::Copy(aBuf, randomVal, sizeof(randomVal));
	return SQLITE_OK;
	}

//Sleep for a little while.  Return the amount of time slept.
int sqlite3SymbianSleep(int ms)
	{
	User::AfterHighRes(TTimeIntervalMicroSeconds32(ms * 1000));
	return ms;
	}

//
//         MUTEX - BEGIN       //
//

//The following pair of routines implement mutual exclusion for
//multi-threaded processes.  Only a single thread is allowed to
//executed code that is surrounded by EnterMutex() and LeaveMutex().
//
//SQLite uses only a single Mutex.  There is not much critical
//code and what little there is executes quickly and without blocking.
//
//Version 3.3.1 and earlier used a simple mutex.  Beginning with
//version 3.3.2, a recursive mutex is required.
void sqlite3SymbianEnterMutex()
	{
	TheMutex.Lock();
	}
	
void sqlite3SymbianLeaveMutex()
	{
	TheMutex.Unlock();
	}

//Return TRUE if the mutex is currently held.
//
//If the thisThrd parameter is true, return true only if the
//calling thread holds the mutex.  If the parameter is false, return
//true if any thread holds the mutex.
int sqlite3SymbianInMutex(int aThisThreadOnly)
	{
	return TheMutex.IsLocked(aThisThreadOnly);
	}

//
//         MUTEX - END       //
//

//Find the current time (in Universal Coordinated Time).  Write the
//current time and date as a Julian Day number into *prNow and
//return 0.  Return 1 if the time and date cannot be found.
int sqlite3SymbianCurrentTime(double *prNow)
	{
	TTime now;
	now.UniversalTime();
	TDateTime date = now.DateTime();
	TInt year = date.Year(), month = date.Month() + 1, day = date.Day() + 1;
	
	TInt jd = ( 1461 * ( year + 4800 + ( month - 14 ) / 12 ) ) / 4 +
          ( 367 * ( month - 2 - 12 * ( ( month - 14 ) / 12 ) ) ) / 12 -
          ( 3 * ( ( year + 4900 + ( month - 14 ) / 12 ) / 100 ) ) / 4 +
          day - 32075;
          
	*prNow = jd;
#ifdef SQLITE_TEST
	if( sqlite3_current_time )
		{
		*prNow = sqlite3_current_time / 86400.0 + 2440587.5;
		}
#endif
  return 0;
}

// If called with aAllocateFlag>1, then return a pointer to thread
// specific data for the current thread.  Allocate and zero the
// thread-specific data if it does not already exist necessary.
//
// If called with aAllocateFlag==0, then check the current thread
// specific data.  Return it if it exists.  If it does not exist,
// then return NULL.
//
// If called with aAllocateFlag<0, check to see if the thread specific
// data is allocated and is all zero.  If it is then deallocate it.
// Return a pointer to the thread specific data or NULL if it is
// unallocated or gets deallocated.
ThreadData* sqlite3SymbianThreadSpecificData(int aAllocateFlag)
	{
	COsThreadData* osThreadData = static_cast <COsThreadData*> (UserSvr::DllTls(TlsHandle()));
	if(aAllocateFlag > 0)
		{
		if(!osThreadData)
			{
			osThreadData = COsThreadData::New();
			if(!osThreadData)
				{
				return 0;	
				}
			TSD_COUNTER_INCR();
			}
		return &osThreadData->SqliteData();
		}
	else if(aAllocateFlag == 0)
		{
		return osThreadData ? &osThreadData->SqliteData() : NULL;
		}
	//
	if(osThreadData)
		{
		if(Mem::Compare(reinterpret_cast <const TUint8*> (&osThreadData->SqliteData()), sizeof(ThreadData), 
					    reinterpret_cast <const TUint8*> (&KZeroThreadData), sizeof(ThreadData)) == 0)
			{
			TSD_COUNTER_DECR();
			delete osThreadData;
			return 0;
			}
		return &osThreadData->SqliteData();
		}
	return 0;
	}

//
//       SQLITE OS porting layer      //
//       memory allocation routines    //
//

/**
*/
void* sqlite3SymbianMalloc(int aSize)
	{
	return User::Alloc(aSize);
	}
	
/**
*/
void* sqlite3SymbianRealloc(void* aPtr, int aSize)
	{
	return User::ReAlloc(aPtr, aSize);
	}
	
/**
*/
void sqlite3SymbianFree(void* aPtr)
	{
	User::Free(aPtr);
	}
	
/**
*/
int sqlite3SymbianAllocationSize(void* aPtr)
	{
	return aPtr ? User::AllocLen(aPtr) : 0;
	}

//

#endif//OS_SYMBIAN

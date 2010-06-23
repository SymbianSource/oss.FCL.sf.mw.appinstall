// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Provides a Symbian version of the main program and Tcl_AppInit
// procedure for Tcl applications (without Tk).
// 
//

#include "tcl.h"
#include "tclPort.h"
#include "tclInt.h"
#include "tclIntPlatDecls.h"
#include <e32test.h>

#ifdef __WINSCW__ 
#include <e32std.h>     //RPointerArray

#include <pls.h> // For emulator WSD API 
const TUid KTCLDLLUid3 = {0}; // Must change
const TInt KMaxDataKey = 10;
#endif // __WINSCW__

/*
 * The following macros convert between TclFile's and fd's.  The conversion
 * simple involves shifting fd's up by one to ensure that no valid fd is ever
 * the same as NULL.  Note that this code is duplicated from tclUnixPipe.c
 */

#define MakeFile(fd) ((TclFile)((fd)+1))
#define GetFd(file) (((int)file)-1)

#ifdef __WINSCW__ 
//The following code will run only on the emulator

//Put the global count into a structure
struct DLLData
{
	// TCL globals
	char* tclExecutableName;
	char* tclNativeExecutableName;

	void* dataKey[KMaxDataKey];
	int inFinalize;
	int subsystemsInitialized;
	void* allocHead;	
	void* defaultEncoding;
	void* systemEncoding;
	Tcl_HashTable encodingTable;	
	SyncObjRecord keyRecord;
	Tcl_HashTable typeTable;
	int typeTableInitialized;
	int encodingsInitialized;	
	char* tclDefaultEncodingDir;
	char* tclLibraryPathStr;	
	int opTableInitialized;
	Tcl_HashTable opHashTable;
	Tcl_HashTable auxDataTypeTable;
	int auxDataTypeTableInitialized;
	void* cwdPathPtr;
	int cwdPathEpoch;
	void* refArray;
	int spaceAvl;
	int inUse;
	TclPlatformType tclPlatform;	
	void* firstNotifierPtr;
	
	// Symbian globals
	char fileNames[8][L_tmpnam + 9];
};

//Initialization function
TInt InitializeGlobals(DLLData* aData)
{
   memset(aData, 0, sizeof(DLLData));
   aData->tclPlatform = TCL_PLATFORM_UNIX;
   return KErrNone;
}

//Define a way to access the structure
//On the first call to this function, memory will be allocated with the specified
//Uid as an identifier and the Initialization function will be called
//Subsequent calls to this function return the allocated memory
struct DLLData* GetGlobals()
{
   return Pls<DLLData>(KTCLDLLUid3, InitializeGlobals);
}

//Clean up memory allocated for PLS used for storing globals
int CleanupGlobals(void)
{
	return FreePls(GetGlobals());
}

void* get_gFileName(int index)
{
   return &(GetGlobals()->fileNames[index]);
}

char** get_tclExecutableName()
{
   return &(GetGlobals()->tclExecutableName);
}

char** get_tclNativeExecutableName()
{
   return &(GetGlobals()->tclNativeExecutableName);
}

void** get_dataKey(int index)
{
   return &(GetGlobals()->dataKey[index]);
}

void* get_inFinalize()
{
   return &(GetGlobals()->inFinalize);
}

void* get_subsystemsInitialized()
{
   return &(GetGlobals()->subsystemsInitialized);
}

void** get_allocHead()
{
   return &(GetGlobals()->allocHead);
}

void** get_defaultEncoding()
{
   return &(GetGlobals()->defaultEncoding);
}

void** get_systemEncoding()
{
   return &(GetGlobals()->systemEncoding);
}

void* get_encodingTable()
{
   return &(GetGlobals()->encodingTable);
}

void* get_keyRecord()
{
   return &(GetGlobals()->keyRecord);
}

void* get_typeTable()
{
   return &(GetGlobals()->typeTable);
}

void* get_typeTableInitialized()
{
   return &(GetGlobals()->typeTableInitialized);
}

void* get_encodingsInitialized()
{
   return &(GetGlobals()->encodingsInitialized);
}

char** get_tclDefaultEncodingDir()
{
   return &(GetGlobals()->tclDefaultEncodingDir);
}

char** get_tclLibraryPathStr()
{
   return &(GetGlobals()->tclLibraryPathStr);
}

void* get_opTableInitialized()
{
   return &(GetGlobals()->opTableInitialized);
}

void* get_opHashTable()
{
   return &(GetGlobals()->opHashTable);
}

void* get_auxDataTypeTableInitialized()
{
   return &(GetGlobals()->auxDataTypeTableInitialized);
}

void* get_auxDataTypeTable()
{
   return &(GetGlobals()->auxDataTypeTable);
}

void** get_cwdPathPtr()
{
   return &(GetGlobals()->cwdPathPtr);
}

void* get_cwdPathEpoch()
{
   return &(GetGlobals()->cwdPathEpoch);
}

void** get_refArray()
{
   return &(GetGlobals()->refArray);
}

void* get_spaceAvl()
{
   return &(GetGlobals()->spaceAvl);
}

void* get_inUse()
{
   return &(GetGlobals()->inUse);
}

/*
 *----------------------------------------------------------------------
 *
 * TclPlatformExit --
 *
 *	This procedure implements the Symbian specific exit routine.
 *  Modelled after Macintosh version. 
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	We exit the process.
 *
 *----------------------------------------------------------------------
 */

void
TclpExit(
    int status)		/* Ignored. */
{
    // Free the PLS
    CleanupGlobals();

    exit(status);
}

void* get_tclPlatform()
{
   return &(GetGlobals()->tclPlatform);
}

void** get_firstNotifierPtr()
{
   return &(GetGlobals()->firstNotifierPtr);
}

#else
//Target device code
char tmpFileName[L_tmpnam + 9];
char fifoFileName[L_tmpnam + 9];
char inFileName[L_tmpnam + 9];
char outFileName[L_tmpnam + 9];
char errFileName[L_tmpnam + 9];
char inFileName1[L_tmpnam + 9];
char outFileName1[L_tmpnam + 9];
char errFileName1[L_tmpnam + 9];	

#endif

#include "tclSymbianGlobals.h"

#ifdef __cplusplus
extern "C" {
#endif

EXPORT_C void ChildProcessCleanup(int isChildProcess, int argc, char **argv)
{
	int i;
    // add fifo close & unlink
    if (isChildProcess == 1)
    {
    	close (TCL_STDIN);
    	close (TCL_STDOUT);
    	close (TCL_STDERR);
        for (i=0; i<=2; i++)
    	{
    		
    		if (argv[i])
    		{
   				if ((i != 1)||(strcmp(inFileName, outFileName) != 0))
    			{
    				unlink(argv[argc-i]);	
    			}
    		}
    	}
    	
    }
    else
    {
    	unlink(inFileName1);
    	unlink(outFileName1);
    	unlink(errFileName1);
    }
}

// Symbian main hook for tclappinit
EXPORT_C int ChildProcessInit (int *argc, char ***argv)
{
    //set the stdin,stdout,stderr to the child process. the fds pass to the posix_spawn() in argv
    TclFile inputFile = NULL;
    TclFile outputFile= NULL;
    TclFile errorFile = NULL;
    int joinThisError;
    int fd[4] = {0, 0, 0, 0};
    char errSpace[200 + TCL_INTEGER_SPACE];
    int anerr = 0;

   //set the stdin,stdout,stderr and pipeid to the child process. the fds pass to the posix_spawn() in argv
	if (*argc >= 5)
		{
		// fifoFile
		if((*argv)[*argc-4])
		{
			fd[0] = open((*argv)[*argc-4],O_WRONLY);
			if (fd[0] == -1)
		    {
		    	fprintf(stderr, "fd[0](fifoFile) errno is %d\r\n", errno);
		    }
			else
				{
				fprintf(stderr, "fifoFile is %s fd[0] is %d\r\n", (*argv)[*argc-4],fd[0]);				
				}
		    //fd = atoi((*argv)[*argc-1]);
		}
		else
		{
			//should add later
		}
		// inputFile
		if(((*argv)[*argc-3])&&(strcmp((*argv)[*argc-3],"STD")))
			{
			fd[3] = open((*argv)[*argc-3],O_RDONLY); 
			inputFile = MakeFile(fd[3]);
			if (fd[3] == -1)
				{
		    	fprintf(stderr,"fd[3](inputFile) errno is %d\r\n", errno);
				}
			else
				{
				fprintf(stderr, "inputFile is %s fd[3] is %d\r\n", (*argv)[*argc-3],fd[3]);					
				}
			    //inputFile = (TclFile) (atoi((*argv)[*argc-4]));
			}
			else
				{
				//should add later
				}
		// outputFile
		if(((*argv)[*argc-2])&&(strcmp((*argv)[*argc-2],"STD")))
			{
			fd[2] = open((*argv)[*argc-2],O_WRONLY);
			outputFile = MakeFile(fd[2]);
		    if (fd[2] == -1)
		    	{
		    	fprintf(stderr, "fd[2](outputFile) errno is %d\r\n", errno);
		    	}
		    else
				{
				fprintf(stderr, "outputFile is %s fd[2] is %d\r\n", (*argv)[*argc-2],fd[2]);					
				}
		    
			//outputFile = (TclFile) (atoi((*argv)[*argc-3]));
			}
		else
			{
			//should add later
			//outputFile = MakeFile(1);
			}
		// errorFile
		if(((*argv)[*argc-1])&&(strcmp((*argv)[*argc-1],"STD")))
		{
			fd[1] = open((*argv)[*argc-1],O_WRONLY);
			errorFile = MakeFile(fd[1]);
			if (fd[1] == -1)
				{
		    	fprintf(stderr, "fd[1] errorFile errno is %d\r\n", errno);
				}
			else
				{
				fprintf(stderr,	"errorFile is %s fd[1] is %d\r\n", (*argv)[*argc-1],fd[1]);
				}
		    //errorFile = (TclFile) (atoi((*argv)[*argc-2]));
		}
		else
		{
			//should add later
		}
		//*argc = *argc-4;
		
		joinThisError = errorFile && (errorFile == outputFile);

		//fd = GetFd(errPipeOut);
    
		//
		// Set up stdio file handles for the child process.
		//

		if (!SetupStdFile(inputFile, TCL_STDIN)
			|| !SetupStdFile(outputFile, TCL_STDOUT)
			|| (!joinThisError && !SetupStdFile(errorFile, TCL_STDERR))
			|| (joinThisError &&
				((dup2(1,2) == -1) ||
				 (fcntl(2, F_SETFD, 0) != 0)))) 
		  //if (!SetupStdFile(errorFile, TCL_STDERR))
		  {
	  	  fprintf(stderr, "child process couldn't set up input/output, error: %d\r\n", errno);
		  sprintf(errSpace,"child process couldn't set up input/output, error: %d\r\n", errno);
	   	 write(fd[0], errSpace, (size_t) strlen(errSpace));
	   	 close(fd[0]);
	   	 unlink((*argv)[*argc-4]);
	   	 _exit(1);
		}

		sprintf(errSpace,"OK\r\n");
		write(fd[0], errSpace, (size_t) strlen(errSpace));
		anerr = close(fd[0]);
		anerr = unlink((*argv)[*argc-4]);
	   	return 1;
	}
    
    return 0;			
}

void TclPrint1(const char* aFmt, const char* aStr)
	{
	TBuf<128> fmt;
	fmt.Copy(TPtrC8((const TUint8*)aFmt));
	TBuf<128> str;
	str.Copy(TPtrC8((const TUint8*)aStr));
	RDebug::Print(fmt, &str);	
	}

void TclPrint2(const char* aFmt, const char* aStr, int aNum)
	{
	TBuf<128> fmt;
	fmt.Copy(TPtrC8((const TUint8*)aFmt));
	TBuf<128> str;
	str.Copy(TPtrC8((const TUint8*)aStr));
	RDebug::Print(fmt, &str, aNum);	
	}

void TclPrint3(const char* aFmt)
	{
	TBuf<128> fmt;
	fmt.Copy(TPtrC8((const TUint8*)aFmt));
	RDebug::Print(fmt);	
	}

#ifdef __cplusplus
}
#endif

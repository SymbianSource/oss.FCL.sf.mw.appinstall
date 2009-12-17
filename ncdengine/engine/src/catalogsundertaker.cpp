/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Implementation of CCatalogsClientServerServer
*
*/


#include "catalogsserverdefines.h"
#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========

_LIT( KExitTypeKill, "Kill" );
_LIT( KExitTypeTerminate, "Terminate" );
_LIT( KExitTypePanic, "Panic" );
_LIT( KExitTypeUnknown, "Unknown" );

static TInt CatalogsUndertaker()
    {
    DLTRACEIN((""));

    TFullName catalogsThreadName;
    TInt err = User::GetDesParameter( 15, catalogsThreadName );
    
    if( err != KErrNone )
        {
        DLERROR(( "Failed to read parameter slot 15, error %d", err ));
        catalogsThreadName = KNullDesC();
        }
    else
        {
        DLINFO(( _L("Read catalogs thread name: %S"), &catalogsThreadName ));
        }

    RUndertaker undertaker;
    TRequestStatus status;
    TInt deadThreadHandleNumber;

    undertaker.Create();

    for( ;; )
        {
        undertaker.Logon( status, deadThreadHandleNumber );
        User::WaitForRequest( status );

        RThread deadThread;
        deadThread.SetHandle( deadThreadHandleNumber );

        const TDesC* type;

        switch( deadThread.ExitType() )
            {
            case EExitKill:
                type = &KExitTypeKill;
                break;

            case EExitTerminate:
                type = &KExitTypeTerminate;
                break;

            case EExitPanic:
                type = &KExitTypePanic;
                break;

            default:
                type = &KExitTypeUnknown;
                DLERROR(( "Exit type: %d", (TInt)deadThread.ExitType() ));
                break;
            }


        DLWARNING(( _L("THREAD %S DEATH observed! %S %S %d"), 
            &deadThread.FullName(),
            type,
            &deadThread.ExitCategory(),
            deadThread.ExitReason() ));
        
        type = type; // to suppress compiler warning

        if( catalogsThreadName == deadThread.FullName() )
            {
            DLERROR(( "Catalogs server thread killed, undertaker exits" ));
            deadThread.Close();
            break;
            }

        deadThread.Close();

        }

    undertaker.Close();
    DLTRACEOUT(("KErrNone"));

    return KErrNone;
    }

/**
 * Platform dependant entry points
 */

IMPORT_C TInt WinsMain();
EXPORT_C TInt WinsMain()
	{
	// WINS DLL entry-point. Just return the real thread function 
    // cast to TInt
	return reinterpret_cast<TInt>( 
	    CatalogsUndertaker );
	}

GLDEF_C TInt E32Main()
    {
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    DLINIT;
    TInt result = CatalogsUndertaker();
    DLUNINIT;
    delete cleanupStack;
    return result;
    }

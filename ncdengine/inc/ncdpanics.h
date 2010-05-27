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
* Description:   NCD Provider panic code definitions.
*
*/


#ifndef NCD_PANICS_H
#define NCD_PANICS_H

/** Catalogs Engine panic category name */
_LIT( KNcdPanicCategory, "NCD" );


/** Catalogs Engine panic category panic codes */
enum TNcdPanic
    {

    /** Raised when an invalid argument is passed to a method, caused by a programming error */
    ENcdPanicInvalidArgument = 0,
    
    /** Raised when an invalid index is passed to a method, caused by a programming error */
    ENcdPanicIndexOutOfRange,
    
    /** Raised when data that should exist, does not. Caused by a programming error */    
    ENcdPanicNoData

    };

#define NCD_ASSERT_ALWAYS( decl, panic ) if ( !(decl) ) { DLERROR(("NCD assertion failed")); User::Panic( KNcdPanicCategory, (panic) ); }

#ifdef CATALOGS_DEBUG_ASSERTS
#   define NCD_ASSERT( decl, panic ) NCD_ASSERT_ALWAYS( (decl), (panic) );
#else
#   define NCD_ASSERT( decl, panic ) 
#endif


    

#endif // NCD_PANICS_H

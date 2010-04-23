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
* Description:   Debug logging utility definitions
*
*/


#ifndef CATALOGSDEBUGDEFS_H
#define CATALOGSDEBUGDEFS_H

#include <e32def.h>
#include "catalogslogger.hrh"

_LIT( KCatalogsDebugMsgQueueName, "CatalogsLoggerMsgQueue" );
_LIT( KCatalogsDebugChunk1Name, "CatalogsLoggerChunk1" );
_LIT( KCatalogsDebugChunk2Name, "CatalogsLoggerChunk2" );
_LIT( KCatalogsDebugChunkWriteSemaphoreName, "CatalogsLoggerChunkSemaphore" );
_LIT( KCatalogsDebugMutexName, "CatalogsLoggerMutex" );
const TInt KCatalogsDebugChunkSize = 128*1024;
_LIT8( KCatalogsDebugLineSeparator, "\n" );

enum TCatalogsDebugFlags
    {
    ECatalogsDebugFlagEnableError = 1 << ECatalogsLoggerSettingError,       // DLERROR
    ECatalogsDebugFlagEnableWarning = 1 << ECatalogsLoggerSettingWarning,   // DLWARNING
    ECatalogsDebugFlagEnableTrace = 1 << ECatalogsLoggerSettingTrace,       // DLTRACEIN, DLTRACEOUT, DLTRACE
    ECatalogsDebugFlagEnableInfo = 1 << ECatalogsLoggerSettingInfo,         // DLINFO
    ECatalogsDebugFlagFlushChunk = 1 << ENrOfCatalogsLoggerSettings         // Chunk flushed
    };    

struct TCatalogsDebugChunkHeader
    {
    TUint32 iFlags;     // Bit field of TCatalogsDebugFlags
    TUint32 iOffset;    // Offset to free space in chunk (relative to after header)
    };

/*
enum TCatalogsLoggerPropertyKeys 
    {
    ECatalogsLoggerSettingsProperty
    };

const TUid KCatalogsDebugPropertyCat = { 0x0B5C26D8 };  // Same as logger application UID 
*/

#endif  //CATALOGSDEBUGDEFS_H 

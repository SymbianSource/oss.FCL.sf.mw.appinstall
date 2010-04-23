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
* Description:   ?description
*
*/


#ifndef C_CATALOGSLOGGERMSGQUEUEOBSERVER_H
#define C_CATALOGSLOGGERMSGQUEUEOBSERVER_H


#include <e32base.h>
#include <e32msgqueue.h>
#include <f32file.h>

#include "catalogsdebugdefs.h"
#include "osmobexsender.h"

class CCatalogsLoggerAppUi;

_LIT( KCatalogsLoggerFileName, "\\logs\\catalogs\\debuglogger.log" );
_LIT( KCatalogsNewLine, "\n" );

const TInt KCatalogsLoggerNrOfSlots = 100;

/**
 *  ?one_line_short_description
 *
 *  ?more_complete_description
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CCatalogsLoggerMsgQueueObserver : public CActive, public MOsmObexSenderObserver
    {
public:
    static CCatalogsLoggerMsgQueueObserver* NewL();

    static CCatalogsLoggerMsgQueueObserver* NewLC();

    virtual ~CCatalogsLoggerMsgQueueObserver();
    void StartLogging();
    void StopLogging();
    inline TBool IsLogging()
        {
        return iIsLogging;
        }

    /**
     * Sends the log file over Bluetooth
     */
    void SendFileL();

    /**
     * Clears the log file
     *
     * @return System wide error code
     */
    TInt ClearLog();

    void Flush();

    inline TInt FunCounter() const { return iFunCounter; }
    inline TInt LogDrive() const { return iLogFileName[0] == 'e' ? EDriveE : EDriveC; }

    void SetEnableFlags( TInt aFlags );

    TUint CurrentChunkOffset();
    TPtrC8 LastData( TUint aMaxSize );

protected:

// from base class CActive

    /**
     * From CActive.
     * Handles request completion.
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     */
    void RunL();

    /**
     * From CActive.
     * Cancels outstanding request.
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     */    
    void DoCancel();

protected:

    // From MOsmObexSenderObserver

    void ObexFileSent();
    void ObexDisconnected( TInt aError, TInt aState );

private:

    CCatalogsLoggerMsgQueueObserver();
    void ConstructL();

private: // data
    RMsgQueue<TInt> iMsgQueue;

    // Obex sender for sending the log file over bluetooth
    TInt iObexSendError;
    COsmObexSender* iObexSender;
    RFs iFs;
    RFile iLogFile;
    RChunk iChunk1;
    RChunk iChunk2;
    RMutex iChunkMutex;
    RSemaphore iChunkWriteSemaphore;
    TInt iLastChunk;
    TInt iFunCounter;
    TFileName iLogFileName;
    TBool iIsLogging;
    TInt iEnableFlags;
    };

#endif // C_CATALOGSLOGGERMSGQUEUEOBSERVER_H


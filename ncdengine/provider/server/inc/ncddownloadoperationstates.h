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
* Description:  
*
*/


#ifndef NCD_DOWNLOAD_OPERATION_STATES_H
#define NCD_DOWNLOAD_OPERATION_STATES_H

/**
 * Download states
 *
 */
enum TNcdDownloadState 
    {
    ENcdDownloadStopped,
    ENcdDownloadStarted,
    ENcdDownloadInProgress,
    ENcdDownloadPaused,
    ENcdDownloadFailed,        
    ENcdDownloadComplete
    };
    
const TUint KNcdDownloadContinueMessageRequired = 20000;    
const TUint KNcdDownloadStartMessageRequired = 20001;    

const TUint KNcdDownloadIsPausable = 20002;    
const TUint KNcdDownloadIsNotPausable = 20003;    
    
#endif // NCD_DOWNLOAD_OPERATION_STATES_H

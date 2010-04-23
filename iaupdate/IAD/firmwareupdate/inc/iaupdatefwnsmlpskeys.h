/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef IAUPDATE_FW_NSML_PS_KEY_H
#define IAUPDATE_FW_NSML_PS_KEY_H


//The P&S key defined here are originally defined in 
//\s60\app\devicecontrol\omadm\OmaDmAppUi\inc\nsmldmsyncinternalpskeys.h
//these keys are used by iaudpate

/**
* DM UI P&S UID
*/
const TUid KPSUidNSmlDMSyncApp = {0x101f6de5};


/**
* DM UI P&S Initiated job key
*/
const TUint32 KNSmlDMSyncUiInitiatedJobKey = 0x00000001;

// Indicates that no user initiated sync jobs are in
// progress.
const TInt KNSmlDMNoUserInitiatedJob = -1;


/**
* DM UI P&S key for FOTA download
*/
const TUint32 KFotaDLStatus = 0x00000008;


/**
* DM UI P&S key for Fota Update Application
*/
const TUint32 KNSmlCurrentFotaUpdateAppName = 0x0000000A;


#endif //IAUPDATE_FW_NSML_PS_KEY_H



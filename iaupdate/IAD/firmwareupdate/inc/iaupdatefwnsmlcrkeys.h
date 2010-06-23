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



#ifndef IAUPDATE_FW_NSML_CR_KEY_H
#define IAUPDATE_FW_NSML_CR_KEY_H


// The CR key defined here are originally defined in 
// \s60\app\devicecontrol\omadm\OmaDmAppUi\inc\NSmlDMSyncPrivateCRKeys.h
// there keys are used by iaupdate

// Device manager key UID
const TUid KCRUidNSmlDMSyncApp = {0x101f6de5};

// CenRep keys for default dm profile used in the
// firmware update over the air.
const TUint32 KNSmlDMDefaultFotaProfileKey = 0x00000002;

//: may not needed
const TUint32 KNSmlFotaProfileDefaultIsActive = 0x0000000D;


#endif //IAUPDATE_FW_NSML_CR_KEY_H



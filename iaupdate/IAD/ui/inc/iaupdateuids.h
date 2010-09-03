/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains UIDs used in Update client UI 
*
*/



#ifndef IA_UPDATE_UIDS_H
#define IA_UPDATE_UIDS_H

/**
 * Default IAUpdate UI Uid
 */
#define KIAUpdateUiUid 0x2000F85A

/**
 * Default IAUpdate Family Uid. The family uid is used with the NCD Engine.
 * Applications of the same family will see each others items.
 */
#define KIAUpdateFamilyUid KIAUpdateUiUid

/**
 * Id:s  for informing background checker about main list refresh
 */
#define KIAUpdateBgcUid1 0x200211f4    // Bg checker UID; P&S Category
const TUid KPSUidBgc = { KIAUpdateBgcUid1 };
const TUint32 KIAUpdateBGNotifyIndicatorRemove = 0x00000003; // P&S Category

#endif // IA_UPDATE_UIDS_H

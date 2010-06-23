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
* Description:   This file contains definitions used in Update client UI 
*
*/



#ifndef IA_UPDATE_UI_DEFINES_H
#define IA_UPDATE_UI_DEFINES_H

/**
 * General values for the IAD UI purposes.
 *
 */
namespace IAUpdateUiDefines
    {

     /**
      * Defines request type passed to IAD UI
      */
     enum TIAUpdateUiRequestType 
         {
         ENoRequest,         
         ECheckUpdates,
         EShowUpdates,
         EUpdateQuery 
         };


    //MACROS

    /**
     * This string is used to check if the UI should be
     * used in a testing role.
     */
    _LIT( KTestRole, "testing" );    

    }

#endif // IA_UPDATE_UI_DEFINES_H

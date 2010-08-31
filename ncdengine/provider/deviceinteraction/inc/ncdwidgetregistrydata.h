/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of CExtendedWidgetInfo
*
*/

#ifndef NCDEXTENDEDWIDGETINFO_H
#define NCDEXTENDEDWIDGETINFO_H

// INCLUDES
#include <f32file.h>
//#include <WidgetRegistryClient.h>  //HLa: Widget registry remove
#include "catalogsutils.h"

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

class CExtendedWidgetInfo : public CBase
    {
    public:
        /**
        * Constructor
        */
        inline CExtendedWidgetInfo()
            {
            iUid = TUid::Uid(0);
            //HLa: Widget registry remove
            //iVersion = HBufC::New( KWidgetRegistryVal + 1 );
            iVersion = HBufC::New( KMaxFileName + 1 ); 
            
            }
        /**
        * Destructor.
        */
        inline virtual ~CExtendedWidgetInfo()
            {
            delete iVersion;
            }

    public:
        TUid             iUid;
        HBufC*           iVersion;
    };

typedef RPointerArray<CExtendedWidgetInfo>  RExtendedWidgetInfoArray;

#endif  //NCDEXTENDEDWIDGETINFO

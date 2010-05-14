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
* Description:   Creates info objects asynchronously
*
*/


#ifndef C_APPMNGR2INFOMAKER_H
#define C_APPMNGR2INFOMAKER_H

#include <e32base.h>                    // CActive

class CAppMngr2Runtime;
class MAppMngr2InfoMakerObserver;
class RFs;


class CAppMngr2InfoMaker : public CActive
    {
public:     // constructor and destructor
    CAppMngr2InfoMaker( CAppMngr2Runtime& aPlugin, MAppMngr2InfoMakerObserver& aObserver,
            RFs& aFs );
    ~CAppMngr2InfoMaker();

public:     // new functions
    const TUid RuntimeUid() const;
    
protected:  // data
    CAppMngr2Runtime& iPlugin;
    MAppMngr2InfoMakerObserver& iObserver;
    RFs& iFs;
    };

#endif  // C_APPMNGR2INFOMAKER_H


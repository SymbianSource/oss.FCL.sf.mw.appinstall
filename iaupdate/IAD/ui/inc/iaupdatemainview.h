/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the IAUpdateMainView
*                class 
*
*/

#ifndef IAUPDATEMAINVIEW_H_
#define IAUPDATEMAINVIEW_H_

#include <HbView.h>
#include <QItemSelection> 

#include "iaupdatenode.h"
#include "iaupdatefwnode.h"
#include "iaupdatebasenode.h"


class HbListWidget;
//class HbListView;


class IAUpdateMainView : public HbView
{
    Q_OBJECT 
    
public:
    IAUpdateMainView();
    virtual ~IAUpdateMainView();
        
    void refresh( const RPointerArray<MIAUpdateNode>& nodes,
                      const RPointerArray<MIAUpdateFwNode>& fwNodes,
                      int error );
public slots:
    void handleStartUpdate();
 
    void handleSettings();
 
    void handleDisclaimer();
    
    void handleSelectionChanged(QItemSelection);
    
private:
    

    
private:
    //HbListView *mListView;
    HbListWidget *mListView;
    RPointerArray<MIAUpdateAnyNode> mAllNodes;

};

#endif /* IAUPDATEMAINVIEW_H_ */

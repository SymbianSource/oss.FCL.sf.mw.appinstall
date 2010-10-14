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
* Description:   This file contains the header file of the IAUpdateAppListItem
*                class 
*
*/

#ifndef IAUPDATEAPPLISTITEM_H_
#define IAUPDATEAPPLISTITEM_H_


#include <hblistviewitem.h>
class HbProgressBar;

class IAUpdateAppListItem : public HbListViewItem
{
    Q_OBJECT
   
public:
    IAUpdateAppListItem(QGraphicsItem * parent = 0);
    ~IAUpdateAppListItem();
    
    HbAbstractViewItem * createItem();
    
    void updateChildItems();
    
    void polish(HbStyleParameters& params);

private:
    HbProgressBar* mProgressBar;
    bool mIsProgress;
    bool mSecondaryTextVisible;
};

#endif /* IAUPDATEAPPLISTITEM_H_ */

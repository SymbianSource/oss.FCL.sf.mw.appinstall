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
* Description:   This module contains the implementation of IAUpdateAppListItem
*                class member functions.
*
*/
#include <HbProgressBar>

#include "iaupdateapplistitem.h"
#include "iaupdateapplistmodel.h"
#include "iaupdatedebug.h"

IAUpdateAppListItem::IAUpdateAppListItem(QGraphicsItem * parent) :
  HbListViewItem(parent)
{
    mProgressBar = NULL;   
    mIsProgress = false;
    mSecondaryTextVisible = true;
}

IAUpdateAppListItem::~IAUpdateAppListItem() 
{
}

HbAbstractViewItem * IAUpdateAppListItem::createItem()
{
    return new IAUpdateAppListItem(*this);
}


void IAUpdateAppListItem::updateChildItems()
{
    HbListViewItem::updateChildItems();
    
    mIsProgress = modelIndex().data(IAUpdateAppListModel::ProgressRole).toBool();
    if (mIsProgress)
    {  // progress state
        if (mSecondaryTextVisible)
            {
            foreach (QGraphicsItem * item, childItems()) 
                {
                    if (HbStyle::itemName(item) == "text-2")     
                    {
                        item->setVisible(false);
                        mSecondaryTextVisible = false;
                        break;
                    }
                }
            }
        if (!mProgressBar)
        {
            mProgressBar = new HbProgressBar(this);
            mProgressBar->setRange(0, 100);
            HbStyle::setItemName(mProgressBar, "progress");
        }
        if (!mProgressBar->isVisible())
        {
            HbStyle::setItemName(mProgressBar, "progress");
            mProgressBar->setVisible(true);
        }
        QList<QVariant> list = modelIndex().data(IAUpdateAppListModel::ProgressValueRole).toList();
        int value = list.at(0).toInt();
        int total = list.at(1).toInt();
        mProgressBar->setRange(0,total);
        mProgressBar->setProgressValue(value);
    }
    else
    {  //normal state
        if (mProgressBar)
        {
            if (mProgressBar->isVisible()) 
            { 
                HbStyle::setItemName(mProgressBar, "");
                mProgressBar->setVisible(false);
            }    
            if (!mSecondaryTextVisible)
            {    
                foreach (QGraphicsItem * item, childItems()) 
                {
                    if (HbStyle::itemName(item) == "text-2") 
                    {
                        item->setVisible(true);
                        mSecondaryTextVisible = true;
                        break;
                    }
                }    
            }
        }
    }
}


void IAUpdateAppListItem::polish(HbStyleParameters& params)
{       
    HbListViewItem::setProperty("progress", mIsProgress);
    HbListViewItem::polish(params);   
}


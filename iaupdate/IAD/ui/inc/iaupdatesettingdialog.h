/*
* Copyright (c) 2010-2011 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CIAUpdateSettingDialog
*
*/

#ifndef ACTION_H_
#define ACTION_H_

#include <hbview.h>
#include <hblineedit.h>
#include <hblistwidget.h>
#include <hblabel.h>
#include <hbdataform.h>
#include <hbdataformmodel.h>

// Access point selection
#include <cmmanager.h>

// Destination query dialog
#include "cmapplsettingsui.h"

class CIAUpdateSettingDialog : public HbView
{
    Q_OBJECT

public:
    explicit CIAUpdateSettingDialog(QGraphicsItem *parent=0);
    virtual ~CIAUpdateSettingDialog();
    
public slots:
    
private slots:
     void toggleChange(QModelIndex startIn, QModelIndex endIn);
     void showResults(uint retval);
     void showPreviousView(); 
     void activated(const QModelIndex& index);
     
signals:
         void toMainView();
    
private:
     
     void queryDestination();
     
     void saveSettingsL();
     
     void initializeView();
     
     uint getInternetSnapIdL();
     
     void getDestinationNameL( uint aItemUid, QString& aItemName );
     
     void initializeFieldsL();
     
    // Form & model    
    HbDataForm* mSettingsForm;
    HbDataFormModel *mModel;
    
    
    // settings ui
    CmApplSettingsUi* mApplSett;
    CmApplSettingsUi::SettingSelection mSelection;

    // Form items
    HbDataFormModelItem *mAutoUpdateItem;
    HbDataFormModelItem *mDestinationItem;
    
    // currently selected destination
    QString mCurrentDest;
    
    // NW connection flags
    bool mSetByNwQuery; // destination set by query / user
    bool mConnected;    // already connected to query ?
    bool mInitialized;  // connection initialized ?
    
    //TInt mAccessPointId;
    
    // Connection manager
    RCmManager mCmManager;
    
    // for back key catching
    HbAction*                   mBackKey;
    
    

};

#endif /* ACTION_H_ */

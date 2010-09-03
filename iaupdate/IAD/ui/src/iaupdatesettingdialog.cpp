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
* Description:   This module contains the implementation of IAUpdateSettingDialog 
*                class member functions.
*
*/

#include <hbdataform.h>
#include <hbdataformmodel.h>
#include <centralrepository.h>
#include <cmconnectionmethoddef.h>
#include <cmdestination.h>
#include <xqconversions.h>
#include <hbaction.h> 

#include "iaupdatesettingdialog.h"
#include "iaupdateprivatecrkeys.h"
#include "iaupdate.hrh"


const TInt KAutoUpdateOn( 0 );
const TInt KAutoUpdateOff( 1 );
const TInt KAutoUpdateOnInHomeNetwork( 2 );

/*
Constructor. It creates a formwidget on the view. 
*/
CIAUpdateSettingDialog::CIAUpdateSettingDialog(QGraphicsItem* parent):HbView(parent)
    {
    
    //setTitle("Software update");
    
    mSettingsForm = new HbDataForm(this);
            

    // open connection manager 
    TRAPD(err,  mCmManager.OpenL()); 
    qt_symbian_throwIfError(err);
    
    // destination field flag inititializations 
    mSetByNwQuery = false; // dest set by user/by destination query
    mConnected = false;    // already connected to query
    mInitialized = false;  // field initialized ?
    
    // Initialize view
    initializeView();
    
    //setWidget(mSettingsForm);
    setWidget(mSettingsForm);
    
    // Create application settings ui
    mApplSett = new CmApplSettingsUi(this);

    // Create a back key action and set it as the default navigation
    // action once the back key is pressed
    mBackKey = new HbAction(Hb::BackNaviAction, this);
    this->setNavigationAction(mBackKey);

    connect(mBackKey, SIGNAL(triggered()), this, SLOT(showPreviousView()));

    }

/*
Destructor
*/
CIAUpdateSettingDialog::~CIAUpdateSettingDialog()
    {
    // close connection manager 
    mCmManager.Close();
    }

// ----------------------------------------------------------------------------
// CIAUpdateSettingDialog::toggleChange
// 
// ----------------------------------------------------------------------------
//
void CIAUpdateSettingDialog::toggleChange(QModelIndex startIn, QModelIndex /*endIn*/)
{
    // HLa: this should work
    HbDataFormModelItem *itm = mModel->itemFromIndex(startIn);

    // Destination ?
    if ( startIn.row() == 0 )
        {
        // no query when initializing fields
        if ( mInitialized )
            {
            // no query if field value set by destination query
            if (!mSetByNwQuery )
                {
                queryDestination();
                mSetByNwQuery = true;
                }
                else
                {
                mSetByNwQuery = false;
                }
            }
        else
            {
            mInitialized = true;
            }
           
        }
    // Auto update ?
    if ( startIn.row() == 1 )
        {
         int currentIndex = mAutoUpdateItem->contentWidgetData(QString("currentIndex")).toInt();
        }
}
// ----------------------------------------------------------------------------
// CIAUpdateSettingDialog::queryDestination
// 
// ----------------------------------------------------------------------------
//
void CIAUpdateSettingDialog::queryDestination()
  {
        QFlags<CmApplSettingsUi::SelectionDialogItems> listItems;
        QSet<CmApplSettingsUi::BearerTypeFilter> filter;
        
        // Show only destinations
       listItems |= CmApplSettingsUi::ShowDestinations;
       // listItems |= CmApplSettingsUi::ShowConnectionMethods;

        mApplSett->setOptions(listItems, filter);
        mApplSett->setSelection(mSelection);
        
        // Connect finished(uint) signal and handle result via it
        if (!mConnected)
            {
            connect(mApplSett, SIGNAL(finished(uint)), this, SLOT(showResults(uint)));
            mConnected = true;
            }
        
        // Start CmApplSettingsUi
        mApplSett->open();

  }
// ----------------------------------------------------------------------------
// CIAUpdateSettingDialog::showResults
// 
// ----------------------------------------------------------------------------
//
void CIAUpdateSettingDialog::showResults(uint retval)
{
    if (retval == CmApplSettingsUi::ApplSettingsErrorNone) {
        mSelection = mApplSett->selection();
        
        uint destinationId = mSelection.id;
        
        QString idString;
        
        TRAPD( err, getDestinationNameL( destinationId, idString ) );
        qt_symbian_throwIfError(err);

        // Destination changed ?
        if ( idString != mCurrentDest )
            {
            mCurrentDest = idString;
            }
    }
   // inform toggleChange that change is not made by user
    mSetByNwQuery = true;
    mDestinationItem->setContentWidgetData(QString("text"), mCurrentDest);
}

// ----------------------------------------------------------------------------
// CIAUpdateSettingDialog::SaveSettingsL
// 
// ----------------------------------------------------------------------------
//
void CIAUpdateSettingDialog::saveSettingsL()
    {
    CRepository* cenrep = CRepository::NewLC( KCRUidIAUpdateSettings );
    TInt err = cenrep->StartTransaction( CRepository::EReadWriteTransaction );
    User::LeaveIfError( err );
    
    cenrep->CleanupCancelTransactionPushL();
    
    // Set destination
    TInt value = mSelection.id;
    err = cenrep->Set( KIAUpdateAccessPoint, value ) ;
    User::LeaveIfError( err );
    
    // Set auto update check
    value = mAutoUpdateItem->contentWidgetData(QString("currentIndex")).toInt();
    
    // Convert ist index index to setting value
    switch ( value )
         {
         case KAutoUpdateOn:
             value = EIAUpdateSettingValueDisableWhenRoaming;
             break;
         case KAutoUpdateOff:
             value = EIAUpdateSettingValueDisable;
             break;
         case KAutoUpdateOnInHomeNetwork:
             value = EIAUpdateSettingValueEnable;
             break;
         default: 
             break;
         }
    err = cenrep->Set( KIAUpdateAutoUpdateCheck, value ); 
    
    User::LeaveIfError( err );
    
    TUint32 ignore = KErrNone;
    User::LeaveIfError( cenrep->CommitTransaction( ignore ) );
    CleanupStack::PopAndDestroy(); // CleanupCancelTransactionPushL()
    CleanupStack::PopAndDestroy( cenrep );
    }


// -----------------------------------------------------------------------------
// CIAUpdateSettingDialog::initializeFieldsL
// -----------------------------------------------------------------------------
//
void CIAUpdateSettingDialog::initializeFieldsL()
    {
    CRepository* cenrep = CRepository::NewLC( KCRUidIAUpdateSettings );
    int destId = 0;
    
    // get access point id
    User::LeaveIfError( cenrep->Get( KIAUpdateAccessPoint, destId ) );
    
    // save id
    mSelection.id = destId;
    
    // On first time: show internet access point
    if ( mSelection.id == 0 )
        {
        User::LeaveIfError(mSelection.id = getInternetSnapIdL());
        }

    // set destination name
    TRAPD( err, getDestinationNameL( mSelection.id, mCurrentDest ) );
   
    if ( err == KErrNotFound )
        { 
        mSelection.id = 0;
        }
    else
        {
         // nothing to do ?
         User::LeaveIfError( err );
        }
    mDestinationItem->setContentWidgetData(QString("text"), mCurrentDest);
    
    // set auto update value
    int value = 0;
    User::LeaveIfError( cenrep->Get( KIAUpdateAutoUpdateCheck, value ) );
    
    // map cenrep value to index
    switch ( value )
         {
         case EIAUpdateSettingValueEnable:
             value = KAutoUpdateOn; // On 
             break;
         case EIAUpdateSettingValueDisable:
             value = KAutoUpdateOff; // Off
             break;
         case EIAUpdateSettingValueDisableWhenRoaming:
             value = KAutoUpdateOnInHomeNetwork; // On in home network
             break;
         default: 
             break;
         }
    
    mAutoUpdateItem->setContentWidgetData("currentIndex", value);
    
   
    CleanupStack::PopAndDestroy( cenrep ); 

    } 


// -----------------------------------------------------------------------------
// CIAUpdateSettingDialog::getDestinationNameL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateSettingDialog::getDestinationNameL( uint aItemUid, QString& aItemName )
    {
    
    if ( aItemUid == 0 )
        {
        aItemUid = getInternetSnapIdL();
        }

    RCmDestination dest = mCmManager.DestinationL( aItemUid );
    CleanupClosePushL( dest );
    HBufC* temp = dest.NameLC();    
    CleanupStack::Pop( temp );
    CleanupStack::PopAndDestroy( &dest ); 
    
    aItemName = XQConversions::s60DescToQString( temp->Des() );

    delete temp;
    
    if ( aItemName.size() == 0 ) 
        {
        User::Leave(KErrNotFound);
        }
    }

// -----------------------------------------------------------------------------
// ActionView::initializeView
// -----------------------------------------------------------------------------
//
void CIAUpdateSettingDialog::initializeView()
    {
    
    setTitle("Software update"); 
    //setTitle(hbTrId("txt_software_title_software_update")); 
    
    mSettingsForm->setHeading("Settings"); 
    //mSettingsForm->setHeading(hbTrId("txt_software_subhead_settings"));
    
    //create a model class
    HbDataFormModel *mModel = new HbDataFormModel();

    // add Destination item
    mDestinationItem = mModel->appendDataFormItem(
        HbDataFormModelItem::ToggleValueItem, QString("Network connection"));
        //HbDataFormModelItem::ToggleValueItem, hbTrId("txt_software_formlabel_network_connection"));

    // add auto update item
    mAutoUpdateItem = mModel->appendDataFormItem(
       HbDataFormModelItem::ComboBoxItem, QString("Auto-check for updates"));
       //HbDataFormModelItem::ComboBoxItem, hbTrId("txt_software_setlabel_autocheck_for_updates"));
    
    // auto update selection values
    QStringList list;
    list.insert(0, QString("On"));
    //list.insert(0, hbTrId("txt_software_setlabel_val_on"));
    list.append(QString("Off")); 
    //list.append(hbTrId("txt_software_setlabel_val_off"));
    list.append(QString("On in home network"));
    //list.append(hbTrId("txt_software_setlabel_val_on_in_home_network"));
    
    mAutoUpdateItem->setContentWidgetData("items", list);

     // connect data changes for launching the access point selection dialog
    connect(mModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)), 
                this, SLOT(toggleChange(QModelIndex, QModelIndex)));

    // connect to function called when data items are displayed
    connect(mSettingsForm, SIGNAL(activated(QModelIndex)), 
                this, SLOT(activated(QModelIndex)));
    
    mSettingsForm->setModel(mModel);
    
    // set values for items
    TRAPD(err, initializeFieldsL());
    qt_symbian_throwIfError(err);
}

// -----------------------------------------------------------------------------
// ActionView::getInternetSnapIdL
// -----------------------------------------------------------------------------
//
uint CIAUpdateSettingDialog::getInternetSnapIdL()
    {
    TUint internetSnapId( 0 );
    
    RArray<TUint32> destinationIdArray;
    mCmManager.AllDestinationsL( destinationIdArray );
    
    for ( TInt i = 0; i < destinationIdArray.Count(); i++ )
        {
        RCmDestination destination = mCmManager.DestinationL( destinationIdArray[i] );
        CleanupClosePushL( destination );
        
        TUint32 metadata = destination.MetadataL( CMManager::ESnapMetadataPurpose );
        if ( metadata == CMManager::ESnapPurposeInternet )
            {
            internetSnapId = destinationIdArray[i];
            // Get the destination name with destination.NameLC() if need.
            CleanupStack::PopAndDestroy( &destination );
            break;
            }
        CleanupStack::PopAndDestroy( &destination );
        }
    destinationIdArray.Close();
    
    return internetSnapId;
    
    }

void CIAUpdateSettingDialog::showPreviousView()
{
    // Save settings    
    TRAPD(err,  saveSettingsL()); 
    qt_symbian_throwIfError(err);
    
    // Switch view
    emit toMainView();
}

void CIAUpdateSettingDialog::activated(const QModelIndex& index)
{
    if ( this->isActiveWindow() && index.row() == 0 )
        {
        }

}

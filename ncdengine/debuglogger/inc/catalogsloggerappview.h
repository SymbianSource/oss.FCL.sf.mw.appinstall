/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?description
*
*/


#ifndef C_CATALOGSLOGGERAPPVIEW_H
#define C_CATALOGSLOGGERAPPVIEW_H

#include <coecntrl.h>
#include <aknsettingitemlist.h>

#include "catalogslogger.hrh"


class CCatalogsLoggerSettingList : public CAknSettingItemList
   {
public:
      
   CAknSettingItem* CreateSettingItemL( TInt aIdentifier );
   void EditItemL( TInt aIndex, TBool aCalledFromMenu ) ;
 
private:

   TBool iFlags[ENrOfCatalogsLoggerSettings];   // One flag for each setting                                                

   };


class CCatalogsLoggerAppView : public CCoeControl
    {
public:
    static CCatalogsLoggerAppView* NewL(const TRect& aRect);
    static CCatalogsLoggerAppView* NewLC(const TRect& aRect);
     ~CCatalogsLoggerAppView();

public:  // from CCoeControl
    void Draw(const TRect& aRect) const;
    TKeyResponse OfferKeyEventL( const TKeyEvent &aKeyEvent, TEventCode aType );

    void SetLoggerStatus( TBool aStatus );
    void SetLoggerNumber( TInt aNumber, TInt aOffset );
    void SetLoggerDrive( TInt aDrive );
    void SetData( TPtrC8 aData );

private:    //From CCoeControl
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex ) const;
    void SizeChanged();


private:
    void ConstructL(const TRect& aRect);
    CCatalogsLoggerAppView();

private:
    CCatalogsLoggerSettingList* iSettingsView;

    TBool iLoggerStatus;
    TInt iLoggerNumber;
    TInt iLoggerOffset;
    TInt iLoggerDrive;
    HBufC* iData;

    };



#endif // C_CATALOGSLOGGERAPPVIEW_H

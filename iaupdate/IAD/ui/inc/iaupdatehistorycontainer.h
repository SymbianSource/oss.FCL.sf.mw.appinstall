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
* Description:   This file contains the header file of the CIAUpdateHistoryContainer
*                class 
*
*/



#ifndef IA_UPDATE_HISTORY_CONTAINER_H
#define IA_UPDATE_HISTORY_CONTAINER_H

#include <coecntrl.h>

class CAknDoubleGraphicStyleListBox;
class MIAUpdateHistory;
class MIAUpdateHistoryItem;


/*! 
  @class CIAUpdateHistoryContainer
  
  @discussion 
  */
class CIAUpdateHistoryContainer : public CCoeControl
    {
    
public:

    /*!
      @function NewL
       
      @discussion Create a CIAUpdateHistoryContainer object, which will draw itself to aRect
      @param aRect the rectangle this view will be drawn to
      @result a pointer to the created instance of CIAUpdateHistoryContainer
      */
    static CIAUpdateHistoryContainer* NewL( const TRect& aRect );

    /*!
      @function NewLC
       
      @discussion Create a CIAUpdateHistoryContainer object, which will draw itself to aRect
      @param aRect the rectangle this view will be drawn to
      @result a pointer to the created instance of CIAUpdateHistoryContainer
      */
    static CIAUpdateHistoryContainer* NewLC( const TRect& aRect );


    /*!
      @function ~CIAUpdateHistoryContainer
      
      @discussion Destroy the object and release all memory objects
      */
     virtual ~CIAUpdateHistoryContainer();


    /*!
     * @function RefreshL
     *
     * @discussion Populate list box with the given nodes.
     * @param aHistory
     */
    void RefreshL( MIAUpdateHistory& aHistory );       


private:  // from CCoeControl

    /*!
     * @function OfferKeyEventL
     *
     * @discussion Handle key events by forwarding them to the list box.
     * @param aKeyEvent The key event.
     * @param aType The event type.
     * @result Indicates whether or not the key event was used by this control. 
     */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

     /*!
      * @see CCoeControl::SizeChanged
      */
     void SizeChanged();

     /*!
      * @see CCoeControl::CountComponentControls
      */
     TInt CountComponentControls() const;

     /*!
      *  @see CCoeControl::ComponentControl
      */
     CCoeControl* ComponentControl( TInt aIndex ) const;
     
     /**
     * Handles a resource relative event
     *
     * @param aType Event type
     */
     void HandleResourceChange( TInt aType );
  

private:

    /*!
      @function ConstructL
      
      @discussion  Perform the second phase construction of a CIAUpdateHistoryContainer object
      @param aRect the rectangle this view will be drawn to
      */
    void ConstructL(const TRect& aRect);

    /*!
      @function CIAUpdateHistoryContainer
      
      @discussion Perform the first phase of two phase construction 
      */
    CIAUpdateHistoryContainer();


    // Creates the state description string.
    HBufC* NodeStateDescriptionLC( const MIAUpdateHistoryItem& aItem );
    

private: // Data

    CDesCArray* iItemTextArray;
    CAknDoubleGraphicStyleListBox* iListBox;

    };


#endif // IA_UPDATE_HISTORY_CONTAINER_H

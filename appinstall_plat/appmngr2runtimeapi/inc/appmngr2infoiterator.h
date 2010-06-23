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
* Description:   Implements MCUIInfoIterator API for CCUIDetailsDialog
*
*/


#ifndef C_APPMNGR2INFOITERATOR_H
#define C_APPMNGR2INFOITERATOR_H

#include <CUIDetailsDialog.h>           // SwiUI::CommonUI::MCUIInfoIterator

class CAppMngr2InfoBase;
class CDesC8Array;

enum TAppMngr2InfoType
    {
    EAppMngr2StatusInstalled,
    EAppMngr2StatusNotInstalled
    };


/**
 * CAppMngr2InfoIterator is the base class for plug-in specific data collecting
 * iterators. CAppMngr2InfoIterator derived iterators can be used to get the data
 * shown in details dialog (CCUIDetailsDialog). Details dialog shows application
 * details and certificate info.
 * 
 * @lib appmngr2pluginapi.lib
 * @since S60 v5.1
 */
class CAppMngr2InfoIterator : public CBase, public SwiUI::CommonUI::MCUIInfoIterator
    {
public:     // constructors and destructor
    /**
     * Constructs new info iterator to show details dialog.
     * 
     * @param aInfo  Reference to application or package info object
     * @param aInfoType  Type info (installed application or not installed package)
     */
    IMPORT_C CAppMngr2InfoIterator( CAppMngr2InfoBase& aInfo, TAppMngr2InfoType aInfoType );
    
    /**
     * Allocates memory for CDesCArrays.
     * 
     * BaseConstructL() must be called from derived ConstructL() before any other
     * leaving functions. BaseConstructL() calls SetAllFieldsL() to get all the
     * displayable details.
     */
    IMPORT_C void BaseConstructL();

    /**
     * Destructor, deletes allocated memory.
     */
    IMPORT_C ~CAppMngr2InfoIterator();
    
public:     // from MCUIInfoIterator
    IMPORT_C TBool HasNext() const;
    IMPORT_C void Next( TPtrC& aKey, TPtrC& aValue );
    IMPORT_C void Reset();

protected:  // new functions
    /**
     * Sets field to given value.
     * 
     * Uses StringLoader to load aResourceId and adds it to MCUIInfoIterator
     * as a new key having value aValue.
     *  
     * @param aResourceId  Resource id for MCUIInfoIterator key
     * @param aValue  Value for the key
     */
    IMPORT_C void SetFieldL( TInt aResourceId, const TDesC& aValue );
    
    /**
     * Sets all default fields, can be overridden in derived classes.
     * 
     * Default implementation sets name (from CAppMngr2InfoBase::Name()),
     * status (either installed or not installed), location drive (from
     * CAppMngr2InfoBase::Location()), and application/file size (from
     * CAppMngr2InfoBase::Details()). Status and location fields are set
     * using SetStatusL() and SetLocationL() functions, so that they can
     * be overridden separately. Finally calls SetOtherFieldsL() to set
     * other fields, if derived class has some additional fields that
     * are not covered here. 
     */
    IMPORT_C virtual void SetAllFieldsL();
    
    /**
     * Sets status (installed or not installed) based on aInfoType
     * defined in constructor. This function can be overridden in
     * derived classes.
     */
    IMPORT_C virtual void SetStatusL();
    
    /**
     * Sets location drive based on CAppMngr2InfoBase::Location().
     * This function can be overridden in derived classes.
     */
    IMPORT_C virtual void SetLocationL();
    
    /**
     * Sets other possble fields, default implemenation is empty.
     * This function can be overridden in derived classes.
     */
    IMPORT_C virtual void SetOtherFieldsL();

protected:  // data
    CAppMngr2InfoBase& iInfo;
    TAppMngr2InfoType iInfoType;
    TInt iCurrentIndex;
    CDesCArray* iKeys;
    CDesCArray* iValues;
    };

#endif  // C_APPMNGR2INFOITERATOR_H


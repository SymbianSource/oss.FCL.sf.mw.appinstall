/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CIAUpdateResult class 
*
*/



#ifndef IA_UPDATE_RESULT_H
#define IA_UPDATE_RESULT_H

#include <e32base.h>


/**
 * CIAUpdateResult gives results of the completed update operation.
 *
 * @see CIAUpdate
 * @see MIAUpdateObserver
 *
 * @since S60 v3.2
 */
class CIAUpdateResult : public CBase
    {
    
public:

    /**
     * @return CIAUpdateResult* CIAUpdateResult object
     * @exception Leaves with system wide error code.
     *
     * @since S60 v3.2
     */
    IMPORT_C static CIAUpdateResult* NewL();

    /**
     * @see NewL
     *
     * @since S60 v3.2
     */
    IMPORT_C static CIAUpdateResult* NewLC();


    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    IMPORT_C virtual ~CIAUpdateResult();


    /**
     * @return TInt Number of successfull updates in an update operation.
     *
     * @since S60 v3.2
     */
    IMPORT_C TInt SuccessCount() const;

    /**
     * @param aSuccessCount Number of successfull updates in an update operation.
     *
     * @since S60 v3.2
     */    
    IMPORT_C void SetSuccessCount( TInt aSuccessCount );


    /**
     * @return TInt Number of failed updates in an update operation.
     *
     * @since S60 v3.2
     */    
    IMPORT_C TInt FailCount() const;

    /**
     * @param aFailCount Number of failed updates in an update operation.
     *
     * @since S60 v3.2
     */
    IMPORT_C void SetFailCount( TInt aFailCount );


    /**
     * @return TInt Number of cancelled updates in an update operation.
     *
     * @since S60 v3.2
     */    
    IMPORT_C TInt CancelCount() const;

    /**
     * @param aCancelCount Number of cancelled updates in an update operation.
     *
     * @since S60 v3.2
     */
    IMPORT_C void SetCancelCount( TInt aCancelCount );


private:

    /** 
     * Constructor is private because this class is not meant for inheritance.
     * 
     * @see NewL
     *
     * @since S60 v3.2
     */ 
    CIAUpdateResult();
    
    /**
     * @see NewL
     *
     * @since S60 v3.2
     */ 
    void ConstructL();
    
    
    // Prevent these
    CIAUpdateResult( const CIAUpdateResult& aObject );
    CIAUpdateResult& operator =( const CIAUpdateResult& aObject );


private: // data
    
    TInt iSuccessCount; 
    TInt iFailCount; 
    TInt iCancelCount; 
    
    };

#endif // IA_UPDATE_RESULT_H


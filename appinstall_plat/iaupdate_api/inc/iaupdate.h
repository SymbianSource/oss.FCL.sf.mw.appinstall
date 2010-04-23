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
* Description:   This file contains the header file of the CIAUpdate class 
*
*/


#ifndef IA_UPDATE_H
#define IA_UPDATE_H

#include <e32base.h>
#include <e32std.h>

class MIAUpdateObserver;
class CIAUpdateManager;
class CIAUpdateParameters;


/**
 * CIAUpdate object provides methods to handle update actions.
 * Updating will be targeted to the update items that qualify the requirements
 * given in CIAUpdateParameters objects. In asynchronous actions, the callback 
 * functions of MIAUpdateObserver object will be informed about the progress 
 * of update actions.
 * 
 * @see MIAUpdateObserver
 * @see CIAUpdateParameters
 *
 * @since S60 v3.2
 */
class CIAUpdate : public CBase
    {

public:

    /**
     * @param aObserver Callback functions of the observer are called
     * when operations that are started from this interface progress.    
     * @return CIAUpdate* Pointer to the created CIAUpdate object that 
     * can be used for update actions.
     *
     * @since S60 v3.2
     */
    IMPORT_C static CIAUpdate* NewL( MIAUpdateObserver& aObserver );
    
    /**
     * @see NewL
     */
    IMPORT_C static CIAUpdate* NewLC( MIAUpdateObserver& aObserver );
    

    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    IMPORT_C virtual ~CIAUpdate();


    /**
     * Checks if any updates are available.
     * This function is asynchronic.
     *
     * @note Update check is directed to items that match:
     *       - CIAUpdateParameters::Uid OR
     *       - CIAUpdateParameters::SearchCriteria
     * @note Uid describes either package UID or SID.
     *       UID is assumed to be SID if update items are not found 
     *       with the given UID. Then, corresponding package UID is 
     *       searched from the installed application registry. 
     *       If corresponding package UID is found, then update items 
     *       are rechecked. 
     * @note If Uid is given, then search criteria is omitted.
     * @note If none of the parameters that are mentioned above is not set, 
     * then all the updates are checked.
     *
     * @note CIAUpdateParameters::ShowProgress defines if a ready-made
     * wait dialog should be shown in case the operation takes longer time. 
     * This happens e.g. if the updates are refreshed from the server.
     *
     * @param aUpdateParameters Defines the update targets.
     *
     * @since S60 v3.2
     */
    IMPORT_C void CheckUpdates( const CIAUpdateParameters& aUpdateParameters );


    /** 
     * Starts IAD UI.
     *
     * UI will show the update items that qualify the requirements given
     * in aUpdateParameters.
     *
     * Even if the updating is handled in the IAD UI, the observer is informed
     * about the progression of operations. So, the calling application
     * may continue correctly after updates are finished.
     * 
     * @note IAD UI will show update items that match:
     *       - CIAUpdateParameters::Uid
     *       - CIAUpdateParameters::SearchCriteria
     * @note Uid describes either package UID or SID.
     *       UID is assumed to be SID if update items are not found 
     *       with the given UID. Then, corresponding package UID is 
     *       searched from the installed application registry. 
     *       If corresponding package UID is found, then update items 
     *       are rechecked. 
     * @note If Uid is given, then search criteria is omitted.
     * @note If none of the parameters that are mentioned above is not set, 
     * then all the updates are shown in the UI.
     *
     * @note When update finishes, the following values are used 
     * for command line execution:
     *      - CIAUpdateParameters::CommandLineExecutable
     *      - CIAUpdateParameters::CommandLineArguments
     * @note If CIAUpdateParameters::CommandLineExecutable is not set, 
     * then no command line execution is done.
     *
     * @param aUpdateParameters Defines the update targets.
     *
     * @since S60 v3.2
     */
    IMPORT_C void ShowUpdates( const CIAUpdateParameters& aUpdateParameters );
    

    /** 
     * All the items that qualify the requirements given in
     * aUpdateParameters will be updated without opening the IAD UI.
     * This function is asynchronic.
     *
     * @note Update is directed to items that match:
     *       - CIAUpdateParameters::Uid
     *       - CIAUpdateParameters::SearchCriteria
     * @note Uid describes either package UID or SID.
     *       UID is assumed to be SID if update items are not found 
     *       with the given UID. Then, corresponding package UID is 
     *       searched from the installed application registry. 
     *       If corresponding package UID is found, then update items 
     *       are rechecked. 
     * @note If Uid is given, then search criteria is omitted.
     * @note If none of the parameters that are mentioned above is not set, 
     * then everything is updated.
     *
     * @note When update finishes, the following values are used 
     * for command line execution:
     *      - CIAUpdateParameters::CommandLineExecutable
     *      - CIAUpdateParameters::CommandLineArguments
     * @note If CIAUpdateParameters::CommandLineExecutable is not set, 
     * then no command line execution is done.
     *
     * @param aUpdateParameters Defines the update targets.
     *
     * @since S60 v3.2
     */
    IMPORT_C void Update( const CIAUpdateParameters& aUpdateParameters );


    /** 
     * This function is provided as a convenience method 
     * that has localized texts for the query dialog. 
     * This function does not start any update. 
     * This function is asynchronic.
     *
     * @note The observer is informed when the update query operation has completed.
     *
     * @since S60 v3.2
     */
    IMPORT_C void UpdateQuery();


private:

    /**
     * Constructor is private because this class is not meant for
     * inheritance.
     *
     * @see NewL
     */
    CIAUpdate();
    
    /** 
     * @see NewL
     */
    void ConstructL( MIAUpdateObserver& aObserver );


    /** 
     * @return CIADUpdateManager& Manages update operations.
     */
    CIAUpdateManager& UpdateManager() const;


    // These are prevented
    CIAUpdate( const CIAUpdate& aObject );
    CIAUpdate& operator =( const CIAUpdate& aObject );

        
private: // data

    // Manages update operations.
    CIAUpdateManager* iUpdateManager;

    };

#endif // IA_UPDATE_H

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
* Description:   Contains MNcdNode interface
*
*/


#ifndef M_NCD_SERVER_REPORT_MANAGER_H
#define M_NCD_SERVER_REPORT_MANAGER_H


#include "catalogsbase.h"
#include "ncdinterfaceids.h"

class MNcdServerReportOperation;
class MNcdServerReportOperationObserver;


/**
 * MNcdServerReportManager interface provides functions
 * that can be used to manage server report sending to the server.
 * As a default, report sending is handled automatically in the background.
 * But, manager can be used if reports should be collected and sent in certain
 * moments. This interface can be gotten from the MNcdProvider by using the
 * QueryInterfaceL function.
 *
 * @note Capabilities define what kind of reports are supported by the
 * application. So, if reports should or should not be sent, then define
 * this by setting correct capabilities.
 *
 * @see MNcdServerReportOperation
 * @see MNcdServerReportOperationObserver
 * @see MNcdProvider
 * @see NcdCapabilities
 *
 *
 */
class MNcdServerReportManager : public virtual MCatalogsBase
    {
    
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdServerReportManagerUid };


    /**
     * Reporting method describes how the reporting is done.
     */
    enum TReportingMethod
        {
        
        /**
         * Reporting is done in the background
         * This is a default method.
         */
        EReportingBackground,
        
        /**
         * Reporting is managed through this API Interface.
         * The API user decides when to send reports instead of allowing
         * reports to be sent in the background when process time allows.
         */
        EReportingManaged
        
        };


    enum TReportingStyle
        {
        
        /**
         * Reporting information is sent in the general format not in some platform
         * specific format. This is a default value.
         */
        EReportingStyleGeneral,
        
        /**
         * Reports will contain S60 specific information.
         */
        EReportingStyleS60
        
        };


    /**
     * Setter for the reporting method.
     *
     * @param aMethod The reporting method that is used when reports are
     * managed.
     *
     * @exception Leave System wide error code
     *
     *
     */
    virtual void SetReportingMethodL( const TReportingMethod& aMethod ) = 0;


    /**
     * Getter for the reporting method.
     *
     * @param TReportingMethod The reporting method that is used 
     * when reports are managed.
     *
     *
     */
    virtual TReportingMethod ReportingMethodL() const = 0;


    /**
     * Setter for the reporting style.
     *
     * @param aStyle The style that is used for reports.
     *
     * @exception Leave System wide error code
     *
     *
     */
    virtual void SetReportingStyleL( const TReportingStyle& aStyle ) = 0;


    /**
     * Getter for the reporting style.
     *
     * @param TReportingMethod The style that is used for reports.
     *
     *
     */
    virtual TReportingStyle ReportingStyleL() const = 0;


    /**
     * Starts the server report sending operation.
     *
     * @note The reference count of the operation object is increased by one. So, Release()
     * function of the operation should be called when operation is not needed anymore.
     *
     * @note The sending process is asynchronous. When sending has completed,
     * the observer callback functions are called.
     *
     * @note After sending is finished the collected reports will be automatically
     * removed.
     *
     * @note This function has an effect only if the reporting method is set to
     * EReportingManaged. If reporting is done in the background, then reports 
     * are sent automatically, and NULL is returned for the operation.
     *
     * @param aObserver Operation observer.
     * @return MNcdServerReportOperation* Operation if one was created. If reporting is done 
     * in the background, then NULL is returned.
     * @exception Leave System wide error code
     *
     *
     */
    virtual MNcdServerReportOperation* SendL( MNcdServerReportOperationObserver& aObserver ) = 0;


protected:

    /**
     * The destructor of an interface is set virtual to make sure that
     * the destructors of derived classes are called appropriately when the
     * object is destroyed.
     *
     * Destructor is defined as protected to prevent direct use of delete 
     * on observer interface pointers.
     *
     * 
     */
    virtual ~MNcdServerReportManager() {}

    };

#endif // M_NCD_SERVER_REPORT_MANAGER_H


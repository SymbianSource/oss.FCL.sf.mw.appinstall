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
* Description:   MNcdEngineConfiguration declaration
*
*/


#ifndef M_NCDENGINECONFIGURATION_H
#define M_NCDENGINECONFIGURATION_H

class CNcdKeyValuePair;

/**
 * Interface that provides access to engine's configuration.
 *
 */
class MNcdEngineConfiguration
    {
public:

    /**
     * Type getter (vanilla, variant etc.)
     *
     * @return Engine's type
     */
    virtual const TDesC& EngineType() const = 0;
    
    /**
     * Version getter
     *
     * @return Version
     */
    virtual const TDesC& EngineVersion() const = 0;
    
    /**
     * UID getter
     *
     * @return UID
     */
    virtual const TDesC& EngineUid() const = 0;

    /**
     * Provisioning getter
     *
     * @return Provisioning
     */
    virtual const TDesC& EngineProvisioning() const = 0;
    
    
    /**
     * Installation drive getter
     *
     * @return Installation drive letter and a colon
     */
    virtual const TDesC& EngineInstallDrive() const = 0;
        
    
    /**
     * Temp drive number getter
     *
     * @return Temp drive number
     */
    virtual TInt EngineTempDrive() const = 0;       

    /**
     * Client's data path getter
     *
     * Client's data files (eg. downloaded content files)
     * should be stored to this directory before they
     * are installed.
     *
     * @param aClientId Client's ID
     * @param aTemp ETrue if the path should be temp data path
     * @return Client's data path
     */
    virtual HBufC* ClientDataPathLC(
        const TDesC& aClientId, 
        TBool aTemp ) = 0;
    
    /**
     * Tells whether engine uses fixed access point defined in config file.
     *
     * @return ETrue If engine uses fixed ap, otherwise false.
     */        
    virtual TBool UseFixedAp() const = 0;
         
    /**
     * Return the details of the fixed access point if such is used.
     *
     * @return The details.
     */
    virtual const RPointerArray<CNcdKeyValuePair>& FixedApDetails() const = 0;
    
    /**
     * Clears client's data files (eg. downloaded but not 
     * installed content files)
     *
     * @param aClientId Client's ID
     * @param if ETrue, temp files are deleted. Otherwise data files are deleted
     */    
    virtual void ClearClientDataL( const TDesC& aClientId, TBool aTemp ) = 0;        
        
protected:

    /**
     * Destructor
     */
    virtual ~MNcdEngineConfiguration()
        {
        }
    };

#endif // M_NCDENGINECONFIGURATION_H

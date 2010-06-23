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
* Description:   MNcdUserConfiguration declaration
*
*/

 
#ifndef M_NCDUSERCONFIGURATION_H
#define M_NCDUSERCONFIGURATION_H 
 
class MCatalogsContext;
class CNcdKeyValuePair;
class CNcdKeyValueMap;
 
/**
 * Interface for user configuration manipulation
 */
class MNcdUserConfiguration
    {
public:

    enum TNcdConfigurationCategory
        {
        /**
         * General configuration: master-server address etc.
         */
        ENcdConfigurationGeneral,
        
        /**
         * Configuration used to fill the client info in protocol
         * requests
         */
        ENcdConfigurationClientInfo
        };
        
public:    
    /**
     * Adds a configuration that will be sent to content providers
     *
     * @param aConfig Configuration to add. The ownership is transferred to
     * the protocol. If a leave occurs, aConfig is deleted.
     */
    virtual void AddConfigurationL( const MCatalogsContext& aContext, 
        CNcdKeyValuePair* aConfig ) = 0;
       
    /**
     * Removes a configuration.
     *
     * @return KErrNone if successful, KErrNotFound if the key was not
     * found
     */ 
    virtual TInt RemoveConfigurationL( const MCatalogsContext& aContext,
        const TDesC& aKey ) = 0;
       
    /**
     * Returns a list of all configurations for the context
     * 
     * @note Ownership of the configurations is transferred to the caller.
     * @return Current configuration for the context. 
     */         
    virtual CNcdKeyValueMap* ConfigurationsLC( 
        const MCatalogsContext& aContext ) = 0;    
    
    
    /**
     * Returns a list of configurations in a particular category
     *
     * @return Configurations
     */
    virtual CNcdKeyValueMap& ConfigurationsL(
        const MCatalogsContext& aContext, 
        TNcdConfigurationCategory aCategory ) = 0;
    
    
protected:
    
    virtual ~MNcdUserConfiguration()
        {
        }
        
    };
    
    
#endif // M_NCDUSERCONFIGURATION_H    
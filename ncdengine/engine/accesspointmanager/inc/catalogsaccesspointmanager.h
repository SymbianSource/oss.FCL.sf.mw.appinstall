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
* Description:   Interface MCatalogsAccessPointManager declation
*
*/


#ifndef M_CATALOGSACCESSPOINTMANAGER_H
#define M_CATALOGSACCESSPOINTMANAGER_H

class CCatalogsAccessPoint;
class MNcdConfigurationProtocolDetail;
class MNcdConfigurationProtocolClientConfiguration;
class CNcdNodeIdentifier;
class CNcdKeyValuePair;

class MCatalogsAccessPointManager
{
public:
    enum TAction 
        {
        EDownload,
        EBrowse,
        EPurchase,
        };
    
    /**
     * Virtual destructor.
     */
    virtual ~MCatalogsAccessPointManager() 
        {
        }

    /**
     * Sets fixed access point. If fixed access point is set, it is created to
     * commsDB and always used. That is, when AccessPointIdL is called, it will always
     * return the fixed AP, if it is set with this function.
     *
     * @param aApDetails Details of the access point. Key-value pairs are AP properties
     *                   where key is the name of the property and value is its value.
     */
    virtual void SetFixedApL( const RPointerArray<CNcdKeyValuePair>& aApDetails ) = 0; 
    
    /**
     * Gets fixed accesspoint.
     *
     * @param aAccessPointId ID of the fixed accesspoint. Value is unchanged if fixed accesspoint is not set.
     * @leave Symbian error code
     */
    virtual void GetFixedApL( TUint32& aAccessPointId ) = 0;        
    
    /**
     * Parses the client configuration details and creates the access points to
     * commsDB if it is missing them. Also parses the contentSources section and
     * maps download, browse and purchase access points to correct namespaces.
     *
     * @param aConfiguration The client configuration response.
     * @param aClientUid The client UID.
     * @param aIgnoreFixedAp If true, accesspoint data is parsed even if fixed 
     * accesspoint was set
     * @param aCreatedAps If a pointer is given then ids of created accesspoints are 
     * added to it. Ownership is NOT transferred
     */
    virtual void ParseAccessPointDataFromClientConfL(
        const MNcdConfigurationProtocolClientConfiguration& aConfiguration,
        const TUid& aClientUid,
        TBool aIgnoreFixedAp = EFalse, 
        RArray<TUint32>* aCreatedAps = NULL ) = 0;

    /**
     * Get accesspoint's id in comms database. Method also validates that 
     * correct accesspoint settings exists in the commsdatabase.
     * So for example if accesspoint has been removed from the commsdatabase
     * it is created again based on the correct settings.
     * 
     * @param aNameSpace The namespace of content source.
     * @param aAction The action.
     * @param aClientUid The UID of the client.
     * @param aAccessPointId Id of the accesspoint in the comms database
     * @return Error code, KErrNotFound if the access point is undefined.
     */
    virtual TInt AccessPointIdL(
        const TDesC& aNameSpace,
        const MCatalogsAccessPointManager::TAction& aAction, 
        const TUid& aClientUid,
        TUint32& aAccessPointId) = 0;    


    /**
     * Get accesspoint's id in comms database. Method also validates that 
     * correct accesspoint settings exists in the commsdatabase.
     * So for example if accesspoint has been removed from the commsdatabase
     * it is created again based on the correct settings.
     * 
     * @param aNameSpace The namespace of content source.
     * @param aCatalogId Id of the catalog.
     * @param aAction The action.
     * @param aClientUid The UID of the client.
     * @param aAccessPointId Id of the accesspoint in the comms database
     * @return Error code, KErrNotFound if the access point is undefined.
     */
    virtual TInt AccessPointIdL(
        const TDesC& aNameSpace, 
        const TDesC& aCatalogId,
        const MCatalogsAccessPointManager::TAction& aAction, 
        const TUid& aClientUid,
        TUint32& aAccessPointId) = 0;
        
    /**
     * Get accesspoint's id in comms database. Method also validates that 
     * correct accesspoint settings exists in the commsdatabase.
     * So for example if accesspoint has been removed from the commsdatabase
     * it is created again based on the correct settings.
     * NOTE! The function leaves if the node with the given identifier does not exist.
     *
     * @param aNodeIdentifier The node identifier.
     * @param aAction The action.
     * @param aClientUid The UID of the client.
     * @param aAccessPointId Id of the accesspoint in the comms database
     * @return Error code, KErrNotFound if the access point is undefined.
     */
    virtual TInt AccessPointIdL(
        const CNcdNodeIdentifier& aNodeIdentifier,
        const MCatalogsAccessPointManager::TAction& aAction, const TUid& aClientUid,
        TUint32& aAccessPointId) = 0;

   	};
	
#endif

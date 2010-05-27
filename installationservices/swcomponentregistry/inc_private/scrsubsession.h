/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Declares SCR subsession implementation classes on the server side.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SCRSUBSESSION_H
#define SCRSUBSESSION_H

#include "scrserver.h"
#include "usifcommon.h"
#include "appregentries.h"
#include "appreginfo.h"
#include "appregentries.h"

namespace Usif
	{
	// forward declarations
	class CStatement;
	class CComponentEntry;
	class CComponentFilter;
	class CAppInfoFilter;
	
	class CScrSubsession : public CScsSubsession
	/**
		Base SCR Subsession. Owns handle to an existing database session.
	 */
		{
	public:
		virtual ~CScrSubsession();
		
	protected:
		CScrSubsession(CScrSession &aSession);
		void ConstructL();
		
	public:
		CStatement *iStatement;
		// Pointer to the server object. Owned by the session object. 
		// Do NOT delete. Added to make the code cleaner and more consistent.
		CScrServer *iServer; 
		// Pointer to the session object. The owner session passes its own pointer. 
		// Do NOT delete. Added to make the code cleaner and more consistent.
		CScrSession *iSession;
		};
		
	class CCompViewSubsessionContext : public CBase
		{
	friend class CScrRequestImpl;
	friend class CComponentViewSubsession;
	public:
		~CCompViewSubsessionContext();
	private:
		TInt iLastSoftwareTypeId;
		HBufC* iLastSoftwareTypeName;
		RArray<TComponentId> iComponentFilterSuperset; ///< A helper array for the component filter. The filter conditions are split into two, and this variable is used for maintaing all components matching the first set of conditions.
		TBool iFilterSupersetInUse; ///< Specifies whether the filter superset array should be used when iterating over the filter query.		
		};
	
	class CComponentViewSubsession : public CScrSubsession
	/**
		This object is created for each component view subsession opened by the SCR client.
		Handles component view creation by using a provided filter and returns the row(s) of the view.
	 */
		{
	public:
		static CComponentViewSubsession* NewL(CScrSession& aSession);
		~CComponentViewSubsession();
		// Implement CScsSession
		TBool DoServiceL(TInt aFunction, const RMessage2& aMessage);
			
	private:
		CComponentViewSubsession(CScrSession& aSession);
		void ConstructL();
			
	private:
		CComponentFilter* iFilter;
		CComponentEntry *iComponentEntry;
		RPointerArray<CComponentEntry> iComponentEntryList;
		CCompViewSubsessionContext* iSubsessionContext;
		};	
	
	class CFileListSubsession : public CScrSubsession
	/**
		This object is created for each file list subsession opened by the SCR client
		to retrieve the list of files owned by a given component.
	 */
		{
	public:
		static CFileListSubsession* NewL(CScrSession& aSession);
		~CFileListSubsession();
		// Implement CScsSession
		TBool DoServiceL(TInt aFunction, const RMessage2& aMessage);
				
	private:
		CFileListSubsession(CScrSession& aSession);
				
	private:
		HBufC *iFilePath;
		RPointerArray<HBufC> iFileList;
		};	
	
    struct TAppUidWithLocaleInfo
         {
         TUid iAppUid;
         TLanguage iLocale;
         };

	class CAppInfoViewSubsessionContext : public CBase
	    {
	    friend class CScrRequestImpl;
	    friend class CAppInfoViewSubsession;
	    public:
	        ~CAppInfoViewSubsessionContext();
	    private:
	        TInt iAppInfoIndex;	     
	        TLanguage iLocale;
	        TInt iScreenMode;
	        RArray<TAppUidWithLocaleInfo> iApps;
	    };
	
	class CAppInfoViewSubsession : public CScrSubsession
	    /**
	        This object is created for each AppInfo view subsession opened by the SCR client.
	        Handles component view creation by using a provided filter.
	     */
	        {
	    public:
	        static CAppInfoViewSubsession* NewL(CScrSession& aSession);
	        ~CAppInfoViewSubsession();
	        TBool DoServiceL(TInt aFunction, const RMessage2& aMessage);
	            
	    private:
	        CAppInfoViewSubsession(CScrSession& aSession);
	        void ConstructL();
	            
	    private:
	        CAppInfoFilter* iAppInfoFilter;
	        TAppRegInfo* iApaAppInfo;
	        CAppInfoViewSubsessionContext* iSubsessionContext;
	        };
	
	class CRegInfoForApplicationSubsessionContext : public CBase
	            {
	            friend class CScrRequestImpl;
	            friend class CRegInfoForApplicationSubsession;
	            public:
	                ~CRegInfoForApplicationSubsessionContext();
	            private:
	                RPointerArray<Usif::CAppViewData> iViewInfoArray; //Application's view data information
	                RArray<TUid> iServiceUidList;
	                TLanguage iAppLanguageForCurrentLocale;
	                RPointerArray<HBufC> iAppOwnedFiles;
	            };
	
	class CRegInfoForApplicationSubsession : public CScrSubsession
	    /**
	        This object is created for an App Uid subsession opened by the SCR client
	        to retrieve the App info.
	     */
	    {
	    public:
	        static CRegInfoForApplicationSubsession* NewL(CScrSession& aSession);
	        ~CRegInfoForApplicationSubsession();
	        // Implement CScsSession
	        TBool DoServiceL(TInt aFunction, const RMessage2& aMessage);
	                
	    private:
	        CRegInfoForApplicationSubsession(CScrSession& aSession);
	        void ConstructL();       
	    private:
	        TUid iAppUid;
	        CRegInfoForApplicationSubsessionContext *iSubsessionContext;
	        };  

	   class CApplicationRegInfoSubsessionContext : public CBase
	       {
	       friend class CScrRequestImpl;
	       friend class CApplicationRegInfoSubsession;
	   public:
	       ~CApplicationRegInfoSubsessionContext();
	       private:
	       RPointerArray<Usif::CServiceInfo> iServiceInfoArray; //Application's service info details
	       CAppServiceInfoFilter* iAppServiceInfoFilter;
	       };

	   class CApplicationRegInfoSubsession : public CScrSubsession
        /**
            This object is created for an App Uid subsession opened by the SCR client
            to retrieve the App info.
         */
	    {
        public:
            static CApplicationRegInfoSubsession* NewL(CScrSession& aSession);
            ~CApplicationRegInfoSubsession();
	        // Implement CScsSession
	        TBool DoServiceL(TInt aFunction, const RMessage2& aMessage);
	        
        private:
            void ConstructL();
	    private:
	        CApplicationRegInfoSubsession(CScrSession& aSession);	            
	        CApplicationRegInfoSubsessionContext *iSubsessionContext;
	    };  
	
	class CAppRegistrySubsessionContext : public CBase
	        {
	        friend class CScrRequestImpl;
	        friend class CAppRegistrySubsession;
	        public:
	            ~CAppRegistrySubsessionContext();
	        private:
	            TLanguage iLanguage;
	            TInt iAppRegIndex;       
	            RArray<TUid> iAppUids;       //array of all the App Uid's      
	        };
	
    class CAppRegistrySubsession : public CScrSubsession
	    /**
	       This object is created for an Application registration subsession 
	       opened by the SCR client to retrieve the Application registration info.
	    */
	    {
	    public:
	        static CAppRegistrySubsession* NewL(CScrSession& aSession);
	        ~CAppRegistrySubsession();
	        // Implement CScsSession
	        TBool DoServiceL(TInt aFunction, const RMessage2& aMessage);
	    private:
	        CAppRegistrySubsession(CScrSession& aSession); 
	        void ConstructL();	   
	        CApplicationRegistrationData* iApplicationRegistrationInfo;
	        CAppRegistrySubsessionContext* iSubsessionContext;
	     };
    
	}// End of namespace Usif


#endif /* SCRSUBSESSION_H */

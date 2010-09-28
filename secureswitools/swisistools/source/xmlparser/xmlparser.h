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
* CScrXmlParser - Used to retrieve details from xml files for creating database and 
* Software environment updates.
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef XMLPARSER_H
#define XMLPARSER_H
#pragma warning(disable: 4786)
#pragma warning(disable: 4291)

#include <vector>
#include <string>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/dom/DOM.hpp>

#include "toolsconf.h"
#include "symbiantypes.h"
#include "utf8_wrapper.h"

std::string ConvertOpaqueData(const std::string& opaqueData);

// Xerces library uses XMLCh (UTF16 format) as its default character type.
// We can store the UTF16 returned form the xerces library in the following
// template class.
typedef std::basic_string<XMLCh> XercesString;


/**
* This template is used to cleanup memory by calling a function as pointed 
* by the first parameter as a function parameter. It can be used only used
* with functions which take a pointer to a pointer as an argument. This is 
* currently a limitation as the xerces library takes the same to release 
* its own memory.
*/

template <typename tFnPtr, typename tPtr> class fn_auto_ptr 
	{
	public:
		explicit fn_auto_ptr(tFnPtr aFnPtr, tPtr* aPtr)
			: iFnPtr(aFnPtr),
			iPtr(aPtr)
		{}

		~fn_auto_ptr() { iFnPtr(&iPtr); }
		
		tPtr *get() const throw()
			{
			return (iPtr); 
			}

	private:
		tFnPtr iFnPtr;
		tPtr* iPtr;
	};


template <typename tFnPtr, typename tPtr> class mem_fn_auto_ptr 
	{
	public:
		
		explicit mem_fn_auto_ptr(tFnPtr aFnPtr, tPtr aPtr)
			: iFnPtr(aFnPtr),
			iPtr(aPtr)
			{}
		
		~mem_fn_auto_ptr() { (iPtr->*iFnPtr)(); }
		
		tPtr operator->() const throw()
			{
			return (get()); 
			}

		tPtr get() const throw()
			{
			return (iPtr); 
			}

	private:
		tFnPtr iFnPtr;
		tPtr iPtr;
	};

template <typename tFnPtr> class static_fn_auto_ptr 
	{
	public:
		
		explicit static_fn_auto_ptr(tFnPtr aFnPtr)
			: iFnPtr(aFnPtr)
			{}
		
		~static_fn_auto_ptr() { &iFnPtr; }

	private:
		tFnPtr iFnPtr;
	};

namespace XmlDetails
{
	struct TScrEnvironmentDetails
			{
		public:
			TScrEnvironmentDetails()
			:iSifPluginUid(0)
			{}
			
			class TLocalizedSoftwareTypeName
				{
			public:
				TLocalizedSoftwareTypeName()
				:iLocale(0)
				{}
								
				int iLocale;
				std::wstring iName;
				};

            class TCustomAccessList
				{
			public:
				TCustomAccessList()
				:iAccessMode(1)
				{}
								
				int iSecureId;
				int iAccessMode;;
				};
			
			std::wstring iUniqueSoftwareTypeName;
			std::vector<TLocalizedSoftwareTypeName> iLocalizedSoftwareTypeNames;
			int iSifPluginUid;
            std::wstring iLauncherExecutable;
			std::vector<TCustomAccessList> iCustomAccessList;
			std::vector<std::wstring> iMIMEDetails;
			}; // struct TScrEnvironmentDetails

		
	class TScrPreProvisionDetail
		{
	public:
		TScrPreProvisionDetail()
		{}

		class TComponentLocalizable
			{

		public:
			TComponentLocalizable()
			:iLocale(0)
			{}

		public: // public Functions
			inline XercesString Name();
			inline XercesString Vendor();
		
		public: // Member variables	
			int iLocale;
			std::wstring iName;
			std::wstring iVendor;
			}; // class TComponentLocalizable
		
		class TComponentProperty
			{

		public:
			TComponentProperty()
			:iLocale(0),
			iIsIntValue(false),
			iIsStr8Bit(false)
			{}

		public: // Member Functions
			inline XercesString Name();
			inline XercesString Value();

		public: // Member variables
			std::wstring iName;
			int iLocale;
			std::wstring iValue;
			bool iIsIntValue;
			int iIsStr8Bit;
			}; // class TComponentProperty

		class TComponentDependency
			{
		public:
			class TComponentDependencyDetail
				{
				public: // Member Functions
					inline XercesString SupplierId();
					inline XercesString FromVersion();
					inline XercesString ToVersion();

				public: // Member Variables
					std::wstring iSupplierId;
					std::wstring iFromVersion;
					std::wstring iToVersion;
				}; // class TComponentDependencyDetail

			public: // Member Functions
				inline XercesString DependentId();

			public: // Member Variables
				std::wstring iDependentId;
				std::vector<TComponentDependencyDetail>	iComponentDependencyList;
			}; // class TComponentDependency

		class TComponentFile
			{
			public:
			TComponentFile()
			{}

			struct TFileProperty
				{	
			public:
				TFileProperty()
				:iIsIntValue(false)
				{}
			public: // Member Functions
				inline XercesString Name();
				inline XercesString Value();
	
			public: // Member Variables
				std::wstring iName;
				std::wstring iValue;
				bool iIsIntValue;
				}; // struct TFileProperty

			public: // Member Functions
			inline XercesString Location();
			
			public: // Member Variables
			std::wstring iLocation;
			std::vector<TFileProperty> iFileProperties;
			
			}; // class TComponentFile
		
		class TComponentDetails
			{
			public:
			TComponentDetails()

			:iIsRomApplication(0),
			iIsRemovable(1),
			iSize(0),
			iScomoState(1),
			iOriginVerified(1),
			iIsHidden(0)
			{}

			class TVersion
				{
			public:

				TVersion()
				{}
			public:
				inline XercesString MajorVersion();
				inline XercesString MinorVersion();
				inline XercesString BuildVersion();

			public:
				std::wstring iMajor;
				std::wstring iMinor;
				std::wstring iBuild;
				}; // class TVersion

			public: // Member Functions
			inline XercesString GlobalId();

			int iIsRomApplication;
			int iIsRemovable;
			TInt64 iSize;
			int iScomoState;
			std::wstring iGlobalId;
			TVersion iVersion;
			int iOriginVerified;
			int iIsHidden;
			};
		
		class TApplicationRegistrationInfo
			{
			public:
				TApplicationRegistrationInfo()
				{}

				class TAppAttribute
					{
					public:
					TAppAttribute()
					:iIsIntValue(false),
					iIsStr8Bit(false)
					{}
					std::wstring iName;
					std::wstring iValue;
					bool iIsIntValue;
					int iIsStr8Bit;
					};

				class TDataType
					{
					public:
					TDataType()
					{}
					int iPriority;
					std::wstring iType;
					};

				class TOpaqueDataType
					{
					public:
					TOpaqueDataType()
					:iLocale(0),
					iIsBinary(false)
					{}
					int iLocale;
					int iServiceUid;
					bool iIsBinary; 
					std::string iOpaqueData;
					};

				class TAppServiceInfo
					{
					public:
					TAppServiceInfo()
					{}
					int iUid;
					std::vector<TDataType> iDataType;
					};

				class TAppLocalizableInfo
					{
					public:
					TAppLocalizableInfo()
					{}

					class TLocalizableAttribute
						{
						public:
						TLocalizableAttribute()
						:iIsIntValue(false),
						iIsStr8Bit(false)
						{}
						std::wstring iName;
						std::wstring iValue;
						bool iIsIntValue;
						int iIsStr8Bit;
						};

					class TViewData
						{
						public:
						TViewData()
						{}

						class TViewDataAttributes
						{
						public:
						TViewDataAttributes()
						:iIsIntValue(false),
						iIsStr8Bit(false)
						{}
						std::wstring iName;
						std::wstring iValue;
						bool iIsIntValue;
						int iIsStr8Bit;
						};
						
						std::vector<TViewDataAttributes> iViewDataAttributes;
						};

					std::vector<TLocalizableAttribute> iLocalizableAttribute;
					std::vector<TViewData> iViewData;
					};

				class TAppProperty
					{
					public:
					TAppProperty()
					:iLocale(0),
					iIntValue(0)
					{}
					int iLocale;
					std::wstring iName;
					int iServiceUid;
					int iIntValue;
					std::string iStrValue;
					bool iIsStr8Bit;
					};

			std::vector<TAppAttribute> iApplicationAttribute;
			std::vector<std::wstring> iFileOwnershipInfo;
			std::vector<TAppServiceInfo> iApplicationServiceInfo;
			std::vector<TAppLocalizableInfo> iApplicationLocalizableInfo;
			std::vector<TAppProperty> iApplicationProperty;
			std::vector<TOpaqueDataType> iOpaqueDataType;
			};
		
		class TComponent
			{
			public:
			TComponent()
			:iComponentDetails()
			{}

			std::vector<TComponentLocalizable>	iComponentLocalizables;
			std::vector<TComponentProperty>		iComponentProperties;
			std::vector<TComponentFile>			iComponentFiles;
			TComponentDependency iComponentDependency;
			std::vector<TApplicationRegistrationInfo> iApplicationRegistrationInfo;
			TComponentDetails iComponentDetails;
			}; // class TComponent

		inline XercesString SoftwareTypeName();

		std::wstring iSoftwareTypeName;
		std::vector<TComponent> iComponents;
		
		}; // class TScrPreProvisionDetail
}

class CScrXmlParser
	{

	public:
		
		/**
		 * Initializes the xml parser and logging feature based on supplied logging parameters.
		 */
		DllExport CScrXmlParser();
		
		/**
		 * Frees allocated memory.
		 */
		DllExport ~CScrXmlParser();
		
		/**
		 * Retrieves the database schema for database creation, from the supplied database file. 
		 */
		DllExport std::vector<std::string>* ParseDbSchema(const std::string& aDbFile);
		
		/**
		 * Retrieves software environment details, from the supplied xml file. 
		 */
		DllExport std::vector<XmlDetails::TScrEnvironmentDetails>* GetEnvironmentDetails(const std::string aEnvDetailFile );
		
		XmlDetails::TScrPreProvisionDetail GetPreProvisionDetails( const std::string aPreProvisionFile );

	private:
		
		/**
		 * Called while retrieving s/w environment details. 
		 */

		XmlDetails::TScrEnvironmentDetails GetEnvironmentData( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);
		
		XmlDetails::TScrPreProvisionDetail::TComponent GetPreProvisionData( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);
		
		XmlDetails::TScrPreProvisionDetail::TComponentLocalizable GetComponentLocalizable( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);
		
		XmlDetails::TScrPreProvisionDetail::TComponentProperty GetComponentProperty( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);
		
		XmlDetails::TScrPreProvisionDetail::TComponentFile GetComponentFile( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);

		XmlDetails::TScrPreProvisionDetail::TComponentDependency GetComponentDependency( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);
		
		XmlDetails::TScrPreProvisionDetail::TComponentFile::TFileProperty GetFileProperty( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);
		
		XmlDetails::TScrPreProvisionDetail::TComponentDetails GetComponentDetails( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);
		
		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo GetApplicationRegistrationInfo(const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);

		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppAttribute GetAppAttribute( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);

		std::wstring GetFileOwnershipInfo( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);

		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TDataType GetDataType( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);

		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TOpaqueDataType GetOpaqueDataType( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);

		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TOpaqueDataType GetServiceOpaqueDataType( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);

		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppServiceInfo GetAppServiceInfo( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement, XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo& aAppRegistrationInfo);

		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo GetAppLocalizableInfo( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);

		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TLocalizableAttribute GetAppLocalizableAttribute( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);

		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TViewData GetAppLocalizableViewData( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);

		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TViewData::TViewDataAttributes GetAppLocalizableViewDataAttributes( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);

		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppProperty GetAppProperty( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);

		XmlDetails::TScrEnvironmentDetails::TLocalizedSoftwareTypeName GetLocalizedSoftwareTypeName( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);
		
        XmlDetails::TScrEnvironmentDetails::TCustomAccessList GetCustomAccessList(const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);

		void ConfigDomParser(xercesc::XercesDOMParser& aDomParser);
	};


XERCES_CPP_NAMESPACE_BEGIN

/**
 * 
 * SchemaErrorHandler - Implementation of the error handler for xml schema validation.
 *						The parser used, implements this customized error handling interface.
 *						This has been registered with the xml parser in the constructor. The parser 
 *						reports all errors and warnings through this interface.
 */

class SchemaErrorHandler : public ErrorHandler
	{
	public:
		/**
		 * Handles all warnings received while xml parsing. 
		 */
		void warning (const SAXParseException &exc);
		
		/**
		 * Handles all errors received while xml parsing. 
		 */
		void error (const SAXParseException &exc);
		
		/**
		 * Handles all fatal errors received while xml parsing. 
		 */
		void fatalError (const SAXParseException &exc);
		
		/**
		 * Called if an error occurs while reintializing the current xml handler. 
		 */
		void resetErrors ();
	};

XERCES_CPP_NAMESPACE_END

// inline function definitions

#ifndef _WIN32
inline XercesString WStringToXercesString(const std::wstring& aString)
	{
	XMLCh* buffer = new XMLCh[ (aString.length() + 1) * 2 ];
	XMLCh* temp = buffer;
	const wchar_t* source = aString.c_str();

	ConvertUCS4toUTF16(&source, source + aString.length(), &temp, buffer + aString.length());

	// Appending NUL to the converted buffer.
	*temp = 0;

	XercesString result(buffer);
	delete[] buffer;

	return result; 
	}

#else

// We need not do anything for WINDOWS, since XercesString
// and WString both are same and will be in UTF-16 encoding format.
#define WStringToXercesString(aWString) (aWString)

#endif // _WIN32


//------------------------------------------------------------------------------------------------------------------------------
//											UTILITY FUNCTIONS
//------------------------------------------------------------------------------------------------------------------------------
#ifndef _WIN32

inline std::wstring XercesStringToWString(const XercesString& aString)
	{
	wchar_t* buffer = new wchar_t[aString.length() + 1];
	const XMLCh* source = aString.c_str();

	// Using a temp variable in place of buffer as ConvertUTF16toUCS4 modifies the source pointer passed.
	wchar_t* temp = buffer;

	ConvertUTF16toUCS4(&source, source + aString.length(), &temp, temp + aString.length());

	// Appending NUL to the converted buffer.
	*temp = NULL;

	std::wstring result(buffer);
	delete[] buffer;
	return result;
	}
#else

// We need not do anything for WINDOWS, since XercesString
// and WString both are same and will be in UTF-16 encoding format.
#define XercesStringToWString(aXercesString) (aXercesString)

#endif // _WIN32


//------------------------------------------------------------------------------------------------------------------------------



inline XercesString XmlDetails::TScrPreProvisionDetail::TComponentDetails::TVersion::MajorVersion()
{
#ifdef _WIN32
	return iMajor;
#else
	return WStringToXercesString(iMajor);
#endif // _WIN32
}


inline XercesString XmlDetails::TScrPreProvisionDetail::TComponentDetails::TVersion::MinorVersion()
{
#ifdef _WIN32
	return iMinor;
#else
	return WStringToXercesString(iMinor);
#endif // _WIN32
}


inline XercesString XmlDetails::TScrPreProvisionDetail::TComponentDetails::TVersion::BuildVersion()
{
#ifdef _WIN32
	return iBuild;
#else
	return WStringToXercesString(iBuild);
#endif // _WIN32
}


inline XercesString XmlDetails::TScrPreProvisionDetail::TComponentDetails::GlobalId()
{
#ifdef _WIN32
	return iGlobalId;
#else  
	return WStringToXercesString(iGlobalId);
#endif // _WIN32
}


inline XercesString XmlDetails::TScrPreProvisionDetail::SoftwareTypeName()
	{
#ifdef _WIN32
	return iSoftwareTypeName;
#else
	return WStringToXercesString(iSoftwareTypeName);
#endif // _WIN32
	}


inline XercesString XmlDetails::TScrPreProvisionDetail::TComponentLocalizable::Name()
	{
#ifdef _WIN32
	return iName;
#else
	return WStringToXercesString(iName);
#endif // _WIN32
	}


inline XercesString XmlDetails::TScrPreProvisionDetail::TComponentLocalizable::Vendor()
	{
#ifdef _WIN32
	return iVendor;
#else
	return WStringToXercesString(iVendor);
#endif // _WIN32
	}


inline XercesString XmlDetails::TScrPreProvisionDetail::TComponentProperty::Name()
	{
#ifdef _WIN32
	return iName;
#else
	return WStringToXercesString(iName);
#endif // _WIN32
	}

inline XercesString XmlDetails::TScrPreProvisionDetail::TComponentProperty::Value()
	{
#ifdef _WIN32
	return iValue;
#else
	return WStringToXercesString(iValue);
#endif // _WIN32
	}


inline XercesString XmlDetails::TScrPreProvisionDetail::TComponentFile::Location()
	{
#ifdef _WIN32
	return iLocation;
#else
	return WStringToXercesString(iLocation);
#endif // _WIN32
	}

inline XercesString XmlDetails::TScrPreProvisionDetail::TComponentFile::TFileProperty::Name()
	{
#ifdef _WIN32
	return iName;
#else
	return WStringToXercesString(iName);
#endif // _WIN32
	}

inline XercesString XmlDetails::TScrPreProvisionDetail::TComponentFile::TFileProperty::Value()
	{
#ifdef _WIN32
	return iValue;
#else
	return WStringToXercesString(iValue);
#endif // _WIN32
	}

inline XercesString XmlDetails::TScrPreProvisionDetail::TComponentDependency::DependentId()
	{
#ifdef _WIN32
	return iDependentId;
#else
	return WStringToXercesString(iDependentId);
#endif // _WIN32
	}

inline XercesString XmlDetails::TScrPreProvisionDetail::TComponentDependency::TComponentDependencyDetail::SupplierId()
	{
#ifdef _WIN32
	return iSupplierId;
#else
	return WStringToXercesString(iSupplierId);
#endif // _WIN32
	}

inline XercesString XmlDetails::TScrPreProvisionDetail::TComponentDependency::TComponentDependencyDetail::FromVersion()
	{
#ifdef _WIN32
	return iFromVersion;
#else
	return WStringToXercesString(iFromVersion);
#endif // _WIN32
	}

inline XercesString XmlDetails::TScrPreProvisionDetail::TComponentDependency::TComponentDependencyDetail::ToVersion()
	{
#ifdef _WIN32
	return iToVersion;
#else
	return WStringToXercesString(iToVersion);
#endif // _WIN32
	}









#endif // _XMLPARSER_H

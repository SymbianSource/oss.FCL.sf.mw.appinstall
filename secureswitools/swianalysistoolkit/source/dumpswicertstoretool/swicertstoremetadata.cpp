/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#include "swicertstoremetadata.h"
#include "datalocator.h"

using namespace std;
const int KMandatory = 0x00;
const int KSystemUpgrade = 0x01;

SwiCertStoreMetaData ::SwiCertStoreMetaData (Stream& aStream, StoreFile& aStore)
	:iStream(aStream), 
	iStore(aStore)
	{
	}

void SwiCertStoreMetaData ::Read(const Options& aOptions)
	{
	iCount++;
	DataLocator dataLoc(iStream, iStore.File());
	StreamReader reader(dataLoc.GetStream());
	reader.Initialise();
	int certCount = reader.ReadInt32();
	for (int j = 0; j < certCount; ++j)
		{
		iAppId.clear();
		reader.ReadInt8();	
		int certSize = reader.ReadInt32(); // certificate size

		if(!aOptions.CreateCCIFile() && !aOptions.CreateDetailCCIFile())
			{
			if(!certSize)
				{
				cout << "warning : " << " Empty Certificate " << endl; 
				}
			}

		bool isWide;
		int certLabelLen = reader.GetDescriptorInfo(isWide, true);	// certificate label length
		char* buf = reader.ReadDescriptor(); 		// certificate label
		string certName((const char*)buf , certLabelLen);
		iCertName = certName;
		iCertTag = iCertName;
		delete [] buf;
		iCertName.append(".der");
			
		unsigned int certId = reader.ReadInt32(); // certificate ID
		unsigned char certOwnerType = reader.ReadInt8(); // certificate owner type(should be only CA certificate)
		if(!aOptions.CreateCCIFile() && !aOptions.CreateDetailCCIFile())
			{
			if(certOwnerType)							//for CA certificate certOwnerType is 0.
				{
				cout << "warning : " << "Not a CA certificate " << endl;
				}
			}
		
		buf = reader.ReadDescriptor(); // subject key ID
		delete [] buf;
		
		buf = reader.ReadDescriptor(); // issuer key ID
		delete [] buf;
		
		int apps = reader.ReadInt32(); // number of applications
	
		for (int i = 0; i < apps; ++i)
			{
			unsigned int app = reader.ReadInt32(); // application UID
			iAppId.push_back(app);
			}
		
		unsigned char trustStatus = reader.ReadInt8(); // trusted flag
		if(!aOptions.CreateCCIFile() && !aOptions.CreateDetailCCIFile())
			{
			if(!trustStatus)
				{
				cout << "warning : " << iCertTag << ": Untrusted  certificate " <<endl;
				}
			}
			
		int streamId = reader.ReadInt32(); // data stream ID
		
		int capsLength = reader.GetDescriptorInfo(isWide, true);

		buf = reader.ReadDescriptor(); // capabilities buffer
		iFileWrite = "\0";
		if( aOptions.CreateDetailCCIFile() )
			{
			iCertCapabilities = ExtractDetailCapabilities(buf,capsLength);
			}
		else
			{
			iCertCapabilities = ExtractCapabilities(buf,capsLength);
			}
		
		delete [] buf;
		
		// This value contains both mandatory and system upgrade values
		iValue = reader.ReadInt8();
		bool isDisplay = ExtractCertificate(iCertName , aOptions , streamId , iValue);
		if( !aOptions.CreateCCIFile() && !aOptions.CreateDetailCCIFile() && isDisplay )
			{
			Display(aOptions);
			}
		}
	}

const string SwiCertStoreMetaData ::ExtractCapabilities(char* aCapability, int aCapsLength)
   	{
   	string capability;
  	int cap = 0;
   	int unknownIndex = 1;
  	char CapsString[10];
   	for(int j=0; j<aCapsLength; j++)
   		{
   		if(*aCapability & 0x01)
   			{
   			if(j < KNumberOfCaps)
   				{
   				capability.append("capability = ");
   				capability.append(CapabilityList[j]);
   				capability.append("\n"); 
   				*aCapability = *aCapability >> 1;
   				}
   			else
   				{
   				capability.append("capability= ");
   				capability.append("Unknown"+unknownIndex);
   				unknownIndex++;
   				}
  			cap += pow((double)2,j);
   			}
   		else
   			{
   			*aCapability = *aCapability >> 1;
   			}
   
   		if((j+1)%(sizeof(aCapability)*2) == 0)
   			{
   			aCapability ++;
   			}
   		}
  	sprintf(CapsString,"%d",cap);
  	iFileWrite.append(CapsString);
  	iFileWrite.append("\n");	

   	return capability;
   	}


const string SwiCertStoreMetaData ::ExtractDetailCapabilities(char* aCapability, int aCapsLength)
	{
	string capability;
	int unknownIndex = 1;
	
	for(int j=0; j<aCapsLength; j++)
		{
		if(*aCapability & 0x01)
			{
			if(j < KNumberOfCaps)
				{
				capability.append("capability = ");
				capability.append(CapabilityList[j]);
				capability.append("\n"); 
				*aCapability = *aCapability >> 1;
				}
			else
				{
				capability.append("capability= ");
				capability.append("Unknown"+unknownIndex);
				unknownIndex++;
				}
			}
		else
			{
			*aCapability = *aCapability >> 1;
			}

		if((j+1)%(sizeof(aCapability)*2) == 0)
			{
			aCapability ++;
			}
		}
	
	iFileWrite.append(capability);
	iFileWrite.append("\n");
	return capability;
	}

void SwiCertStoreMetaData::Display(const Options& aOptions)
	{
	string iniFileWrite,appID;
	for (vector<unsigned int>::iterator it = iAppId.begin(); it!= iAppId.end();++it)
		{
		if (*it == ESwInstallUid)
			{
			appID.append("application = SWInstall\n");
			}
		if (*it == ESwinstallOcspUid)
			{
			appID.append("application = SWInstallOCSP\n");
			}
		}

	int value = (int)iValue;
	int isMandatory = !!( value & (1 << KMandatory)); 
	int isSystemUpgrade = !!(value & (1 << KSystemUpgrade));

	char buf[2],buf1[2];
	sprintf(buf,"%d",isMandatory);
	sprintf(buf1,"%d",isSystemUpgrade);
	string mandatory = 	buf;
	string systemUpgrade = buf1;
	
	iniFileWrite = "\n["+iCertTag+"]\n\nfile = "+ iCertName+"\n"+iCertCapabilities+appID+"Mandatory = "+mandatory+"\n"+"SystemUpgrade = "+systemUpgrade+"\n";
	cout << iniFileWrite << endl;
	ofstream iniFile("swicertstore.txt",ios::app);
	iniFile.write(iniFileWrite.c_str(),iniFileWrite.length());
	iniFile.close();
	}

bool SwiCertStoreMetaData::ExtractCertificate(std::string& aCertLabel,const Options& aOptions,int aStreamId,unsigned char aMandatory)
	{
	Handle handle(aStreamId);
	Stream stream = iStore.FindStream(handle);
	Certificate cert(stream, iStore);
	bool isDisplay = cert.Read(aCertLabel.c_str(),aOptions);
	
	const char* buf = cert.GetIssuerName().c_str();
	char* issuername = strstr(buf ,"/CN");
	if (issuername)
		{
		issuername += 4;
		}
	else
		{
		issuername = strstr (buf, "/");
		if (issuername)
			{
			issuername += 3;
			}
		}
	if (issuername)
		{
		char* endstr = strchr (issuername, '/');
		if (endstr)
			{
			*endstr = 0;
			}
		}

	if(isDisplay)
		{
		iIssuer.append(issuername);
		iIssuer.append("\n");
		iIssuer.append("----------------------");
		iIssuer.append("\n");
		iIssuer.append(iFileWrite);
		iMandatoryCert.append(issuername);
		iMandatoryCert.append(" = ");

		char mandatoryStr[2];
		sprintf(mandatoryStr,"%d",(int)aMandatory);
		iMandatoryCert.append(mandatoryStr);
		iMandatoryCert.append("\n");
		}
	return isDisplay;
	}

SwiCertStoreMetaData::~SwiCertStoreMetaData()
	{
	}

int SwiCertStoreMetaData::iCount = 0;

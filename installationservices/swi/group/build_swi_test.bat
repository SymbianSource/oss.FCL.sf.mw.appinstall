@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of the License "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem
rem Compilations needed in order to compile swi test code
rem =========================================================
 
 
rem If SWI production code is not comiled the following tests 
rem fail consistently: swicaptests, testappinuse and tsisregistrytest
call cd %SECURITYSOURCEDIR%\installationservices\swi\group
call bldmake bldfiles 
call abld -v -k  build winscw udeb

  
rem perform test exports  
-----------------------------
 
call cd %SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\group
call bldmake -k bldfiles 
call abld -v -k test export

call cd %SECURITYSOURCEDIR%\caf2\test\RefTestAgent\group
call bldmake -k bldfiles 
call abld -v -k test export
 
call cd %SECURITYSOURCEDIR%\installationservices\switestfw\group
call bldmake -k bldfiles 
call abld -v -k test export
 
rem If securityconfig test export is not performed there are 
rem numerous test failures.  
call cd %SECURITYSOURCEDIR%\installationservices\swiconfig\group
call bldmake -k bldfiles 
call abld -v -k test export

call cd %SECURITYSOURCEDIR%\installationservices\swi\group
call bldmake -k bldfiles 
call abld -v -k test export

call cd %SECURITYSOURCEDIR%\installationservices\swidevicetools\group
call bldmake -k bldfiles 
call abld -v -k test export

call cd %SECURITYSOURCEDIR%\secureswitools\swisistools\group
call bldmake -k bldfiles 
call abld -v -k test export
 


rem perform test compilation 
----------------------------

call cd %SECURITYSOURCEDIR%\commonutils\group
call bldmake bldfiles 
call abld -v -k test build winscw udeb

call cd %SECURITYSOURCEDIR%\swi\group
call bldmake bldfiles 
call abld -v -k test build winscw udeb



rem Additional compilations needed to execute swi tests
rem ===================================================


rem Needed for several testcases
call cd %SECURITYSOURCEDIR%\testframework\group
call bldmake bldfiles 
call abld -v -k test build winscw udeb

rem Needed for tests: testdrm, testdrmasync and testdrmFH
call cd %SECURITYSOURCEDIR%\caf2\test\RefTestAgent\groupcall bldmake bldfiles 
call abld -v -k test build winscw udeb
 

rem Needed for tests: cr956_test, tswiconsole and tswiconsole_allowunsigned
call cd %SECURITYSOURCEDIR%\swi\devicetools\group
call bldmake bldfiles 
call abld -v -k test build winscw udeb


rem Needed for testcase testmulti
call cd %SECURITYSOURCEDIR%\swi\sistools\group
call bldmake bldfiles 
call abld -v -k test build winscw udeb


 
  
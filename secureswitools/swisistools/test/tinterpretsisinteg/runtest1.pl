#
# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
# This perl runtest1.pl script file calls the batch file which will internally execute runtest.pl 
#


my $epocroot = $ENV{'EPOCROOT'};
my $path = $epocroot."epoc32\\winscw\\c\\tswi\\tinterpretsisinteg\\runtest1.bat";

system"$path $ARGV[0]" ;


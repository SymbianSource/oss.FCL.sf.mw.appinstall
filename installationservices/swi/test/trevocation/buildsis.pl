#
# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Test tool used to make and sign SISX files signed by certificate
# chains of aribtrary lengths as defiend by a template file.
#

use warnings;
use strict;
use Getopt::Long;

my $CA_DIR='..\\tsisfile\\data\\signedsis';
my $CERTS_DIR="certs";
my $REQS_DIR="reqs";
my $KEYS_DIR="keys";
my $PKG_DIR="pkg";
my $SIS_DIR="sis";
my $TEMPLATE_DIR="templates";
my $UID_FILE="uid.txt";
my $INI_DIR="ini";
my $SCRIPT_DIR="c:\\tswi\\trevocation\\scripts";
my $DATA_DIR="c:\\tswi\\trevocation\\data";
my $TMP_DIR="tmp";
my $DAYS="3650";

my $PLATFORM = '';
my $CONFIGURATION = '';

my $CA_TEXT=
'
subjectKeyIdentifier=hash
#authorityKeyIdentifier=keyid:always,issuer:always
basicConstraints=critical,CA:TRUE, pathlen:5
keyUsage=critical,keyCertSign

';


&main;
exit(0);

sub main() {
	my $section = '';
	my $packageTemplate = '';
	my @chains = ();
	my @files = ();
	my $installText = '';
	my $uninstallText = '';
	my $iniFile = '';
	my $name = '';

	GetOptions('platform=s' => \$PLATFORM,
			   'configuration=s' => \$CONFIGURATION,
			   'sis-dir=s' => \$SIS_DIR,
			   'days=s', \$DAYS);
	
	if ($#ARGV < 0) {
		&usage;
	}
	my ($config) = @ARGV;
	open IN, $config or die "Cannot open $config\n";

	while (<IN>) {
		chomp;

		s/<DATA_DIR>/${DATA_DIR}/g;
		s/<SCRIPT_DIR>/${SCRIPT_DIR}/g;
		s/<PLATFORM>/$PLATFORM/g;
		s/<CONFIGURATION>/$CONFIGURATION/g;
		
		# Extract section name
		if (/^\s*\[\w+\]/) {
			s/.*\[\s*(.*)\s*].*/$1/g;
			$section = lc;			
		}
    	elsif ($section eq "install") {
			$installText .= "$_\n";
	    }
      	elsif ($section eq "uninstall") {
			$uninstallText .= "$_\n";
	    }
		# Skip comments
     	elsif (/^\s*\#/) {
			next;
	    }
	    elsif (/\s*\w+\s*=.*/) { # Parse key/value pair

			my ($key, $value) = split /=/;

			$key =~ s/\s//g;			
			$key = lc($key);
			if ($section eq "chains") {
				if ($key =~ /chain\d+/) {
					parseChain($value, \@chains);
				}
			}
			elsif ($section eq "main") {
				if ($key eq 'template') {
					$packageTemplate = $value;
				}
				elsif ($key eq 'name') {
					$name = $value;
				}
                elsif ($key eq 'inifile') {
					$iniFile = $value;
				}
			}
			elsif ($section eq "files") {
				if ($key =~ /^\s*sis.*/) {
					my $componentUID = embeddedSIS($value);
					my $sisText = "\@\"${SIS_DIR}\\${PLATFORM}\\$value\",(${componentUID})\n";
					push @files, $sisText;
				}
				else {
					push @files, "$value\n";
				}
			}
     	}
	}
   close IN;	

   die "\'iniFile\' not defined in main section" unless $iniFile ne '';
   die "\'name\' not defined in main section" unless $name ne '';

   my @chainFiles;  # The combined chain files
   my @chainKeys;   # The keys for the leaf of each chain

   my $chainNum = 0;
   foreach my $chain (@chains) {
	   print "\tProcessing chain $chainNum\n";
	   buildChain($name, 
				  $chainNum, 
				  \@chainFiles,
				  \@chainKeys,
				  @$chain);
	   $chainNum++;
   }

   my $temp = $packageTemplate;
   $temp =~ s/.*\\(\w+).*/$1/;	

   my $packageFile = "${PKG_DIR}\\${name}.pkg";
   my $uid = getUID();
   buildPackage($PLATFORM, 
				$CONFIGURATION, 
				$uid,
				"${TEMPLATE_DIR}\\$packageTemplate", 
				$packageFile,
				@files);
   
   # Create unsigned SIS file
   mkdir("${SIS_DIR}\\${PLATFORM}");
   my $sisFile = "${SIS_DIR}\\${PLATFORM}\\${name}.sis";
   makesis($packageFile, $sisFile);

  for (my $i = 0; $i <= $#chainFiles; $i++) {
	  signsis("${sisFile}", "${sisFile}.tmp", $chainFiles[$i], $chainKeys[$i]);
	  unlink $sisFile;
	  rename "${sisFile}.tmp", "${sisFile}";
  }

  # Add install/uninstall script entries for this package
  addIniEntries($iniFile, $uid, $name, $installText, $uninstallText);
}

sub buildChain(\$\$\$\$\@) {
	my ($name, $chainNum, $chainFiles, $chainKeys, @extensions) = @_;
  
	my $root = shift @extensions;
	my $certNum = 0;
	my $signingCert = "${CA_DIR}\\$root\\ca.pem";
	my $signingKey  = "${CA_DIR}\\$root\\ca.key.pem";
	my @chainElements = (); # List of cert filenames in the current chain

	# For each extension create a request for a certificate and
	# sign that request with the previous certificate, or the root
	# certificate for the first element
	foreach my $extension (@extensions) {
		my $isCA = $certNum < $#extensions;

		die "Cannot open extension file ext\\$extension.cfg\n"
			unless -e "ext/$extension.cfg";
		
		my $file = "${name}_${extension}.${chainNum}.${certNum}";
		my $subject = "/C=UK/O=Symbian/CN=Entity Cert ${name} ${extension} ${chainNum} ${certNum}";
		createRequest($isCA,
					  "${REQS_DIR}\\${file}.req.pem",
					  $subject,
					   "${KEYS_DIR}\\${file}.key.pem",
					  'openssl.cfg');
		
		signRequest($isCA, 
					"${REQS_DIR}\\${file}.req.pem",
					"${CERTS_DIR}\\${file}.cert.pem",
					$signingCert,
					$signingKey,
					"ext\\$extension.cfg");
		
		$signingCert = "${CERTS_DIR}\\${file}.cert.pem";
		$signingKey = "${KEYS_DIR}\\${file}.key.pem";
		
		push @chainElements, "${CERTS_DIR}\\${file}.cert.pem";
		$certNum++;
	}
	# Record the filename of the leaf signing key. This is needed by sisgnsis	
	push @$chainKeys, $signingKey; 

	# Concatenate all the certificates for signsis
	my $chainFile = "${CERTS_DIR}\\${name}_chain_${chainNum}.pem";
	push @$chainFiles, $chainFile;

	open OUT, ">$chainFile" or 
		die "Cannot write to $chainFile\n";
	foreach (@chainElements) {
		open IN, $_ or 
			die "Cannot open $_\n";
		while (<IN>) {
			print OUT;
		}
		close IN;
	}
	close OUT;
}

sub buildPackage(\$\$\$\$\$\@) {
	my ($PLATFORM, $CONFIGURATION, $packageUID, $packageTemplate, $packageFile, @files) = @_;
	my $packageUIDText = sprintf "0x%8.8x", $packageUID;
	
	print "\tCreating package $packageFile\n";

	open(IN, "${packageTemplate}") or
		die "Cannot open package file template $packageTemplate\n";

	open(OUT, ">$packageFile") or
		die "Cannot write to package file $packageFile\n";

	# Substitute the list of files for the place holder in the template
	# package file.
	while (<IN>) {
		s/\#\#PACKAGE_UID\#\#/${packageUIDText}/g;

		if (/\#\#FILES\#\#/) {
			foreach (@files) {
				# The SISX files have to be built for every supported
				# platform and configuration
				print OUT;
			}
		}
		else {
			print OUT;
		}
	}
	close IN;
	close OUT;
}

sub parseChain(\$\$) {
	my ($chainDesc, $chains) = @_;   
	my @elements = split(/,/, $chainDesc);
	push @$chains, \@elements;
}

sub createRequest(\$,\$\$\$\$) {
	my ($isCA, $requestFile, $subject, $keyFile, $config) = @_;

	my @args = ('openssl',
				'req',
				'-newkey',
				'rsa:512',
				'-nodes',
				'-out',
				$requestFile,
				'-keyout', 
				$keyFile,
				'-subj',
				$subject,
				'-config', 
				$config, 
				'-days', 
				'3650');

	system (@args) == 0 or
		die "Cannot create certificate request $requestFile\n";	
}

sub signRequest(\$\$\$\$\$) {
	my ($isCA, $requestFile, $certFile, $CAcert, $CAkey, $extfile) = @_;
	my $tmpFile = "${extfile}.tmp";

	my @args = ('openssl',
				'x509',
				'-req',
				'-in',
				$requestFile,
				'-out',
		 		$certFile,
				'-CA',
				$CAcert,
				'-CAkey',
				$CAkey,
				'-extfile',
				$tmpFile,
				'-days',
				"${DAYS}");
	
	# Workaround to insert CA extensions 
	# Open SSL expects all the extensions to be kept together so we
	# can't just specify the CA bits in the request because otherwise
	# it will overwrite them
	system 'copy', $extfile, $tmpFile;
	system 'attrib', '-r', $tmpFile;

	if ($isCA) {
		open OUT, ">>$tmpFile" or die "Cannot open temporary file $tmpFile\n";
		print OUT $CA_TEXT;
		close OUT;
	}

	system (@args) == 0 or 			   
			   die "Cannot sign certificate request $requestFile\n";
	unlink $tmpFile;
}

sub makesis(\$\$) {
	my ($packageFile, $sisFile) = @_;
	print "\tCreating SIS file $sisFile\n";
	system 'makesis', $packageFile, $sisFile;
}

sub signsis(\$\$\$\$) {
	my ($input, $output, $certificate, $key) = @_;

	print "\tSigning $input with certchain=${certificate} key=${key}\n";

	system('signsis',
		   $input,
		   $output,
		   $certificate,
		   $key) == 0 or
			   die "Cannot sign SIS file $input $output $certificate $key\n";
}

sub usage {
	print STDERR "Usage: buildsis.pl --platform <platform> --configuration <udeb|urel> --sis-dir <sis dir> <test spec>\n";
	exit -1;
}

# Make sure each package has a unique UID by repeatedly adding one
# from a pre-determined base.
sub getUID {

	if (! -e "uid.txt.tmp") {
		system 'copy','uid.txt','uid.txt.tmp';
		system 'attrib','-r','uid.txt.tmp';
	}

	open IN, "uid.txt.tmp" or die "Cannot open uid.txt.tmp\n";
	my $uidText = <IN>;
	my $uid = hex($uidText);
	close IN;   
	
	die "Invalid uid = $uidText\n" unless $uid != 0;
	
	open OUT, ">uid.txt.tmp" or die "Cannot write to uid.txt.tmp\n";
	print OUT sprintf("0x%8.8x", $uid + 1);
	close OUT;
	return $uid;
}

# Add install & uninstall entries
sub addIniEntries(\$\$\$\$\$) {
	my ($iniFile, $uid, $name, $installText, $uninstallText) = @_;
	
	print "\tAdding install entry to ${INI_DIR}\\${iniFile}\n";
	
	open OUT, ">>${INI_DIR}\\${iniFile}" or die "Cannot write to ${iniFile}\n";

	print OUT "[${name}]\n";
	print OUT "sis=${DATA_DIR}\\${name}.sis\n";
    print OUT "${installText}\n";

	print "\tAdding uninstall entry to ${INI_DIR}\\${iniFile}\n";

	print OUT "[u_${name}]\n";
	print OUT "uid=${uid}\n";
    print OUT "${uninstallText}\n";

	close OUT;
}

# Takes the name of an embedded SIS file and returns the text required 
# by the package body to embed this file.
# In addition, this function calculates the component uid of the package file
# by searching the package directory for a file which matches the name of the SIS 
# file.
sub embeddedSIS(\$) {
	my ($sisfile) = @_;

	my $absSISfile = "${SIS_DIR}\\${PLATFORM}\\$sisfile";

	die "Cannot find embedded sis file $absSISfile\n"
		unless (-e $absSISfile);		

	my $packageFile = $sisfile;
	$packageFile =~ s/\.sis/.pkg/g;
	
	my $absPackageFile = "${PKG_DIR}\\${packageFile}";
	open PKG, "${absPackageFile}" or die "Cannot open ${absPackageFile}\n";

	my $componentUID = '';
	while (<PKG>) {
		chomp;

		# Extract the component uid
		if (s/\s*\#.*\(([0-9A-Fa-fx]+)\).*/$1/) {
			$componentUID = $_;
		}
	}
	close PKG;

	if ($componentUID eq "") {
		die "Cannot extract UID from package ${absPackageFile}\n";	
	}	
	return $componentUID;
}

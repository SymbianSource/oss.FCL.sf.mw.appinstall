#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#
#

use strict;

my $cert_path = "$ENV{'SECURITYSOURCEDIR'}\\installationservices\\switestfw\\testcertificates";
sub get(%$;$)
{
    my $hash = $_[0];
    my $what = $_[1];

    die "hash key undefined" if not defined $what;

    if (not defined($hash->{$what}))
    {
	return $_[2] if defined $_[2];
	die "'$what' is undefined";
    }
    
    return $hash->{$what};
}

#
# This is an inflexible workaround - we should use 'openssl ca'
#
sub recreate_cacert(%)
{
    my $arg = $_[0];

    print "Recreate a self signed CA certificate.\n";

    my $cert = get($arg, "cacert");

    my $key = get($arg, "cakey");
    -e $key or die "$key does not exist";
    my $oldcert = get($arg, "oldcert");
    -e $oldcert or die "$oldcert does not exist";

    (my $sec, my $min, my $hour, my $mday, my $mon,
     my $year, my $wday, my $yday, my $isdst) = localtime(time);
    
    my $saveddate = sprintf("%02u-%02u-%02u", $mday, $mon + 1, $year + 1900);
    
    my $date = get($arg, "date");
    my $days = get($arg, "days");

    my @command = ("call date $date"); 
    
    system(@command);
    if (($? >> 8) != 0)
    {
	my @command = ("call date $saveddate"); 
    
	system(@command);
	die "Failed whilst creating CA cert.";
    }
    
    @command = ("openssl", "x509", 
		"-signkey", $key,
		"-in", $oldcert,
		"-out", $cert,
		"-outform", "DER",
		"-days", $days);

    system(@command);
    if (($? >> 8) != 0)
    {
	my @command = ("call date $saveddate"); 
    
	system(@command);
	die "Failed whilst creating CA cert.";
    }

    @command = ("call date $saveddate"); 
    
    system(@command);
    ($? >> 8) == 0 or die "Failed whilst creating CA cert.";
}

sub create_cacert(%)
{
    my $arg = $_[0];
    
    my $cert = get($arg, "cacert");
    my $key = get($arg, "cakey");
    my $caconfig = get($arg, "caconfig");
    -e $caconfig or die "$caconfig does not exist";
    
    my $outform = get($arg, "outform", "DER");

    print "Create a self signed CA certificate.\n";

    my @command = ("openssl", "req", 
		   "-x509", 
		   "-config", $caconfig, 
		   "-newkey", "rsa:1024",
		   "-keyout", $key,
		   "-nodes", 
		   "-out", $cert,
		   "-extensions", "v3_ca", 
		   "-outform", $outform,
		   "-days", "3650");
    
    system(@command);
    ($? >> 8) == 0 or die "Failed whilst creating CA cert.";

}

#
# Prepare for certstore creation.
#
sub prepare_certs(%)
{
    my $arg = $_[0];

    print "Prepare certs for certstore creation\n";
    
    my $cacert = get($arg, "cacert");
    -e $cacert or die "$cacert does not exist.";
    my $spec = get($arg, "spec");
    -e $spec or die "$spec does not exist.";

    $cacert =~ m|(.*)[\\/]+(.*)$|;
    my $certdir = $1;
    my $certfile = $2;
    
    if ($certfile =~ m|(.*)\.pem|i)
    {
	print "Convert $cacert to $certdir\\$1.der\n";

	my @command = ("openssl", "x509", 
		       "-inform", "PEM",
		       "-outform", "DER",
		       "-in", $cacert,
		       "-out", "$certdir\\$1.der");
    
	system(@command);
	($? >> 8) == 0 or die "Failed whilst changing certificate format.";

	$certfile = "$1.der";
	$cacert = "$certdir\\$certfile";
    }
    
    my $outdir = "\\epoc32\\winscw\\c\\tswi\\certstore\\wsc\\$certdir";
    use File::Path;
    mkpath $outdir, 1, 0777;
    
    -d $outdir or die "Couldn't create $outdir";

    print "Copy $cacert to $outdir\n";
    -e "$outdir\\$certfile" and unlink "$outdir\\$certfile";
    
    use File::Copy;
    copy "$cacert", "$outdir\\$certfile" 
	or die "Failed to copy $cacert to $outdir\\$certfile";
    
    $spec =~ m|(.*)[\\/]+(.*)$|;
    my $specdir = $1;
    my $specfile = $2;

    print "Copy $spec to $outdir\n";
    -e "$outdir\\$specfile" and unlink "$outdir\\$specfile";

    copy "$spec", "$outdir\\$specfile" 
	or die "Couldn't copy $spec to $outdir\\$specfile";
}

sub create_certstore(%)
{
    my $arg = $_[0];

    my $store = get($arg, "store");
    my $spec = get($arg, "spec");
    -e $spec or die "$spec does not exist";
    
    $store =~ m|(.*)[\\/]+(.*)$|;
    my $certdir = $1;
    my $storefile = $2;

    print "build $store using c:\\tswi\\certstore\\wsc\\$spec\n";

    my @command = ("\\epoc32\\release\\winscw\\udeb\\swicertstoretool",
		   "c:\\tswi\\certstore\\wsc\\$spec",
		   "c:\\tswi\\certstore\\wsc\\$store", 
		   "c:\\tswi\\certstore\\wsc\\$certdir\\buildlog.txt");

    system(@command);
    ($? >> 8) == 0 or die "Failed whilst building the store '$store'";
    
    my $outdir = "\\epoc32\\winscw\\c\\tswi\\certstore\\wsc\\$certdir";

    rename("$outdir\\$storefile", "$store") or die "Move failed";

    rename("$outdir\\buildlog.txt", "$certdir\\buildlog.txt") 
	or die "Move failed";

    # There must be an easier way to remove a non-empty directory
    use File::Glob ':glob';
    foreach (glob("$outdir/*"))
    {
	unlink $arg;
    }
    rmtree $outdir or die "Failed to remove $outdir";
}

my $romtcb = "$cert_path\\swi\\test\\tsisfile\\data\\signedsis\\SymbianTestRootTCBCARSA";
my $romexpired = "$cert_path\\swi\\test\\tsisfile\\data\\signedsis\\SymbianTestRootExpiredCARSA";

my @stores = (
 	      {"store" => "certs\\capabilities\\swicertstore_update.dat",
 	       "caconfig" => "certs\\capabilities\\rsa.config",
 	       "spec" => "certs\\capabilities\\certstore.spec",
 	       "cakey" => "$cert_path\\swi\\test\\writableswicertstore\\certs\\capabilities\\cakey.pem",
 	       "cacert" => "$cert_path\\swi\\test\\writableswicertstore\\certs\\capabilities\\cacert.der"},
 	      {"store" => "certs\\mandatory\\swicertstore_update.dat",
 	       "caconfig" => "certs\\mandatory\\rsa.config",
 	       "spec" => "certs\\mandatory\\certstore.spec",
 	       "cakey" => "certs\\mandatory\\cakey.pem",
 	       "cacert" => "certs\\mandatory\\cacert.der"},
	      {"store" => "certs\\expired\\swicertstore_update.dat",
	       "spec" => "certs\\expired\\certstore.spec",
	       "cakey" => "$romtcb\\cakey.pem",
	       "cacert" => "$cert_path\\swi\\test\\writableswicertstore\\certs\\expired\\cacert.der",
	       "oldcert" => "$romtcb\\cacert.pem",
	       "date" => "01-01-01",
	       "days" => "1"},
	      {"store" => "certs\\renewed\\swicertstore_update.dat",
	       "spec" => "certs\\renewed\\certstore.spec",
	       "cakey" => "$romexpired\\cakey.pem",
	       "cacert" => "$cert_path\\swi\\test\\writableswicertstore\\certs\\renewed\\cacert.der",
	       "oldcert" => "$romexpired\\cacert.pem",
	       "date" => "01-01-05",
	       "days" => "3650"}
	      );


foreach my $arg (@stores)
{
    if (exists($arg->{"oldcert"}))
    {
	recreate_cacert($arg);
    }
    else
    {
	create_cacert($arg);
    }

    prepare_certs($arg);
    create_certstore($arg);
}

sub createchain(%)
{
    my $arg = $_[0];

    my $cacert = get($arg, "cacert");
    -e $cacert or die "$cacert does not exist";

    my $cakey = get($arg, "cakey");
    -e $cakey or die "$cakey does not exist";

    my $cert = get($arg, "cert");
    my $req = get($arg, "req");
    my $key = get($arg, "key");
    
    my $serial = get($arg, "serial");
    
    $arg->{"outform"} = "PEM";
    create_cacert($arg);

    print "Create an intermediate.\n";

    my $config = get($arg, "config");
    -e $config or die "$config does not exist";
    
    my @command = ("openssl", "req", 
		   "-newkey", "rsa:512", 
		   "-nodes", 
		   "-out", $req, 
		   "-keyout", $key, 
		   "-config", $config, 
		   "-days", "3650");

    system(@command);
    ($? >> 8) == 0 or die "Failed whilst creating intermediate cert req.";

    print "Sign intermediate '$cert' with '$cacert'\n";
    @command = ("openssl", "x509", "-req",
		"-in", $req,
		"-out", $cert, 
		"-CA", $cacert,
		"-CAkey", $cakey,
		"-CAserial", $serial,
		"-CAcreateserial",
		"-extfile", $config, # or caconfig?
		"-extensions", "v3_ca",
		"-outform", "DER",
		"-days", "3650");

    system(@command);
    ($? >> 8) == 0 or die "Failed whilst creating intermediate cert.";
}

my %chain = ("cakey" => "certs\\chain\\cakey.pem",
	     "cacert" => "certs\\chain\\cacert.pem",
	     "caconfig" => "certs\\chain\\rsa.config",
	     "config" => "certs\\chain\\intermediate\\rsa.config",
	     "cert" => "$cert_path\\swi\\test\\writableswicertstore\\\\swi\\test\\writableswicertstore\\certs\\chain\\intermediate\\cacert.der",
	     "key" => "$cert_path\\swi\\test\\writableswicertstore\\\\swi\\test\\writableswicertstore\\certs\\chain\\intermediate\\cakey.pem",
	     "req" => "certs\\chain\\intermediate\\cert.req",
	     "serial" => "certs\\chain\\intermediate\\cert.serial",
	     "store" => "certs\\chain\\swicertstore_update.dat",
	     "spec" => "certs\\chain\\certstore.spec"
	     );

createchain(\%chain);
prepare_certs(\%chain);
create_certstore(\%chain);



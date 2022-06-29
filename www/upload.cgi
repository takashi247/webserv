#!/usr/bin/perl -w

use CGI;

$is_uploadable = $ENV{'X_IS_UPLOADABLE'};

$query = new CGI;

if ($is_uploadable eq "true") {

$upload_dir = $ENV{'X_UPLOAD_DIR'};

$filename = $query->param("photo");
$email_address = $query->param("email_address");
$filename =~ s/.*[\/\\](.*)/$1/;
$upload_filehandle = $query->upload("photo");

open UPLOADFILE, ">$upload_dir/$filename";

while ( <$upload_filehandle> )
{
  print UPLOADFILE;
}

close UPLOADFILE;

print $query->header();
print <<END_HTML;

<HTML>
<HEAD>
<TITLE>Thanks!</TITLE>
</HEAD>

<BODY>

<h1>Thanks for uploading your photo!</h1>
<P>Your email address: $email_address</P>
<P>Your photo:</P>
<img src="/upload/$filename" border="0">

</BODY>
</HTML>

END_HTML

} else {

print $query->header(-status => '405')

}

print chr(26);
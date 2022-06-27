#!/usr/bin/perl -w

use CGI;

$is_uploadable = $ENV{'IS_UPLOADABLE'};

if ($is_uploadable eq "true") {

$upload_dir = $ENV{'UPLOAD_DIR'};

$query = new CGI;

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

print <<END_HTML;

<HTML>
<HEAD>
<TITLE>Error</TITLE>
</HEAD>

<BODY>

<h1>You are not allowed to upload files!</h1>

</BODY>
</HTML>

END_HTML

}

print chr(26);
#!/usr/bin/perl -w

use CGI;

$is_uploadable = $ENV{'X_IS_UPLOADABLE'};

$query = new CGI;

if ($is_uploadable eq "true") {

$upload_dir = $ENV{'X_UPLOAD_DIR'};

$filename = $query->param("file");
$filename =~ s/.*[\/\\](.*)/$1/;
$upload_filehandle = $query->upload("file");

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
</HTML>

END_HTML

} else {

print $query->header(-status => '405')

}

print chr(26);

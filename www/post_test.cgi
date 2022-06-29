#!/usr/bin/perl -w

use CGI;

$query = new CGI;

$filename = $query->param("photo");
$email_address = $query->param("email_address");

print $query->header();
print <<END_HTML;

<HTML>
<HEAD>
<TITLE>Thanks!</TITLE>
</HEAD>

<BODY>

<P>Thanks for your input!</P>
<P>Your email address: $email_address</P>
<P>Your photo: $filename</P>

</BODY>
</HTML>

END_HTML

print chr(26);
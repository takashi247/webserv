#!/usr/bin/perl -w
use CGI;

$query = new CGI;

$name = $query->param("name");
$email_address = $query->param("email_address");

print <<END_HTML;

<HTML>
<HEAD>
<TITLE>Thanks!</TITLE>
</HEAD>

<BODY>

<P>Thanks for your response!</P>
<P>Your name: $name</P>
<P>Your email address: $email_address</P>

</BODY>
</HTML>

END_HTML

print chr(26);
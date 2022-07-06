#!/usr/bin/perl -w
use CGI;

$query = new CGI;

print $query->header(
  -location => '/'
);

print <<END_HTML;

<HTML>
<HEAD>
<TITLE>Thanks!</TITLE>
</HEAD>

<BODY>

<P>This is a temporary redirection to google.com</P>

</BODY>
</HTML>

END_HTML
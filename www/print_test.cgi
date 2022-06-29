#!/usr/bin/perl

use CGI;

$query = new CGI;

print $query->header();

print <<"END_HTML";

<HTML>

<HEAD>
<TITLE>Simple Virtual HTML Document</TITLE>
</HEAD>

<BODY>
<H1>Virtual HTML</H1><HR>
Hey look I just created a virtual (yep, virtual) HTML document!
</BODY>

</HTML>

END_HTML

print chr(26);
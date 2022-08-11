#!/usr/bin/perl

use CGI;

$query = new CGI;

print $query->header();

print "hello";

exit(1);

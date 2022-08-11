#!/usr/bin/perl -w

use CGI;

$query = new CGI;

print $query->header();
for my $param_name ($query->param) {
    print $param_name . ' = ' . $query->param($param_name);
}

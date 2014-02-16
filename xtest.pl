#! /usr/bin/perl -w
# see http://search.cpan.org/~kryde/X11-Protocol-Other-28/lib/X11/Protocol/Ext/XTEST.pm

use X11::Protocol;

my $X = X11::Protocol->new;
$X->init_extension('XTEST')
  or print "XTEST extension not available";

$X->XTestFakeInput(name => 'MotionNotify', root_x => 123, root_y => 456);
$X->XTestFakeInput(name => 'ButtonPress', detail => 1);	#lmb
$X->XTestFakeInput(name => 'MotionNotify', root_x => 200, detail => 1); # relatove movement
$X->XTestFakeInput(name => 'ButtonRelease', detail => 1);	#lmb

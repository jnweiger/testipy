				jw, Tue Feb 11 12:35:47 CET 2014

Abstract
========

TurtlePy is a technology independent GUI-testing framework, inspired by SikuliX.
It interacts with the system under test only through framebuffer output (pixels) and 
keyboard/mouse input. The test script language is python.



Existing gui test frameworks
============================

Before we invent this obvious wheel, let try to find similar wheels so that 
we don't necessarily re-invent it. This is what we have:

pyUseCase aka StoryText aka TextTest
------------------------------------
  http://texttest.sourceforge.net/index.php?page=ui_testing
  Needs code instrumentation. 
  Thus it works only with applications properly instrumented.

Selenium
--------
  http://docs.seleniumhq.org/
  Web Browser only. Has issues with FireFox 26.

Xext/XTest
----------
  Generate artificial input events in the X11-Server.
  Perl and python bindings exist. Low level only.
  zypper in python-xlib-0.14-105.1.2.i586

python-uinput
-------------
  Uses udev/kernel module to generate input events. 
  Supports keyboard and mouse. This can test drive applications with console input.
  https://github.com/tuomasjjrasanen/python-uinput

igor
----
  https://github.com/fabiand/igor
  http://dummdida.tumblr.com/tagged/igor
  Igor expects a working PXE environment (so a lan with some DHCP and PXE
  server like Cobbler) and some hosts or a libvirtd instance.
  Why do we want to boot for testing?
  
jenkins
-------
  collects reports, has a nice dashboard, but does not test anything itself.

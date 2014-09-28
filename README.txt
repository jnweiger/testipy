				jw, Tue Feb 11 12:35:47 CET 2014

Abstract
========

XTestiPy is a technology independent GUI-testing framework, inspired by SikuliX.
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



Research Refences
=================
  
 https://github.com/NYPL/map-vectorizer employs python and openCV
  OpenCV circle and cross detection
 
 pypotrace from pypi.

 autotrace library developed by Martin Weber. 
	Corner detection is different that with potrace, but both look good.
	http://autotrace.sourceforge.net/index.html#download is down. latest
	release is from 2002.

 Sonja Katrin Bayer: Bildschirmtypographie
  - http://www.alles-buch.uni-erlangen.de/bayer.pdf
  - Nach DIN EN ISO 9 241-3 soll aber das Verhältnis der höheren Leuchtdichte
    zur niedrigeren Leuchtdichte mindestens 3:1 betragen (gemessen und
    verglichen werden die Leuchtdichte im Inneren eines Buchstabens und auf dem
    Buchstaben selbst). Als besonders schwer leserlich gilt blaue oder rote
    Schrift auf schwarzem Hintergrund oder gelbe Schrift auf weißem
    Hintergrund.  Darüber hinaus sind einige Farbkombinationen wie Blau auf Rot
    oder Grün auf Lila zu vermeiden, was darin begründet liegt, dass das
    menschliche Auge Farben mit weit auseinander liegenden Wellenlängen nicht
    gleichzeitig scharf wahrnehmen kann
  - Assuming that screen Designers adhere to these basic rules, we can derive 
    the following artefacts: 
    * color is irrelevant. We can reduce to a lumiosity gray map.
    * lumiosity changes of 2:1 or less are irrelevant. That is badly readable by 
      humans, so we can also ignore this. 
    * We degrade contrast by adding a black level of ca 20. This simulates
      reflections and ambient light in the human eye. It also helps against 
      unintended good contrast in very dark areas.
  - Gesetz der visuellen Wahrnehmung: 
    * der Nähe, 
    * der Gleichheit/Ähnlichkeit, 
    * der Guten Fortsetzung, 
    * der Schliessung, 
    * der Symmetrie
  - Strichbreite soll 8 bis 117 Prozent der Schrifthöhe sein.
  - Wesentliche Informationen werde aus der oberen Hälfte des Wortbildes bezogen.
  ZZ

 Edward L Delosh: Pattern Recognition
  - http://lamar.colostate.edu/~bclegg/PY453/pattern_recognition.pdf
  - Figure 3-2 features an example with a glyph that is halfway between A and H. 
    Exploiting context it is still perfectly readable as THE CAT.

Segmentation Algorithms
=======================
  Watershed Algorithm
  - http://docs.opencv.org/trunk/doc/py_tutorials/py_imgproc/py_watershed/py_watershed.html
  - extremly cool! 
  P-Algorithm
  - on http://cmm.ensmp.fr/~beucher/publi/P-Algorithm_SB_BM.pdf
  - an improvement on watershed.


Textbox segmentation
====================
Given a screenshot from a computer screen. 
Convert RGB to HLS. Run a spectral analysis on all three (six HLS+RGB ?) color components. 
Apply maximum function.
Scan horizontal lines for strings of highest frequencies.
 - Ignore all hightfrequency noise that is below the ISO 9 241-3 thresholds.
 - for every HF substring, 
   * move upwards/downwards, until the HF signal strength is at a minimum.
     There we have the upper/lower boundary of a text box. 
   * The start/end of a HF substring are the left/right boundary of a text box.
   * Add a safety margin of 2 or 3 pixels at each side.
   * Convert the box to grayscale, stretch the histogram to fill at least 50% of the lumiosity range.  entire range.
   * Run ocr on a sufficiently upscaled version of the rectangle.
     - Run multiple passes with dovetailing into reduced top/left safety margin. Reduced to 0 or even -2 or -3 pixels.
     - Take the ocr text result from the pass with the best quality metrics and denote its bounding box
 - record frequencies statistics.
 - Run again, with a downscaled version of the entire page, so that the typical frequencies are low pass filtered (smoothed away).
 - Run again, with further downscales, until nothing remains recognized. 
   This downscaling is an attempt to read banner text printed with ascii art, and similar higher order textures.

Test cases:
 - xdaliclock -transparent
 - banner Hello World

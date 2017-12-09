testipy
=======

TestiPy is a technology independent GUI-testing framework, inspired by SikuliX.
It interacts with the system under test only through framebuffer output 
(pixels) and keyboard/mouse input.


References
==========

* Selenium http://docs.seleniumhq.org/

* https://github.com/msanders/autopy/

* apt-get install libxtst-dev xautomation

* http://texttest.sourceforge.net/index.php?page=ui_testing
  pyUseCase aka StoryText aka TextTest 

* Xext/XTest, Generate artificial input events. bindings exist. Low level.

* python-uinput https://github.com/tuomasjjrasanen/python-uinput - Uses 
  udev/kernel module to generate input events.  Supports keyboard and mouse.
  This can test drive applications with console input.

* igor https://github.com/fabiand/igor http://dummdida.tumblr.com/tagged/igor

* w3.mi.parisdescartes.fr/map5/Publis/2006-09.pdf
   From Gestalt Theory to Image Analysis
  
* https://github.com/NYPL/map-vectorizer employs python and openCV
  OpenCV circle and cross detection
 
* pypotrace from pypi.

* autotrace library developed by Martin Weber. 
	Corner detection is different that with potrace, but both look good.
	http://autotrace.sourceforge.net/index.html#download is down. latest
	release is from 2002.

* Sonja Katrin Bayer: Bildschirmtypographie
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





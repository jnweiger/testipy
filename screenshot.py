#!/usr/bin/python3
#
# https://askubuntu.com/questions/351830
#
import gi
gi.require_version('Gdk', '3.0')

from gi.repository import Gdk

window = Gdk.get_default_root_window()
x, y, width, height = window.get_geometry()

print("The size of the root window is {} x {}".format(width, height))

# get_from_drawable() was deprecated. See:
# https://developer.gnome.org/gtk3/stable/ch24s02.html#id-1.6.3.4.7
pb = Gdk.pixbuf_get_from_window(window, x, y, width, height)

if pb:
    pb.savev("screenshot.png", "png", (), ())
    print("Screenshot saved to screenshot.png.")
else:
    print("Unable to get the screenshot.")

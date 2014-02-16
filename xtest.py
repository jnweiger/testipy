#!/usr/bin/python
# see http://sourceforge.net/p/python-xlib/code/HEAD/tree/trunk/examples/

from Xlib import X
from Xlib.display import Display
from Xlib.ext.xtest import fake_input
import Xlib.XK
import time, sys


# from http://shallowsky.com/software/crikey/pykey-0.1 
def send_key(display, emulated_key, root=None):
    shift_mask = 0 # or Xlib.X.ShiftMask

    if root is None: root = display.screen().root
    window = display.get_input_focus()._data["focus"]

    keysym = Xlib.XK.string_to_keysym(emulated_key)	# try name
    if keysym == 0: keysym = ord(emulated_key)		# try ascii code
    keycodes = display.keysym_to_keycodes(keysym)
    (keycode,shift_mask) = keycodes[0]
    if 0:
      # does not work. Sends @ insted of "
      if shift_mask != 0 :
        Xlib.ext.xtest.fake_input(display, Xlib.X.KeyPress, 50)
      Xlib.ext.xtest.fake_input(display, Xlib.X.KeyPress, keycode)
      Xlib.ext.xtest.fake_input(display, Xlib.X.KeyRelease, keycode)
      if shift_mask != 0 :
        Xlib.ext.xtest.fake_input(display, Xlib.X.KeyRelease, 50)
    else:
      event = Xlib.protocol.event.KeyPress(
        time = int(time.time()),
        root = root,
        window = window,
        same_screen = 0, child = Xlib.X.NONE,
        root_x = 0, root_y = 0, event_x = 0, event_y = 0,
        state = shift_mask,
        detail = keycode
        )
      window.send_event(event, propagate = True)
      event = Xlib.protocol.event.KeyRelease(
        time = int(time.time()),
        root = display.screen().root,
        window = window,
        same_screen = 0, child = Xlib.X.NONE,
        root_x = 0, root_y = 0, event_x = 0, event_y = 0,
        state = shift_mask,
        detail = keycode
        )
      window.send_event(event, propagate = True)


d = Display()
if not d.query_extension("XTEST") :
  print "XTEST extension not installed"
  sys.exit(0)

fake_input(d, X.MotionNotify, x=123, y=456)
fake_input(d, X.ButtonPress, 1)	# mark
fake_input(d, X.MotionNotify, x=200, detail=1)	# relative movement, highlight
fake_input(d, X.ButtonRelease, 1) 	# copy test to selection
d.sync()
send_key(d, 'e')
send_key(d, 'c')
send_key(d, 'h')
send_key(d, 'o')
send_key(d, ' ')
send_key(d, '"')
d.sync()
fake_input(d, X.ButtonPress, 2, time=10) 	# paste ...
d.sync()
fake_input(d, X.ButtonRelease, 2, time=10) 	# ... paste
d.sync()
send_key(d, '"')
d.sync()

#! /usr/bin/python
#
# Study http://www.qaautomation.net/?p=388
# pydoc selenium.webdriver.remote.webelement
#
from pprint import pprint
from selenium import webdriver
from time import sleep

def eprint(e):
  print("<%s> %r" % (e.tag_name, e.text[:60]))

browser = webdriver.Firefox()
browser.get('http://seleniumhq.org/')

pprint(browser.title)
p = browser.find_element(by='name', value='q')
# how do we enter text?
p.send_keys("python")
eprint(p)	# does not show the entered text :-(
p.submit()

# ch = p.find_elements_by_xpath('./*')
# for c in ch:
#   eprint(c)

sleep(4)
browser.quit()

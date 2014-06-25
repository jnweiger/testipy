#! /usr/bin/python
#
# Study http://www.qaautomation.net/?p=388
#       http://www.realpython.com/blog/python/headless-selenium-testing-with-python-and-phantomjs
# pydoc selenium.webdriver.remote.webelement
#
# sudo zypper in python-pip
# sudo pip install selenium
# ??what is brew??
# sudo brew install phantomjs
#
# pydoc selenium.webdriver.remote.webelement
#
from pprint import pprint
from selenium import webdriver
from time import sleep

def dom_print(e):

  print("<%s> %r" % (e.tag_name, e.text[:60]))
  pprint({'geom':[e.location, e.size], 'id':e.get_attribute('id'), 'name':e.get_attribute('name'), 'class':e.get_attribute('class')})

  path=''
  p = e
  while p:

    tag = p.tag_name 
    v = p.get_attribute('id')
    if v == '': v = None
    if v is not None:
      tag += '[id=' + v + ']'
    else:
      v = p.get_attribute('name')
      if v == '': v = None
      if v is not None:
        tag += '[name=' + v + ']'
      else:
        v = p.get_attribute('class')
        if v == '': v = None
        if v is not None:
          tag += '[class=' + v + ']'
    if len(path): tag += '.'
    path = tag + path
      
    try:
      p = p.find_element(by='xpath', value='..')
    except:
      p = None
  print path
    

browser = webdriver.Firefox()
# browser = webdriver.PhantomJS()
browser.get('http://seleniumhq.org/')

pprint(browser.title)
p = browser.find_element(by='name', value='q')
dom_print(p)
# how do we enter text?
p.send_keys("python")
p.submit()

sleep(2)
browser.quit()

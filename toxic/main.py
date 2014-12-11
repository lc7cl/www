#!/bin/python3

import urllib.request

def start():
    f = urllib.request.urlopen('http://www.baidu.com')
    #print(f.read().decode('utf-8'))
    print(f.read())


if __name__ == '__main__':
    start()

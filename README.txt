This package provides class to control the GPIO on iMX233-OLinuXino.
Current release does no support any peripheral functions.

Example
=======

Typical usage::
    
    #!/usr/bin/env python

    import iMX233_GPIO as GPIO

    #init module
    GPIO.init()
    
    #configure module
    GPIO.setinput(GPIO.PIN#)
    GPIO.setoutput(GPIO.PIN#)
    
    #set GPIO high
    GPIO.output(GPIO.PIN#, 1)
    
    #set GPIO low
    GPIO.output(GPIO.PIN#, 0)
    
    #read input
    state = GPIO.input(GPIO.PIN#)
    

iMX233_GPIO
===========

Python iMX233_GPIO module based on Olimex iMX233_GPIO 0.1.0


This package provides class to control the GPIO on iMX233-OLinuXino.
Current release does no support any peripheral functions.

Example
=======

Typical usage::

    #!/usr/bin/env python

    import iMX233_GPIO as GPIO

    #configure module
    GPIO.setinput(GPIO.PIN#)
    GPIO.setoutput(GPIO.PIN#)

    #set GPIO high
    GPIO.output(GPIO.PIN#, 1)

    #set GPIO low
    GPIO.output(GPIO.PIN#, 0)

    #read input
    state = GPIO.input(GPIO.PIN#)

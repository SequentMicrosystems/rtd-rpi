[![librtd](res/sequent.jpg)](https://www.sequentmicrosystems.com)
# librtd

This is the python library to control the [rtd](https://sequentmicrosystems.com/index.php?route=product/product&path=33&product_id=66) 4/8 Channel RTD Data Acquisition for Raspberry Pi.

## Install

```bash
~$ sudo apt-get update
~$ sudo apt-get install build-essential python-pip python-dev python-smbus git
~$ git clone https://github.com/SequentMicrosystems/rtd-rpi.git
~$ cd rtd-rpi/python/rtd/
~/rtd-rpi/python/rtd$ sudo python setup.py install
```

If you use python3.x repace the last line with:

```
~/rtd-rpi/python/rtd$ sudo python3 setup.py install
```

## Update

```bash
~$ cd rtd-rpi/
~/rtd-rpi$ git pull
~$ cd rtd-rpi/python/rtd/
~/rtd-rpi/python/rtd$ sudo python setup.py install
```

If you use python3.x repace the last line with:

```
~/rtd-rpi/python/rtd$ sudo python3 setup.py install
```
## Usage 

Now you can import the megaio library and use its functions. To test, read relays status from the board with stack level 0:

```bash
~$ python
Python 2.7.9 (default, Sep 17 2016, 20:26:04)
[GCC 4.9.2] on linux2
Type "help", "copyright", "credits" or "license" for more information.
>>> import librtd
>>> librtd.get(0, 1)
26.654391
>>>
```

## Functions


### get(stack, channel)
Get one rtd temperature.

stack - stack level of the MEGA-RTD card (selectable from address jumpers [0..7])

channel - channel number (id) [1..8]

return tempetature in deg Celsius



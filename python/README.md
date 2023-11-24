[![librtd](res/sequent.jpg)](https://www.sequentmicrosystems.com)
# librtd

This is the python library to control the [RTD Data Acquisition Stackable Card for Raspberry Pi](https://sequentmicrosystems.com/product/rtd-data-acquisition-card-for-rpi/).

## Install

```bash
sudo pip install SMrtd
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

- stack - stack level of the MEGA-RTD card (selectable from address jumpers [0..7]).
- channel - channel number (id) [1..8].

return tempetature in deg Celsius.

### getRes(stack, channel)

- stack - stack level of the MEGA-RTD card (selectable from address jumpers [0..7]).
- channel - channel number (id) [1..8].

return sensor resistance in ohms.

### get_poly5(stack: int, channel: int)

Convert RTD reading to Temperature, using 5th order polynomial fit of Temperature as a function of Resistance.
This fit provides much-improved accuracy through the temperature range of [-200C, 660C], particularly near the high
and low ranges, compared to the default linear fitting function baked into the Sequent RTD Data Acquisition
Stackable Card for Raspberry Pi.  The coefficients for this fit were developed in a project documented
in https://github.com/ewjax/max31865
```
        temp_C = (c5 * res^5) + (c4 * res^4) + (c3 * res^3) + (c2 * res^2) + (c1 * res) + c0
```
 - param stack: 0-7, which hat to read
 - param channel: 1-8, which RTD to read on indicated hat
 - return: temperature, in Celcius

Tanks to [ewjax](https://github.com/ewjax) for the polynomial fit contribution
# librtd

This is the python library to control the [rtd](https://sequentmicrosystems.com/index.php?route=product/product&path=33&product_id=66) 4/8 Channel RTD Data Acquisition for Raspberry Pi.

## Install

```bash
sudo pip install SMrtd
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



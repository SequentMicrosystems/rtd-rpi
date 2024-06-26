Steps below are tested on  a brand new load of raspibian 64-bit on a Raspberry Pi 5:

    1. Before starting any of this, ensure DSO is set to dc-coupling

To set up and test regular serial:

    1. Positions of dip switches does not matter for switches: 485-TERM, 485-TX, 485-RX

    2. raspi-config > Interfaces > Serial

    3. Login Shell? No

    4. Hardware enable? Yes

    5. reboot

    6. You should now have /dev/ttyAMA0 which was not there before

    7. If the RTD hat is on the Pi, run "rtd 0 rs485wr 0 0 0 0 0" (without double quotes. This seems to persist through reboots/power off, so you only need to run it once.)

    8. Hook DSO up to GND and TX0 (Pin 8) on Pi GPIO

    9. Run python with python test_serial.py (No SUDO needed.)

    10. Should see output on DSO


AFTER above steps are working, we can verify RS485 output through the RTD HAT:

    1. Follow all of the above steps and verify normal serial on above specified pin, then:

    2. 485-TERM dip switch position doesn't really matter if you're just verifying with DSO

    3. Set 485-TX dip sw to on

    4. Set 485-RX dip sw to on

    5. Hook DSO up with GND on A or B RS485 lines and the probe hooked up to whichever (A or B) line gnd is not hooked up to. These lines have been tested from the connector just to the left of the GPIO header on the RTD HAT (left if pi GPIO header is on top of board) no shared GND needed between DSO and Pi

    6. run "rtd 0 rs485wr 0 0 0 0 0" (without double quotes. This seems to persist through reboots/power off, so you only need to run it once, no need if you already ran it in previous steps)

    7. Run python with python test_serial.py (No SUDO needed.)

    8. Should see output on DSO

[![rtd-rpi](res/sequent.jpg)](https://sequentmicrosystems.com)

# Modbus

The [RTD](https://sequentmicrosystems.com/product/raspberry-pi-building-automation/) Data Acquisition Expansion cards for Raspberry Pi can be accessed thru Modbus RTU protocol over RS-485 port.
You can set-up the RS-485 port with **rtd** command.

Example:
```bash
~$ rtd 0 rs485wr 1 9600 1 0 1
```
Set Modbus RTU , Baudrate: 9600bps, 1 Stop Bit,  parity: None, slave address offset: 1
```bash
~$ rtd -h rs485wr
```
display the full set of options

## Slave Address
The slave address is add with the "stack level" jumpers. For example the jumpers configuration for stack level 1  (one jumper in position ID0) slave address offset to 1 corespond to slave address 2.

## Modbus object types
Some of modbus RTU object type with standard addresses are implemented : Discrete Inputs, Holding registers.

### Input registers

Access level Read Only, Size 16 bits

| Device function | Register Address | Modbus Address | Measurement Unit |
| --- | --- | --- | --- |
| TEMP_1 | 30001 | 0x00 | 0.1 degC |
| TEMP_2 | 30002 | 0x01 | 0.1 degC |
| TEMP_3 | 30003 | 0x02 | 0.1 degC |
| TEMP_4 | 30004 | 0x03 | 0.1 degC |
| TEMP_5 | 30005 | 0x04 | 0.1 degC |
| TEMP_6 | 30006 | 0x05 | 0.1 degC |
| TEMP_7 | 30007 | 0x06 | 0.1 degC |
| TEMPN_8 | 30008 | 0x07 | 0.1 degC |
| R_IN_1 | 30009 | 0x08 | ohm |
| R_IN_2 | 30010 | 0x09 | ohm |
| R_IN_3 | 30011 | 0x0a | ohm |
| R_IN_4 | 30012 | 0x0b | ohm |
| R_IN_5 | 30013 | 0x0c | ohm |
| R_IN_6 | 30014 | 0x0d | ohm |
| R_IN_7 | 30015 | 0x0e | ohm |
| R_IN_8 | 30016 | 0x0f | ohm |


### Holding registers

Access level Read/Write, Size 16 bits

| Device function | Register Address | Modbus Address | Measurement Unit |
| --- | --- | --- | --- |
| LED_TEMP_TH_1 | 40001 | 0x00 | degC |
| LED_TEMP_TH_2 | 40002 | 0x01 | degC |
| LED_TEMP_TH_3 | 40003 | 0x02 | degC |
| LED_TEMP_TH_4 | 40004 | 0x03 | degC |
| LED_TEMP_TH_5 | 40001 | 0x00 | degC |
| LED_TEMP_TH_6 | 40002 | 0x01 | degC |
| LED_TEMP_TH_7 | 40003 | 0x02 | degC |
| LED_TEMP_TH_8 | 40004 | 0x03 | degC |



## Function codes implemented

* Read Coils (0x01)
* Read Discrete Inputs (0x02)
* Read Holding Registers (0x03)
* Read Input Registers (0x04)
* Write Single Coil (0x05)
* Write Single Register (0x06)
* Write Multiple Coils (0x0f)
* Write Multiple registers (0x10)

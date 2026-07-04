[![rtd-rpi](readmeres/sequent.jpg)](https://sequentmicrosystems.com)

# Modbus

The [RTD](https://sequentmicrosystems.com/products/rtd-data-acquisition-card-for-rpi) Data Acquisition Expansion cards for Raspberry Pi can be accessed through Modbus RTU protocol over the RS-485 port.
You can configure the RS-485 port using the **rtd** command.

Example:
```bash
rtd 0 rs485wr 1 9600 1 0 1
```
Set Modbus RTU, Baudrate: 9600bps, 1 Stop Bit,  parity: None, slave address offset: 1
```bash
rtd -h rs485wr
```
display the full set of options

## Slave Address
The slave address is added with the "stack level" jumpers. For example, the jumpers configuration for stack level 1 (one jumper in position ID0), slave address offset to 1 corresponds to slave address 2.

## Modbus object types
Some of the Modbus RTU object types with standard addresses are implemented: Coils, Input Registers, and Holding Registers.

### Coils

Access level Read/Write, Size 1 bit

| Coil name | Register Address | Modbus Address | Function |
| --- | --- | --- | --- |
| LED1_THRESHOLD_HI_EN | 0001 | 0x00 | If enabled, turn on LED1 when channel 1 temperature is higher than the threshold 1 | 
| LED1_THRESHOLD_LO_EN | 0002 | 0x01 | If enabled, turn on LED1 when channel 1 temperature is lower than the threshold 1 | 
| LED2_THRESHOLD_HI_EN | 0003 | 0x02 | If enabled, turn on LED2 when channel 2 temperature is higher than the threshold 2 | 
| LED2_THRESHOLD_LO_EN | 0004 | 0x03 | If enabled, turn on LED2 when channel 2 temperature is lower than the threshold 2 | 
| LED3_THRESHOLD_HI_EN | 0005 | 0x04 | If enabled, turn on LED3 when channel 3 temperature is higher than the threshold 3 | 
| LED3_THRESHOLD_LO_EN | 0006 | 0x05 | If enabled, turn on LED3 when channel 3 temperature is lower than the threshold 3 | 
| LED4_THRESHOLD_HI_EN | 0007 | 0x06 | If enabled, turn on LED4 when channel 4 temperature is higher than the threshold 4 | 
| LED4_THRESHOLD_LO_EN | 0008 | 0x07 | If enabled, turn on LED4 when channel 4 temperature is lower than the threshold 4 | 
| LED1_THRESHOLD_HI_EN | 0009 | 0x08 | If enabled, turn on LED5 when channel 5 temperature is higher than the threshold 5 | 
| LED1_THRESHOLD_LO_EN | 0010 | 0x09 | If enabled, turn on LED5 when channel 5 temperature is lower than the threshold 5 | 
| LED2_THRESHOLD_HI_EN | 0011 | 0x0a | If enabled, turn on LED6 when channel 6 temperature is higher than the threshold 6 | 
| LED2_THRESHOLD_LO_EN | 0012 | 0x0b | If enabled, turn on LED6 when channel 6 temperature is lower than the threshold 6 | 
| LED3_THRESHOLD_HI_EN | 0013 | 0x0c | If enabled, turn on LED7 when channel 7 temperature is higher than the threshold 7 | 
| LED3_THRESHOLD_LO_EN | 0014 | 0x0d | If enabled, turn on LED7 when channel 7 temperature is lower than the threshold 7 | 
| LED4_THRESHOLD_HI_EN | 0015 | 0x0e | If enabled, turn on LED8 when channel 8 temperature is higher than the threshold 8 | 
| LED4_THRESHOLD_LO_EN | 0016 | 0x0f | If enabled, turn on LED8 when channel 8 temperature is lower than the threshold 8 | 
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
| TEMP_8 | 30008 | 0x07 | 0.1 degC |
| R_IN_1 | 30009 | 0x08 | ohm |
| R_IN_2 | 30010 | 0x09 | ohm |
| R_IN_3 | 30011 | 0x0a | ohm |
| R_IN_4 | 30012 | 0x0b | ohm |
| R_IN_5 | 30013 | 0x0c | ohm |
| R_IN_6 | 30014 | 0x0d | ohm |
| R_IN_7 | 30015 | 0x0e | ohm |
| R_IN_8 | 30016 | 0x0f | ohm |


### Holding registers

Access level Read/Write, Size 16 bits. Led threshold registers.

| Device function | Register Address | Modbus Address | Measurement Unit | Obs |
| --- | --- | --- | --- | --- |
| LED_TEMP_TH_1 | 40001 | 0x00 | degC | Channel 1 Input temperature threshold for LED1 state |
| LED_TEMP_TH_2 | 40002 | 0x01 | degC | Channel 2 Input temperature threshold for LED2 state |
| LED_TEMP_TH_3 | 40003 | 0x02 | degC | Channel 3 Input temperature threshold for LED3 state |
| LED_TEMP_TH_4 | 40004 | 0x03 | degC | Channel 4 Input temperature threshold for LED4 state |
| LED_TEMP_TH_5 | 40005 | 0x04 | degC | Channel 5 Input temperature threshold for LED5 state |
| LED_TEMP_TH_6 | 40006 | 0x05 | degC | Channel 6 Input temperature threshold for LED6 state |
| LED_TEMP_TH_7 | 40007 | 0x06 | degC | Channel 7 Input temperature threshold for LED7 state |
| LED_TEMP_TH_8 | 40008 | 0x07 | degC | Channel 8 Input temperature threshold for LED8 state |
| BAUD_100 | 40009 | 0x08 | bps / 100 | RS485 port Baud / 100|
| ADD_OFFSET | 40010 | 0x09 | N/A | Modbus slave address offset |
| SENSOR_TYPE | 40011 | 0x0a | 0/1 | PT100/PT1000 input sensor type |


## Function codes implemented

* Read Coils (0x01)
* Read Discrete Inputs (0x02)
* Read Holding Registers (0x03)
* Read Input Registers (0x04)
* Write Single Coil (0x05)
* Write Single Register (0x06)
* Write Multiple Coils (0x0f)
* Write Multiple registers (0x10)

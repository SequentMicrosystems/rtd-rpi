import smbus
import struct

#bus = smbus.SMBus(1)    # 0 = /dev/i2c-0 (port I2C0), 1 = /dev/i2c-1 (port I2C1)

DEVICE_ADDRESS = 0x40     #7 bit address (will be left shifted to add the read write bit)

RTD_TEMPERATURE_ADD = 0
RTD_RESISTANCE_ADD = 59
	
def get(stack, channel):
	if stack < 0 or stack > 7:
		raise ValueError('Invalid stack level')
		return
    
	if channel < 1 or channel > 8:
		raise ValueError('Invalid channel number')
		return
   	bus = smbus.SMBus(1) 
	try:
		buff = bus.read_i2c_block_data(DEVICE_ADDRESS+stack, RTD_TEMPERATURE_ADD + (4 * (channel - 1)), 4);
		val = struct.unpack('f', bytearray(buff))
	except Exception as e:
		val[0] = -273.15
	bus.close()	
	return val[0]

def getRes(stack, channel):
	if stack < 0 or stack > 7:
		raise ValueError('Invalid stack level')
		return
    
	if channel < 1 or channel > 8:
		raise ValueError('Invalid channel number')
		return
   	bus = smbus.SMBus(1) 
	try:
		buff = bus.read_i2c_block_data(DEVICE_ADDRESS+stack, RTD_RESISTANCE_ADD + (4 * (channel - 1)), 4);
		val = struct.unpack('f', bytearray(buff))
	except Exception as e:
		val[0] = 0
	bus.close()	
	return val[0]
		

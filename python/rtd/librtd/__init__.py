import smbus
import struct

#bus = smbus.SMBus(1)    # 0 = /dev/i2c-0 (port I2C0), 1 = /dev/i2c-1 (port I2C1)

DEVICE_ADDRESS = 0x40     #7 bit address (will be left shifted to add the read write bit)

RTD_TEMPERATURE_ADD = 0
	
def get(stack, channel):
	bus = smbus.SMBus(1)
	if stack < 0 or stack > 7:
		raise ValueError('Invalid stack level')
		return
    
	if channel < 1 or channel > 8:
		raise ValueError('Invalid relay number')
		return
    
  buff = [0, 0, 0, 0]  
  try:
    buff = bus.read_i2c_block_data(DEVICE_ADDRESS+stack, RTD_TEMPERATURE_ADD + 4 * (channel - 1));
    val = struct.unpack('f', buff)
  except Exception as e:
    val = -1  
  return val   
		

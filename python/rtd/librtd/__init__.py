import smbus
import struct

# bus = smbus.SMBus(1)    # 0 = /dev/i2c-0 (port I2C0), 1 = /dev/i2c-1 (port I2C1)

DEVICE_ADDRESS = 0x40  # 7 bit address (will be left shifted to add the read write bit)

RTD_TEMPERATURE_ADD = 0
RTD_RESISTANCE_ADD = 59


def get(stack, channel):
    if stack < 0 or stack > 7:
        raise ValueError('Invalid stack level')
    if channel < 1 or channel > 8:
        raise ValueError('Invalid channel number')
    val = (-273.15)
    bus = smbus.SMBus(1)
    try:
        buff = bus.read_i2c_block_data(DEVICE_ADDRESS + stack, RTD_TEMPERATURE_ADD + (4 * (channel - 1)), 4)
        val = struct.unpack('f', bytearray(buff))
    except Exception as e:
        bus.close()
        raise ValueError('Fail to communicate with the RTD card with message: \"' + str(e) + '\"')
    bus.close()
    return val[0]


def getRes(stack, channel):
    if stack < 0 or stack > 7:
        raise ValueError('Invalid stack level')
    if channel < 1 or channel > 8:
        raise ValueError('Invalid channel number')
    val = (-273.15)
    bus = smbus.SMBus(1)
    try:
        buff = bus.read_i2c_block_data(DEVICE_ADDRESS + stack, RTD_RESISTANCE_ADD + (4 * (channel - 1)), 4)
        val = struct.unpack('f', bytearray(buff))
    except Exception as e:
        bus.close()
        raise ValueError('Fail to communicate with the RTD card with message: \"' + str(e) + '\"')
    bus.close()
    return val[0]


def get_poly5(stack: int, channel: int) -> float:
    """
    Convert RTD reading to Temperature, using 5th order polynomial fit of Temperature as a function of Resistance.
    This fit provides much improved accuracy through the temperature range of [-200C, 660C], particularly near the high
    and low ranges, compared to the default linear fitting function baked into the Sequent RTD Data Acquisition
    Stackable Card for Raspberry Pi.  The coefficients for this fit were developed in a project documented
    in https://github.com/ewjax/max31865

        temp_C = (c5 * res^5) + (c4 * res^4) + (c3 * res^3) + (c2 * res^2) + (c1 * res) + c0

    :param stack: 0-7, which hat to read
    :param channel: 1-8, which RTD to read on indicated hat
    :return: temperature, in celcius
    """

    # coeffs for 5th order fit
    c5 = -2.10678E-11
    c4 = 2.27311E-08
    c3 = -8.20888E-06
    c2 = 2.38589E-03
    c1 = 2.24745E+00
    c0 = -2.42522E+02

    # get RTD resistance
    res = getRes(stack, channel)

    # do the math
    #   Rearrange a bit to make it friendlier (less expensive) to calculate
    #   temp_C = res ( res ( res ( res ( res * c5 + c4) + c3) + c2) + c1) + c0
    temp_C = res * c5 + c4

    temp_C *= res
    temp_C += c3

    temp_C *= res
    temp_C += c2

    temp_C *= res
    temp_C += c1

    temp_C *= res
    temp_C += c0

    return temp_C

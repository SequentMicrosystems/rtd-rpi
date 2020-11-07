# update

This is the [Sequent Microsystems](https://www.sequentmicrosystems.com) RTD Data Aquisition Stackable Card 
firmware update tool.

## Usage

```bash 
~$ git clone https://github.com/SequentMicrosystems/rtd-rpi.git 
~$ cd rtd-rpi/update/ 
~/rtd-rpi/update$ ./update 0 
``` 

If you clone the repository already, skip the first step.
The command will download the newest firmware version from our server and write it  to the board. 
The stack level of the board must be provided as a parameter.


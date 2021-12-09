import time
from fritzconnection.lib.fritzstatus import FritzStatus
import board
import busio
import adafruit_mcp4725


def main():
    """Main programm"""

    SLEEP_TIMER_SECONDS = 1
    MAX_UPLOAD_MBPS = 20        #Maybe chose 50
    MAX_DOWNLOAD_MBPS = 170     #Maybe chose 200

    #initialize the I2C connection with the sensor
    i2c = busio.I2C(board.SCL, board.SDA)
    dacUpload = adafruit_mcp4725.MCP4725(i2c)
    dacDownload = adafruit_mcp4725.MCP4725(i2c, address=0x63)
    tmpValueUploadOld = 0
    tmpValueDownloadOld = 0

    #Create an Fritz Status Object
    fc = FritzStatus(address='192.168.178.1', password="")

    blnLoopActive = True

    #Start the loop
    while blnLoopActive:
        #transmission_rate: The upstream and downstream values as a tuple in bytes per second. Use this for periodical calling.
        #print( "Upload: " + str(convertToMbps(fc.transmission_rate[0])) + "Mbps // Download: " + str(convertToMbps(fc.transmission_rate[1])) + "Mbps" )
        
        tmpValueUpload = convertToAnalogOutput(30, convertToMbps(fc.transmission_rate[0])) 
        #print("Normalized Value upload: " + str(tmpNormalizedValueUpload))

        tmpValueDownload = convertToAnalogOutput(200, convertToMbps(fc.transmission_rate[1])) 
        #print("Normalized Value download: " + str(tmpNormalizedValueDownload))

        #print("Normalized Upload: " + str(tmpValueUpload) + " Normalized Download: " + str(tmpValueDownload))
        
        #dacUpload.value = int(tmpValueUpload)
        #dacDownload.value = int(tmpValueDownload)
        transition(dacUpload, tmpValueUploadOld, tmpValueUpload, 0.5)
        transition(dacDownload, tmpValueDownloadOld, tmpValueDownload, 0.5)

        tmpValueUploadOld = tmpValueUpload
        tmpValueDownloadOld = tmpValueDownload
        blnLoopActive = True
        #time.sleep(SLEEP_TIMER_SECONDS)


def convertToAnalogOutput(maxMbps, currentMbps):
    """
    Convert Mbps to Analog Output over I2C
    """
    tmpRetVal = 0
    #if (currentMbps < 1):
    #    tmpRetVal = 0
    #elif (float(currentMbps) > float(maxMbps)):
    #    tmpRetVal = 1
    #else:

    tmpRetVal = 65535/float(maxMbps)*float(currentMbps)

    return tmpRetVal


def convertToMbps(bytesPerSeconds):
    """
    BytesPerSeconds = Input 
    ---
    Convert the Data (bytes per second) to Mbps Mega bits per second 
    """

    #1 Mega bit per Second (Mbps) = 1 000 000 Bits / Second
    #First convert to Bit (1 Byte = 8 Bit) Than convert to Mbps by deviding through 1 000 000
    return (bytesPerSeconds * 8)/1000000


def transition(dac, lastValue, newValue, timneSpan):
    """
    transition between the analog values
    Do this to prevent jumping pointer
    """

    intItterations = timneSpan/0.1
    blnCountUp = False
    currentValue = lastValue
    #print("Iterations: " + str(intItterations) + "  " + str(currentValue) + " ")

    if (lastValue < newValue):
        blnCountUp = True

        valueDiff = (abs(newValue) - abs(lastValue)) / intItterations

    else:

        valueDiff = (abs(lastValue) - abs(newValue)) / intItterations

    for idx in range(int(intItterations)):

        #print(str(idx))
        if blnCountUp:
            currentValue = currentValue + valueDiff
        else:
            currentValue = currentValue - valueDiff
    
        #print(str(currentValue))
        dac.value = int(currentValue)
        time.sleep(0.1)


if __name__ == '__main__':

    #Start the main programm
    main()

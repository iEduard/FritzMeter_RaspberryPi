import time
from fritzconnection.lib.fritzstatus import FritzStatus

def main():
    """Main program"""

    fc = FritzStatus(address="192.168.178.1", password="")
    _transmissionRate = fc.transmission_rate
    _maxTransmitionRate = fc.max_bit_rate

    print("Upload: " + str(_transmissionRate[0]) + " Max Rate: " + str(_maxTransmitionRate[0]/1000000))
    print("Download: " + str(_transmissionRate[1]) + " Max Rate: " + str(_maxTransmitionRate[1]/1000000))

if __name__ == '__main__':

    #Start the main programm
    main()

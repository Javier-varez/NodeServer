#ifndef NODESERVER_NRF24L01_H
#define NODESERVER_NRF24L01_H

#include <string>
#include <pio/gpio.h>
#include <pio/peripheral_manager_client.h>
#include <pio/spi_device.h>

class nRF24L01 {
public:
    nRF24L01(std::string CE, std::string INT, std::string SPI);
    ~nRF24L01();

private:
    APeripheralManagerClient *mClient;
    AGpio *mCE;
    AGpio *mInt;
    ASpiDevice *mSpiDev;
};

#endif //NODESERVER_NRF24L01_H
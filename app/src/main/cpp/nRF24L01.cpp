#include "nRF24L01.h"

#include <pio/gpio.h>
#include <pio/peripheral_manager_client.h>
#include <pio/spi_device.h>

#include "log.h"
#define TAG "nRF24L01_CPP"



std::string nRF24L01::getString() {
    return "TEST";
}

nRF24L01::nRF24L01() {

}

nRF24L01::~nRF24L01() {

}
#include "nRF24L01.h"

#include <time.h>

#include "log.h"

#define TAG "nRF24L01_CPP"

nRF24L01::nRF24L01(std::string CE, std::string INT, std::string CS, std::string SPI) {
    mClient = APeripheralManagerClient_new();
    ASSERT(mClient, "Failed to open mClient");

    APeripheralManagerClient_openGpio(mClient, CE.c_str(), &mCE);
    ASSERT(mCE, "Failed to open GPIO mCE");

    ASSERT(AGpio_setDirection(mCE, AGPIO_DIRECTION_OUT_INITIALLY_HIGH) == 0,
           "Failed to set mCE direction");

    APeripheralManagerClient_openGpio(mClient, INT.c_str(), &mInt);
    ASSERT(mInt, "Failed to open GPIO mInt");

    ASSERT(AGpio_setDirection(mInt, AGPIO_DIRECTION_IN) == 0,
           "Failed to set mInt direction");

    ASSERT(AGpio_setEdgeTriggerType(mInt, AGPIO_EDGE_RISING) == 0,
           "Failed to set the mInt edge trigger type");

    APeripheralManagerClient_openGpio(mClient, CS.c_str(), &mCS);
    ASSERT(mCS, "Failed to open GPIO mCS");

    ASSERT(AGpio_setDirection(mCS, AGPIO_DIRECTION_OUT_INITIALLY_HIGH) == 0,
           "Failed to set mCS direction");

    APeripheralManagerClient_openSpiDevice(mClient, SPI.c_str(), &mSpiDev);
    ASSERT(mSpiDev, "Failed to open mSpiDev");

    ASSERT(ASpiDevice_setMode(mSpiDev, ASPI_MODE0) == 0,
           "Failed to set Spi mode");
    ASSERT(ASpiDevice_setCsChange(mSpiDev, 0) == 0,
           "Failed to set Spi Cs Change");
}

nRF24L01::~nRF24L01() {
    AGpio_delete(mCE);
    AGpio_delete(mInt);
    AGpio_delete(mCS);
    ASpiDevice_delete(mSpiDev);
}

bool nRF24L01::init() {
    // Set chip enable low
    AGpio_setValue(mCE, 0);

    // Wait POR just in case
    struct timespec delay = {
            .tv_sec = 0,
            .tv_nsec = 20*1000*1000
    };
    nanosleep(&delay, nullptr);

    // Power up, set mode, enable CRC (1 byte)
    writeRegister(CONFIG, CONFIG_EN_CRC | mConfiguration.mode);

    // Setup automatic retransmission to 1500us delay to allow 32 bytes payloads
    writeRegister(SETUP_RETR, 0x4f);

    // Configure channel
    writeRegister(RF_CH, mConfiguration.channel);

    // Configure PA
    writeRegister(RF_SETUP, (mConfiguration.output_power << 1) | RF_SETUP_LNA_HCURR);

    // Configure TX address
    writeRegister(TX_ADDR, mConfiguration.addr);

    // Configure RX address (PIPE 0)
    writeRegister(RX_ADDR_P0, mConfiguration.addr);

    // Configure RX payload size
    writeRegister(RX_PW_P0, 32);

    // Disable AutoACK
    writeRegister(EN_AA, 0x00);

    writeRegister(EN_RXADDR, 0x01);

    AGpio_setValue(mCS, 0);
    sendCommand(FLUSH_TX);
    AGpio_setValue(mCS, 1);

    AGpio_setValue(mCS, 0);
    sendCommand(FLUSH_RX);
    AGpio_setValue(mCS, 1);

    writeRegister(STATUS, STATUS_RX_DR | STATUS_TX_DS | STATUS_MAX_RT);

    powerUp();

    return true;
}

int nRF24L01::setMode(nRF24L01_Mode mode) {
    mConfiguration.mode = mode;

    uint8_t config_reg = readRegister(CONFIG);
    if (mode == TRANSMITTER) {
        config_reg &= ~CONFIG_PRIM_RX;
        applyIRQMask(MASK_RX_DR | MASK_MAX_RT);
    }
    else if (mode == RECEIVER) {
        config_reg |= CONFIG_PRIM_RX;
        applyIRQMask(MASK_TX_DS | MASK_MAX_RT);
    }

    return writeRegister(CONFIG, config_reg);
}

bool nRF24L01::powerUp() {
    // Power up
    writeRegister(CONFIG, readRegister(CONFIG) | CONFIG_PWR_UP);

    // Delay more than 2ms to provide powerup
    struct timespec delay = {
            .tv_sec = 0,
            .tv_nsec = 4*1000*1000
    };
    nanosleep(&delay, nullptr);

    return true;
}

void nRF24L01::applyIRQMask(uint8_t mask) {
    // Config IRQ Mask
    uint8_t data = readRegister(CONFIG);

    // Apply mask
    data &= ~(MASK_RX_DR | MASK_TX_DS | MASK_MAX_RT);
    data |= mask;

    writeRegister(CONFIG, data);
}

bool nRF24L01::writeRegister(uint8_t reg, uint8_t data) {
    bool rc = false;
    AGpio_setValue(mCS, 0);

    if (sendCommand(W_REGISTER | reg) == true) {
        rc = ASpiDevice_writeBuffer(mSpiDev, &data, 1) == 0;
    }
    AGpio_setValue(mCS, 1);

    return rc;
}

bool nRF24L01::writeRegister(uint8_t reg, const std::array<uint8_t , ADDR_LENGTH> &data) {
    bool rc = false;
    AGpio_setValue(mCS, 0);

    if (sendCommand(W_REGISTER | reg) == true) {
        rc = ASpiDevice_writeBuffer(mSpiDev, data.begin(), 1) == 0;
    }
    AGpio_setValue(mCS, 1);

    return rc;}

bool nRF24L01::sendCommand(uint8_t command) {
    return ASpiDevice_writeBuffer(mSpiDev, &command, 1) == 0;
}

uint8_t nRF24L01::readRegister(uint8_t reg) {
    uint8_t value = 0;
    AGpio_setValue(mCS, 0);
    if (sendCommand(R_REGISTER | reg) == true) {
        value = ASpiDevice_readBuffer(mSpiDev, &value, 1);
    }
    AGpio_setValue(mCS, 1);
    return value;
}

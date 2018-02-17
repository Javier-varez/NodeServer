#include "nRF24L01.h"
#include <stdlib.h>
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
    ASSERT(ASpiDevice_setFrequency(mSpiDev, 1000000) == 0,
           "Failed to set Spi Bus Frequency");

    mConfiguration.mode = INVALID_MODE;
    mConfiguration.output_power = INVALID_PA;
    mConfiguration.crc = INVALID_CRC;
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
    AGpio_setValue(mCS, 1);

    powerDown();

    // Wait POR just in case
    struct timespec delay = {
            .tv_sec = 0,
            .tv_nsec = 20*1000*1000
    };
    nanosleep(&delay, nullptr);

    // Set mode, enable CRC (1 byte)
    setCRC(DEFAULT_CRC_MODE);
    uint8_t config = readRegister(CONFIG);
    setMode(DEFAULT_MODE);

    // Setup automatic retransmission to 1500us delay to allow 32 bytes payloads
    writeRegister(SETUP_RETR, 0x4f);

    // Configure channel
    setChannel(DEFAULT_CHANNEL);

    // Configure PA
    setOutputPower(DEFAULT_OUT_POWER);
    setDataRate(DEFAULT_DATA_RATE);

    // Configure address
    setAddress(DEFAULT_ADDRESS);

    // Configure RX payload size
    writeRegister(RX_PW_P0, DEFAULT_PAYLOAD);

    // Disable AutoACK
    writeRegister(EN_AA, 0x00);

    // Enable data pipe 0
    writeRegister(EN_RXADDR, 0x01);

    AGpio_setValue(mCS, 0);
    sendCommand(FLUSH_TX);
    AGpio_setValue(mCS, 1);

    AGpio_setValue(mCS, 0);
    sendCommand(FLUSH_RX);
    AGpio_setValue(mCS, 1);

    // Clear Status register
    clearIRQ(STATUS_RX_DR | STATUS_TX_DS | STATUS_MAX_RT);

    powerUp();

    return true;
}

bool nRF24L01::transmit(uint8_t *payload) {
    bool rc = 0;
    rc = writePayload(payload, 32); // 32 bytes payload
    AGpio_setValue(mCE, 1);

    // 15 us delay for transmission of a single packet
    struct timespec delay = {
            .tv_sec = 0,
            .tv_nsec = 15*1000
    };
    nanosleep(&delay, nullptr);
    AGpio_setValue(mCE, 0);

    return rc;
}

bool nRF24L01::pollForRXPacket() {
    uint8_t status = 0;
    bool rc = false;

    AGpio_setValue(mCE, 1);

    while (!(status & STATUS_RX_DR)) {
        status = readRegister(STATUS);
    }

    // Clear IT flag by writing 1
    rc = writeRegister(STATUS, STATUS_RX_DR);

    AGpio_setValue(mCE, 0);
    return rc;
}

bool nRF24L01::pollForRXPacketWithTimeout(int timeout_ms) {
    bool rc = false;
    uint8_t reg = 0x00;

    // CHECK IF Flag is already active
    reg = readRegister(STATUS);
    if (reg & STATUS_RX_DR)
        return true;

    // Else, wait for IRQ
    clearIRQ(STATUS_RX_DR);
    // Start receiving
    AGpio_setValue(mCE, 1);

    int intFd = -1;
    if (AGpio_getPollingFd(mInt, &intFd) == 0) {
        fd_set readSet;
        FD_SET(intFd, &readSet);

        struct timeval timeout = {
                .tv_sec = timeout_ms / 1000,
                .tv_usec = (timeout_ms % 1000) * 1000
        };

        select(intFd+1, &readSet, NULL, NULL, &timeout);
    } else {
        return false;
    }

    // Received IRQ, check for STATUS_RX_DR FLAG
    reg = readRegister(STATUS);
    if (reg & STATUS_RX_DR)
        rc = true;

    // Enter Standby
    AGpio_setValue(mCE, 0);

    return rc;
}


bool nRF24L01::pollForTXPacket() {
    uint8_t status = 0;
    bool rc = 0;
    while (!(status & STATUS_TX_DS)) {
        status = readRegister(STATUS);
    }

    // Clear IT flag by writing 1
    status = STATUS_TX_DS;
    rc = writeRegister(STATUS, STATUS_TX_DS);
    return rc;
}

bool nRF24L01::pollForTXPacketWithTimeout(int timeout_ms) {
    // CHECK IF Flag is already active
    if (readRegister(STATUS) & STATUS_TX_DS) {
        clearIRQ(STATUS_TX_DS);
        return 1;
    }

    // Wait for IRQ and check if it corresponds to TX event
    int intFd = -1;
    if (AGpio_getPollingFd(mInt, &intFd) == 0) {
        fd_set readSet;
        FD_SET(intFd, &readSet);

        struct timeval timeout = {
                .tv_sec = timeout_ms / 1000,
                .tv_usec = (timeout_ms % 1000) * 1000
        };

        select(intFd+1, &readSet, NULL, NULL, &timeout);
    } else {
        return false;
    }

    if (readRegister(STATUS) & STATUS_TX_DS) {
        // Clear IT flag by writing 1
        writeRegister(STATUS, STATUS_TX_DS);
        return  1;
    }

    return 0;
}


bool nRF24L01::writePayload(uint8_t *buf, uint8_t len) {
    uint8_t rc = 0;
    AGpio_setValue(mCS, 0);

    if (sendCommand(W_TX_PAYLOAD) == 0) {
        rc = ASpiDevice_writeBuffer(mSpiDev, (void*) buf, len) == 0;
    }
    AGpio_setValue(mCS, 1);

    return rc;
}

bool nRF24L01::readPayload(uint8_t *buf, uint8_t len) {
    bool rc = 0;
    AGpio_setValue(mCS, 0);

    if (sendCommand(R_RX_PAYLOAD) == 0) {
        rc = ASpiDevice_readBuffer(mSpiDev, (void*) buf, PAYLOAD_SIZE) == 0;
    }
    AGpio_setValue(mCS, 1);

    // Clear flag if fifo is already empty
    uint8_t reg;
    reg = readRegister(FIFO_STATUS);
    if (reg & RX_EMPTY)
        clearIRQ(STATUS_RX_DR);

    return rc;
}

bool nRF24L01::powerUp() {
    // Power up
    writeRegister(CONFIG, (uint8_t) (readRegister(CONFIG) | CONFIG_PWR_UP));

    // Delay more than 2ms to provide powerup
    struct timespec delay = {
            .tv_sec = 0,
            .tv_nsec = 4*1000*1000
    };
    nanosleep(&delay, nullptr);

    return true;
}

bool nRF24L01::powerDown() {
    // Power Down
    writeRegister(CONFIG, (uint8_t) (readRegister(CONFIG) & ~CONFIG_PWR_UP));

    // Delay more than 2ms to provide powerDown
    struct timespec delay = {
            .tv_sec = 0,
            .tv_nsec = 4*1000*1000
    };
    nanosleep(&delay, nullptr);

    return true;
}

void nRF24L01::clearIRQ(uint8_t irq) {
    // Clear status reg
    writeRegister(STATUS, irq);
}

void nRF24L01::applyIRQMask(uint8_t mask) {
    // Config IRQ Mask
    uint8_t data = readRegister(CONFIG);

    // Apply mask
    data &= ~(MASK_RX_DR | MASK_TX_DS | MASK_MAX_RT);
    data |= mask;

    writeRegister(CONFIG, data);
}

bool nRF24L01::setMode(nRF24L01_Mode mode) {
    mConfiguration.mode = mode;

    uint8_t config_reg = readRegister(CONFIG);
    if (mode == TRANSMITTER) {
        config_reg &= ~CONFIG_PRIM_RX;
        //applyIRQMask(MASK_RX_DR | MASK_MAX_RT);
    }
    else if (mode == RECEIVER) {
        config_reg |= CONFIG_PRIM_RX;
        //applyIRQMask(MASK_TX_DS | MASK_MAX_RT);
    }

    return writeRegister(CONFIG, config_reg);
}

void nRF24L01::setOutputPower(nRF24L01_PA output_power) {
    mConfiguration.output_power = output_power;
    uint8_t reg = readRegister(RF_SETUP);
    reg &= ~(RF_SETUP_LNA_PWRM);
    writeRegister(RF_SETUP, (mConfiguration.output_power << 1) | RF_SETUP_LNA_HCURR | reg);
};

void nRF24L01::setDataRate(nRF24L01_DR dr) {
    mConfiguration.data_rate = dr;
    uint8_t reg = readRegister(RF_SETUP);
    reg &= ~(RF_SETUP_RF_DR);
    writeRegister(RF_SETUP, mConfiguration.data_rate | RF_SETUP_LNA_HCURR | reg);
};

void nRF24L01::setChannel(uint8_t channel) {
    mConfiguration.channel = channel;
    writeRegister(RF_CH, mConfiguration.channel);
}

void nRF24L01::setAddress(std::array<uint8_t, ADDR_LENGTH> addr) {
    mConfiguration.addr = addr;
    writeRegister(TX_ADDR, mConfiguration.addr);
    writeRegister(RX_ADDR_P0, mConfiguration.addr);
}

void nRF24L01::setCRC(nRF24L01_CRC crc_mode) {
    mConfiguration.crc = crc_mode;
    uint8_t config = readRegister(CONFIG);
    config &= ~(CONFIG_EN_CRC | CONFIG_CRC0);
    config |= crc_mode;
    writeRegister(CONFIG, config);
}

nRF24L01_Mode nRF24L01::getMode() {
    return mConfiguration.mode;
}

nRF24L01_CRC nRF24L01::getCRC() {
    return mConfiguration.crc;
}

nRF24L01_PA nRF24L01::getOutputPower() {
    return mConfiguration.output_power;
}

nRF24L01_DR nRF24L01::getDataRate() {
    return mConfiguration.data_rate;
}

uint8_t nRF24L01::getChannel() {
    return mConfiguration.channel;
}

std::array<uint8_t, ADDR_LENGTH> nRF24L01::getAddress() {
    return mConfiguration.addr;
};

bool nRF24L01::writeRegister(uint8_t reg, uint8_t data) {
    bool rc = false;
    AGpio_setValue(mCS, 0);

    if (sendCommand((uint8_t)(W_REGISTER | reg))) {
        rc = ASpiDevice_writeBuffer(mSpiDev, &data, 1) == 0;
    }
    AGpio_setValue(mCS, 1);

    return rc;
}

bool nRF24L01::writeRegister(uint8_t reg, const std::array<uint8_t , ADDR_LENGTH> &data) {
    bool rc = false;
    AGpio_setValue(mCS, 0);

    if (sendCommand((uint8_t)(W_REGISTER | reg))) {
        rc = ASpiDevice_writeBuffer(mSpiDev, data.begin(), 1) == 0;
    }
    AGpio_setValue(mCS, 1);

    return rc;
}

bool nRF24L01::sendCommand(uint8_t command) {
    return ASpiDevice_writeBuffer(mSpiDev, &command, 1) == 0;
}

uint8_t nRF24L01::readRegister(uint8_t reg) {
    uint8_t value = 0;
    AGpio_setValue(mCS, 0);
    if (sendCommand((uint8_t)(R_REGISTER | reg))) {
        ASpiDevice_readBuffer(mSpiDev, &value, 1);
    }
    AGpio_setValue(mCS, 1);
    return value;
}

package com.noosrequired.javier_varez.nodeserver;

public class nRF24L01 {

    static {
        System.loadLibrary("nRF24L01_lib");
    }

    public enum outputPower {
        m0dBm(3),
        m6dBm(2),
        m12dBm(1),
        m18dBm(0),
        INVALID_PA(0xFF);

        int mIndex;
        outputPower(int index) {
            mIndex = index;
        }

        boolean equals(mode m) {
            return this.mIndex == m.mIndex;
        }

        int getOutputPower() {
            return mIndex;
        }
    }

    public enum mode {
        RECEIVER(1),
        TRANSMITTER(0),
        INVALID_MODE(0xFF);

        int mIndex;
        mode(int index) {
            mIndex = index;
        }

        boolean equals(mode m) {
            return this.mIndex == m.mIndex;
        }

        int getMode() {
            return mIndex;
        }
    }

    public enum IRQ {
        TX_MAX_RT(0x10),
        TX_DATA_SENT(0x20),
        RX_DATA_READY(0x40);

        byte mIndex;
        IRQ (int index) {
            mIndex = (byte) index;
        }
        byte getMask() {
            return mIndex;
        }
    }

    public enum CRC {
        CRC_OFF(0),
        CRC_1BYTE(0x08),
        CRC_2BYTES (0x0C),
        CRC_INVALID(0xFF);

        byte mIndex;
        CRC(int index) {
            mIndex = (byte) index;
        }

        byte getCRC() {
            return mIndex;
        }
    }

    private native boolean nRF24L01Create(String CE, String Int, String CS, String SpiDevice);
    private native void nRF24L01Init();
    private native void nRF24L01Transmit(byte [] array);
    private native void nRF24L01Receive(byte [] array);
    private native void nRF24L01PollForRXPacket();
    private native void nRF24L01PollForTXPacket();
    private native void nRF24L01PollForRXPacketWithTimeout(int timeout_ms);
    private native void nRF24L01PollForTXPacketWithTimeout(int timeout_ms);
    private native void nRF24L01SetMode(int mode);
    private native void nRF24L01SetOutputPower(int outputPower);
    private native void nRF24L01SetChannel(int channel);
    private native void nRF24L01SetAddress(byte [] address);
    private native void nRF24L01SetCRC(byte crc);
    private native int nRF24L01GetMode();
    private native int nRF24L01GetOutputPower();
    private native int nRF24L01GetChannel();
    private native byte[] nRF24L01GetAddress();
    private native int nRF24L01GetCRC();
    private native void nRF24L01ClearIRQ(byte IRQ);
    private native void nRF24L01ApplyIRQMask(byte mask);
    private native void nRF24L01Destroy();

    public void create(String CE, String Int, String CS, String SpiDevice) {
        nRF24L01Create(CE, Int, CS, SpiDevice);
    }

    public void init() {
        nRF24L01Init();
    }

    public void setMode(mode m) {
        nRF24L01SetMode(m.getMode());
    }

    public void setOutputPower(outputPower power) {
        nRF24L01SetOutputPower(power.getOutputPower());
    }

    public void setChannel(int channel) {
        nRF24L01SetChannel(channel);
    }

    public void setAddress(byte [] address) {
        nRF24L01SetAddress(address);
    }

    public void setCRC(CRC crc) {
        nRF24L01SetCRC(crc.getCRC());
    }

    public mode getMode() {
        int index = nRF24L01GetMode();
        for (mode m: mode.values()) {
            if (index == m.getMode()) {
                return m;
            }
        }
        return mode.INVALID_MODE;
    }

    public outputPower getOutputPower() {
        int index = nRF24L01GetOutputPower();
        for (outputPower power: outputPower.values()) {
            if (index == power.getOutputPower()) {
                return power;
            }
        }
        return outputPower.INVALID_PA;
    }

    public int getChannel() {
        return  nRF24L01GetChannel();
    }

    public byte[] getAddress() {
        return  nRF24L01GetAddress();
    }

    public CRC getCRC() {
        int index = nRF24L01GetCRC();
        for (CRC crc: CRC.values()) {
            if (index == crc.getCRC()) {
                return crc;
            }
        }
        return CRC.CRC_INVALID;
    }

    public void clearIRQ(IRQ [] irq_mask) {
        byte mask = 0x00;
        for (IRQ irq: irq_mask) {
            mask |= irq.getMask();
        }
        nRF24L01ClearIRQ(mask);
    }

    public void clearIRQ(IRQ irq_mask) {
        nRF24L01ClearIRQ(irq_mask.getMask());
    }

    public void applyIRQMask(IRQ [] irq_mask) {
        byte mask = 0x00;
        for (IRQ irq: irq_mask) {
            mask |= irq.getMask();
        }
        nRF24L01ApplyIRQMask(mask);
    }

    public void applyIRQMask(IRQ irq_mask) {
        nRF24L01ApplyIRQMask(irq_mask.getMask());
    }

    public void destroy() {
        nRF24L01Destroy();
    }
}

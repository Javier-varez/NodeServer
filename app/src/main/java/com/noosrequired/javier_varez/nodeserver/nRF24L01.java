package com.noosrequired.javier_varez.nodeserver;

public class nRF24L01 {

    static {
        System.loadLibrary("nRF24L01_lib");
    }

    public enum outputPower {
        m0dBm(0),
        m6dBm(1),
        m12dBm(2),
        m18dBm(3);

        int mIndex;
        outputPower(int index) {
            mIndex = index;
        }

        int getOutputPower() {
            return mIndex;
        }
    }

    public enum mode {
        RECEIVER(1),
        TRANSMITTER(0);

        int mIndex;
        mode(int index) {
            mIndex = index;
        }

        int getMode() {
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

    public void destroy() {
        nRF24L01Destroy();
    }
}

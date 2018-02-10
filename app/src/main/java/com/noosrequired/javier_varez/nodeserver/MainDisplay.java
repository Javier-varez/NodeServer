package com.noosrequired.javier_varez.nodeserver;

import android.app.Activity;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

public class MainDisplay extends Activity {

    static String TAG = "MainDisplay";
    nRF24L01 comms_dev = new nRF24L01();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main_display);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);

        AsyncTask.execute(new Runnable() {
            @Override
            public void run() {
                comms_dev.create("BCM7", "BCM25", "BCM24", "SPI0.0");
                comms_dev.init();
                comms_dev.setMode(nRF24L01.mode.RECEIVER);
                comms_dev.setChannel(24);
                comms_dev.setDataRate(nRF24L01.dataRate._1Mbps);
                comms_dev.setOutputPower(nRF24L01.outputPower.m0dBm);
                nRF24L01.IRQ [] mask =
                        new nRF24L01.IRQ[] {nRF24L01.IRQ.TX_DATA_SENT, nRF24L01.IRQ.TX_MAX_RT};
                comms_dev.clearIRQ(nRF24L01.IRQ.RX_DATA_READY);
                comms_dev.applyIRQMask(mask);
                Log.d(TAG, "Device Configured!");
                comms_dev.pollForRXPacket();

                byte [] payload = new byte[32];
                comms_dev.receive(payload);
                Log.d(TAG, "Payload" + payload.toString());
            }
        });

        Log.d(TAG, "Hola!");
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        comms_dev.destroy();
    }


}

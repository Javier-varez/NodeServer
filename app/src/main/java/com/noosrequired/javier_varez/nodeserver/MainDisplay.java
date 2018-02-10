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
                comms_dev.setOutputPower(nRF24L01.outputPower.m0dBm);
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

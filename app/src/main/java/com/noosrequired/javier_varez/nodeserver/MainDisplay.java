package com.noosrequired.javier_varez.nodeserver;

import android.app.Activity;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

public class MainDisplay extends Activity {

    static {
        System.loadLibrary("nRF24L01_lib");
    }

    static String TAG = "MainDisplay";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main_display);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);

        AsyncTask.execute(new Runnable() {
            @Override
            public void run() {
                nRF24L01Create("BCM7", "BCM25", "", "SPI0.0");
            }
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        nRF24L01Destroy();
    }

    public native boolean nRF24L01Create(String CE, String Int, String CS, String SpiDevice);
    public native void nRF24L01Destroy();
}

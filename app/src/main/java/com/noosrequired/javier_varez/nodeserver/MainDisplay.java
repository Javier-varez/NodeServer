package com.noosrequired.javier_varez.nodeserver;

import android.app.Activity;
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
        tv.setText(stringFromJNI());
    }


    public native String stringFromJNI();
}

package com.example.prutc.irhomecontroller

import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import io.netpie.microgear.Microgear

class MainActivity : AppCompatActivity() {

    private var microgear = Microgear(this)
    private var appid = "IRHomeController"
    private var key = "jOXh9JkDCQ0HM9D"
    private var secret = "UOxlaPdawNjghaQ8Ui5gRKfeQ"
    private var alias = "android"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        val callback = MicrogearCallBack()
        microgear.connect(appid, key, secret, alias)
        microgear.setCallback(callback)
        microgear.subscribe("Topictest")
        Thread(object : Runnable {
            var count = 1
            override fun run() {
                while (!Thread.interrupted())
                    try {
                        runOnUiThread {
                            microgear.chat("android", "hello")
//                            microgear.publish("Topictest", "$count.  Test message")
                            count++
                        }
                        Thread.sleep(2000)
                    } catch (e: InterruptedException) {
                        // ooops
                    }
            }
        }).start()
    }

    override fun onDestroy() {
        super.onDestroy()
        microgear.disconnect()
    }

    override fun onResume() {
        super.onResume()
        microgear.bindServiceResume()
    }
}

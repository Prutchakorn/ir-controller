package com.example.prutc.irhomecontroller
import android.os.Bundle
import io.netpie.microgear.MicrogearEventListener
import android.os.Handler
import android.util.Log


class MicrogearCallBack : MicrogearEventListener {
    var handler = Handler {
        val bundle = it.data
        val str = bundle.getString("myKey")
        Log.d("Boss", str)
        true
    }

    override fun onConnect() {
        val msg = handler.obtainMessage()
        msg.data = Bundle().apply {
            putString("myKey", "Now I'm connected with netpie")
        }
        handler.sendMessage(msg)
        Log.i("Connected", "Now I'm connected with netpie")
    }

    override fun onMessage(topic: String, message: String) {
        val msg = handler.obtainMessage()
        msg.data = Bundle().apply {
            putString("myKey", "$topic : $message")
        }
        handler.sendMessage(msg)
        Log.i("Message", "$topic : $message")
    }

    override fun onPresent(token: String) {
        val msg = handler.obtainMessage()
        msg.data = Bundle().apply {
            putString("myKey", "New friend Connect :$token")
        }
        handler.sendMessage(msg)
        Log.i("present", "New friend Connect :$token")
    }

    override fun onAbsent(token: String) {
        val msg = handler.obtainMessage()
        msg.data = Bundle().apply {
            putString("myKey", "Friend lost :$token")
        }
        handler.sendMessage(msg)
        Log.i("absent", "Friend lost :$token")
    }

    override fun onDisconnect() {
        val msg = handler.obtainMessage()
        msg.data = Bundle().apply {
            putString("myKey", "Disconnected")
        }
        handler.sendMessage(msg)
        Log.i("disconnect", "Disconnected")
    }

    override fun onError(error: String) {
        val msg = handler.obtainMessage()
        msg.data = Bundle().apply {
            putString("myKey", "Exception : $error")
        }
        handler.sendMessage(msg)
        Log.i("exception", "Exception : $error")
    }

    override fun onInfo(info: String) {
        val msg = handler.obtainMessage()
        msg.data = Bundle().apply {
            putString("myKey", "Exception : $info")
        }
        handler.sendMessage(msg)
        Log.i("info", "Info : $info")
    }
}
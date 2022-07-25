package com.xiaomi.aaudiodemo

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.widget.Button
import java.io.File

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        findViewById<Button>(R.id.start).setOnClickListener {

            Thread{
                Environment.getExternalStorageDirectory()?.let{
                    var path = it.absolutePath.plus("/${System.currentTimeMillis()}.pcm")
                     Log.e("hxt","path=${path}")
                     TestJni.startReocrd(path)
                };

            }.start()
        }

        findViewById<Button>(R.id.stop).setOnClickListener {
                TestJni.stopReocrd()
        }


    }
}
package com.xiaomi.aaudiodemo

import android.Manifest
import android.content.Intent
import android.content.pm.PackageManager
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import androidx.core.app.ActivityCompat


/**
 * 申请权限
 */
class PermissionActivity : AppCompatActivity() {
    var permissions = arrayOf(
        Manifest.permission.RECORD_AUDIO,
        Manifest.permission.WRITE_EXTERNAL_STORAGE,
        Manifest.permission.READ_EXTERNAL_STORAGE,
    )

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        checkPermissions()

    }

    private fun checkPermissions() {
       loop@ for (i in permissions.indices) {
            var permission = permissions[i]
            if (ActivityCompat.checkSelfPermission(this, permission)
                != PackageManager.PERMISSION_GRANTED
            ) {
                ActivityCompat.requestPermissions(
                    this, arrayOf(
                        permission
                    ),
                    110
                )
                break@loop
            }
           if(i == permissions.lastIndex){
               jump2Next()
           }
        }
    }

    private fun jump2Next() {
        var intent = Intent(this,MainActivity::class.java)
        startActivity(intent)
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
       if(requestCode==110){
           checkPermissions()
       }
    }
}
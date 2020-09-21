package com.euhat.euhatexpert

import android.Manifest
import android.app.AlertDialog
import android.content.pm.PackageManager
import android.os.Bundle
import android.text.InputType
import android.util.Log
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import kotlinx.android.synthetic.main.activity_main.*
import java.io.*
import java.net.Inet6Address
import java.net.InetAddress
import java.net.NetworkInterface
import java.net.SocketException
import java.nio.charset.StandardCharsets
import java.util.*



class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        try {
            System.loadLibrary("native-lib")
        } catch (ex:Throwable) {
            ex.printStackTrace()
        }

        textVersion.setText(textVersion.text.toString() + " " + getBuildTag())

        requestPermission()

        //writeLog("hi, bingo!")
    }

    fun afterPermissionsAllowed()
    {
        editPort.setText("8083")
        editIp.setText(getHostIP())
        editVisitCode.setText(getVisitCode(getLocalMacAddressFromIp()))

        var showPassword = false
        btnTogglePassword.setOnClickListener(object :View.OnClickListener {
            override fun onClick(v: View?) {
                showPassword = !showPassword
                if (showPassword) {
                    editVisitCode.setInputType(InputType.TYPE_CLASS_NUMBER)
                    btnTogglePassword.setText("HIDE")
                } else {
                    editVisitCode.setInputType(InputType.TYPE_NUMBER_VARIATION_PASSWORD + InputType.TYPE_CLASS_NUMBER)
                    btnTogglePassword.setText("SHOW")
                }
            }
        })

        btnEnd.isEnabled = false;
        btnStart.setOnClickListener(object: View.OnClickListener {
            override fun onClick(v: View?) {
                var port = Integer.parseInt(editPort.getText().toString())
                var visitCode = editVisitCode.getText().toString()
                startServer(port, visitCode)
                btnStart.isEnabled = false
                btnEnd.isEnabled = true
                editPort.isEnabled = false
                editVisitCode.isEnabled = false
            }
        })

        btnEnd.setOnClickListener(object: View.OnClickListener {
            override fun onClick(v: View?) {
                stopServer()
                btnEnd.isEnabled = false
                btnStart.isEnabled = true
                editPort.isEnabled = true
                editVisitCode.isEnabled = true
            }
        })
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<String?>,
        grantResults: IntArray
    ) {
        if (permissions.size > 0) {
            when (requestCode) {
                1 ->
                    if (grantResults[0] == PackageManager.PERMISSION_GRANTED &&
                        grantResults[1] == PackageManager.PERMISSION_GRANTED &&
                        grantResults[2] == PackageManager.PERMISSION_GRANTED &&
                        grantResults[3] == PackageManager.PERMISSION_GRANTED) {
                        afterPermissionsAllowed()
                    } else {
                        AlertDialog.Builder(this)
                            .setMessage("Please allow read & write permissions in system setting!")
                            .setPositiveButton(
                                "OK"
                            ) { dialoginterface, i ->
                                requestPermission()
                            }
                            .show()
                    }
                else -> {
                }
            }
        }
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
    }

    private fun checkPermissionAllGranted(permissions: Array<String>): Boolean {
        for (permission in permissions) {
            if (ContextCompat.checkSelfPermission(this, permission) != PackageManager.PERMISSION_GRANTED) {
                return false
            }
        }
        return true
    }

    fun requestPermission()
    {
        var permissions = arrayOf(
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.INTERNET,
            Manifest.permission.ACCESS_NETWORK_STATE
        )

        if (!checkPermissionAllGranted(permissions)) {
            ActivityCompat.requestPermissions(this@MainActivity, permissions, 1);
        }
        else
            afterPermissionsAllowed()
    }

    external fun getVisitCode(mac:String): String
    external fun startServer(port:Int, visitCode:String): Int
    external fun stopServer(): Int
    external fun getBuildTag(): String

    fun getHostIP(): String? {

        var hostIp: String? = null
        try {
            val nis = NetworkInterface.getNetworkInterfaces()
            var ia: InetAddress
            while (nis.hasMoreElements()) {
                val ni = nis.nextElement() as NetworkInterface
                val ias = ni.getInetAddresses()
                while (ias.hasMoreElements()) {
                    ia = ias.nextElement()
                    if (ia is Inet6Address) {
                        continue // skip ipv6
                    }
                    val ip = ia.getHostAddress()
                    if ("127.0.0.1" != ip) {
                        hostIp = ia.getHostAddress()
                        break
                    }
                }
            }
        } catch (e: SocketException) {
            Log.i("euhat", "SocketException")
            e.printStackTrace()
        }
        return hostIp
    }

    fun getLocalMacAddressFromIp(): String {
        var mac_s: String = ""
        try {
            val mac: ByteArray
            val ne = NetworkInterface.getByInetAddress(
                InetAddress.getByName(getHostIP())
            )
            mac = ne.hardwareAddress
            mac_s = byte2hex(mac)
        } catch (e: Exception) {
            e.printStackTrace()
        }
        return mac_s
    }

    fun byte2hex(b: ByteArray): String {
        var hs = StringBuffer(b.size)
        var stmp = ""
        val len = b.size
        for (n in 0 until len) {
            stmp = Integer.toHexString(b[n].toInt() and 0xFF)
            hs = if (stmp.length == 1) hs.append("0").append(stmp) else {
                hs.append(stmp)
            }
        }
        return hs.toString()
    }

    private fun writeLog(msg: String) {
        try {
            Log.e("EuhatLogJava", msg)

            val apkFilePath: String = getExternalFilesDir("apk")!!.getAbsolutePath()
            val fos =
                FileOutputStream(apkFilePath + "/test.txt", true)
            val osWriter = OutputStreamWriter(fos)
            val writer = BufferedWriter(osWriter)
            val cal = Calendar.getInstance()
            val timeStamp = cal[Calendar.MINUTE]
                .toString() + "-" + cal[Calendar.SECOND] + "-" + cal[Calendar.MILLISECOND] + ": "
            writer.write(timeStamp)
            writer.write(msg)
            writer.newLine()
            writer.flush()
            osWriter.flush()
            fos.flush()
        } catch (e: IOException) {
            Log.e("EuhatLogJava", e.toString());
            e.printStackTrace()
        }
    }

    public companion object {

        init {
//            Debug.waitForDebugger();
//            System.loadLibrary("native-lib")
        }
    }
}

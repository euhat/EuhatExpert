package com.euhat.euhatexpert

import android.content.ClipData
import android.content.ClipboardManager
import android.content.Context
import android.os.Looper
import android.util.Log


class OpFileJni {

    public fun callFromCpp(str: String, i: Int): String {
        Log.e("EuhatLog", "callFromCpp, str:[$str], i:$i")
        return "returnFromJava"
    }

    public fun updateClipboard(mainActivity: MainActivity, str: String) {

        Looper.prepare()

        val clipboard: ClipboardManager = mainActivity.getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager

        val clipData = ClipData.newPlainText(null, str)

        clipboard.setPrimaryClip(clipData)
    }
}
package com.euhat.euhatexpert

import android.util.Log

class OpFileJni {
    public fun callFromCpp(str:String, i:Int): String {
        Log.e("EuhatLog", "callFromCpp, str:[$str], i:$i")
        return "returnFromJava"
    }
}
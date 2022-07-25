package com.xiaomi.aaudiodemo;

public class TestJni {
    static {
        System.loadLibrary("demo-record");
    }

    public static native  void startReocrd(String fileName);
    public static native  void stopReocrd();
}

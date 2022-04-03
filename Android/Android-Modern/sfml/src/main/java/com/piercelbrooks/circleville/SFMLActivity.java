
// Author: Pierce Brooks

package com.piercelbrooks.circleville;

import android.app.NativeActivity;
import android.content.Context;
import android.os.Bundle;
import android.os.PersistableBundle;
import android.util.Log;

import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

public abstract class SFMLActivity extends NativeActivity implements Lock {
    static {
        loadLibraries();
    }

    private static final String TAG = "SFML";
    private static final String VIBRATOR_SERVICE = Context.VIBRATOR_SERVICE;
    private static SFMLActivity self = null;

    private ReentrantLock lock;

    public SFMLActivity() {
        super();
        lock = new ReentrantLock();
        self = this;
    }

    public static SFMLActivity getSelf() {
        return self;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        commonOnCreate(savedInstanceState, null);
        super.onCreate(savedInstanceState);
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState, @Nullable PersistableBundle persistentState) {
        commonOnCreate(savedInstanceState, persistentState);
        super.onCreate(savedInstanceState, persistentState);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    protected void onResume() {
        super.onResume();
        getWindow().getDecorView().requestFocus();
    }

    @Override
    protected void onPause() {
        super.onPause();
        finish();
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
    }

    private void commonOnCreate(@Nullable Bundle savedInstanceState, @Nullable PersistableBundle persistentState) {
        loadLibraries();
    }

    private static void loadLibrary(String library) {
        loadLibrary(library, true);
    }

    private static void loadLibrary(String library, boolean variant) {
        Log.d(TAG, "loadLibrary "+library);
        if (variant) {
            if (BuildConfig.DEBUG) {
                System.loadLibrary(library+"-d");
                return;
            }
        }
        System.loadLibrary(library);
    }

    private static void loadLibraries() {
        loadLibrary("openal", false);
        loadLibrary("sfml-system");
        loadLibrary("sfml-window");
        loadLibrary("sfml-audio");
        loadLibrary("sfml-graphics");
        loadLibrary("sfml-network");
        loadLibrary("sfml-activity");
        loadLibrary("circleville", false);
    }

    @Override
    public String toString() {
        return TAG;
    }

    @Override
    public Object getSystemService(@NonNull String name) {
        return super.getSystemService(name);
    }

    @Override
    public void lock() {
        lock.lock();
    }

    @Override
    public void lockInterruptibly() throws InterruptedException {
        lock.lockInterruptibly();
    }

    @Override
    public boolean tryLock() {
        return lock.tryLock();
    }

    @Override
    public boolean tryLock(long time, TimeUnit unit) throws InterruptedException {
        return lock.tryLock(time, unit);
    }

    @Override
    public void unlock() {
        lock.unlock();
    }

    @Override
    public Condition newCondition() {
        return lock.newCondition();
    }
}

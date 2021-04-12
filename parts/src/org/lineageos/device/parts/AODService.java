/*
 * Copyright (C) 2020 The MoKee Open Source Project
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

package org.lineageos.device.parts;

import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.provider.Settings;
import android.provider.Settings.SettingNotFoundException;
import android.util.Log;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class AODService extends Service {

    private static final String TAG = "AODService";
    private static final boolean DEBUG = false;

    private static final long AOD_DELAY_MS = 1000;

    private ExecutorService mExecutorService;
    private SettingObserver mSettingObserver;
    private ScreenReceiver mScreenReceiver;
    private LightListener mLightListener;

    private Handler mHandler = new Handler(Looper.getMainLooper());
    private boolean mInteractive = true;
    private boolean mOldBoostAOD = false;

    @Override
    public void onCreate() {
        super.onCreate();
        if (DEBUG) Log.d(TAG, "Creating service");

        mSettingObserver = new SettingObserver(this);
        mScreenReceiver = new ScreenReceiver(this);
        mLightListener = new LightListener(this);

        mSettingObserver.enable();

        if (Utils.isAODEnabled(this)) {
            mScreenReceiver.enable();
            mExecutorService = Executors.newSingleThreadExecutor();
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (DEBUG) Log.d(TAG, "Destroying service");

        mSettingObserver.disable();
        mScreenReceiver.disable();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        if (DEBUG) Log.d(TAG, "Starting service");
        return START_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    void onSettingChange() {
        if (Utils.isAODEnabled(this)) {
            Log.d(TAG, "AOD enabled");
            mScreenReceiver.enable();
        } else {
            Log.d(TAG, "AOD disabled");
            mScreenReceiver.disable();
        }
    }

    void onDisplayOn() {
        Log.d(TAG, "Device interactive");
        mInteractive = true;
        mLightListener.disable();
        mHandler.removeCallbacksAndMessages(null);
    }

    void onDisplayOff() {
        Log.d(TAG, "Device non-interactive");
        mHandler.postDelayed(() -> {
            Log.d(TAG, "Trigger AOD");
            mInteractive = false;
            mExecutorService.execute(AODUpdater);
            if (getBrightnessMode(0) == 1) {
                mLightListener.enable();
            } else {
                Utils.boostAOD("0");
            }
        }, AOD_DELAY_MS);
    }

    void onChangedLuxState(boolean mBoostAOD) {
        if (!mInteractive && mOldBoostAOD != mBoostAOD) {
            Log.d(TAG, "Handle ambient lighting around the phone");
            if (mBoostAOD) {
                Utils.boostAOD("1");
            } else {
                Utils.boostAOD("0");
            }
            mOldBoostAOD = mBoostAOD;
        }
    }

    int getBrightnessMode(int defaultValue) {
        int brightnessMode = defaultValue;
        try {
            brightnessMode = Settings.System.getInt(getContentResolver(),
                    Settings.System.SCREEN_BRIGHTNESS_MODE);
        } catch (SettingNotFoundException snfe) {
        }
        return brightnessMode;
    }

    Runnable AODUpdater = new Runnable() {
        @Override
        public void run() {
            Log.d(TAG, "Trigger AOD");
            while (!mInteractive) {
                mHandler.postDelayed(() -> {
                    Utils.enterAOD();
                }, AOD_DELAY_MS);
                try {
                    Thread.sleep(7500);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    };

}

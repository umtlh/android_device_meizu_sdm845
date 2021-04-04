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
import android.util.Log;

public class AODService extends Service {

    private static final String TAG = "AODService";
    private static final boolean DEBUG = false;

    private static final long AOD_DELAY_MS = 1000;
    private static final long PULSE_RESTORE_DELAY_MS = 11000; // maximum pulse notification time 10s

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
        mInteractive = false;
        mHandler.postDelayed(() -> {
            if (!mInteractive) {
                Log.d(TAG, "Trigger AOD");
                mLightListener.enable();
                Utils.enterAOD();
            }
        }, AOD_DELAY_MS);
    }

    void onDozePulse() {
        Log.d(TAG, "Doze pulse state detected");
        mHandler.removeCallbacksAndMessages(null);
        mHandler.postDelayed(() -> {
            if (!mInteractive) {
                Log.d(TAG, "Trigger AOD");
                Utils.enterAOD();
            }
        }, PULSE_RESTORE_DELAY_MS);
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
}

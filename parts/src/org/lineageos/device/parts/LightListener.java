/*
 * Copyright (C) 2020 The MoKee Open Source Project
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

package org.lineageos.device.parts;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

class LightListener implements SensorEventListener {

    private static final String TAG = "LightListener";
    private static final boolean DEBUG = false;

    private static final long COMPARATIVE_LUX_VALUE = 375;
    private static final int SENSOR_DELAY_CUSTOM = 100000000; // 10s

    private final AODService mService;

    private final SensorManager mSensorManager;
    private final Sensor mSensor;

    private ExecutorService mExecutorService;

    private boolean mIsListening = false;
    private boolean mBoostAOD = false;

    LightListener(AODService service) {
        mService = service;
        mSensorManager = (SensorManager) service.getSystemService(Context.SENSOR_SERVICE);
        mSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_LIGHT, false);
        mExecutorService = Executors.newSingleThreadExecutor();
    }

    private Future<?> submit(Runnable runnable) {
        return mExecutorService.submit(runnable);
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        if (event.values[0] > COMPARATIVE_LUX_VALUE) {
            mBoostAOD = true;
        } else {
            mBoostAOD = false;
        }
        mService.onChangedLuxState(mBoostAOD);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
    }

    void enable() {
        if (mIsListening) return;
        if (DEBUG) Log.d(TAG, "Enabling");
        mIsListening = true;
        submit(() -> {
            mSensorManager.registerListener(this, mSensor, SENSOR_DELAY_CUSTOM);
        });
    }

    void disable() {
        if (!mIsListening) return;
        if (DEBUG) Log.d(TAG, "Disabling");
        mIsListening = false;
        submit(() -> {
            mSensorManager.unregisterListener(this, mSensor);
        });
    }

}

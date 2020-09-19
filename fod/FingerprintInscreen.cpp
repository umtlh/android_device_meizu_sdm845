/*
 * Copyright (C) 2019 The LineageOS Project
 * Copyright (C) 2020 The MoKee Open Source Project
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#define LOG_TAG "FingerprintInscreenService"

#include "FingerprintInscreen.h"
#include "StellerClientCallback.h"

#include <android-base/logging.h>
#include <cutils/properties.h>
#include <hidl/HidlTransportSupport.h>
#include <fstream>
#include <cmath>
#include <thread>

#define NOTIFY_FINGER_DETECTED 1
#define NOTIFY_FINGER_REMOVED 2

#define HBM_ENABLE_PATH "/sys/class/meizu/lcm/display/hbm"
#define BRIGHTNESS_PATH "/sys/class/backlight/panel0-backlight/brightness"
#define DC_LIGHT_PATH "sys/class/meizu/lcm/display/DC_Enable" // If Flyme 7.9.4.20A Daily only

#define FOD_POS_X 149 * 3
#define FOD_POS_Y 531 * 3
#define FOD_SIZE 62 * 3

#define HBM_OFF_DELAY 35
#define HBM_ON_DELAY 205

namespace vendor {
namespace lineage {
namespace biometrics {
namespace fingerprint {
namespace inscreen {
namespace V1_1 {
namespace implementation {

/*
 * Write value to path and close file.
 */
template <typename T>
static void set(const std::string& path, const T& value) {
    std::ofstream file(path);
    file << value;
}

template <typename T>
static T get(const std::string& path, const T& def) {
    std::ifstream file(path);
    T result;

    file >> result;
    return file.fail() ? def : result;
}

FingerprintInscreen::FingerprintInscreen()
    : mDC{0}
    , mHBM{0}
    , mHBMCheckOn{0}
    , mHBMCheckOff{0}
    , mFingerPressed{false}
    {
    mSteller = ISteller::getService();
    mStellerClientCallback = new StellerClientCallback();
}

Return<int32_t> FingerprintInscreen::getPositionX() {
    return FOD_POS_X;
}

Return<int32_t> FingerprintInscreen::getPositionY() {
    return FOD_POS_Y;
}

Return<int32_t> FingerprintInscreen::getSize() {
    return FOD_SIZE;
}

Return<void> FingerprintInscreen::onStartEnroll() {
    return Void();
}

Return<void> FingerprintInscreen::onFinishEnroll() {
    return Void();
}

Return<void> FingerprintInscreen::onPress() {
    mFingerPressed = true;
    mDC = get(DC_LIGHT_PATH, 0);
    set(DC_LIGHT_PATH, 0);
    mHBMCheckOn = get (HBM_ENABLE_PATH, 0);
    if (mHBMCheckOn == 0) {
        LOG(INFO) << "onPress: HBM was not enabled properly, enabling HBM!";
        set(HBM_ENABLE_PATH, 1);
        LOG(INFO) << "onPress: HBM enabled!";
    } else {
        LOG(INFO) << "onPress: HBM already enabled!";
    }
    std::thread([this]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(170));
        if (mFingerPressed) {
            notifyHal(NOTIFY_FINGER_DETECTED, 0);
        }
    }).detach();
    return Void();
}

Return<void> FingerprintInscreen::onRelease() {
    mFingerPressed = false;
    notifyHal(NOTIFY_FINGER_REMOVED, 0);
    set(DC_LIGHT_PATH, mDC);
    return Void();
}

Return<void> FingerprintInscreen::onShowFODView() {
    return Void();
}

Return<void> FingerprintInscreen::onHideFODView() {
    std::thread([this]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(47));
        mHBMCheckOff = get (HBM_ENABLE_PATH, 0);
        if (mHBMCheckOff != mHBM) {
            LOG(INFO) << "onHideFODView: restoring HBM!";
            set(HBM_ENABLE_PATH, mHBM);
            LOG(INFO) << "onHideFODView: HBM restored!";
        } else {
            LOG(INFO) << "onHideFODView: no need to restore HBM!";
        }
    }).detach();
    return Void();
}

Return<bool> FingerprintInscreen::handleAcquired(int32_t, int32_t) {
    return false;
}

Return<bool> FingerprintInscreen::handleError(int32_t, int32_t) {
    return false;
}

Return<void> FingerprintInscreen::setLongPressEnabled(bool) {
    return Void();
}

Return<int32_t> FingerprintInscreen::getDimAmount(int32_t) {
    int dimAmount;
    int brightness = get(BRIGHTNESS_PATH, 0);
    float alpha = 1.0 - pow(brightness / 1023.0f, 0.455);
    float min = (float) property_get_int32("fod.dimming.min", 0);
    float max = (float) property_get_int32("fod.dimming.max", 255);
    dimAmount = min + (max - min) * alpha;
    LOG(INFO) << "getDimAmount: dimAmount = " << dimAmount;
    return dimAmount;
}

Return<bool> FingerprintInscreen::shouldBoostBrightness() {
    return false;
}

Return<int32_t> FingerprintInscreen::getHbmOffDelay() {
    return HBM_OFF_DELAY;
}

Return<int32_t> FingerprintInscreen::getHbmOnDelay() {
    return HBM_ON_DELAY;
}

Return<bool> FingerprintInscreen::supportsAlwaysOnHBM() {
    return true;
}

Return<bool> FingerprintInscreen::noDim() {
    return false;
}

Return<void> FingerprintInscreen::switchHbm(bool enabled) {
    if (enabled) {
        mHBM = get(HBM_ENABLE_PATH, 0);
        set(HBM_ENABLE_PATH, 1);
        LOG(INFO) << "switchHbm: HBM enabled!";
    } else {
        set(HBM_ENABLE_PATH, 0);
        LOG(INFO) << "switchHbm: HBM disabled, waiting for restoring!";
    }
    return Void();
}

Return<void> FingerprintInscreen::setCallback(const sp<IFingerprintInscreenCallback>&) {
    return Void();
}

void FingerprintInscreen::notifyHal(int32_t status, int32_t data) {
    Return<void> ret = this->mSteller->notify(status, data, mStellerClientCallback);
    if (!ret.isOk()) {
        LOG(ERROR) << "notifyHal(" << status << ") error: " << ret.description();
    }
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace inscreen
}  // namespace fingerprint
}  // namespace biometrics
}  // namespace lineage
}  // namespace vendor

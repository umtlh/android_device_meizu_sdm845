/*
 * Copyright (C) 2019 The LineageOS Project
 * Copyright (C) 2020 The MoKee Open Source Project
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#define LOG_TAG "InscreenService"

#include "FingerprintInscreen.h"
#include "StellerClientCallback.h"
#include "KeyEventWatcher.h"

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include <android-base/logging.h>
#include <android-base/properties.h>
#include <cutils/properties.h>
#include <hardware_legacy/power.h>
#include <hidl/HidlTransportSupport.h>
#include <fstream>
#include <cmath>
#include <thread>

#define NOTIFY_FINGER_DETECTED 1
#define NOTIFY_FINGER_REMOVED 2

#define NOTIFY_HAL_DELAY 100

#define BOOST_ENABLE_PATH "/sys/class/meizu/fp/qos_set"
#define BRIGHTNESS_PATH "/sys/class/backlight/panel0-backlight/brightness"

#define FOD_POS_X 149 * 3
#define FOD_POS_Y 531 * 3
#define FOD_SIZE 62 * 3

#define TOUCHPANAL_DEV_PATH "/dev/input/event5"
#define KEY_FOD 0x0272

namespace vendor {
namespace lineage {
namespace biometrics {
namespace fingerprint {
namespace inscreen {
namespace V1_0 {
namespace implementation {

using android::base::GetProperty;

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

static KeyEventWatcher *keyEventWatcher;

static void sighandler(int) {
    LOG(INFO) << "Exiting";
    keyEventWatcher->exit();
}

int AlphaMap [] = {14, 16, 18, 19, 20, 21, 23, 24, 25, 27, 29, 29, 31, 32, 32, 33, 34, 35, 35, 36, 37, 38, 39, 40, 41, 42, 42, 43, 44, 44, 45, 45, 46, 47, 47, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 54, 54, 55, 55, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 65, 66, 66, 67, 67, 68, 68, 68, 69, 69, 70, 70, 71, 71, 71, 71, 72, 72, 72, 73, 73, 74, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 77, 78, 78, 78, 79, 79, 80, 80, 80, 80, 81, 81, 81, 81, 82, 82, 82, 83, 83, 83, 84, 84, 84, 84, 85, 85, 85, 85, 86, 86, 86, 87, 87, 87, 87, 87, 88, 88, 88, 88, 89, 89, 89, 90, 90, 90, 91, 91, 91, 91, 91, 92, 92, 92, 93, 93, 93, 93, 94, 94, 94, 94, 95, 95, 95, 95, 96, 96, 96, 97, 97, 97, 98, 98, 98, 98, 99, 99, 99, 99, 100, 100, 100, 100, 101, 101, 101, 101, 102, 102, 102, 103, 103, 103, 103, 104, 104, 104, 104, 105, 105, 105, 105, 106, 106, 106, 106, 106, 107, 107, 107, 107, 108, 108, 109, 109, 109, 109, 109, 109, 110, 110, 110, 110, 110, 111, 111, 111, 111, 111, 112, 112, 112, 112, 113, 113, 114, 114, 114, 114, 114, 114, 115, 115, 115, 115, 115, 115, 116, 116, 116, 116, 117, 117, 117, 117, 117, 117, 118, 118, 118, 119, 119, 119, 119, 119, 120, 120, 120, 120, 120, 120, 121, 121, 121, 121, 122, 122, 122, 122, 122, 122, 123, 123, 124, 124, 124, 124, 124, 124, 125, 125, 125, 125, 125, 125, 125, 126, 126, 126, 126, 126, 126, 126, 126, 126, 127, 127, 127, 127, 127, 127, 128, 128, 128, 128, 129, 129, 129, 129, 129, 129, 130, 130, 130, 130, 130, 130, 131, 131, 131, 131, 131, 131, 132, 132, 132, 132, 133, 133, 133, 133, 133, 133, 134, 134, 134, 134, 135, 135, 135, 135, 135, 135, 136, 136, 136, 136, 137, 137, 137, 137, 137, 137, 137, 138, 138, 138, 138, 138, 138, 138, 139, 139, 139, 139, 139, 139, 140, 140, 140, 140, 140, 140, 141, 141, 141, 141, 141, 141, 141, 142, 142, 142, 142, 142, 142, 143, 143, 143, 143, 143, 143, 143, 144, 144, 144, 144, 144, 144, 145, 145, 145, 145, 145, 145, 145, 145, 146, 146, 146, 146, 146, 146, 146, 146, 147, 147, 147, 147, 147, 147, 147, 147, 148, 148, 148, 148, 148, 148, 149, 149, 149, 149, 149, 149, 149, 149, 150, 150, 150, 150, 150, 150, 151, 151, 151, 151, 151, 151, 151, 151, 152, 152, 152, 152, 152, 152, 152, 153, 153, 153, 153, 153, 153, 154, 154, 154, 154, 154, 154, 154, 155, 155, 155, 155, 155, 155, 155, 155, 156, 156, 156, 156, 156, 156, 156, 156, 157, 157, 157, 157, 157, 157, 157, 157, 158, 158, 158, 158, 158, 158, 158, 158, 159, 159, 159, 159, 159, 159, 160, 160, 160, 160, 160, 160, 160, 160, 161, 161, 161, 161, 161, 161, 162, 162, 162, 162, 162, 162, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 164, 164, 164, 164, 164, 164, 165, 165, 165, 165, 165, 165, 165, 165, 166, 166, 166, 166, 166, 166, 166, 167, 167, 167, 167, 167, 167, 167, 168, 168, 168, 168, 168, 168, 168, 169, 169, 169, 169, 169, 169, 169, 169, 170, 170, 170, 170, 170, 171, 171, 171, 171, 171, 171, 171, 171, 171, 172, 172, 172, 172, 172, 172, 172, 173, 173, 173, 173, 173, 173, 174, 174, 174, 174, 174, 174, 174, 174, 174, 174, 174, 175, 175, 175, 175, 175, 176, 176, 176, 176, 176, 176, 176, 176, 177, 177, 177, 177, 177, 177, 177, 177, 177, 177, 178, 178, 178, 178, 178, 178, 178, 178, 179, 179, 179, 179, 179, 179, 179, 179, 180, 180, 180, 180, 180, 180, 180, 180, 181, 181, 181, 181, 181, 181, 181, 181, 182, 182, 182, 182, 182, 182, 182, 183, 183, 183, 183, 183, 183, 183, 183, 183, 184, 184, 184, 184, 184, 184, 185, 185, 185, 185, 185, 185, 185, 185, 185, 185, 186, 186, 186, 186, 186, 186, 186, 186, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 188, 188, 188, 188, 188, 188, 188, 188, 189, 189, 189, 189, 189, 189, 189, 189, 190, 190, 190, 190, 190, 190, 190, 190, 191, 191, 191, 191, 191, 191, 191, 191, 191, 192, 192, 192, 192, 192, 192, 192, 193, 193, 193, 193, 193, 193, 193, 193, 194, 194, 194, 194, 194, 194, 194, 194, 194, 194, 195, 195, 195, 195, 195, 195, 195, 195, 195, 195, 196, 196, 196, 196, 196, 196, 196, 196, 197, 197, 197, 197, 197, 197, 197, 197, 197, 197, 197, 198, 198, 198, 198, 198, 198, 199, 199, 199, 199, 199, 199, 199, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 202, 202, 202, 202, 202, 202, 202, 202, 203, 203, 203, 203, 203, 203, 203, 203, 203, 203, 204, 204, 204, 204, 204, 204, 204, 204, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 206, 206, 206, 206, 206, 206, 206, 206, 207, 207, 207, 207, 207, 207, 207, 207, 208, 208, 208, 208, 208, 208, 208, 208, 208, 208, 208, 209, 209, 209, 209, 209, 209, 209, 210, 210, 210, 210, 210, 210, 210, 210, 210, 210, 210, 211, 211, 211, 211, 211, 211, 211, 211, 211, 211, 212, 212, 212, 212, 212, 212, 212, 212, 213, 213, 213, 213, 213, 213, 213, 213, 213, 213, 213, 214, 214, 214, 214, 214, 214, 214, 214, 214, 214, 214, 215, 215, 215, 215, 215, 215, 215, 215, 215, 215, 216, 216, 216, 216, 216, 216, 216, 216, 216, 216, 216, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 218, 218, 218, 218, 218, 218, 218, 218, 218, 218};

FingerprintInscreen::FingerprintInscreen()
    : mFingerPressed{false}
    , mIconShown{false}
    {
    mSteller = ISteller::getService();
    mStellerClientCallback = new StellerClientCallback();

    keyEventWatcher = new KeyEventWatcher(TOUCHPANAL_DEV_PATH, [this](const std::string&, input_event evt) {
        if (evt.code == KEY_FOD) {
            notifyKeyEvent(evt.value);
        }
    });

    signal(SIGTERM, sighandler);
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
    acquire_wake_lock(PARTIAL_WAKE_LOCK, LOG_TAG);
    mFingerPressed = true;
    set(BOOST_ENABLE_PATH, 1);
    std::thread([this]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(NOTIFY_HAL_DELAY));
        if (mFingerPressed) {
            notifyHal(NOTIFY_FINGER_DETECTED, 0);
        }
    }).detach();
    return Void();
}

Return<void> FingerprintInscreen::onRelease() {
    mFingerPressed = false;
    notifyHal(NOTIFY_FINGER_REMOVED, 0);
    release_wake_lock(LOG_TAG);
    return Void();
}

Return<void> FingerprintInscreen::onShowFODView() {
    mIconShown = true;
    mFODModel = GetProperty("vendor.meizu.fp_vendor", "");
    LOG(INFO) << "mFODModel: " << mFODModel;
    return Void();
}

Return<void> FingerprintInscreen::onHideFODView() {
    mIconShown = false;
    return Void();
}

Return<bool> FingerprintInscreen::handleAcquired(int32_t acquiredInfo, int32_t vendorCode) {
    LOG(INFO) << "handleAcquired: acquiredInfo " << acquiredInfo << " vendorCode: " << vendorCode;
    return false;
}

Return<bool> FingerprintInscreen::handleError(int32_t error, int32_t vendorCode)  {
    LOG(ERROR) << "handleError: error " << error << " vendorCode: " << vendorCode;
    return false;
}

Return<void> FingerprintInscreen::setLongPressEnabled(bool) {
    return Void();
}

Return<int32_t> FingerprintInscreen::getDimAmount(int32_t) {
    int index = get(BRIGHTNESS_PATH, 0);
    LOG(INFO) << "getDimAmount: get brightness = " << index;
    if (index <= 0 || index > (sizeof ( AlphaMap ) / sizeof ( *AlphaMap ))) {
        return 0.0f;
    }
    LOG(INFO) << "getDimAmount: AlphaMap = " << AlphaMap[index - 1] << ", index = " << index;
    return (1.0f - ((float)AlphaMap[index - 1] / 255.0f)) * 255.0f;
}

Return<bool> FingerprintInscreen::shouldBoostBrightness() {
    return false;
}

Return<void> FingerprintInscreen::setCallback(const sp<IFingerprintInscreenCallback>& callback) {
    std::lock_guard<std::mutex> _lock(mCallbackLock);
    mCallback = callback;
    return Void();
}

void FingerprintInscreen::notifyHal(int32_t status, int32_t data) {
    Return<void> ret = this->mSteller->notify(status, data, mStellerClientCallback);
    if (!ret.isOk()) {
        LOG(ERROR) << "notifyHal(" << status << ") error: " << ret.description();
    }
}

void FingerprintInscreen::notifyKeyEvent(int value) {
    if (!mIconShown) {
        return;
    }

    LOG(INFO) << "notifyKeyEvent: " << value;

    std::lock_guard<std::mutex> _lock(mCallbackLock);
    if (mCallback == nullptr) {
        return;
    }

    if (value) {
        Return<void> ret = mCallback->onFingerDown();
        if (!ret.isOk()) {
            LOG(ERROR) << "FingerDown() error: " << ret.description();
        }
    } else {
        Return<void> ret = mCallback->onFingerUp();
        if (!ret.isOk()) {
            LOG(ERROR) << "FingerUp() error: " << ret.description();
        }
    }
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace inscreen
}  // namespace fingerprint
}  // namespace biometrics
}  // namespace lineage
}  // namespace vendor

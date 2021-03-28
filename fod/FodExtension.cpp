/*
 * Copyright (C) 2020 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <compositionengine/FodExtension.h>

#include <log/log.h>
#include <fstream>

#define HBM_ENABLE_PATH "/sys/class/meizu/lcm/display/hbm"

/*
 * Write value to path and close file.
 */
template <typename T>
static void set(const std::string& path, const T& value) {
    std::ofstream file(path);
    file << value;
}

uint32_t getFodZOrder(uint32_t z, bool touched) {
    if (touched) {
        set(HBM_ENABLE_PATH, 1);
        ALOGI("getFodZOrder: HBM is on!");
    } else {
        set(HBM_ENABLE_PATH, 0);
        ALOGI("getFodZOrder: HBM is off!");
    }

    return z;
}

uint64_t getFodUsageBits(uint64_t usageBits, bool touched) {
    (void) touched;
    return usageBits;
}

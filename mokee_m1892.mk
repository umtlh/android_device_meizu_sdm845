#
# Copyright (C) 2020 The MoKee Open Source Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/product_launched_with_o_mr1.mk)

# Inherit from m1892 device
$(call inherit-product, device/meizu/sdm845/m1892/m1892.mk)

# Inherit some common MoKee stuff.
$(call inherit-product, vendor/mokee/config/common_full_phone.mk)

# Credits to XiNGRZ

PRODUCT_PROPERTY_OVERRIDES += \
    ro.mk.maintainer=TRANSAERO21

PRODUCT_NAME := mokee_m1892
PRODUCT_BRAND := Meizu
PRODUCT_DEVICE := m1892
PRODUCT_MANUFACTURER := Meizu
PRODUCT_MODEL := 16th Plus

PRODUCT_GMS_CLIENTID_BASE := android-meizu

PRODUCT_BUILD_PROP_OVERRIDES += \
    TARGET_DEVICE="16thPlus" \
    PRODUCT_NAME="meizu_16thPlus_CN" \
    PRIVATE_BUILD_DESC="meizu_16thPlus_CN-user 10 QKQ1.191222.002 1595524937 release-keys"

BUILD_FINGERPRINT := meizu/qssi/qssi:10/QKQ1.191222.002/1595524937:user/release-keys

PRODUCT_PRODUCT_PROPERTIES += \
    fod.dimming.min=40 \
    fod.dimming.max=255

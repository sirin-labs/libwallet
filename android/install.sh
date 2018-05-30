#!/bin/bash

adb push obj/local/armeabi-v7a/libusb1.0.so /sdcard/
adb shell "cat > /system/lib/libusb1.0.so < /sdcard/libusb1.0.so"
adb shell rm /sdcard/libusb1.0.so

adb push obj/local/armeabi-v7a/libwallet.so /sdcard/
adb shell "cat > /system/lib/libwallet.so < /sdcard/libwallet.so"
adb shell rm /sdcard/libwallet.so

adb push obj/local/armeabi-v7a/libhidapi.so /sdcard/
adb shell "cat > /system/lib/libhidapi.so < /sdcard/libhidapi.so"
adb shell rm /sdcard/libhidapi.so

for B in setupdev wipedev getaddress signethtx getinfo
do
	adb push "obj/local/armeabi-v7a/$B" /sdcard/
	adb shell "cat > /system/bin/$B < /sdcard/$B"
	adb shell "chmod 0755 /system/bin/$B"
	adb shell rm "/sdcard/$B"
done

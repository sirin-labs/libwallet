#!/bin/sh
protoc --cpp_out=../libwallet/trezor/protob/ --proto_path ../libwallet/trezor/protob/ ../libwallet/trezor/protob/config.proto
protoc --cpp_out=../libwallet/trezor/protob/ --proto_path ../libwallet/trezor/protob/ ../libwallet/trezor/protob/messages.proto
protoc --cpp_out=../libwallet/trezor/protob/ --proto_path ../libwallet/trezor/protob/ ../libwallet/trezor/protob/storage.proto
protoc --cpp_out=../libwallet/trezor/protob/ --proto_path ../libwallet/trezor/protob/ ../libwallet/trezor/protob/types.proto

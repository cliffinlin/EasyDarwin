#!/bin/sh
sudo kill -9 $(pidof easydarwin)
cp -f ./x64/easydarwin ./easydarwin

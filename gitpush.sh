#!/bin/bash

echo "Attempting to synchronize repos";

cd d:ArdProjects/FingerprintSensorESP8266/lib/;
git pull --rebase;
git push;
cd d:ArdProjects/FingerprintSensor/lib/;
git pull --rebase;
git push;

#!/bin/bash

# This script will start the test driver and restart 
#  :if it exits with a code other than 0.
#
# To set it up to run on restart of the board:
#       crontab -e
# And add this line (inserting the right path to this script):
#       @reboot /static_test_driver/test_driver_manager.sh

until /static_test_driver/bin/sketch > ./log.txt; do
    echo "Static test driver crashed with exit code $?. Respawning..." >&2
    sleep 1
done

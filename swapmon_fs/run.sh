#!/bin/bash

# module parameters
SERVER_IP='10.1.10.138'
SERVER_PORT=10000

IB_IF=$1
[ -z "$IB_IF" ] && IB_IF="enp23s0f1np1"



CLIENT_IP=`ip -o -4 addr list ${IB_IF} | awk '{print $4}' | cut -d/ -f1`
# ip -o -4 addr list enp23s0f1np1 | awk '{print $4}' | cut -d/ -f1
if [ -z "$CLIENT_IP" ]; then
	echo "Could not extract IP address from interface ${IB_IF}"
	exit 1
fi


SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
pushd ${SCRIPT_DIR}

# check whether module is already inserted and remove it
MODULE_NAME=`modinfo -F name *.ko`
lsmod | awk '{print $1}' | grep ${MODULE_NAME} &> /dev/null
if [ $? -eq 0 ]; then
	# remove module if already inserted
	sudo rmmod ${MODULE_NAME}
fi

set -eux

# build module
make clean && make

# TODO(dimlek): what happens when multiple ko files exist?
MODULE_FILENAME=`modinfo -F filename *.ko`
# echo 'module mcswap +p' | sudo tee /sys/kernel/debug/dynamic_debug/control

# insert module
sudo insmod ${MODULE_FILENAME} cip=${CLIENT_IP} \
	endpoint=${SERVER_IP}:${SERVER_PORT} enable_async_mode=1 enable_poll_mode=1 # force
if [ $? -ne 0 ]; then
	echo "Error when inserting module, use 'dmesg' command for kernel logs"
fi

echo "${MODULE_NAME} module inserted successfully."

popd

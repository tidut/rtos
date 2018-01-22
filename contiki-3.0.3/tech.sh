#!/bin/bash

clear
echo "*****************************************************************************"
echo "*****************************************************************************"

echo "This script is used to Build the Conitki-Os version 3.0"
echo
echo $1
echo $2
echo

	pwd
	
if [ "$1" == 1 ]; then
	echo "1 for Board - Current Sensor"
	echo "Press Enter"
	echo

	read Number1

	cd ./platform/cc2538dk/dev/
	rm -rf def.h
	touch def.h
	echo '#define BOARD_1' >> def.h
	cd ../../../

	pwd
	cd ./examples/er-rest-example/
	make TARGET=cc2538dk clean
	make TARGET=cc2538dk 

	cd ../../

	pwd

	if [ -d "$Output" ]; then
		echo
		echo		
	else
		mkdir Output
		echo
		echo
	fi
	
	rm -rf ./Output/Board-Current-sensor*.hex
	rm -rf ./Output/Board-Current-sensor*.elf
	rm -rf ./Output/Board-Current-sensor*.bin

	cp -R ./examples/er-rest-example/er-example-server.hex Output/
	cp -R ./examples/er-rest-example/er-example-server.elf Output/
	cp -R ./examples/er-rest-example/er-example-server.bin Output/

	cd Output
	mv er-example-server.hex Board-Current-sensor_$2.hex
	mv er-example-server.elf Board-Current-sensor_$2.elf
	mv er-example-server.bin Board-Current-sensor_$2.bin
	cd ..
elif [ "$1" == 2 ]; then
	echo "2 for Board - Potentiometer Sensor"
	echo "Press Enter"
	echo
	read Number1
	cd platform/cc2538dk/dev/
	rm -rf def.h
	touch def.h
	echo '#define BOARD_2' >> def.h
	cd ../../../
	echo "Board 2 Selected"
	cd examples/er-rest-example/
	make TARGET=cc2538dk clean
	make TARGET=cc2538dk 

	cd ../../

	if [ -d "$Output" ]; then
		echo
		echo
	else
		mkdir Output
		echo
		echo
	fi
	
	rm -rf Output/Board-Pot-sensor*.hex
	rm -rf Output/Board-Pot-sensor*.elf
	rm -rf Output/Board-Pot-sensor*.bin

	cp -R examples/er-rest-example/er-example-server.hex Output/
	cp -R examples/er-rest-example/er-example-server.elf Output/
	cp -R examples/er-rest-example/er-example-server.bin Output/

	cd Output
	mv er-example-server.hex Board-Pot-sensor_$2.hex
	mv er-example-server.elf Board-Pot-sensor_$2.elf
	mv er-example-server.bin Board-Pot-sensor_$2.bin
	cd ..
elif [ "$1" == 3 ]; then
	echo "3 for Board - Energy Sensor"
	echo "Press Enter"
	echo
	read Number1
	echo "Board 3 Selected"
	cd platform/cc2538dk/dev/
	rm -rf def.h
	touch def.h
	echo '#define BOARD_3' >> def.h
	cd ../../../
	cd examples/er-rest-example/
	make TARGET=cc2538dk clean
	make TARGET=cc2538dk 

	cd ../../

	if [ -d "$Output" ]; then
		echo
		echo	
	else
		mkdir Output
		echo
		echo
	fi	
	
	rm -rf Output/Board-Energy-sensor*.hex
	rm -rf Output/Board-Energy-sensor*.elf
	rm -rf Output/Board-Energy-sensor*.bin

	cp -R examples/er-rest-example/er-example-server.hex Output/
	cp -R examples/er-rest-example/er-example-server.elf Output/
	cp -R examples/er-rest-example/er-example-server.bin Output/

	cd Output
	mv er-example-server.hex Board-Energy-sensor_$2.hex
	mv er-example-server.elf Board-Energy-sensor_$2.elf
	mv er-example-server.bin Board-Energy-sensor_$2.bin
	cd ..
	
elif [ "$1" == 4 ]; then
	echo "4 for Board - Air Sensor"
	echo "Press Enter"
	echo
	read Number1
	echo "Board 4 Selected"
	cd platform/cc2538dk/dev/
	rm -rf def.h
	touch def.h
	echo '#define BOARD_4' >> def.h
	cd ../../../
	cd examples/er-rest-example/
	make TARGET=cc2538dk clean
	make TARGET=cc2538dk 

	cd ../../

	if [ -d "$Output" ]; then
		echo
		echo
	
	else
		mkdir Output
		echo
		echo
	fi	
	
	rm -rf Output/Board-Air-sensor*.hex
	rm -rf Output/Board-Air-sensor*.elf
	rm -rf Output/Board-Air-sensor*.bin

	cp -R examples/er-rest-example/er-example-server.hex Output/
	cp -R examples/er-rest-example/er-example-server.elf Output/
	cp -R examples/er-rest-example/er-example-server.bin Output/

	cd Output
	mv er-example-server.hex Board-Air-sensor_$2.hex
	mv er-example-server.elf Board-Air-sensor_$2.elf
	mv er-example-server.bin Board-Air-sensor_$2.bin
	cd ..

elif [ "$1" == 5 ]; then
	echo "5 for Board - Relay Sensor"
	echo "Press Enter"
	echo
	read Number1
	echo "Board 5 Selected"
	cd platform/cc2538dk/dev/
	rm -rf def.h
	touch def.h
	echo '#define BOARD_5' >> def.h
	cd ../../../
	cd examples/er-rest-example/
	make TARGET=cc2538dk clean
	make TARGET=cc2538dk 

	cd ../../

	if [ -d "$Output" ]; then
		echo
		echo
	
	else
		mkdir Output
		echo
		echo
	fi	
	
	rm -rf Output/Board-Relay-sensor*.hex
	rm -rf Output/Board-Relay-sensor*.elf
	rm -rf Output/Board-Relay-sensor*.bin

	echo "Files removed"
	cp -R examples/er-rest-example/er-example-server.hex Output/
	cp -R examples/er-rest-example/er-example-server.elf Output/
	cp -R examples/er-rest-example/er-example-server.bin Output/

	echo "Files copied"
	
	cd Output
	mv er-example-server.hex Board-Relay-sensor_$2.hex
	mv er-example-server.elf Board-Relay-sensor_$2.elf
	mv er-example-server.bin Board-Relay-sensor_$2.bin
	echo "Files renamed"
	cd ..

elif [ "$1" == 6 ]; then
	echo "6 for Board - Thermocouple Sensor"
	echo "Press Enter"
	echo
	read Number1
	echo "Board 6 Selected"
	cd platform/cc2538dk/dev/
	rm -rf def.h
	touch def.h
	echo '#define BOARD_6' >> def.h
	cd ../../../
	cd examples/er-rest-example/
	make TARGET=cc2538dk clean
	make TARGET=cc2538dk 

	cd ../../

	if [ -d "$Output" ]; then
		echo
		echo
	
	else
		mkdir Output
		echo
		echo
	fi	
	
	rm -rf Output/Board-Thermo-sensor*.hex
	rm -rf Output/Board-Thermo-sensor*.elf
	rm -rf Output/Board-Thermo-sensor*.bin

	cp -R examples/er-rest-example/er-example-server.hex Output/
	cp -R examples/er-rest-example/er-example-server.elf Output/
	cp -R examples/er-rest-example/er-example-server.bin Output/

	cd Output
	mv er-example-server.hex Board-Thermo-sensor_$2.hex
	mv er-example-server.elf Board-Thermo-sensor_$2.elf
	mv er-example-server.bin Board-Thermo-sensor_$2.bin	
	cd ..
else
	echo
	echo
	echo "Sorry you have not selected the Board"
fi

echo
echo
echo "Bye"
echo

export -n BOARD

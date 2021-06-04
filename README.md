# Mixer stand for ML-experiments #

## Commands: ##
Dump Serial data to file:

``(stty raw; cat > received.txt) < /dev/ttyUSB0``

## Dependencies: ## 
* ``pio lib install "miq19/eModbus"``
* ``pio lib install "gyverlibs/TimerMs"``
* ``pio lib install "milesburton/DallasTemperature"``
* ``pio lib install "paulstoffregen/OneWire"``
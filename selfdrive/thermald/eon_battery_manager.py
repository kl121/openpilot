from selfdrive.thermald.power_monitoring import VBATT_PAUSE_CHARGING
from selfdrive.hardware import HARDWARE,EON
BATT_PERC_MIN = 30
BATT_PERC_MAX = 65

def setEONChargingStatus(car_voltage_mV, batteryPercent) :
    if EON:
        print("log purpose : entering setEONChargingStatus ")
        if HARDWARE.get_battery_charging() :
            print("log purpose : HARDWARE.get_battery_charging()  True ")
            if batteryPercent > BATT_PERC_MAX or car_voltage_mV  < VBATT_PAUSE_CHARGING * 1e3 :
                print("log purpose : HARDWARE.set_battery_charging(False)  False ")
                HARDWARE.set_battery_charging(False)
        else :
            print("log purpose : HARDWARE.get_battery_charging()  False ")
            if batteryPercent < BATT_PERC_MIN and car_voltage_mV  > VBATT_PAUSE_CHARGING * 1e3:
                print("log purpose : HARDWARE.set_battery_charging(True)  True ")
                HARDWARE.set_battery_charging(True)

from selfdrive.thermald.power_monitoring import VBATT_PAUSE_CHARGING
from selfdrive.hardware import HARDWARE,EON
BATT_PERC_MIN = 30
BATT_PERC_MAX = 65

def setEONChargingStatus(car_voltage_mV, batteryPercent) :
    if EON:
        if HARDWARE.get_battery_charging :
            if batteryPercent > BATT_PERC_MAX or car_voltage_mV  < VBATT_PAUSE_CHARGING * 1e3 :
                HARDWARE.set_battery_charging(False)
        else :
            if batteryPercent < BATT_PERC_MIN and car_voltage_mV  > VBATT_PAUSE_CHARGING * 1e3:
                HARDWARE.set_battery_charging(True)

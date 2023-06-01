#ifndef __sensordatacollector_H__
#define __sensordatacollector_H__

#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>

#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>

#include <sensor.h>
#include <bluetooth.h>
#include <time.h>

#include<device/power.h>
#include <feedback.h>

#include "manage_write.h"
#include <sensor/_create.h>
#include <sensor/hrm_listener.h>
#include <sensor/hrm_green_listener.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "sensordatacollector"
#define HRM_SENSOR_LOG_TAG "HRM_SENSOR_EVENT"
#define HRM_GREEN_SENSOR_LOG_TAG "HRM_GREEN_SENSOR_EVENT"
#define MEDIA_LOG_TAG "MEDIA_STORAGE_EVENT"

#if !defined(PACKAGE)
#define PACKAGE "org.example.sensordatacollector"
#endif

#endif /* __sensordatacollector_H__ */

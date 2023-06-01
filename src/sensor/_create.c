#include "sensordatacollector.h"
#include <sensor.h>

bool check_sensor_supported(sensor_type_e type, char* log) {
	int retval;
	bool sensor_supported = false;
	retval = sensor_is_supported(type, &sensor_supported);
	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, log,
				"Function sensor_is_supported() return value = %s",
				get_error_message(retval));
		dlog_print(DLOG_ERROR, log,
				"Failed to checks whether a HRM sensor is supported in the current device.");
		return false;
	} else {
		dlog_print(DLOG_INFO, log,
				"Succeeded in checking whether a %s sensor is supported in the current device.",
				log);
	}
	if (!sensor_supported) {
		dlog_print(DLOG_ERROR, log,
				"Function sensor_is_supported() output supported = %d",
				sensor_supported);
		return false;
	}
	return true;
}
bool init_sensor(sensor_type_e type, sensor_h sensor, char* log) {
	int retval;
	retval = sensor_get_default_sensor(type, &sensor);
	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_INFO, log,
				"Function sensor_get_default_sensor() return value =%s",
				get_error_message(retval));
		return false;
	} else
		dlog_print(DLOG_INFO, log, "succeeded to get [%s] default sensor", log);
	return true;
}



bool init_handle(bool init, bool create, bool start, char* log,
bool health_usable) {
	if (!init) {
		dlog_print(DLOG_ERROR, log,
				"Failed to get the handle for the default sensor of a[%s] sensor.",
				log);
		health_usable = false;
	} else
		dlog_print(DLOG_INFO, log,
				"Succeeded in getting the handle for the default sensor of a [%s] sensor.",
				log);
	if (!create) {
		dlog_print(DLOG_ERROR, log, "Failed to create a [%s] sensor listener.",
				log);
		health_usable = false;
	} else
		dlog_print(DLOG_INFO, log,
				"Succeeded in creating a %s sensor listener.", log);

	if (!start) {
		dlog_print(DLOG_ERROR, log,
				"Failed to start observing the sensor events regarding a [%s] sensor listener.",
				log);
	} else
		dlog_print(DLOG_INFO, log,
				"Succeeded in starting observing the sensor events regarding a [%s] sensor listener.",
				log);
	return health_usable;
}
//bool init_handle(bool init, sensor_type_e type) {
//	switch (type) {
//	case SENSOR_HRM:
//		if (!initialize_hrm_sensor()) {
//			dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
//					"Failed to get the handle for the default sensor of a HRM sensor.");
//			health_usable = false;
//		} else
//			dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
//					"Succeeded in getting the handle for the default sensor of a HRM sensor.");
//
//		if (!create_hrm_sensor_listener(hrm_sensor_handle)) {
//			dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
//					"Failed to create a HRM sensor listener.");
//			health_usable = false;
//		} else
//			dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
//					"Succeeded in creating a HRM sensor listener.");
//
//		if (!start_hrm_sensor_listener())
//			dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
//					"Failed to start observing the sensor events regarding a HRM sensor listener.");
//		else
//			dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
//					"Succeeded in starting observing the sensor events regarding a HRM sensor listener.");
//	}
//}

bool case_DENY(char* log, bool health_usable) {
	dlog_print(DLOG_INFO, log,
			"Function ppm_check_permission() output result = PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_DENY");
	dlog_print(DLOG_ERROR, log,
			"The application doesn't have permission to use a sensor privilege.");
	health_usable = false;
	return health_usable;
}
bool case_ASK(char*log, bool permission) {

	bool helath_usable;
	dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
			"The user has to be asked whether to grant permission to use a sensor privilege.");
	if (!permission) {
		dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
				"Failed to request a user's response to obtain permission for using the sensor privilege.");
		helath_usable = false;
	} else {
		dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
				"Succeeded in requesting a user's response to obtain permission for using the sensor privilege.");
		helath_usable = true;
	}
	return helath_usable;
}

bool request_cb_handle(bool init, bool create, bool start, char*log) {
	dlog_print(DLOG_INFO, log,
			"The user granted permission to use a sensor privilege for an indefinite period of time.");
	if (!init) {
		dlog_print(DLOG_ERROR, log,
				"Failed to get the handle for the default sensor of a [%s] sensor.",
				log);
		ui_app_exit();
	} else
		dlog_print(DLOG_INFO, log,
				"Succeeded in getting the handle for the default sensor of a [%s] sensor.",
				log);
	if (!create) {
		dlog_print(DLOG_ERROR, log, "Failed to create a [%s] sensor listener.",
				log);
		ui_app_exit();
	} else
		dlog_print(DLOG_INFO, log,
				"Succeeded in creating a [%s] sensor listener.", log);
	if (!start) {
		dlog_print(DLOG_ERROR, log,
				"Failed to start observing the sensor events regarding a [%s] sensor listener.",
				log);
		ui_app_exit();
	} else
		dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
				"Succeeded in starting observing the sensor events regarding a [%s] sensor listener.",
				log);

}

bool request_cb_DENY_FOREVER_handle(char* log) {
	dlog_print(DLOG_INFO, log,
			"Function request_sensor_permission_response_callback() output result = PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_FOREVER");
	dlog_print(DLOG_ERROR, log,
			"The user denied granting permission to use a sensor privilege for an indefinite period of time.");
	ui_app_exit();
}
bool request_cb_DENY_ONCE_handle(char* log) {
	dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
			"Function request_sensor_permission_response_callback() output result = PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_ONCE");
	dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
			"The user denied granting permission to use a sensor privilege once.");
	ui_app_exit();
}


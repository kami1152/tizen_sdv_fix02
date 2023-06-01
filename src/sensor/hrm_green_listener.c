#include "sensordatacollector.h"
#include <sensor/hrm_green_listener.h>

sensor_listener_h hrm_green_sensor_listener_handle = 0;

unsigned int hrm_green_sensor_listener_event_update_interval_ms = 3000;

void hrm_green_sensor_listener_event_callback(sensor_h sensor,
		sensor_event_s events[], void*user_data);

bool create_hrm_green_sensor_listener(sensor_h hrm_green_sensor_handle) {
	int retval;
	retval = sensor_create_listener(hrm_green_sensor_handle,
			&hrm_green_sensor_listener_handle);
	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, HRM_GREEN_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_create_listener() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	if (!set_hrm_green_sensor_listener_attribute()) {
		dlog_print(DLOG_ERROR, HRM_GREEN_SENSOR_LOG_TAG,
				"%s/%s/%d: Failed to set an attribute to control the behavior of a HRM Green sensor listener.",
				__FILE__, __func__, __LINE__);
		return false;
	} else
		dlog_print(DLOG_INFO, HRM_GREEN_SENSOR_LOG_TAG,
				"%s/%s/%d: Succeeded in setting an attribute to control the behavior of a HRM Green sensor listener.",
				__FILE__, __func__, __LINE__);
	if (!set_hrm_sensor_listner_event_callback()) {
		dlog_print(DLOG_ERROR, HRM_GREEN_SENSOR_LOG_TAG,
				"%s/%s/%d: Failed to register the callback function to be invoked when sensor events are delivered via a HRM green sensor listener.",
				__FILE__, __func__, __LINE__);
		return false;
	} else
		dlog_print(DLOG_INFO, HRM_GREEN_SENSOR_LOG_TAG,
				"%s/%s/%d: Succeeded in registering the callback function to be invoked when sensor events are delivered via a HRM sensor listener.",
				__FILE__, __func__, __LINE__);

	return true;
}

bool set_hrm_green_sensor_listener_attribute() {
	int retval;
	retval = sensor_listener_set_attribute_int(hrm_green_sensor_listener_handle,
			SENSOR_ATTRIBUTE_PAUSE_POLICY, SENSOR_PAUSE_NONE);
	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, HRM_GREEN_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_set_attribute_int() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	return true;
}

bool set_hrm_sensor_listner_event_callback() {
	int retval;
	retval = sensor_listener_set_event_cb(hrm_green_sensor_listener_handle,
			hrm_green_sensor_listener_handle,
			hrm_green_sensor_listener_event_callback, NULL);
	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, HRM_GREEN_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_set_event_cb() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	return true;
}
void hrm_green_sensor_listener_event_callback(sensor_h sensor,
		sensor_event_s events[], void*user_data) {
	int value = (int) events[0].values[0];
	char buf[512];
	snprintf(buf,512,"hrm_green_value : %d\n",value);
	write_date(buf,1);
	dlog_print(DLOG_INFO, HRM_GREEN_SENSOR_LOG_TAG,
			"%s/%s/%d: Function sensor_events_callback() output value = %d",
			__FILE__, __func__, __LINE__, value);
}

bool start_hrm_green_sensor_listener(sensor_h sensor, sensor_event_s *event,
		void*user_data) {
	sensor_type_e type;
	int retval;
	retval = sensor_listener_start(hrm_green_sensor_listener_handle);
	dlog_print(DLOG_INFO, HRM_GREEN_SENSOR_LOG_TAG, "%i : %s : %i", retval,
			get_error_message(retval), hrm_green_sensor_listener_handle);
	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, HRM_GREEN_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_start() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	return true;
}
bool stop_hrm_green_sensor_listener() {
	int retval;
	retval = sensor_listener_stop(hrm_green_sensor_listener_handle);
	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_DEBUG,HRM_GREEN_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_stop() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	return true;

}
bool destroy_hrm_green_sensor_listener() {
	int retval;
	retval = sensor_destroy_listener(hrm_green_sensor_listener_handle);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, HRM_GREEN_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_destroy_listener() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	} else {
		hrm_green_sensor_listener_handle = 0;
	}
	return true;
}
bool check_hrm_green_sensor_listener_is_created() {
	if (hrm_green_sensor_listener_handle == 0)
		return false;
	return true;
}

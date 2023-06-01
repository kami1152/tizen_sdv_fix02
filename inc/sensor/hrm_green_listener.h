
#include <sensordatacollector.h>

bool create_hrm_green_sensor_listener(sensor_h hrm_green_sensor_handle);
bool set_hrm_green_sensor_listener_attribute();
bool set_hrm_sensor_listner_event_callback();
void hrm_green_sensor_listener_event_callback(sensor_h sensor,
		sensor_event_s events[], void*user_data);
bool start_hrm_green_sensor_listener();
bool stop_hrm_green_sensor_listener();
bool destroy_hrm_green_sensor_listener();
bool check_hrm_green_sensor_listener_is_created();

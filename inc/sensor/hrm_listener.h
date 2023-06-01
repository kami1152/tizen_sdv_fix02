#ifndef SENSOR_HRM_LISTENER_H_
#define SENSOR_HRM_LISTENER_H_

#include <sensordatacollector.h>

bool create_hrm_sensor_listener(sensor_h handle);
bool set_hrm_sensor_listener_attribute();
bool set_hrm_sensor_listener_event_callback();

bool start_hrm_sensor_listener();
bool stop_hrm_sensor_listener();
bool destroy_hrm_sensor_listener();
bool check_hrm_sensor_listener_is_created();
void on_sensor_event(sensor_h sensor, sensor_event_s *event, void *user_data);
#endif /* SENSOR_HRM_LISTENER_H_ */

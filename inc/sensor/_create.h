#include "sensordatacollector.h"

bool check_sensor_supported(sensor_type_e type, char* log);
bool init_sensor(sensor_type_e type, sensor_h sensor, char* log);
bool case_ASK(char*log, bool permission);
bool case_DENY(char* log, bool health_usable);
bool init_handle(bool init, bool create, bool start, char* log,
		bool health_usable);
bool request_cb_handle(bool init, bool create, bool start, char*log);
bool request_cb_DENY_FOREVER_handle(char* log);

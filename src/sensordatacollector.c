#include "sensordatacollector.h"
#include <privacy_privilege_manager.h>

#include <sensor/hrm_listener.h>
#include <sensor/_create.h>
#include <sensor/hrm_green_listener.h>
#include <app_common.h>

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;

	Evas_Object *screen;
	Evas_Object *grid_main;
	Evas_Object *grid_cover;

	Evas_Object *btn_unlock;

	Evas_Object *btn_level_0;
	Evas_Object *btn_level_1;
	Evas_Object *btn_level_2;
	Evas_Object *btn_level_3;
	Evas_Object *btn_level_0_text;
	Evas_Object *btn_level_1_text;
	Evas_Object *btn_level_2_text;
	Evas_Object *btn_level_3_text;

	Evas_Object *report_screen;
	Evas_Object *report_screen_bg;
	Evas_Object *report_screen_text;

	Evas_Object *bg;

	Evas_Object *nv;
	Elm_Object_Item *nv_it;
} appdata_s;

static void clicked_level_0(void *user_data, Evas* e, Evas_Object *obj,
		void *event_info);
static void clicked_level_1(void *user_data, Evas* e, Evas_Object *obj,
		void *event_info);
static void clicked_level_2(void *user_data, Evas* e, Evas_Object *obj,
		void *event_info);
static void clicked_level_3(void *user_data, Evas* e, Evas_Object *obj,
		void *event_info);
static void clicked_unlock(void *user_data, Evas* e, Evas_Object *obj,
		void *event_info);

static void clicked_up_level_0(void *user_data, Evas* e, Evas_Object *obj,
		void *event_info);
static void clicked_up_level_1(void *user_data, Evas* e, Evas_Object *obj,
		void *event_info);
static void clicked_up_level_2(void *user_data, Evas* e, Evas_Object *obj,
		void *event_info);
static void clicked_up_level_3(void *user_data, Evas* e, Evas_Object *obj,
		void *event_info);

static Eina_Bool click_animation_level_0(void *data);
static Eina_Bool click_animation_level_1(void *data);
static Eina_Bool click_animation_level_2(void *data);
static Eina_Bool click_animation_level_3(void *data);
static Eina_Bool click_up_animation_level_0(void *data);
static Eina_Bool click_up_animation_level_1(void *data);
static Eina_Bool click_up_animation_level_2(void *data);
static Eina_Bool click_up_animation_level_3(void *data);

//static Eina_Bool report_animation(void *data, double pos);
static void report_animation(void *data, Ecore_Thread *thread);
static Ecore_Thread *report_thread;

static double report_animation_speed = 0.3;
static double report_animation_showing_time = 2;
static void* report_draw(void *data);
static int report_color[] = { 0, 0, 0, 255 };

static void _set_cover_lock(void *data, Ecore_Thread *thread, void *msg_data);
static void _encore_thread_cover_lock(void *data, Ecore_Thread *thread);
static void _end_cover_lock_thread(void *data, Ecore_Thread *thread);

static int cover_lock_counter = 0;
static int cover_lock_parameter = 10;

// 1 is pressed, 0 is detached;
static int level_0_state = 0;
static int level_1_state = 0;
static int level_2_state = 0;
static int level_3_state = 0;

static int long_press_parameter = 2;
static int level_0_pressed_time = 0;
static int level_1_pressed_time = 0;
static int level_2_pressed_time = 0;
static int level_3_pressed_time = 0;

static void _encore_thread_check_long_press(void *data, Ecore_Thread *thread);

static int color_level_0[] = { 244, 244, 244, 255 };
static int color_level_1[] = { 112, 173, 70, 255 };
static int color_level_2[] = { 0, 113, 192, 255 };
static int color_level_3[] = { 254, 0, 0, 255 };

static int hover_color_level_0[] = { 210, 210, 210, 255 };
static int hover_color_level_1[] = { 50, 118, 21, 255 };
static int hover_color_level_2[] = { 0, 50, 144, 255 };
static int hover_color_level_3[] = { 187, 0, 0, 255 };

sensor_type_e sensor_type = SENSOR_HRM;

sensor_h hrm_sensor_handle = 0;
sensor_h hrm_green_sensor_handle = 0;

bool check_hrm_sensor_is_supported();
bool initialize_device_sensor();

const char *sensor_privilege = "http://tizen.org/privilege/healthinfo";
const char *mediastorage_privilege = "http://tizen.org/privilege/mediastorage";

bool check_and_request_sensor_permission();
bool request_sensor_permission();
void request_health_sensor_permission_response_callback(ppm_call_cause_e cause,
		ppm_request_result_e result, const char *privilege, void *user_data);
void request_media_permission_response_callback(ppm_call_cause_e cause,
		ppm_request_result_e result, const char *privilege, void *user_data);

static void win_delete_request_cb(void *data, Evas_Object *obj,
		void *event_info) {
	ui_app_exit();
}

static void win_back_cb(void *data, Evas_Object *obj, void *event_info) {
	appdata_s *ad = data;
	/* Let window go to hide state. */
	elm_win_lower(ad->win);
}

static void create_base_gui(appdata_s *ad) {
	/* Window */
	/* Create and initialize elm_win.
	 elm_win is mandatory to manipulate window. */
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win,
				(const int *) (&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request",
			win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb,
			ad);

	/* Conformant */
	/* Create and initialize elm_conformant.
	 elm_conformant is mandatory for base gui to have proper size
	 when indicator or virtual keypad is visible. */
	ad->conform = elm_conformant_add(ad->win);
	elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND,
	EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	ad->bg = elm_bg_add(ad->conform);
	elm_bg_color_set(ad->bg, 255, 255, 255);
	elm_object_content_set(ad->conform, ad->bg);

	ad->screen = elm_grid_add(ad->conform);
	elm_object_content_set(ad->conform, ad->screen);
	evas_object_show(ad->screen);

	Evas_Object *screen_bg = elm_bg_add(ad->screen);
	elm_bg_color_set(screen_bg, 255, 255, 255);
	elm_grid_pack(ad->screen, screen_bg, 0, 0, 100, 100);
	evas_object_show(screen_bg);

	ad->grid_main = elm_grid_add(ad->screen);
	elm_grid_pack(ad->screen, ad->grid_main, 0, 0, 100, 100);
	evas_object_show(ad->grid_main);

	ad->btn_level_0 = evas_object_rectangle_add(ad->grid_main);
	evas_object_color_set(ad->btn_level_0, color_level_0[0], color_level_0[1],
			color_level_0[2], color_level_0[3]);
	elm_grid_pack(ad->grid_main, ad->btn_level_0, 0, 0, 50, 50);
	evas_object_show(ad->btn_level_0);
	ad->btn_level_0_text = elm_label_add(ad->grid_main);
	evas_object_color_set(ad->btn_level_0_text, 0, 0, 0, 255);
	elm_object_text_set(ad->btn_level_0_text,
			"<align=center><font_size=30><color=#000000FF><b>없음</b></color></font></align>");

	elm_grid_pack(ad->grid_main, ad->btn_level_0_text, 0, 25, 50, 10);
	evas_object_show(ad->btn_level_0_text);
	evas_object_event_callback_add(ad->btn_level_0, EVAS_CALLBACK_MOUSE_DOWN,
			clicked_level_0, ad);
	evas_object_event_callback_add(ad->btn_level_0, EVAS_CALLBACK_MOUSE_UP,
			clicked_up_level_0, ad);
	evas_object_event_callback_add(ad->btn_level_0_text,
			EVAS_CALLBACK_MOUSE_DOWN, clicked_level_0, ad);
	evas_object_event_callback_add(ad->btn_level_0_text, EVAS_CALLBACK_MOUSE_UP,
			clicked_up_level_0, ad);

	ad->btn_level_1 = evas_object_rectangle_add(ad->grid_main);
	evas_object_color_set(ad->btn_level_1, color_level_1[0], color_level_1[1],
			color_level_1[2], color_level_1[3]);
	elm_grid_pack(ad->grid_main, ad->btn_level_1, 50, 0, 50, 50);
	evas_object_show(ad->btn_level_1);
	ad->btn_level_1_text = elm_label_add(ad->grid_main);
	evas_object_color_set(ad->btn_level_1_text, 255, 255, 255, 255);
	elm_object_text_set(ad->btn_level_1_text,
			"<align=center><font_size=30><b>경증</b></font></align>");
	elm_grid_pack(ad->grid_main, ad->btn_level_1_text, 50, 25, 50, 10);
	evas_object_show(ad->btn_level_1_text);
	evas_object_event_callback_add(ad->btn_level_1, EVAS_CALLBACK_MOUSE_DOWN,
			clicked_level_1, ad);
	evas_object_event_callback_add(ad->btn_level_1, EVAS_CALLBACK_MOUSE_UP,
			clicked_up_level_1, ad);
	evas_object_event_callback_add(ad->btn_level_1_text,
			EVAS_CALLBACK_MOUSE_DOWN, clicked_level_1, ad);
	evas_object_event_callback_add(ad->btn_level_1_text, EVAS_CALLBACK_MOUSE_UP,
			clicked_up_level_1, ad);

	ad->btn_level_2 = evas_object_rectangle_add(ad->grid_main);
	evas_object_color_set(ad->btn_level_2, color_level_2[0], color_level_2[1],
			color_level_2[2], color_level_2[3]);
	elm_grid_pack(ad->grid_main, ad->btn_level_2, 0, 50, 50, 50);
	evas_object_show(ad->btn_level_2);
	ad->btn_level_2_text = elm_label_add(ad->grid_main);
	evas_object_color_set(ad->btn_level_2_text, 255, 255, 255, 255);
	elm_object_text_set(ad->btn_level_2_text,
			"<align=center><font_size=30><b>심함</b></font></align>");
	elm_grid_pack(ad->grid_main, ad->btn_level_2_text, 0, 65, 50, 10);
	evas_object_show(ad->btn_level_2_text);
	evas_object_event_callback_add(ad->btn_level_2, EVAS_CALLBACK_MOUSE_DOWN,
			clicked_level_2, ad);
	evas_object_event_callback_add(ad->btn_level_2, EVAS_CALLBACK_MOUSE_UP,
			clicked_up_level_2, ad);
	evas_object_event_callback_add(ad->btn_level_2_text,
			EVAS_CALLBACK_MOUSE_DOWN, clicked_level_2, ad);
	evas_object_event_callback_add(ad->btn_level_2_text, EVAS_CALLBACK_MOUSE_UP,
			clicked_up_level_2, ad);

	ad->btn_level_3 = evas_object_rectangle_add(ad->grid_main);
	evas_object_color_set(ad->btn_level_3, color_level_3[0], color_level_3[1],
			color_level_3[2], color_level_3[3]);
	elm_grid_pack(ad->grid_main, ad->btn_level_3, 50, 50, 50, 50);
	evas_object_show(ad->btn_level_3);
	ad->btn_level_3_text = elm_label_add(ad->grid_main);
	evas_object_color_set(ad->btn_level_3_text, 255, 255, 255, 255);
	elm_object_text_set(ad->btn_level_3_text,
			"<align=center><font_size=30><b>최악</b></font></align>");
	elm_grid_pack(ad->grid_main, ad->btn_level_3_text, 50, 65, 50, 10);
	evas_object_show(ad->btn_level_3_text);
	evas_object_event_callback_add(ad->btn_level_3, EVAS_CALLBACK_MOUSE_DOWN,
			clicked_level_3, ad);
	evas_object_event_callback_add(ad->btn_level_3, EVAS_CALLBACK_MOUSE_UP,
			clicked_up_level_3, ad);
	evas_object_event_callback_add(ad->btn_level_3_text,
			EVAS_CALLBACK_MOUSE_DOWN, clicked_level_3, ad);
	evas_object_event_callback_add(ad->btn_level_3_text, EVAS_CALLBACK_MOUSE_UP,
			clicked_up_level_3, ad);

	// cover
	ad->grid_cover = elm_grid_add(ad->screen);
	elm_grid_pack(ad->screen, ad->grid_cover, 0, 0, 100, 100);
	evas_object_show(ad->grid_cover);

	Evas_Object *cover_bg = elm_bg_add(ad->grid_cover);
	elm_bg_color_set(cover_bg, 0, 0, 0);
	elm_grid_pack(ad->grid_cover, cover_bg, 0, 0, 100, 100);
	evas_object_show(cover_bg);

	ad->btn_unlock = elm_button_add(ad->grid_cover);
	evas_object_color_set(ad->btn_unlock, 0, 255, 91, 255);
	elm_object_text_set(ad->btn_unlock,
			"<align=center><font_size=30><b>활성화</b></font></align>");
	elm_grid_pack(ad->grid_cover, ad->btn_unlock, 25, 40, 50, 20);
	evas_object_smart_callback_add(ad->btn_unlock, "clicked", clicked_unlock,
			ad);
	evas_object_show(ad->btn_unlock);

	// report_screen
	ad->report_screen = elm_grid_add(ad->screen);
	elm_grid_pack(ad->screen, ad->report_screen, 100, 0, 100, 100);
	evas_object_show(ad->report_screen);

	ad->report_screen_bg = elm_bg_add(ad->report_screen);
	elm_bg_color_set(ad->report_screen_bg, 255, 255, 255);
	elm_grid_pack(ad->report_screen, ad->report_screen_bg, 0, 0, 100, 100);
	evas_object_show(ad->report_screen_bg);

	ad->report_screen_text = elm_label_add(ad->report_screen);
	evas_object_color_set(ad->report_screen_text, 0, 0, 0, 255);
	elm_object_text_set(ad->report_screen_text,
			"<align=center><font_size=28><b>반응이 기록되었습니다.</b></font></align>");
	elm_grid_pack(ad->report_screen, ad->report_screen_text, 0, 45, 100, 20);
	evas_object_show(ad->report_screen_text);

	evas_object_show(ad->win);

	ecore_thread_feedback_run(_encore_thread_cover_lock, _set_cover_lock,
			_end_cover_lock_thread, NULL, ad, EINA_FALSE);
	ecore_thread_feedback_run(_encore_thread_check_long_press, NULL, NULL, NULL,
			ad, EINA_FALSE);
}

static bool app_create(void *data) {
	/* Hook to take necessary actions before main event loop starts
	 Initialize UI resources and application's data
	 If this function returns true, the main loop of application starts
	 If this function returns false, the application is terminated */

	appdata_s *ad = data;
	create_base_gui(ad);

	if (!check_hrm_sensor_is_supported()) {
		dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
				"A HRM sensor is not supported.");
		return false;
	} else
		dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG, "A HRM sensor is supported.");

	return true;
}

static void app_control(app_control_h app_control, void *data) {
	/* Handle the launch request. */
}

static void app_pause(void *data) {
	/* Take necessary actions when application becomes invisible. */
	appdata_s *ad = data;
}

static void app_resume(void *data) {
	/* Take necessary actions when application becomes visible. */
	device_power_request_lock(POWER_LOCK_DISPLAY, 0);
	device_power_request_lock(POWER_LOCK_CPU, 0);

	appdata_s *ad = data;

	if (!check_and_request_sensor_permission()) {
		dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
				"Failed to check if an application has permission to use the sensor privilege.");
		ui_app_exit();
	} else
		dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
				"Succeeded in checking if an application has permission to use the sensor privilege.");

}

static void app_terminate(void *data) {
	/* Release all resources. */
	int retval;

	if (check_hrm_sensor_listener_is_created()) {
		if (!destroy_hrm_sensor_listener())
			dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
					"Failed to release all the resources allocated for a HRM sensor listener.");
		else
			dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
					"Succeeded in releasing all the resources allocated for a HRM sensor listener.");
	}
}

static void ui_app_lang_changed(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE,
			&locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void ui_app_orient_changed(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}

static void ui_app_region_changed(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void ui_app_low_battery(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_LOW_BATTERY*/
}

static void ui_app_low_memory(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_LOW_MEMORY*/
}

int main(int argc, char *argv[]) {
	appdata_s ad = { 0, };
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = { 0, };
	app_event_handler_h handlers[5] = { NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY],
			APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY],
			APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
			APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
			APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
			APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}

//////////////////////////////////////////////////////////////////
///////////////////////////// Screen /////////////////////////////
//////////////////////////////////////////////////////////////////

static void clicked_level_0(void *user_data, Evas* e, Evas_Object *obj,
		void *event_info) {
	appdata_s *ad = user_data;

	cover_lock_counter = 0;
	ecore_animator_add(click_animation_level_0, ad);
	level_0_state = 1;
}

static void clicked_up_level_0(void *user_data, Evas* e, Evas_Object *obj,
		void *event_info) {
	appdata_s *ad = user_data;

	cover_lock_counter = 0;
	ecore_animator_add(click_up_animation_level_0, ad);
	level_0_state = 0;
	level_0_pressed_time = 0;
}

static void clicked_level_1(void *user_data, Evas* e, Evas_Object *obj,
		void *event_info) {
	appdata_s *ad = user_data;

	cover_lock_counter = 0;
	ecore_animator_add(click_animation_level_1, ad);
	level_1_state = 1;
}

static void clicked_up_level_1(void *user_data, Evas* e, Evas_Object *obj,
		void *event_info) {
	appdata_s *ad = user_data;

	cover_lock_counter = 0;
	ecore_animator_add(click_up_animation_level_1, ad);
	level_1_state = 0;
	level_1_pressed_time = 0;
}

static void clicked_level_2(void *user_data, Evas* e, Evas_Object *obj,
		void *event_info) {
	appdata_s *ad = user_data;

	cover_lock_counter = 0;
	ecore_animator_add(click_animation_level_2, ad);
	level_2_state = 1;
}

static void clicked_up_level_2(void *user_data, Evas* e, Evas_Object *obj,
		void *event_info) {
	appdata_s *ad = user_data;

	cover_lock_counter = 0;
	ecore_animator_add(click_up_animation_level_2, ad);
	level_2_state = 0;
	level_2_pressed_time = 0;
}

static void clicked_level_3(void *user_data, Evas* e, Evas_Object *obj,
		void *event_info) {
	appdata_s *ad = user_data;

	cover_lock_counter = 0;
	ecore_animator_add(click_animation_level_3, ad);
	level_3_state = 1;
}

static void clicked_up_level_3(void *user_data, Evas* e, Evas_Object *obj,
		void *event_info) {
	appdata_s *ad = user_data;

	cover_lock_counter = 0;
	ecore_animator_add(click_up_animation_level_3, ad);
	level_3_state = 0;
	level_3_pressed_time = 0;
}

static Eina_Bool click_animation_level_0(void *data) {
	appdata_s *ad = data;
	evas_object_color_set(ad->btn_level_0, hover_color_level_0[0],
			hover_color_level_0[1], hover_color_level_0[2],
			hover_color_level_0[3]);
	evas_object_color_set(ad->btn_level_0_text, 220, 220, 220, 220);
	return ECORE_CALLBACK_RENEW;
}
static Eina_Bool click_up_animation_level_0(void *data) {
	appdata_s *ad = data;
	evas_object_color_set(ad->btn_level_0, color_level_0[0], color_level_0[1],
			color_level_0[2], color_level_0[3]);
	evas_object_color_set(ad->btn_level_0_text, 255, 255, 255, 255);
	return ECORE_CALLBACK_RENEW;
}

static Eina_Bool click_animation_level_1(void *data) {
	appdata_s *ad = data;
	evas_object_color_set(ad->btn_level_1, hover_color_level_1[0],
			hover_color_level_1[1], hover_color_level_1[2],
			hover_color_level_1[3]);
	evas_object_color_set(ad->btn_level_1_text, 220, 220, 220, 220);
	return ECORE_CALLBACK_RENEW;
}
static Eina_Bool click_up_animation_level_1(void *data) {
	appdata_s *ad = data;
	evas_object_color_set(ad->btn_level_1, color_level_1[0], color_level_1[1],
			color_level_1[2], color_level_1[3]);
	evas_object_color_set(ad->btn_level_1_text, 255, 255, 255, 255);
	return ECORE_CALLBACK_RENEW;
}

static Eina_Bool click_animation_level_2(void *data) {
	appdata_s *ad = data;
	evas_object_color_set(ad->btn_level_2, hover_color_level_2[0],
			hover_color_level_2[1], hover_color_level_2[2],
			hover_color_level_2[3]);
	evas_object_color_set(ad->btn_level_2_text, 220, 220, 220, 220);
	return ECORE_CALLBACK_RENEW;
}
static Eina_Bool click_up_animation_level_2(void *data) {
	appdata_s *ad = data;
	evas_object_color_set(ad->btn_level_2, color_level_2[0], color_level_2[1],
			color_level_2[2], color_level_2[3]);
	evas_object_color_set(ad->btn_level_2_text, 255, 255, 255, 255);
	return ECORE_CALLBACK_RENEW;
}

static Eina_Bool click_animation_level_3(void *data) {
	appdata_s *ad = data;
	evas_object_color_set(ad->btn_level_3, hover_color_level_3[0],
			hover_color_level_3[1], hover_color_level_3[2],
			hover_color_level_3[3]);
	evas_object_color_set(ad->btn_level_3_text, 220, 220, 220, 220);
	return ECORE_CALLBACK_RENEW;
}
static Eina_Bool click_up_animation_level_3(void *data) {
	appdata_s *ad = data;
	evas_object_color_set(ad->btn_level_3, color_level_3[0], color_level_3[1],
			color_level_3[2], color_level_3[3]);
	evas_object_color_set(ad->btn_level_3_text, 255, 255, 255, 255);
	return ECORE_CALLBACK_RENEW;
}

float lerp(double a, double b, double alpha) {
	return a * (1 - alpha) + b * alpha;
}

Evas_Coord x = 100;

static void report_animation(void *data, Ecore_Thread *thread) {
	appdata_s *ad = data;

	double timer = 0;
	while (timer <= report_animation_speed * 2 + report_animation_showing_time) {
		usleep(10000);
		if (timer <= report_animation_speed) {
			x = (Evas_Coord) (100 - 100 * (timer / report_animation_speed));
			x = (Evas_Coord) lerp(x, 0, timer / report_animation_speed);
			ecore_main_loop_thread_safe_call_sync(report_draw, ad);
		} else if (timer
				<= report_animation_speed + report_animation_showing_time) {
		} else if (timer
				<= report_animation_speed * 2 + report_animation_showing_time) {
			x = (Evas_Coord) (0
					- 100
							* ((timer - report_animation_speed
									- report_animation_showing_time)
									/ report_animation_speed));
			x = (Evas_Coord) lerp(x, -100,
					-(timer - report_animation_speed
							- report_animation_showing_time)
							/ report_animation_speed);
			ecore_main_loop_thread_safe_call_sync(report_draw, ad);
		}
		timer += 0.01;
	}
	x = 100;
	ecore_main_loop_thread_safe_call_sync(report_draw, ad);
	ecore_thread_cancel(report_thread);
}

static void* report_draw(void *data) {
	appdata_s *ad = data;
	elm_bg_color_set(ad->report_screen_bg, report_color[0], report_color[1],
			report_color[2]);
	elm_grid_pack(ad->screen, ad->report_screen, x, 0, 100, 100);

	return NULL;
}

static void clicked_unlock(void *user_data, Evas* e, Evas_Object *obj,
		void *event_info) {
	appdata_s *ad = user_data;
	evas_object_hide(ad->grid_cover);
	cover_lock_counter = 0;
}

static void _set_cover_lock(void *data, Ecore_Thread *thread, void *msgdata) {
	appdata_s *ad = data;
	evas_object_show(ad->grid_cover);
}

static void _encore_thread_cover_lock(void *data, Ecore_Thread *thread) {
	while (1) {
		if (cover_lock_counter >= cover_lock_parameter) {
			ecore_thread_feedback(thread,
					(void*) (uintptr_t) cover_lock_counter);
			cover_lock_counter = 0;
		}
		sleep(1);
		cover_lock_counter += 1;
	}
}

static void _end_cover_lock_thread(void *data, Ecore_Thread *thread) {
	appdata_s *ad = data;
}

static void _encore_thread_check_long_press(void *data, Ecore_Thread *thread) {
	appdata_s *ad = data;

	while (1) {
		if (level_0_pressed_time >= long_press_parameter) {
			level_0_state = 0;
			level_0_pressed_time = 0;
			feedback_play(FEEDBACK_PATTERN_VIBRATION_ON);
			for (int i = 0; i < 4; i++) {
				report_color[i] = color_level_0[i];
			}
			report_thread = ecore_thread_feedback_run(report_animation, NULL,
			NULL, NULL, ad, EINA_FALSE);
		}
		if (level_1_pressed_time >= long_press_parameter) {
			level_1_state = 0;
			level_1_pressed_time = 0;
			feedback_play(FEEDBACK_PATTERN_VIBRATION_ON);
			for (int i = 0; i < 4; i++) {
				report_color[i] = color_level_1[i];
			}
			report_thread = ecore_thread_feedback_run(report_animation, NULL,
			NULL, NULL, ad, EINA_FALSE);
		}
		if (level_2_pressed_time >= long_press_parameter) {
			level_2_state = 0;
			level_2_pressed_time = 0;
			feedback_play(FEEDBACK_PATTERN_VIBRATION_ON);
			for (int i = 0; i < 4; i++) {
				report_color[i] = color_level_2[i];
			}
			report_thread = ecore_thread_feedback_run(report_animation, NULL,
			NULL, NULL, ad, EINA_FALSE);
		}
		if (level_3_pressed_time >= long_press_parameter) {
			level_3_state = 0;
			level_3_pressed_time = 0;
			feedback_play(FEEDBACK_PATTERN_VIBRATION_ON);
			for (int i = 0; i < 4; i++) {
				report_color[i] = color_level_3[i];
			}
			report_thread = ecore_thread_feedback_run(report_animation, NULL,
			NULL, NULL, ad, EINA_FALSE);
		}

		sleep(1);
		if (level_0_state == 1) {
			level_0_pressed_time += 1;
			cover_lock_counter = 0;
		}
		if (level_1_state == 1) {
			level_1_pressed_time += 1;
			cover_lock_counter = 0;
		}
		if (level_2_state == 1) {
			level_2_pressed_time += 1;
			cover_lock_counter = 0;
		}
		if (level_3_state == 1) {
			level_3_pressed_time += 1;
			cover_lock_counter = 0;
		}
	}
}

//sensor_is_supported()

bool check_hrm_sensor_is_supported() {

	check_sensor_supported(SENSOR_HRM, HRM_SENSOR_LOG_TAG);
	check_sensor_supported(SENSOR_HRM_LED_GREEN,
	HRM_GREEN_SENSOR_LOG_TAG);

}

//initialize_sensor()
bool initialize_device_sensor(sensor_type_e type) {
	switch (type) {
	case SENSOR_HRM:
		if (init_sensor(SENSOR_HRM, hrm_sensor_handle,
		HRM_SENSOR_LOG_TAG)) {
			sensor_get_default_sensor(SENSOR_HRM, &hrm_sensor_handle);
			return true;
		}
		break;
	case SENSOR_HRM_LED_GREEN:
		if (init_sensor(SENSOR_HRM_LED_GREEN, hrm_green_sensor_handle,
		HRM_GREEN_SENSOR_LOG_TAG)) {
			sensor_get_default_sensor(SENSOR_HRM_LED_GREEN,
					&hrm_green_sensor_handle);
			return true;
		}
		break;
	default:
		return false;
	}
}
bool create_device_sensor(sensor_type_e type) {
	sensor_h handle = 0;
	switch (type) {
	case SENSOR_HRM:
		return create_hrm_sensor_listener(handle);

	case SENSOR_HRM_LED_GREEN:
		return create_hrm_green_sensor_listener(handle);
	default:
		return false;
	}
}
bool start_device_sensor(sensor_type_e type) {
	switch (type) {
	case SENSOR_HRM:
		return start_hrm_sensor_listener();
	case SENSOR_HRM_LED_GREEN:
		return start_hrm_green_sensor_listener();
	default:
		return false;
	}

}

bool check_and_request_sensor_permission() {
	bool health_usable = true;
	bool media_usable = true;

	int health_retval;
	int mediastorage_retval;
	ppm_check_result_e health_result;
	ppm_check_result_e mediastorage_result;

	health_retval = ppm_check_permission(sensor_privilege, &health_result);
	mediastorage_retval = ppm_check_permission(mediastorage_privilege,
			&mediastorage_result);

	if (health_retval == PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE) {
		switch (health_result) {
		case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ALLOW:
			/* Update UI and start accessing protected functionality */
			dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
					"The application has permission to use a sensor privilege.");

			if (!check_hrm_sensor_listener_is_created()) {
				health_usable = init_handle(
						initialize_device_sensor(SENSOR_HRM),
						create_device_sensor(SENSOR_HRM),
						start_device_sensor(SENSOR_HRM), HRM_SENSOR_LOG_TAG,
						health_usable);
			}
			if (!check_hrm_green_sensor_listener_is_created()) {
				health_usable = init_handle(
						initialize_device_sensor(SENSOR_HRM_LED_GREEN),
						create_device_sensor(SENSOR_HRM_LED_GREEN),
						start_device_sensor(SENSOR_HRM_LED_GREEN),
						HRM_GREEN_SENSOR_LOG_TAG, health_usable);
			}
			break;
		case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_DENY:
			/* Show a message and terminate the application */
			case_DENY(HRM_SENSOR_LOG_TAG, health_usable);
			break;
		case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ASK:
			case_ASK(HRM_SENSOR_LOG_TAG, request_sensor_permission());
			break;
		}
	} else {
		/* retval != PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE */
		/* Handle errors */
		dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
				"Function ppm_check_permission() return %s",
				get_error_message(health_retval));
		health_usable = false;
	}

	if (mediastorage_retval == PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE) {
		if (mediastorage_result
				== PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ALLOW) {
			dlog_print(DLOG_INFO, MEDIA_LOG_TAG,
					"The application has permission to use a storage privilege.");
		} else if (mediastorage_result
				== PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ASK) {
			dlog_print(DLOG_INFO, MEDIA_LOG_TAG,
					"The user has to be asked whether to grant permission to use a sensor privilege.");

			if (!request_sensor_permission()) {
				dlog_print(DLOG_ERROR, MEDIA_LOG_TAG,
						"Failed to request a user's response to obtain permission for using the sensor privilege.");
				media_usable = false;
			} else {
				dlog_print(DLOG_INFO, MEDIA_LOG_TAG,
						"Succeeded in requesting a user's response to obtain permission for using the sensor privilege.");
				media_usable = true;
			}
		} else {
			dlog_print(DLOG_INFO, MEDIA_LOG_TAG,
					"Function ppm_check_permission() output result = PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_DENY");
			dlog_print(DLOG_ERROR, MEDIA_LOG_TAG,
					"The application doesn't have permission to use a sensor privilege.");
			media_usable = false;
		}
	} else {
		/* retval != PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE */
		/* Handle errors */
		dlog_print(DLOG_ERROR, MEDIA_LOG_TAG,
				"Function ppm_check_permission() return %s",
				get_error_message(mediastorage_retval));
		media_usable = false;
	}
	return health_usable && media_usable;
}

bool request_sensor_permission() {
	int health_retval;
	int mediastorage_retval;

	health_retval = ppm_request_permission(sensor_privilege,
			request_health_sensor_permission_response_callback, NULL);
	mediastorage_retval = ppm_request_permission(mediastorage_privilege,
			request_media_permission_response_callback, NULL);

	if (health_retval == PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE
			&& mediastorage_retval == PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE) {
		return true;
	} else if (health_retval
			== PRIVACY_PRIVILEGE_MANAGER_ERROR_ALREADY_IN_PROGRESS
			&& mediastorage_retval
					== PRIVACY_PRIVILEGE_MANAGER_ERROR_ALREADY_IN_PROGRESS) {
		return true;
	} else {
		dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
				"Function ppm_request_permission() return value = %s",
				get_error_message(health_retval));
		dlog_print(DLOG_INFO, MEDIA_LOG_TAG,
				"Function ppm_request_permission() return value = %s",
				get_error_message(mediastorage_retval));
		return false;
	}
}

void request_health_sensor_permission_response_callback(ppm_call_cause_e cause,
		ppm_request_result_e result, const char *privilege, void *user_data) {
	if (cause == PRIVACY_PRIVILEGE_MANAGER_CALL_CAUSE_ERROR) {
		/* Log and handle errors */
		dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
				"Function request_sensor_permission_response_callback() output cause = PRIVACY_PRIVILEGE_MANAGER_CALL_CAUSE_ERROR");
		dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
				"Function request_sensor_permission_response_callback() was called because of an error.");
	} else {
		dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
				"Function request_sensor_permission_response_callback() was called with a valid answer.");

		switch (result) {
		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_ALLOW_FOREVER:
			/* Update UI and start accessing protected functionality */
			request_cb_handle(initialize_device_sensor(SENSOR_HRM),
					create_hrm_sensor_listener(hrm_sensor_handle),
					start_hrm_sensor_listener(), HRM_SENSOR_LOG_TAG);
			request_cb_handle(initialize_device_sensor(SENSOR_HRM_LED_GREEN),
					create_hrm_green_sensor_listener(hrm_green_sensor_handle),
					start_hrm_green_sensor_listener(hrm_green_sensor_handle),
					HRM_GREEN_SENSOR_LOG_TAG);
			break;
		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_FOREVER:
			/* Show a message and terminate the application */
			request_cb_DENY_FOREVER_handle(HRM_SENSOR_LOG_TAG);
			request_cb_DENY_FOREVER_handle(HRM_GREEN_SENSOR_LOG_TAG);
			break;
		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_ONCE:
			/* Show a message with explanation */
			dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
					"Function request_sensor_permission_response_callback() output result = PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_ONCE");
			dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
					"The user denied granting permission to use a sensor privilege once.");
			ui_app_exit();
			break;
		}
	}
}

void request_media_permission_response_callback(ppm_call_cause_e cause,
		ppm_request_result_e result, const char *privilege, void *user_data) {
}

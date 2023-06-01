#include "sensordatacollector.h"

void write_date(char* buf, int n) {
	FILE *fp;
	if (n == 0) {
		fp = fopen("/opt/usr/home/owner/media/Documents/hrm_data.txt", "a+");
		if (fp == NULL) {
			fclose(fp);
			return;
		}
		fputs(buf, fp);
		fclose(fp);
	}
	if (n == 1) {
			fp = fopen("/opt/usr/home/owner/media/Documents/hrm_green_data.txt", "a+");
			if (fp == NULL) {
				fclose(fp);
				return;
			}
			fputs(buf, fp);
			fclose(fp);
		}
}

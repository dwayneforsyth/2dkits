
#ifndef DISK_SYSTEM_H
#define DISK_SYSTEM_H

void initialise_disk(void);
void disk_dir_list(char *path, char *match);
esp_err_t web_disk_dir_list(httpd_req_t *req);
#endif

/*
  这节课中，我们在ESP32 event loop handler中使用了freeRtos Event Group 来取代上节课的xTaskCreate
  注意: Event Loop 是 乐鑫的东西， Event Group是FreeRTOS的东西，他们是完全不同的两个东西
*/

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "freertos/event_groups.h"


static EventGroupHandle_t s_wifi_event_group;

#define STA_START BIT0
#define SCAN_DONE BIT 1

void run_on_event(void *handler_arg, esp_event_base_t base, int32_t id, void *event_data)
{
  ESP_LOGE("EVENT_HANDLE", "BASE:%s, ID:%d", base, id);

  switch (id)
  {
  case WIFI_EVENT_STA_START:
    ESP_LOGE("EVENT_HANDLE", "WIFI_EVENT_STA_START");
    xEventGroupSetBits(s_wifi_event_group, STA_START);
    break;
  case WIFI_EVENT_SCAN_DONE:
    ESP_LOGE("EVENT_HANDLE", "WIFI_EVENT_SCAN_DONE");
    xEventGroupSetBits(s_wifi_event_group, SCAN_DONE);
    break;
  default:
    break;
  }
}

void app_task(void *pt)
{
  ESP_LOGI("APP_TASK", "APP Task 创建完成");
  esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, run_on_event, NULL);
  while (1)
  {
    xEventGroupWaitBits(s_wifi_event_group,
                        STA_START,
                        pdFALSE,
                        pdFALSE,
                        portMAX_DELAY);
    ESP_LOGE("APP_TASK", "接收到了STA START事件, 可以运行AP Scan了");
    wifi_country_t wifi_country_config = {
        .cc = "CN",
        .schan = 1,
        .nchan = 13,
    };
    ESP_ERROR_CHECK(esp_wifi_set_country(&wifi_country_config));
    ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, false));

    xEventGroupWaitBits(s_wifi_event_group,
                        SCAN_DONE,
                        pdFALSE,
                        pdFALSE,
                        portMAX_DELAY);
    ESP_LOGE("APP_TASK", "接收到了SCAN DONE事件, 可以打印出AP扫描结果了");
    uint16_t ap_num = 0;
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_num));
    ESP_LOGI("WIFI", "AP Count : %d", ap_num);

    uint16_t max_aps = 20;
    wifi_ap_record_t ap_records[max_aps];
    memset(ap_records, 0, sizeof(ap_records));

    uint16_t aps_count = max_aps;
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&aps_count, ap_records));

    ESP_LOGI("WIFI", "AP Count: %d", aps_count);

    printf("%30s %s %s %s\n", "SSID", "频道", "强度", "MAC地址");

    for (int i = 0; i < aps_count; i++)
    {
      printf("%30s  %3d  %3d  %02X-%02X-%02X-%02X-%02X-%02X\n", ap_records[i].ssid, ap_records[i].primary, ap_records[i].rssi, ap_records[i].bssid[0], ap_records[i].bssid[1], ap_records[i].bssid[2], ap_records[i].bssid[3], ap_records[i].bssid[4], ap_records[i].bssid[5]);
    }
    vTaskDelete(NULL);
  }
}

void app_main(void)
{

  ESP_LOGI("WIFI", "0. 初始化NVS存储");
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_LOGI("WIFI", "1. Wi-Fi 初始化阶段");
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));

  ESP_LOGI("WIFI", "创建App Task 和 FreeRTOS Event Group");
  s_wifi_event_group = xEventGroupCreate();
  xTaskCreate(app_task, "App Task", 1024 * 12, NULL, 1, NULL);

  ESP_LOGI("WIFI", "2. Wi-Fi 初始化阶段");
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

  ESP_LOGI("WIFI", "3. Wi-Fi 启动阶段");
  ESP_ERROR_CHECK(esp_wifi_start());

  while (1)
  {
    // ESP_LOGI("MAIN_TASK", "MAIN Task 无聊的运行中");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
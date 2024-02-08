/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include "LQ_SGP18T.h"	 
#include "lcd_init.h"
#include "lcd.h"
#include "pic.h"
#include "kl.h"
#include "esp_spi_flash.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"


#include "esp_http_client.h"
#include "cJSON.h"

static const char *TAG = "example";
// char ptrTaskList[250];
/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/


static uint8_t s_led_state = 0;
#define MAX_HTTP_OUTPUT_BUFFER 2048

static void http_test_task(void *pvParameters)
{

//02-1 定义需要的变量
      char output_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};   //用于接收通过http协议返回的数据
    int content_length = 0;  //http协议头的长度
    

    //02-2 配置http结构体
   
   //定义http配置结构体，并且进行清零
    esp_http_client_config_t config ;
    memset(&config,0,sizeof(config));

    //向配置结构体内部写入url
    static const char *URL = "http://quan.suning.com/getSysTime.do";
    config.url = URL;
    //初始化结构体
    esp_http_client_handle_t client = esp_http_client_init(&config);	//初始化http连接
    //设置发送请求 
    esp_http_client_set_method(client, HTTP_METHOD_GET);
    //02-3 循环通讯
    while(1)
    {
    // 与目标主机创建连接，并且声明写入内容长度为0
    esp_err_t err = esp_http_client_open(client, 0);

    //如果连接失败
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
    } 
    //如果连接成功
    else {

        //读取目标主机的返回内容的协议头
        content_length = esp_http_client_fetch_headers(client);

        //如果协议头长度小于0，说明没有成功读取到
        if (content_length < 0) {
            ESP_LOGE(TAG, "HTTP client fetch headers failed");
        } 

        //如果成功读取到了协议头
        else {

            //读取目标主机通过http的响应内容
            int data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
            if (data_read >= 0) {

                //打印响应内容，包括响应状态，响应体长度及其内容
                ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %lld",
                esp_http_client_get_status_code(client),				//获取响应状态信息
                esp_http_client_get_content_length(client));			//获取响应信息长度
                printf("data:%s\n", output_buffer);
				//对接收到的数据作相应的处理
                cJSON* root = NULL;
                root = cJSON_Parse(output_buffer);
                cJSON* time =cJSON_GetObjectItem(root,"sysTime2");
                printf("%s\n",time->valuestring);
                LCD_ShowString(0,0,(u8 *)time->valuestring,RED,WHITE,12,0);
            } 
            //如果不成功
            else {
                ESP_LOGE(TAG, "Failed to read response");
            }
        }
    }

    //关闭连接
    esp_http_client_close(client);

    //延时
    vTaskDelay(3000/portTICK_PERIOD_MS);
    vTaskDelete(NULL);
    }
}


static void blink_led(void)
{
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    gpio_set_level(GPIO_NUM_2, s_led_state);
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink GPIO LED!");
    gpio_reset_pin(GPIO_NUM_2);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
}




void start_scan()
{
    wifi_country_t config = {
        .cc = "CN",
        .schan = 1,
        .nchan = 13,
        .policy = WIFI_COUNTRY_POLICY_AUTO,
    };
    esp_wifi_set_country(&config);
    wifi_scan_config_t scan_config = {
     .ssid= NULL,             /**< SSID of AP */
     .bssid= NULL,              /**< MAC address of AP */
     .channel = 0,           /**< channel, scan the specific channel */
     .show_hidden = false,            /**< enable to scan AP whose SSID is hidden */
     .scan_type=WIFI_SCAN_TYPE_ACTIVE,  /**< scan type, active or passive */
     .scan_time={
        .active = {
                .min=120,
                .max=150,
        },
            .passive=1200,
        },  /**< scan time per channel */
    };
    esp_wifi_scan_start(&scan_config,false);
    vTaskDelete(NULL);
}
void show_pid()
{   
    char ptrTaskList[300];
    vTaskList(ptrTaskList);
    printf("Task        State   prio    Stack   Num\n");
    printf(ptrTaskList);
    vTaskDelete(NULL);
}

void start_show_pid()
{
    xTaskCreate(show_pid,"show_pid",1024,NULL,2,NULL);
};

void show_scan()
{
    uint16_t ap_num =  0;
    esp_wifi_scan_get_ap_num(&ap_num);
    ESP_LOGI("wifi","%d",ap_num);
    wifi_ap_record_t ap_info[20];
    memset(ap_info,0,sizeof(ap_info));
    esp_wifi_scan_get_ap_records(&ap_num,&ap_info);
    ESP_LOGI("wifi","%d",ap_num);
    printf("%30s %s %s %s\n", "SSID", "频道", "强度", "MAC地址");
    for (int i = 0; i < ap_num; i++)
    {
        printf("%30s  %3d  %3d  %02X-%02X-%02X-%02X-%02X-%02X\n", ap_info[i].ssid, ap_info[i].primary, ap_info[i].rssi, ap_info[i].bssid[0], ap_info[i].bssid[1], ap_info[i].bssid[2], ap_info[i].bssid[3], ap_info[i].bssid[4], ap_info[i].bssid[5]);
    }
    // start_show_pid();
    vTaskDelay(1000/portTICK_PERIOD_MS);
    esp_wifi_connect();
    vTaskDelay(1000/portTICK_PERIOD_MS);
    printf("-----------------start_scan\n");
    vTaskDelay(1000/portTICK_PERIOD_MS);
    vTaskDelete(NULL);
}

void kill_wifi()
{
    esp_wifi_disconnect();
    esp_wifi_stop();
    esp_wifi_deinit();
    vTaskDelete(NULL);
}
// s8.1：调用函数 esp_wifi_disconnect() 断开 Wi-Fi 连接。

// s8.2：调用函数 esp_wifi_stop() 终止 Wi-Fi 驱动程序。

// s8.3：调用函数 esp_wifi_deinit() 清理 Wi-Fi 驱动程序。


void run_on_event(void *handler_arg, esp_event_base_t base, int32_t id, void *event_data)
{
  ESP_LOGI("EVENT_HANDLE", "BASE:%s, ID:%ld", base, id);
  if(base == WIFI_EVENT)
  {
    switch (id)
    {
        case WIFI_EVENT_STA_START:
            ESP_LOGE("EVENT_HANDLE", "WIFI_EVENT_STA_START");
            xTaskCreate(start_scan, "start_scan", 1024, NULL, 2, NULL);
            break;
        case WIFI_EVENT_SCAN_DONE:
            ESP_LOGE("EVENT_HANDLE", "WIFI_EVENT_SCAN_DONE");
            xTaskCreate(show_scan, "show_scan", 1024*20, NULL, 2, NULL);
            break;
        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGE("EVENT_HANDLE", "连接成功");
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGE("EVENT_HANDLE", "连接失败");
            break;
    }
  }
    else if(base == IP_EVENT)
    {
        switch (id)
        {
            case IP_EVENT_STA_GOT_IP:
                ESP_LOGE("EVENT_HANDLE", "IP_EVENT_STA_GOT_IP");
                vTaskDelay(2000/portTICK_PERIOD_MS);
                // xTaskCreate(kill_wifi, "kill_wifi", 1024, NULL, 2, NULL);
                xTaskCreate(&http_test_task, "http_test_task", 8192, NULL, 5, NULL);
                break;
            case IP_EVENT_STA_LOST_IP:
                ESP_LOGE("EVENT_HANDLE", "IP_EVENT_STA_LOST_IP");
                break;
        }
    }
}



void task_list(void)
{   
    // char ptrTaskList2[200];
    esp_event_handler_register(ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, run_on_event, NULL);
    while (1)
    {
        // 
        // vTaskList(ptrTaskList2);
        // printf("Task        State   prio    Stack   Num\n");
        // printf(ptrTaskList2);
        vTaskDelay(2000/portTICK_PERIOD_MS);
    }
}



void app_main(void)
{
    uint8_t x =0;
    /* Configure the peripheral according to the LED type */
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_init = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_init);

    xTaskCreate(task_list, "task_list", 10240, NULL, 1, NULL);
// typedef enum {
//     WIFI_MODE_NULL = 0,  /**< null mode */
//     WIFI_MODE_STA,       /**< WiFi station mode */
//     WIFI_MODE_AP,        /**< WiFi soft-AP mode */
//     WIFI_MODE_APSTA,     /**< WiFi station + soft-AP mode */
//     WIFI_MODE_MAX
// } wifi_mode_t;
    esp_wifi_set_mode(WIFI_MODE_STA);
    wifi_config_t configs = {
        .sta = {
            .ssid = "duan",
            .password = "13417319586",
            .bssid_set = 0,
            .channel = 0,
            .listen_interval = 0,
            .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
            .threshold.rssi = -127,
            .threshold.authmode = WIFI_AUTH_OPEN,
        },            
    };

    esp_wifi_set_config(ESP_IF_WIFI_STA,&configs);
    esp_wifi_start();
// typedef struct {
//     uint8_t *ssid;               /**< SSID of AP */
//     uint8_t *bssid;              /**< MAC address of AP */
//     uint8_t channel;             /**< channel, scan the specific channel */
//     bool show_hidden;            /**< enable to scan AP whose SSID is hidden */
//     wifi_scan_type_t scan_type;  /**< scan type, active or passive */
//     wifi_scan_time_t scan_time;  /**< scan time per channel */
// } wifi_scan_config_t;

    // wifi_country_t config = {
    //     .cc = "CN",
    //     .schan = 1,
    //     .nchan = 13,
    //     .policy = WIFI_COUNTRY_POLICY_AUTO,
    // };
    // esp_wifi_set_country(&config);
    // wifi_scan_config_t scan_config = {
    //  .ssid= NULL,             /**< SSID of AP */
    //  .bssid= NULL,              /**< MAC address of AP */
    //  .channel = 0,           /**< channel, scan the specific channel */
    //  .show_hidden = false,            /**< enable to scan AP whose SSID is hidden */
    //  .scan_type=WIFI_SCAN_TYPE_ACTIVE,  /**< scan type, active or passive */
    //  .scan_time={
    //     .active = {
    //             .min=120,
    //             .max=150,
    //     },
    //         .passive=1200,
    //     },  /**< scan time per channel */
    // };
    // esp_wifi_scan_start(&scan_config,true);

    // uint16_t ap_num =  0;
    // esp_wifi_scan_get_ap_num(&ap_num);
    // ESP_LOGI("wifi","%d",ap_num);
    // wifi_ap_record_t ap_info[20];
    // memset(ap_info,0,sizeof(ap_info));
    // esp_wifi_scan_get_ap_records(&ap_num,&ap_info);
    // ESP_LOGI("wifi","%d",ap_num);
    // // for(uint8_t i=0;i<ap_num;i++)
    // // printf("
    // //  bssid = %02x:%02x:%02x:%02x:%02x:%02x,\n",ap_info[i].bssid[0],ap_info[i].bssid[1],ap_info[i].bssid[2],ap_info[i].bssid[3],ap_info[i].bssid[4],ap_info[i].bssid[5]);
    // printf("%30s %s %s %s\n", "SSID", "频道", "强度", "MAC地址");

    // for (int i = 0; i < ap_num; i++)
    // {
    //     printf("%30s  %3d  %3d  %02X-%02X-%02X-%02X-%02X-%02X\n", ap_info[i].ssid, ap_info[i].primary, ap_info[i].rssi, ap_info[i].bssid[0], ap_info[i].bssid[1], ap_info[i].bssid[2], ap_info[i].bssid[3], ap_info[i].bssid[4], ap_info[i].bssid[5]);
    // }
    // char ptrTaskList[250];
    // vTaskList(ptrTaskList);
    // printf("Task        State   prio    Stack   Num\n");
    // printf(ptrTaskList);
    configure_led();
    LCD_Init();//
	LCD_Fill(0,0,LCD_W,LCD_H,RED);
    int t=0;
    // show_pid();
    while (1) {
        ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
        // blink_led();
        /* Toggle the LED state */
        s_led_state = !s_led_state;
        LCD_ShowString(0,16,(u8 *)"LCD_W:",RED,WHITE,16,0);
		LCD_ShowIntNum(48,16,LCD_W,3,RED,WHITE,16);
		LCD_ShowString(0,32,(u8 *)"LCD_H:",RED,WHITE,16,0);
		LCD_ShowIntNum(48,32,LCD_H,3,RED,WHITE,16);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
        // printf("Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
		LCD_ShowString(0,48,(u8 *)"Nun:",RED,WHITE,16,0);
		LCD_ShowFloatNum1(32,48,t,4,RED,WHITE,16);
        t++;
        LCD_ShowPicture(80,20,40,40,gImage_kl);
        // ESP_LOGI("wifi","");
        // vTaskDelay(5000 / portTICK_PERIOD_MS);
        // ap_num =  0;
        // esp_wifi_scan_start(&scan_config,true);
        // esp_wifi_scan_get_ap_num(&ap_num);
        // ESP_LOGI("wifi","%d",ap_num);
        // // wifi_ap_record_t ap_info[20];
        // memset(ap_info,0,sizeof(ap_info));
        // esp_wifi_scan_get_ap_records(&ap_num,&ap_info);
        // ESP_LOGI("wifi","%d",ap_num);
        // // for(uint8_t i=0;i<ap_num;i++)
        // // printf("
        // //  bssid = %02x:%02x:%02x:%02x:%02x:%02x,\n",ap_info[i].bssid[0],ap_info[i].bssid[1],ap_info[i].bssid[2],ap_info[i].bssid[3],ap_info[i].bssid[4],ap_info[i].bssid[5]);
        // printf("%30s %s %s %s\n", "SSID", "频道", "强度", "MAC地址");

        // for (int i = 0; i < ap_num; i++)
        // {
        //     printf("%30s  %3d  %3d  %02X-%02X-%02X-%02X-%02X-%02X\n", ap_info[i].ssid, ap_info[i].primary, ap_info[i].rssi, ap_info[i].bssid[0], ap_info[i].bssid[1], ap_info[i].bssid[2], ap_info[i].bssid[3], ap_info[i].bssid[4], ap_info[i].bssid[5]);
        // }



        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }

	// ILI9163B_init();
	// vTaskDelay(1000 / portTICK_PERIOD_MS);
	// while(1){ 
	// 	ILI9163B_display_full(BLUE);									//红色屏幕
	// 	LCD_prints16(4,9,(uint8 *)"DRAW PART",WHITE,RED);	
    //     vTaskDelay(3000 / portTICK_PERIOD_MS);
    //     printf("---------i-------\n");
    //     ILI9163B_display_full(RED);	
    //     vTaskDelay(3000 / portTICK_PERIOD_MS);
    // }
}

/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include <esp_event.h>
#include <esp_netif.h>
#include <nvs_flash.h>
#include "esp_spiffs.h"

/**
 * This is an example which echos any data it receives on configured UART back to the sender,
 * with hardware flow control turned off. It does not use UART driver event queue.
 *
 * - Port: configured UART
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 * - Pin assignment: see defines below (See Kconfig)
 */

#define ECHO_TEST_TXD (CONFIG_EXAMPLE_UART_TXD)
#define ECHO_TEST_RXD (CONFIG_EXAMPLE_UART_RXD)
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

#define ECHO_UART_PORT_NUM      (CONFIG_EXAMPLE_UART_PORT_NUM)
#define ECHO_UART_BAUD_RATE     (CONFIG_EXAMPLE_UART_BAUD_RATE)
#define ECHO_TASK_STACK_SIZE    (CONFIG_EXAMPLE_TASK_STACK_SIZE)

static const char *TAG = "UART_ECHO";

#define BUF_SIZE (2048)

namespace SPIFFS {
    void read_data(const char* filename, uint8_t* data_ptr, size_t size) {
        FILE *file = fopen(filename, "r");

        if (file == NULL) {
            return;
        }

        fseek(file, 0, SEEK_END);
        int file_length = ftell(file);
        fseek(file, 0, SEEK_SET);

        if(file_length <= 0) {
            return;
        }

        int temp_c;
        int loop_var = 0;

        while ((temp_c = fgetc(file)) != EOF)
        {
            data_ptr[loop_var] = temp_c;
            loop_var++;
        }

        data_ptr[loop_var] = '\0';
        fclose(file);

        return;
    }

    void write_data(const char* filename, uint8_t* data_ptr, size_t size) {
        FILE *file = fopen(filename, "w");

        if (file == NULL) {
            return;
        }

        int res = fwrite(data_ptr, 1, size, file);

        if(res != size) {
            ESP_LOGE(TAG, "Error writing to file(%d <> %d\n", res, size);
        }

        fclose(file);

        return;
    }

    void init_spiffs() {
        esp_vfs_spiffs_conf_t conf = {
            .base_path = "/spiffs",
            .partition_label = NULL,
            .max_files = 5,
            .format_if_mount_failed = true
        };

        ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));
    }
}

namespace UART {

    int uart_rx_len = 0;
    uint8_t uart_rx_buf[BUF_SIZE] = {0};
    uint8_t uart_tx_buf[BUF_SIZE] = {0};
    const char* uart_file = "/spiffs/uart_data.txt";
    
    void uart_echo_task(void *arg) {

        bool store_to_send = false;

        while (1) {
            uart_rx_len = 0;
            memset(uart_rx_buf, 0x00, BUF_SIZE);
            memset(uart_tx_buf, 0x00, BUF_SIZE);

            // Read data from the UART
            uart_rx_len = uart_read_bytes(ECHO_UART_PORT_NUM, uart_rx_buf, (BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);

            if (uart_rx_len) {
                uart_rx_buf[uart_rx_len] = '\0';
                // ESP_LOGI(TAG, "Recv str: %s", (char *) uart_rx_buf);

                // Store recieved data to uart file
                SPIFFS::write_data(uart_file, uart_rx_buf, uart_rx_len);

                store_to_send = true;
            }

            if(store_to_send) {
                store_to_send = false;
                
                // Extract data from the uart file
                SPIFFS::read_data(uart_file, uart_tx_buf, uart_rx_len);

                // Write data back to the UART
                uart_write_bytes(ECHO_UART_PORT_NUM, (const char *) uart_tx_buf, uart_rx_len);
            }
        }
    }

    void init_uart() {
        /* Configure parameters of an UART driver,
        * communication pins and install the driver */
        uart_config_t uart_config = {
            .baud_rate = ECHO_UART_BAUD_RATE,
            .data_bits = UART_DATA_8_BITS,
            .parity    = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .source_clk = UART_SCLK_DEFAULT,
        };
        int intr_alloc_flags = 0;

    #if CONFIG_UART_ISR_IN_IRAM
        intr_alloc_flags = ESP_INTR_FLAG_IRAM;
    #endif

        ESP_ERROR_CHECK(uart_driver_install(ECHO_UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
        ESP_ERROR_CHECK(uart_param_config(ECHO_UART_PORT_NUM, &uart_config));
        ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));

        // uart echo task
        xTaskCreate(uart_echo_task, "uart_echo_task", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
    }
}

extern "C" void app_main(void)
{
    // Initialise system resources
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(nvs_flash_init());

    UART::init_uart();
    SPIFFS::init_spiffs();

    ESP_LOGI(TAG, "System Initialiation Done, Entering Main loop...\n");

    while (true) {
        // ESP_LOGI(TAG, "Main Loop...\n");
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}

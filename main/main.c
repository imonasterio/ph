#include <stdio.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"

#include "mqtt.h"
#include "adc.h"

QueueHandle_t queue_mV;
SemaphoreHandle_t xSemaphore_1 = NULL;
SemaphoreHandle_t xSemaphore_2 = NULL;
#define ESP_INTR_FLAG_DEFAULT 0
#define PIN_BUTTON_1 13
#define PIN_BUTTON_2 12

// Interrupt routine, called when push button is pressed
void IRAM_ATTR button_isr_handler_1(void* arg) {

   // gives the semaphore so that it is free for the push button task
   xSemaphoreGiveFromISR(xSemaphore_1, NULL);
}
void IRAM_ATTR button_isr_handler_2(void* arg) {

    // gives the semaphore so that it is free for the push button task
   xSemaphoreGiveFromISR(xSemaphore_2, NULL);
}

void init_GPIO(){
   //configure the PUSH PIN as a GPIO pin
   gpio_pad_select_gpio(PIN_BUTTON_1);
   gpio_pad_select_gpio(PIN_BUTTON_2);
   
   // select the PUSH BUTTON as input pin
   gpio_set_direction(PIN_BUTTON_1, GPIO_MODE_INPUT);
   gpio_set_direction(PIN_BUTTON_2, GPIO_MODE_INPUT);
  
   // install the ISR service with the default settings.
   gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
   // add the handler for the ISR service
   gpio_isr_handler_add(PIN_BUTTON_1, button_isr_handler_1, NULL);
   gpio_isr_handler_add(PIN_BUTTON_2, button_isr_handler_2, NULL);
   // enable falling edge interrupt (1->0)
   gpio_set_intr_type(PIN_BUTTON_1, GPIO_INTR_NEGEDGE);
   gpio_set_intr_type(PIN_BUTTON_2, GPIO_INTR_NEGEDGE);
}

void mqtt(void *arg) {
    while(1) {
        if(xSemaphoreTake(xSemaphore_1,( TickType_t ) 10) == pdTRUE) {
            app_main_mqtt(queue_mV);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }   
}

void adc(void *arg) {
    while(1) {
        if(xSemaphoreTake(xSemaphore_2,( TickType_t ) 10) == pdTRUE) {
            app_main_adc(queue_mV);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }   
}

void app_main(void)
{
   init_GPIO();

   queue_mV = xQueueCreate(SIZE_QUEQUE_VOLTAGE, sizeof( uint32_t ));

   // Created Semaphore Binary 
   xSemaphore_1 = xSemaphoreCreateBinary();
   xSemaphore_2 = xSemaphoreCreateBinary();
   xTaskCreate(mqtt, "mqtt", 1024*8, NULL, 5, NULL);
   xTaskCreate(adc, "adc", 1024*3, NULL, 5, NULL);

    
}

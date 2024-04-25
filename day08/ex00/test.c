.....
#define LED_HOST HSPI_HOST
#define DMA_CHAN 1

#define DATA_PIN 13
#define CLK_PIN 14

#define NUM_LEDS 40

static const BGRb no_color = (BGRb){.brightness = 31, .blue = 0, .green = 0, .red = 0};
static const BGRb red_color = (BGRb){.brightness = 31, .blue = 0, .green = 0, .red = 255};
static const BGRb blue_color = (BGRb){.brightness = 31, .blue = 255, .green = 0, .red = 0};
static const BGRb green_color = (BGRb){.brightness = 31, .blue = 0, .green = 255, .red = 0};

static void draw_leds(spi_device_handle_t spi, uint8_t *data, unsigned long data_len)
{
    esp_err_t ret;
    static spi_transaction_t trans;

    trans.length = data_len;
    trans.tx_buffer = data;

    ret = spi_device_queue_trans(spi, &trans, portMAX_DELAY);
    //ret = spi_device_transmit(spi, &trans);
    ESP_ERROR_CHECK(ret);
}

void app_main()
{
    APA102_init(NUM_LEDS);
    unsigned long data_len = APA102_get_data_length();

    esp_err_t ret;
    spi_device_handle_t spi;

    spi_bus_config_t buscfg = {
        .mosi_io_num = DATA_PIN,
        .sclk_io_num = CLK_PIN,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4094,
        .flags = SPICOMMON_BUSFLAG_MASTER
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1000000,
        .mode = 0,
        .spics_io_num = -1,
        .queue_size = 7,
        .address_bits = 0,
        .command_bits = 0
    };

    // initialize spi bus
    ret = spi_bus_initialize(LED_HOST, &buscfg, DMA_CHAN);
    ESP_ERROR_CHECK(ret);

    // attach LED to spi bus
    ret = spi_bus_add_device(LED_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);

    ret = spi_device_acquire_bus(spi, portMAX_DELAY);
    ESP_ERROR_CHECK(ret);

    // set each LED
    APA102_set_all(no_color);
    
    uint8_t *data = APA102_get_data();
    draw_leds(spi, data, data_len);

    
    printf("data len: %ld bytes\n", data_len);

    for (int b = 0; b < data_len; b++) {
        printf("byte %d: %d\n", b, *(data+b));
    }


    for (;;) {
        printf("no color\n");
        APA102_set_all(no_color);
        data = APA102_get_data();
        draw_leds(spi, data, data_len);

        vTaskDelay(1000 / portTICK_PERIOD_MS);

        printf("red color\n");
        APA102_set_all(red_color);
        data = APA102_get_data();
        draw_leds(spi, data, data_len);

        vTaskDelay(1000 / portTICK_PERIOD_MS);

        printf("green color\n");
        APA102_set_all(green_color);
        data = APA102_get_data();
        draw_leds(spi, data, data_len);

        vTaskDelay(1000 / portTICK_PERIOD_MS);

        printf("blue color\n");
        APA102_set_all(blue_color);
        data = APA102_get_data();
        draw_leds(spi, data, data_len);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}
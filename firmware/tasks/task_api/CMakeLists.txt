idf_component_register(SRCS "task_api.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_adc driver utils linux_task gyml8511_task dht11_task cjmcu811_task
                    dht11 gyml8511 cjmcu-811)


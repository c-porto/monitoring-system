idf_component_register(SRCS "task_api.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_adc driver utils http_task sensor_read_task)
                    


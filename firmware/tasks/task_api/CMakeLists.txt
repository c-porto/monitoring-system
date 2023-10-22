idf_component_register(SRCS "task_api.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES utils queue_api linux_task dht11_task dht11)


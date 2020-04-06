idf_component_register(SRCS "wifi_manager.c" "soft_ap_prov_handlers.c" "soft_ap_prov.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_netif wifi_provisioning)

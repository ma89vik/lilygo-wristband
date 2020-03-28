# Embed the server root certificate into the final binary
idf_build_get_property(project_dir PROJECT_DIR)
idf_component_register(SRCS main.c acc_filter.c battery.c board.c imu.c pwr_mgmt.c display.c input.c touchpad.c ota.c state_ctrl.c time_sync.c provisioning.c
                    "screens/desktop.c" "screens/menu.c" "screens/ota_screen.c" "screens/prov_screen.c" 
                    "screens/res/d1_icon.c" "screens/res/d2_icon.c" "screens/res/d3_icon.c"
                    INCLUDE_DIRS "." "screens"
                    EMBED_TXTFILES ${project_dir}/server_certs/ca_cert.pem)

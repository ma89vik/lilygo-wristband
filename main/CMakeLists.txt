# Embed the server root certificate into the final binary
idf_build_get_property(project_dir PROJECT_DIR)
idf_component_register(SRCS main.c battery.c board.c pwr_mgmt.c display.c desktop.c input.c touchpad.c
                    INCLUDE_DIRS ".")

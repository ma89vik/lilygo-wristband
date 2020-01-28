idf_component_register(SRCS "window.c"
                            "widget.c"
                            "decode_image.c"
                            "widget_button.c"
                            "widget_icon.c"
                            "widget_rect.c"
                            "widget_grid.c"
                            "widget_text.c"

                    INCLUDE_DIRS "include"
                    REQUIRES display
                    REQUIRES esp_rom)
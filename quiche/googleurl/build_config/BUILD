load("//build_config:common_settings.bzl", "bool_flag")

licenses(["notice"])

config_setting(
    name = "windows_x86_64",
    values = {"cpu": "x64_windows"},
)

bool_flag(
    name = "system_icu",
    build_setting_default = True,
)

config_setting(
    name = "with_system_icu",
    flag_values = {
        ":system_icu": "True",
    },
)

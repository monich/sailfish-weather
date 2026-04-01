# SPDX-FileCopyrightText: 2013 - 2023 Jolla Ltd.
# SPDX-FileCopyrightText: 2024 - 2025 Jolla Mobile Ltd
#
# SPDX-License-Identifier: BSD-3-Clause

TEMPLATE = subdirs

settings.subdir = application/settings

SUBDIRS = components application settings

OTHER_FILES = \
    BACKEND.md \
    README.md \
    rpm/sailfish-weather.spec \

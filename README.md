# WTAIN_Driver on Linux Platform
WTAIN-M is programmable analog input, from Weeder Technologies. used in industry to collect analog signal from the system. This repository contains a working driver written in C

## Generating Documentation
To generate the Doxygen documentation, use the following command:
`doxygen Doxyfile`
The documentation will be available in the `docs/html` folder.

## Building the project
- `meson setup build`
- `ninja -C build/`

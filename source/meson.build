# Define the source files
weeder_sources = files(
  'Weeder/analog_input.c'
)

application_sources = files(
  'main.c'
)

# Include directories for header files
inc = include_directories('Weeder')

# Create the executable
executable('WTAIN_M_Driver',
  sources: application_sources + weeder_sources,
  include_directories: inc,
  install: true
)
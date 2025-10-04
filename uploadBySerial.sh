
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

stm32flash -b 115200 -S 0x08000000 -g 0x08000000 -w ${SCRIPT_DIR}/Release/stmidi.bin /dev/ttyUSB0

#!/bin/bash
# termOS grub installer, auto-resolves termOS path

# get the directory of this script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# termOS binary path
TERMOS_PATH="${SCRIPT_DIR}/termOS"

if [[ ! -f "$TERMOS_PATH" ]]; then
    echo "termOS binary not found at $TERMOS_PATH"
    exit 1
fi

# detect root partition
ROOT_PART=$(df / | tail -1 | awk '{print $1}')

# grub menuentry template
ENTRY="menuentry \"termOS\" {
    linux ${SCRIPT_DIR}/vmlinuz-6.14.0-37-generic root=${ROOT_PART} rw init=${TERMOS_PATH}
    initrd ${SCRIPT_DIR}/initrd.img-6.14.0-37-generic
}"

# write to /etc/grub.d/42_termOS
echo "$ENTRY" | sudo tee /etc/grub.d/42_termOS > /dev/null
sudo chmod +x /etc/grub.d/42_termOS

# regenerate grub.cfg
sudo update-grub

echo "termOS menu entry installed! Reboot and select 'termOS' in grub."

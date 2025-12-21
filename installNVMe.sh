#!/bin/bash
# termOS NVMe installer

set -e

# prompt for NVMe drive details
read -p "NVMe controller number (X in /dev/nvmeXnYpZ)? " CONTROLLER
read -p "NVMe namespace number (usually 1) (Y in /dev/nvmeXnYpZ)? " NAMESPACE
read -p "NVMe partition number (Z in /dev/nvmeXnYpZ)? " PARTITION

DEV="/dev/nvme${CONTROLLER}n${NAMESPACE}p${PARTITION}"
echo "Selected device: $DEV"

# confirm
read -p "This will erase everything on $DEV. Are you sure? [y/N] " CONFIRM
if [[ "$CONFIRM" != "y" && "$CONFIRM" != "Y" ]]; then
    echo "Aborting."
    exit 1
fi

# format partition
sudo mkfs.ext4 "$DEV"

# create mount point
MNT="/mnt/termOS_install"
sudo mkdir -p "$MNT"

# mount partition
sudo mount "$DEV" "$MNT"

# copy termOS files
echo "Copying termOS files..."
# assumes this script is in the parent of termOS directory
sudo cp -r ./* "$MNT/"

# fix ownership so user can write without sudo
sudo chown -R $USER:$USER "$MNT"
sudo chmod -R 755 "$MNT"

# unmount
sudo umount "$MNT"

# grub menuentry template
ENTRY="menuentry \"termOS\" {
    set root=(hd${CONTROLLER},${PARTITION})
    linux /vmlinuz-6.14.0-37-generic root=${ROOT_PART} ro init=/_/termOS
    initrd /initrd.img-6.14.0-37-generic
}"

# write to /etc/grub.d/42_termOS
echo "$ENTRY" | sudo tee /etc/grub.d/42_termOS > /dev/null
sudo chmod +x /etc/grub.d/42_termOS

# regenerate grub.cfg
sudo update-grub

echo "termOS installed on $DEV!"
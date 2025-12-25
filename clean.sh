#!/bin/sh
#

sudo systemctl disable ft_shield
sudo systemctl stop ft_shield
sudo rm /etc/systemd/system/ft_shield.service 
sudo rm /bin/ft_shield 
sudo systemctl daemon-reload

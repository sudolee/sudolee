#!/bin/bash -e

mkdir -p ~/.config/autostart/ && 
	cp /usr/share/applications/org.fcitx.Fcitx5.desktop ~/.config/autostart/
#KDE Wayland requires the input method process to be invoked by KWin. [2] To achieve that, quit any running Fcitx 5 process, head to System Settings > Input Devices > Virtual Keyboard, then select Fcitx 5

# 然后前往 Fcitx5设置 -> 配置附加组件 -> 经典用户界面 -> 主题 设置主题。

echo 'XMODIFIERS=@im=fcitx' >> /etc/environment
echo 'export XMODIFIERS=@im=fcitx' >> ~/.xprofile

1. How to disable laptop suspending on lid close ?
    set in /etc/systemd/logind.conf:
    HandleLidSwitch=ignore
    HandleLidSwitchDocked=ignore

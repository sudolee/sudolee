1. make Tab auto-completion case-insensitive in Bash:
    1.1 In order to make bash case-insensitive for to current user:
    ```
    # If ~/.inputrc doesn't exist yet: First include the original /etc/inputrc
    # so it won't get overriden
    if [ ! -a ~/.inputrc ]; then echo '$include /etc/inputrc' > ~/.inputrc; fi

    # Add shell-option to ~/.inputrc to enable case-insensitive tab completion
    echo 'set completion-ignore-case On' >> ~/.inputrc
    ```
    1.2 To Make the changes systemwide:
    ```
    # add option to /etc/inputrc to enable case-insensitive tab completion for all users
    echo 'set completion-ignore-case On' >> /etc/inputrc
    # you may have to use this instead if you are not a superuser:
    echo 'set completion-ignore-case On' | sudo tee -a /etc/inputrc
    ```

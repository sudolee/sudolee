#!/bin/bash
#
# Configure cscope for vim...
#

mkdir -p ~/.vim/plugin/
rm -f ~/.vim/plugin/cscope_macros.vim
cp -f cscope_macros.vim ~/.vim/plugin/
echo ">>> Configure cscope for vim successful. <<<"

bash ./my_vimrc.sh

exit

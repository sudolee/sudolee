#!/bin/bash -e



rm -fr ~/{.vimrc,.vim}
cp -f vimrc ~/.vimrc

curl -fLo ~/.vim/autoload/plug.vim --create-dirs \
  https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim

vim +PlugInstall +qall

#!/bin/sh

#sudo cp -f colors/* /usr/share/vim/vim7*/colors/
rm -rf ~/.vim
rm -f ~/.vimrc
cp -rf vim ~/.vim
cp -f vimrc ~/.vimrc


echo \
'------------------------------------------------------------
Installed plugins for vim:
	+ taglist.vim
	+ cscope_macros.vim
	+ NERD_tree.vim
	+ snipMate.vim
	Any issure, pls see ~/.vim/doc/ for detials
------------------------------------------------------------'

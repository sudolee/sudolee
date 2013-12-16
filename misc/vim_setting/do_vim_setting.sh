#!/bin/sh

rm -fr ~/.vim
rm -f ~/.vimrc
cp -fr vim ~/.vim
cp -f vimrc ~/.vimrc


echo \
'------------------------------------------------------------
Installed plugins for vim:
	+ taglist.vim
	+ cscope_macros.vim
	+ NERD_tree.vim
	Any issure, pls see ~/.vim/doc/ for detials
------------------------------------------------------------'

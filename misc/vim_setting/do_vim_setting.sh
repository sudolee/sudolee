#!/bin/sh

rm -rf ~/.vim
rm -f ~/.vimrc
cp -rf vim ~/.vim
cp -f vimrc ~/.vimrc


echo \
'------------------------------------------------------------
Installed plugins for vim:
	+ taglist.vim
	+ cscope_macros.vim
	+ snipMate.vim
	+ c.vim
	+ neocomplcache
	+ NERD_tree.vim
	Any issure, pls see ~/.vim/doc/ for detials
------------------------------------------------------------'

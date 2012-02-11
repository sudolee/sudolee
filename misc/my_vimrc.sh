#!/bin/bash
#
# Configure vim and vim-gnome...
#

rm -f ~/.vimrc
echo "\
\"
\" Just copy this text document to ~/.vimrc for setting vim
\"

if v:lang =~ \"utf8$\" || v:lang =~ \"UTF-8$\"
	set fileencodings=utf-8,latin1
endif

set nu
set cursorline
set tabstop=4
set shiftwidth=4
set hlsearch
set incsearch
set bg=dark
set history=1024
set wrap
\"set nowrap
set noexpandtab
set laststatus=2
set showcmd
set smartcase
set gcr=a:blinkon0
filetype plugin indent on
\"set statusline=%<%F%h%m%r%h%w%y\ %=\ line:%l\,%L\ %P
set statusline=%<%F%h%m%r%h%w%y\ %{&ff}\ %{strftime(\\\"%d/%m/%y\ -\ %H:%M\\\")}%=\ ascii:%b\ lin:%l\,%L\ %P\
" >> ~/.vimrc

rm -f ~/.gvimrc
echo "\
colors torte
" >> ~/.gvimrc

echo ">>> Configure vim(and vim-gnome) successful. <<<"

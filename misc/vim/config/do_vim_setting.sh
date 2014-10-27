#!/bin/bash -e

rm -fr ~/.vim
rm -f ~/.vimrc
cp -fr vim ~/.vim
cp -f vimrc ~/.vimrc

echo \
'::Installed plugins for vim:
    + cscope_macros.vim
    + taglist.vim
    + snipMate
    + snippets
    + clang_complete
    + tlib
    + mw-utils
:: Any issure, pls see ~/.vim/doc/ for detials.'

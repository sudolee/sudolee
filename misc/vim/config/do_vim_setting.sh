#!/bin/bash -e

rm -fr ~/.vim
rm -f ~/.vimrc
cp -fr vim ~/.vim
cp -f vimrc ~/.vimrc

echo \
'::Installed plugins for vim:
    + cscope_macros.vim
    + taglist-plus
    + clang_complete
    + minibufexplorerpp
    + better-snipmate-snippet
:: Any issure, pls see ~/.vim/doc/ for detials.'

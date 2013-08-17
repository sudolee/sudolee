#!/bin/sh

git config --global core.editor 'vim'
git config --global user.name 'matti'
git config --global user.email 'desoxydate@gmail.com'

if [ $1 ];then
	echo machine code.google.com login desoxydate@gmail.com password $1 > ~/.netrc
	echo machine learning-wiki.google.com login desoxydate@gmail.com password $1 >> ~/.netrc
	echo machine wiki.learning-wiki.google.com login desoxydate@gmail.com password $1 >> ~/.netrc
else
	echo 'Pls input sudolee s passwd as first parameter.'
fi

git config --global color.branch always 
git config --global color.diff always 
git config --global color.ui always
git config --global color.status always

return

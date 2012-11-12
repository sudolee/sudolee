#!/bin/sh

git config --global core.editor 'vim'
git config --global user.name 'sudolee'
git config --global user.email 'desoxydate@gmail.com'

if [ $1 ];then
	echo machine code.google.com login desoxydate@gmail.com password $1 > ~/.netrc
	echo machine learning-wiki.google.com login desoxydate@gmail.com password $1 >> ~/.netrc
	echo machine wiki.learning-wiki.google.com login desoxydate@gmail.com password $1 >> ~/.netrc
else
	echo 'Pls input sudolee s passwd as first parameter.'
fi

exit 0

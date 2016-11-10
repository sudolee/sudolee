# convert to dos format
find -name '*.h'   | xargs unix2dos
find -name '*.c'   | xargs unix2dos
find -name '*.cc'  | xargs unix2dos
find -name '*.cpp' | xargs unix2dos

# add BOM head
find -name '*.h'   >  __s.sh
find -name '*.c'   >> __s.sh
find -name '*.cc'  >> __s.sh
find -name '*.cpp' >> __s.sh

vim "+%s/^/vim \"+set bobm\" \"+x\" /g" "+x" __s.sh

bash  __s.sh
rm -f __s.sh

#!/usr/bin/python
#
# you can try 3 times to guess a magic number.
#

retry = 3
magic_no = 12

while retry:
	guess = int(raw_input("Please enter an integer: "))

	if guess == magic_no:
		print "> Great, your guess is right."
		break
	elif guess > magic_no:
		print "> guess is larger than the val."
	else:
		print "> guess is less than the val."

	print ">> try again!"
	retry -= 1
else:
	print "> sorry, you are loss. @_@!!!"

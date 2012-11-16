#!/usr/bin/python
#
# you can try 3 times to guess a magic number.
#

magic_no = 12

#for retry in range(3, 0, -1):
for retry in [1, 2, 3]:
	guess = int(raw_input("Please enter an integer: "))

	if guess == magic_no:
		print "> Great, your guess is right!"
		break
	elif guess > magic_no:
		print "> guess is larger than the val."
	else:
		print "> guess is less than the val."

	print ">> try again!"
else:
	print "> sorry, you are loss. @_@!!!"

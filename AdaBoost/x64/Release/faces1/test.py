import glob

files = glob.glob('./*.jpg')
oup = open('ann1.txt', 'w')
for f in files:
	oup.write(f)


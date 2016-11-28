from glob import iglob
import shutil
import os

dest = open('total.vec','wb')

fs = ['faces/pos-samples.vec', 'faces1.vec']

def bytes_from_file(filename, chunksize=576):
    with open(filename, 'rb') as f:
        print f
        while True:
            chunk = f.read(chunksize)
            print len(chunk)
            if chunk:
                for b in chunk:
                    yield b
            else:
                break


for file in fs:
	shutil.copyfileobj(open(file, 'rb'), dest)

dest.close()

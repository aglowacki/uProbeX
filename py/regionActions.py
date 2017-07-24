
'''
Region action functions have to accept 6 arguments. 
func ( centerX, centerY, width, height, factorX, factorY )

centerX, centerY, width and height are in microprobe coordinates.
factorX and factorY are for the user to define which direction for the scan
'''

from epics import caput

#my custom transform function
def createScanRecord(name, cX, cY, width, height, fX, fY):
   caput('invidioc:scan1.P1SP', cX)
   caput('invidioc:scan1.P2SP', cY)
   caput('invidioc:scan1.P3SP', width)
   caput('invidioc:scan1.P4SP', height)

def saveToFile(name, cX, cY, width, height, fX, fY):
   msg = name+' '+str(cX)+' '+str(cY)+' '+str(width)+' '+str(height)+' '+str(fX)+' '+str(fY)
   f = open('batchScan.txt', 'w')
   f.write(msg)
   f.close()

def messageBox(name, cX, cY, width, height, fX, fY):
   import tkMessageBox
   tkMessageBox.showinfo(title="createScanRecord", message=msg)

def reallyLongScan(name, cX, cY, width, height, fX, fY):
   for i in range(1,100000000):
      x = (i*i) / (i * 2)

if(__name__ == '__main__'):
    saveToFile('abc', 0.1, 0.2, 0.3, 0.4, 0.5, 0.6)

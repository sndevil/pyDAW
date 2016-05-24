from ctypes import *

class SoundSample(Structure):
    _fields_ = [("d", c_double * 2)]


dll = cdll.LoadLibrary("C:\Documents and Settings\Mobarezi\My Documents\Visual Studio 2010\Projects\pyDAWlib\Debug\pyDAWlib.dll")


mixer = dll.NewMixer()
outfile = dll.CreateOutfile("D:\\123.wav",16,2,44100)

path = c_wchar_p("C:\Documents and Settings\Mobarezi\My Documents\Downloads\Music\Slicing Wood.wav")
dll.AddTrack(mixer,path,0)

out = SoundSample()
length = c_long(dll.GetLength(mixer))
print('length of file is ' + str(length))
for i in range(0,length.value-1,2):
    
    dll.WriteOutfile(outfile,mixer,i)
    #dll.GetSample(mixer,i,out.d)
    #print ('P called' + str(i))

dll.CloseOutfile(outfile)

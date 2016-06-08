from ctypes import *

class SoundSample(Structure):
    _fields_ = [("d", c_double * 2)]

class EQBands(Structure):
    _fields_ = [("f", c_double * 3)]


dll = cdll.LoadLibrary("C:\Documents and Settings\Mobarezi\My Documents\Visual Studio 2010\Projects\pyDAWlib\Debug\pyDAWlib.dll")


mixer = dll.NewMixer()
outfile = dll.CreateOutfile("D:\\123.wav",16,2,44100)

path = c_wchar_p("D:\\Slicing Wood.wav")
dll.AddTrack(mixer,path,0)

eqband = EQBands()
eqband.f[0] = c_double(20000)
eqband.f[1] = c_double(10000)
eqband.f[2] = c_double(-1)

dll.AddReverb(mixer,c_int(0),c_int(1000),c_double(100))
#dll.AddEQ(mixer,c_int(0),eqband.f,c_int(1))
#dll.AddHighpass(mixer,c_int(0),c_int(5000),c_int(1),c_int(100))

out = SoundSample()
length = c_long(dll.GetLength(mixer) * 2)
print('length of file is ' + str(length))
for i in range(0,length.value-1,2): 
    dll.WriteOutfile(outfile,mixer,i)
    if (i % 10000 == 0):
        print (str(i/length.value*100)+'%')
    #dll.GetSample(mixer,i,out.d)
    #print ('P called' + str(i))

dll.CloseOutfile(outfile)

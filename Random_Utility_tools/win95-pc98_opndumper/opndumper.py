# -*- coding: utf-8 -*-
"""
OPN binary patch extractor

Based on information found at
    http://madscient.hatenablog.jp/entry/2013/08/03/052840
    
"""

import struct
from sys import argv

class OPNOpStruct:
    def __init__(self):
        self.enabled = False
        self.ar = 0x00
        self.d1r = 0x00
        self.d2r = 0x00
        self.rr = 0x0
        self.sl = 0x0
        self.tl = 0x00
        self.ksr = 0x0
        self.mul = 0x0
        self.dt = 0x0
        self.ams = 0x0
    #end def
#end class

class OPNStruct:
    def __init__(self):
        self.op = [OPNOpStruct() for i in xrange(4)]
        self.feedback = 0x00
        self.algorithm = 0x0
        self.lfo_waveform = 0x00
        self.lfo_syncdelaytime = 0x00
        self.lfo_spd = 0x00
        self.lfo_pmd = 0x00
        self.lfo_amd = 0x00
        self.pm_sens = 0x0
    #end def
#end class


class OPMOpStruct:
    def __init__(self):
        self.enabled = False
        self.ar = 0x00
        self.d1r = 0x00
        self.d2r = 0x00
        self.rr = 0x0
        self.sl = 0x0
        self.tl = 0x00
        self.ksr = 0x0
        self.mul = 0x0
        self.dt1 = 0x0
        self.dt2 = 0x0
        self.ams = 0x0    
    #end def
#end class

class OPMStruct:
    def __init__(self):
        self.op = [OPMOpStruct() for i in xrange(4)]
        self.feedback = 0x00
        self.algorithm = 0x0
        self.lfo_waveform = 0x00
        self.lfo_syncdelaytime = 0x00
        self.lfo_spd = 0x00
        self.lfo_pmd = 0x00
        self.lfo_amd = 0x00
        self.pm_sens = 0x0
    #def init
#end class

def printOPNPatch(opn_patch):
    print "Patch:"
    print "{0} {1} {2} {3} {4} {5} {6} {7}".format(
        opn_patch.feedback,
        opn_patch.algorithm,
        opn_patch.lfo_waveform,
        opn_patch.lfo_syncdelaytime,
        opn_patch.lfo_spd,
        opn_patch.lfo_pmd,
        opn_patch.lfo_amd,
        opn_patch.pm_sens    
    )        
    for j in xrange(4):
        print "OP{11}: {0} {1} {2} {3} {4} {5} {6} {7} {8} {9} {10}".format(
        opn_patch.op[j].enabled,
        opn_patch.op[j].ar,
        opn_patch.op[j].d1r,
        opn_patch.op[j].d2r,
        opn_patch.op[j].rr,
        opn_patch.op[j].sl,
        opn_patch.op[j].tl,
        opn_patch.op[j].ksr,
        opn_patch.op[j].mul,
        opn_patch.op[j].dt,
        opn_patch.op[j].ams,
        j
    )
    #end for
#end def

def readOPNBin(f, opn_patches):
    readlim = 0x2000
    readcnt = 0
    patchcnt = 0    
    
    # read OPN bin from N88-BASIC data blob
    while readcnt < readlim:
        #ADDR 0 - FB / ALG
        byte = struct.unpack('B', f.read(1))[0]
        readcnt = readcnt + 1
        opn_patches[patchcnt].algorithm = (byte & 0x7)
        opn_patches[patchcnt].feedback = ((byte>>3) & 0x7)
        
        # ADDR 1 to 4 - (AR op 1to4)
        for n in xrange(4):
            byte = struct.unpack('B', f.read(1))[0]
            readcnt = readcnt + 1
            opn_patches[patchcnt].op[n].ar = 0x1F - (byte & 0x1F)
        #end for
        
        # ADDR 5 - OP enable
        byte = struct.unpack('B', f.read(1))[0]
        readcnt = readcnt + 1
        for n in xrange(4):
            opn_patches[patchcnt].op[n].enabled = (((byte>>n) & 0x1) > 0)
        #end for
        
        # ADDR 6 to 9 - (D1R op 1to4)            
        for n in xrange(4):
            byte = struct.unpack('B', f.read(1))[0]
            readcnt = readcnt + 1
            opn_patches[patchcnt].op[n].d1r = 0x1F - (byte & 0x1F)
        #end for
        
        # ADDR 10 - LFO Waveform
        byte = struct.unpack('B', f.read(1))[0]
        readcnt = readcnt + 1
        opn_patches[patchcnt].lfo_waveform = (byte & 0x1F)
        
        # ADDR 11 to 14 - (D2R op 1to4)
        for n in xrange(4):
            byte = struct.unpack('B', f.read(1))[0]
            readcnt = readcnt + 1
            opn_patches[patchcnt].op[n].d2r = 0x1F - (byte & 0x1F)
        #end for    
        
        # ADDR 15 - LFO sync
        byte = struct.unpack('B', f.read(1))[0]
        readcnt = readcnt + 1
        opn_patches[patchcnt].lfo_syncdelaytime = (byte & 0x1F)
        
        # ADDR 16 to 19 - (RR op 1to4)
        for n in xrange(4):
            byte = struct.unpack('B', f.read(1))[0]
            readcnt = readcnt + 1
            opn_patches[patchcnt].op[n].rr = 0xF - (byte & 0xF)
        #end for    
        
        # ADDR 20 LFO speed
        byte = struct.unpack('B', f.read(1))[0]
        readcnt = readcnt + 1
        opn_patches[patchcnt].lfo_spd = (byte & 0xFF)

        # ADDR 21 to 24 - (SL op 1to4)
        for n in xrange(4):
            byte = struct.unpack('B', f.read(1))[0]
            readcnt = readcnt + 1
            opn_patches[patchcnt].op[n].sl = 0xF - (byte & 0xF)
        #end for    
        
        # ADDR 25 - LFO PMD
        byte = struct.unpack('B', f.read(1))[0]
        readcnt = readcnt + 1
        opn_patches[patchcnt].lfo_pmd = (byte & 0xFF)
        
        # ADDR 26 to 29 - (TL op 1to4)
        for n in xrange(4):
            byte = struct.unpack('B', f.read(1))[0]
            readcnt = readcnt + 1
            opn_patches[patchcnt].op[n].tl = 0x7F - (byte & 0x7F)
        #end for
        
        # ADDR 30 - LFO AMD
        byte = struct.unpack('B', f.read(1))[0]
        readcnt = readcnt + 1
        opn_patches[patchcnt].lfo_amd = (byte & 0xFF)
        
        # ADDR 31 to 34 - (KSR op 1to4)
        for n in xrange(4):
            byte = struct.unpack('B', f.read(1))[0]
            readcnt = readcnt + 1
            opn_patches[patchcnt].op[n].ksr = (byte & 0x3)
        #end for    
            
        # ADDR 35 - PM Sensitivity
        byte = struct.unpack('B', f.read(1))[0]
        readcnt = readcnt + 1
        opn_patches[patchcnt].pm_sens =  (byte & 0xF)
        
        # ADDR 36 to 39 - (MUL op 1to4)
        for n in xrange(4):
            byte = struct.unpack('B', f.read(1))[0]
            readcnt = readcnt + 1
            opn_patches[patchcnt].op[n].mul = (byte & 0xF)
        #end for    
        
        # ADDR 40 - skip
        byte = struct.unpack('B', f.read(1))[0]
        readcnt = readcnt + 1
        
        # ADDR 41 to 44 - (DT op 1to4)
        for n in xrange(4):
            byte = struct.unpack('B', f.read(1))[0]
            readcnt = readcnt + 1
            opn_patches[patchcnt].op[n].dt = (byte & 0xF)
        #end for
        
        # ADDR 45 - skip
        byte = struct.unpack('B', f.read(1))[0]
        readcnt = readcnt + 1
        
        # ADDR 46 to 49 - (AM Sense op 1to4)
        for n in xrange(4):
            byte = struct.unpack('B', f.read(1))[0]
            readcnt = readcnt + 1
            opn_patches[patchcnt].op[n].ams = (byte & 0xF)
        #end for

        while (readcnt % 64) is not 0:
            byte = struct.unpack('B', f.read(1))[0]
            readcnt = readcnt + 1    
        #end while
        
        printOPNPatch(opn_patches[patchcnt])        
        
        patchcnt = patchcnt + 1
        print "CUNT L {0}".format(patchcnt)
        
    #end while
    return opn_patches
#end def

def convOPNToOPM(opn_patches, opm_patches):
    for i in xrange(128):
        for j in xrange(4):
            opm_patches[i].op[j].enabled = opn_patches[i].op[j].enabled
            opm_patches[i].op[j].ar = opn_patches[i].op[j].ar
            opm_patches[i].op[j].d1r = opn_patches[i].op[j].d1r
            opm_patches[i].op[j].d2r = opn_patches[i].op[j].d2r
            opm_patches[i].op[j].rr = opn_patches[i].op[j].rr
            opm_patches[i].op[j].sl = opn_patches[i].op[j].sl
            opm_patches[i].op[j].tl = opn_patches[i].op[j].tl
            opm_patches[i].op[j].ksr = opn_patches[i].op[j].ksr
            opm_patches[i].op[j].mul = opn_patches[i].op[j].mul
            opm_patches[i].op[j].dt1 = opn_patches[i].op[j].dt
            opm_patches[i].op[j].dt2 = 0x00 # TODO verify
            opm_patches[i].op[j].ams = opn_patches[i].op[j].ams
        #end for
        
        opm_patches[i].feedback = opn_patches[i].feedback
        opm_patches[i].algorithm = opn_patches[i].algorithm
        opm_patches[i].lfo_waveform = opn_patches[i].lfo_waveform 
        opm_patches[i].lfo_syncdelaytime = opn_patches[i].lfo_syncdelaytime 
        opm_patches[i].lfo_spd = opn_patches[i].lfo_spd
        opm_patches[i].lfo_pmd = opn_patches[i].lfo_pmd 
        opm_patches[i].lfo_amd = opn_patches[i].lfo_amd 
        opm_patches[i].pm_sens = opn_patches[i].pm_sens 
    #end for
    
    return opm_patches
#end def

def writeOPMBank(opm_patches, filename):
    with open(filename, 'w') as f:
        f.write('//VOPM Voice data\n')
        f.write('//OPN -> OPM (N88-BASIC FM patch binary data)\n\n')
        
        for n in xrange(128):
            f.write('@:{0} OPN Patch {0:03d}\n'.format(n))
            f.write('//  LFRQ AMD PMD WF NFRQ\n')
            f.write('LFO: {0} {1} {2} {3} {4}\n'.format(
                opm_patches[n].lfo_spd,
                opm_patches[n].lfo_amd,
                opm_patches[n].lfo_pmd,
                opm_patches[n].lfo_waveform,
                opm_patches[n].lfo_syncdelaytime
                )
            )
            f.write('// PAN FL CON AMS PMS SLOT NE\n')
            f.write('CH: 64 {0}  {1} 0   0  127  0\n'.format(
                opm_patches[n].feedback,
                opm_patches[n].algorithm
                )
            )
            f.write('//  AR D1R D2R RR D1L  TL KS MUL DT1 DT2 AMS-EN\n')
            f.write('M1: {0} {1} {2} {3} {4} {5} {6} {7} {8} {9} {10}\n'.format(
                opm_patches[n].op[0].ar,
                opm_patches[n].op[0].d1r,
                opm_patches[n].op[0].d2r,
                opm_patches[n].op[0].rr,
                opm_patches[n].op[0].sl,
                opm_patches[n].op[0].tl,
                opm_patches[n].op[0].ksr,
                opm_patches[n].op[0].mul,
                opm_patches[n].op[0].dt1,
                opm_patches[n].op[0].dt2,
                opm_patches[n].op[0].ams
                )
            )
            f.write('C1: {0} {1} {2} {3} {4} {5} {6} {7} {8} {9} {10}\n'.format(
                opm_patches[n].op[2].ar,
                opm_patches[n].op[2].d1r,
                opm_patches[n].op[2].d2r,
                opm_patches[n].op[2].rr,
                opm_patches[n].op[2].sl,
                opm_patches[n].op[2].tl,
                opm_patches[n].op[2].ksr,
                opm_patches[n].op[2].mul,
                opm_patches[n].op[2].dt1,
                opm_patches[n].op[2].dt2,
                opm_patches[n].op[2].ams
                )
            )
            f.write('M2: {0} {1} {2} {3} {4} {5} {6} {7} {8} {9} {10}\n'.format(
                opm_patches[n].op[1].ar,
                opm_patches[n].op[1].d1r,
                opm_patches[n].op[1].d2r,
                opm_patches[n].op[1].rr,
                opm_patches[n].op[1].sl,
                opm_patches[n].op[1].tl,
                opm_patches[n].op[1].ksr,
                opm_patches[n].op[1].mul,
                opm_patches[n].op[1].dt1,
                opm_patches[n].op[1].dt2,
                opm_patches[n].op[1].ams
                )
            )
            f.write('C2: {0} {1} {2} {3} {4} {5} {6} {7} {8} {9} {10}\n'.format(
                opm_patches[n].op[3].ar,
                opm_patches[n].op[3].d1r,
                opm_patches[n].op[3].d2r,
                opm_patches[n].op[3].rr,
                opm_patches[n].op[3].sl,
                opm_patches[n].op[3].tl,
                opm_patches[n].op[3].ksr,
                opm_patches[n].op[3].mul,
                opm_patches[n].op[3].dt1,
                opm_patches[n].op[3].dt2,
                opm_patches[n].op[3].ams
                )
            )
            f.write('\n\n')
        #end for
    #end with
#end def

def writeFMMIDIBank(opn_patches, filename):
    with open(filename, 'w') as f:
        for n in xrange(128):
            f.write('@{0}\n'.format(n))
            f.write('{0} {1} {2}\n'.format(
                opn_patches[n].algorithm,
                opn_patches[n].feedback,
                opn_patches[n].lfo_waveform)
            )
            for m in [0,2,1,3]:
               f.write('{0} {1} {2} {3} {4} {5} {6} {7} {8} {9}\n'.format(
                   opn_patches[n].op[m].ar,
                   opn_patches[n].op[m].d1r,
                   opn_patches[n].op[m].d2r,  # SR
                   opn_patches[n].op[m].rr,
                   opn_patches[n].op[m].sl,
                   opn_patches[n].op[m].tl,
                   opn_patches[n].op[m].ksr,
                   opn_patches[n].op[m].mul,
                   opn_patches[n].op[m].dt,
                   opn_patches[n].op[m].ams
                   )
            #end for
            )
            f.write('\n\n')
        #end for
    #end with
#end def

def main(filename):
    with open(filename, 'rb') as f:
        # collect OPN preset data        
        opn_patches = [OPNStruct() for i in range(256)]
        opn_patches = readOPNBin(f, opn_patches)
        
        # test if prints are correct
        for n in xrange(256):
            printOPNPatch(opn_patches[n])
        #end for
        
        # convert to OPM
        opm_patches = [[OPMStruct() for i in xrange(128)] for j in xrange(2)]        
        for n in xrange(2):
            opm_patches[n] = convOPNToOPM(opn_patches[128*n:128*(n+1)], opm_patches[n])
        #end for
        
        # write to .OPM format patches
        #writeOPMBank(opm_patches[0], 'OUT1.OPM')
        #writeOPMBank(opm_patches[1], 'OUT2.OPM')
        writeFMMIDIBank(opn_patches, 'programs.txt')
    #end with
#end def

if __name__ == '__main__':
    main(argv[0] if argv.count is 1 else 'pc98opnbin')
#end if
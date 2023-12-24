import csv
import sys

saveddict = {}

with open(sys.argv[1], 'r') as fp:
    data = csv.reader(fp)
    data = list(data)

    tonetype = ""
    tonebank = ""
    tonename = ""
    toneMSB  = -1
    toneLSB  = -1
    tonePC   = -1
    category = ""

    cur_tuple = None

    for row in range(len(data)):
        if row == 0:
            continue
        newtype, newbank, _, tonename, toneMSB, toneLSB, tonePC, category = data[row]

        if newtype != tonetype or newbank != tonebank:
            tonetype = newtype
            tonebank = newbank
            cur_tuple = (tonetype, tonebank)
            saveddict[cur_tuple] = {}

        if toneMSB not in saveddict[cur_tuple]:
            saveddict[cur_tuple][toneMSB] = {}
        
        if toneLSB not in saveddict[cur_tuple][toneMSB]:
            saveddict[cur_tuple][toneMSB][toneLSB] = {}

        saveddict[cur_tuple][toneMSB][toneLSB][tonePC] = tonename
        

# print set list
#prefix = "Roland INTEGRA-7"
prefix = "General MIDI Level 2"
print()
print("[List of Patch Sets]")
print(f"NumSets={len(saveddict)}")
for i, n in enumerate(saveddict):
    print(f"Set{i+1}={prefix} {n[0]}:{n[1]}")

# print bank list
for n in saveddict:
    print()
    print(f"[{prefix} {n[0]}:{n[1]}]")
    print("Type=Banks")
    print("Method=CC32")
    defaultprinted = False
    for o in saveddict[n]:
        for p in saveddict[n][o]:
            if not defaultprinted:
                print(f"Default={p}/{o}")
                defaultprinted = True
            print(f"Bank{p}/{o}={n[0]}: {n[1]} Bank {p}/{o}")

# print patch list
for n in saveddict:
    for o in saveddict[n]:
        for p in saveddict[n][o]:
            print()
            print(f"[{n[0]}: {n[1]} Bank {p}/{o}]")
            print("Type=Patches")
            for e in saveddict[n][o][p]:
                print(f"{e}={saveddict[n][o][p][e]}")
                    

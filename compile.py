import os
import sys

if len(sys.argv)==2:
  print "Will only compile %s"%sys.argv[1]
  f=sys.argv[1]
  os.system("cp %s main.cpp"%f)
  os.system("make")
  os.system("mv main out/%s"%f.replace(".cpp","").replace("scripts/",""))
  os.system("rm main.cpp main.o")

else:
  for f in os.listdir("./scripts/"):
    if not ".cpp" in f[-4:]:
      continue
    print "**** Compiling %s"%f
    os.system("cp scripts/%s main.cpp"%f)
    os.system("make")
    os.system("mv main out/%s"%f.replace(".cpp",""))
    os.system("rm main.cpp main.o")


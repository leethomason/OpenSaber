from pathlib import Path
import sys
import shutil
import os

actions = [
    {
        # the actual wav12 library
        "src" : "../../wav12ly/wav12", 
        "dst" : ["./i2saudio2/src/wav12", "./lsm6d/src/wav12", "./saber/src/wav12"], 
        "files" : ["*.cpp", "*.h"]
    },{
        # useful files from the wav12 project
        "src" : "../../wav12ly",
        "dst" : ["./i2saudio2", "./saber"],
        "files" : ["manifest.h", "manifest.cpp", "i2saudiodrv.cpp", "i2saudiodrv.h", "spistream.cpp", "spistream.h"]
    },{
        "src" : "./fpm/include/fpm",
        "dst" : ["./i2saudio2/src/fpm/include/fpm",  "./lsm6d/src/fpm", "./saber/src/fpm/include/fpm"],
        "files" : ["*.hpp"]
    },{
        "src" : "./nada_flashmem",
        "dst" : ["./i2saudio2/src/nada_flashmem", "./lsm6d/src/nada_flashmem"],
        "files" : ["*.h", "*.cpp"]
    },{
        "src" : "./util",
        "dst" : ["./i2saudio2/src/util", "./lsm6d/src/util"],
        "files" : ["*.h", "*.cpp"]
    }
]

copy = False
if len(sys.argv) == 2 and sys.argv[1] == "--x":
    copy = True

for action in actions:
    for f in action['files']:
        src = Path(action['src']).glob(f)
        for s in src:
            for d in action['dst']:
                dst = Path(d) / s.name
                #check(s, dst)
                print(s, "->", dst)
                if copy:
                    os.makedirs(d, exist_ok=True)
                    shutil.copyfile(s, dst)
    

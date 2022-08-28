from pathlib import Path
import sys
import shutil

actions = [
    {
        "src" : "../../wav12ly/wav12", 
        "dst" : ["./i2saudio2/src/wav12", "./saber/src/wav12"], 
        "files" : ["*.cpp", "*.h"]
    },{
        "src" : "../../wav12ly",
        "dst" : ["./i2saudio2", "./saber"],
        "files" : ["manifest.h", "manifest.cpp", "i2saudiodrv.cpp", "i2saudiodrv.h", "spistream.cpp", "spistream.h"]
    },{
        "src" : "./fpm/include/fpm",
        "dst" : ["./i2saudio2/src/fpm/include/fpm", "./saber/src/fpm/include/fpm"],
        "files" : ["*.hpp"]
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
                    shutil.copyfile(s, dst)
    

from pathlib import Path
import sys
import shutil

actions = [
    {
        # wav12ly library files
        "src" : "../../wav12ly/wav12", 
        "dst" : ["./i2saudio2/src/wav12", "./saber/src/wav12"], 
        "files" : ["*.cpp", "*.h"]
    },{
        # wav12ly utility files
        "src" : "../../wav12ly",
        "dst" : ["./i2saudio2", "./saber"],
        "files" : ["manifest.h", "manifest.cpp"]
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
    

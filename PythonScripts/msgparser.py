import sys
import re

# cur file, end file
if(len(sys.argv) != 3):
    sys.exit(1)

linenum = 1

fstr = ""
print(sys.argv[1])
with open(sys.argv[1], "r") as infile:
    for line in infile:
        if linenum >= 16 and linenum <= 49:
            fstr += line
        linenum += 1

print(fstr)
lines = re.split('#define', fstr)

resDict = []
for line in lines:
    if(line == ''):
        continue
    print("The line is ")
    print(line)
    errmsg = re.search('//\!<(.*)', line).group(1)
    name = re.search('(FINGERPRINT_[A-Z]*)\s', line).group(1)

    # sanitize errmsg
    errmsg = ' '.join(errmsg.split())
    resDict.append([name, errmsg])

outfile = open(sys.argv[2], 'w')
outfile.truncate(0)

for [name, errmsg] in resDict:
    outfile.write("case {}:\n".format(name))
    outfile.write("\tSerial.println(\"{}\");\n".format(errmsg))
    outfile.write("\tbreak;\n")


outfile.close()


#  //split off message
# FINGERPRINT_.*\s


# define FINGERPRINT_OK 0x00 //!< Command execution is complete

# switch(val) {
# case FINGERPRINT_OK:
#   "Command execution is complete."
#   break;
# }
#{FINGERPRINT_OK, }

from os import walk
import os
import sys
import filecmp

PASSED = True


def get_files(dir_name):
    fns = os.listdir(dir_name)
    fns.sort()
    return fns

if __name__ == "__main__":

    if len(sys.argv) != 3:
        print('Usage: python3 rcheck.py <your output dir> <test output dir>')
        exit()
    out_dir = sys.argv[1]
    out_dir_files = get_files(out_dir)
    print("Files identified: " + str(out_dir_files))

    exp_dir = sys.argv[2]
    exp_dir_files = get_files(exp_dir)

    # does the output dir contain the same list of files
    if [x.casefold() for x in out_dir_files] == [x.casefold() for x in exp_dir_files]:
        print('file list matched, comparing...')
        #compare each individual file
        for i in range(len(exp_dir_files)):
            out_fn = os.path.join(out_dir, out_dir_files[i])
            exp_fn = os.path.join(exp_dir, exp_dir_files[i])
            check = filecmp.cmp(out_fn, exp_fn)
            if not check:
                print("difference fond in %s, please debug using cmp and hexdump..." % out_dir_files[i])
                PASSED = False
    else:
        print("Directory content different from expected...")
        print("Files expected: " + str(exp_dir_files))
        PASSED = False
    
    if PASSED:
        print("PASSED!")
    else:
        print("FAILED!")
        

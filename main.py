import CppHeaderParser
import os
import sys


def find_files(path, extension, extension2, excludes):
    """Loops through the subdirectories below the source file, finding any .h or .cpp files.
        Appends paths to each file to appropriate to list. Any .h files go to list_all_file_paths. Any .cpp go to
        list_all_test_file_paths"""
    for root, dirs_list, files_list in os.walk(path, topdown=True):
        # Removes specified directories from os.walk,
        dirs_list[:] = [d for d in dirs_list if d not in excludes]
        for file_name in files_list:
            if os.path.splitext(file_name)[-1] == extension:
                list_all_files.append(file_name)
                list_all_file_paths.append(root + "/" + file_name)
            elif os.path.splitext(file_name)[-1] == extension2:
                list_all_test_files.append(file_name)
                list_all_test_file_paths.append(root + "/" + file_name)


def main(argv):
    """ Arguments passed via command line, checks for -exclude and -dir where:
    -exclude source src will exclude the source and src folders from the os.walk so that nothing
    below them in the folder structure will be parsed

    -dir specifies the directory from which to start the os.walk

    Flow:
    For each file path appended, identify each class and extract a list of methods within .h file.

    The list of methods is cast from a CppHeader class into str()

    Check to see if a test_<class>.cpp file exists, if it doesn't, creates the file and writes the includes from the
    original .h file.

    If the file exists, passes the above line and compares the methods in the file to the original .h and writes
    any new test cases that aren't there. """

    dir = os.getcwd()
    print("The initial CWD: " + os.getcwd())
    isExclusion = False
    isDirectory = False

    exclusionList = list()
    for v in argv:
        if v.startswith("-"):
            if "exclude" in v:
                isExclusion = True
            elif "dir" in v:
                isDirectory = True
        else:
            if isExclusion:
                exclusionList.append(v)
            elif isDirectory:
                dir = v

    find_files(dir, ".h", ".cpp", exclusionList)
    print("Processing...")
    for idx, x in enumerate(list_all_file_paths):
        # Assigns cppheader file to current file in the list
        cppheader = CppHeaderParser.CppHeader(x)
        # Clear list of classes to avoid re-iterating over previous file's classes
        list_all_classes = list()

        # Populate list of classes from our current cppheader file
        for i in cppheader.classes:
            list_all_classes.append(i)

        mod_list = list()
        # Change extension to .cpp
        for j in list_all_files:
            new_ext = os.path.splitext(j)[0] + ".cpp"
            mod_list.append(new_ext)

        # Check if test_x.cpp exists, if it does not, this creates the file and adds includes.
        if not os.path.exists(my_file_path + '/test/test_' + mod_list[idx]):
            # Adding the includes, handles both " " and < > cases
            with open(my_file_path + '/test/test_' + mod_list[idx], 'a') as f:
                f.write("#define GTEST_COUT std::cerr << [          ] [ WARNING ]" "\n")
                # Puts includes at the top of the file
                for incl in cppheader.includes:
                    f.write("#include " + " %s" % incl + "\n")
                f.write("\n")

        with open(my_file_path + '/test/test_' + mod_list[idx], 'a+') as f:
            for y in list_all_classes:
                # Creates a list of methods from current cppheader
                new_method_list = (CppHeaderParser.CppClass.get_all_method_names(cppheader.classes[y]))
                list_of_comp_method_strings = []
                create_curr_test_list()
                # Cast list of methods from CppParser class to string str()
                for i in new_method_list:
                    m1 = str(i)
                    list_of_comp_method_strings.append(m1)
                new_meths = list_check(list_of_comp_method_strings, old_methods)
                if not new_meths:
                    pass
                else:
                    for z in new_meths:
                        t1 = "test_" + str(y)
                        t2 = str(z)
                        # Template that matches GTest's basic function TEST(class_function, what_we_are_testing)
                        template = "TEST(" + t1 + ", " + t2 + ")\n{\n GTEST_COUT << This test has not been written yet! << std::endl; \n}"
                        f.write(template + "\n\n")


def create_curr_test_list():
    """ Builds a list of strings from existing test files to be compared using list_check()
    Each line in the test_<class>.cpp is sliced, and methods are appended. """
    for x, idx in enumerate(list_all_test_file_paths):
        with open(list_all_test_file_paths[x], 'r') as r:
            for line in r:
                if line.startswith("TEST("):
                    s = line[5:]
                    # Appends the class string, removing )\n and whitespaces
                    old_methods.append(s.split(',')[1][1:-2])


# Compares list of function names in source file to current test file, returns missing functions to be added
def list_check(list_x, list_y):
    """ Takes two lists and finds the difference using sets.
    Typical case is a list of methods, compared with a list of existing methods.
    Any methods left after the comparison are new methods to be added. """
    if not list(set(list_x) - set(list_y)):
        pass
    else:
        # print("This is list x: " + str(list_x))
        # print("This is list y: " + str(list_y))
        print("New methods added are -> " + str(list(set(list_x) - set(list_y))))
    return list(set(list_x) - set(list_y))


my_file_path = os.path.dirname(os.path.abspath(__file__))  # os.path.dirname(__file__)  # Directory containing main.py

list_all_test_files = []  # Initialise empty container list for test cpp files

list_all_files = []  # Initialise empty container list for header files below my_file_path in hierarchy

list_all_test_file_paths = []  # Initialise empty container list for full paths to test files

list_all_file_paths = []  # Initialise empty container list for full paths to header files

old_methods = []  # Initialise container for methods found when scanning directories

create_curr_test_list()

if __name__ == '__main__':
    main(sys.argv[1:])

print("\n")
print("Program has finished!")


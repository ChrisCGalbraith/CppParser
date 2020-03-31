import CppHeaderParser
import os
import argparse


def find_files(path, extension, extension2, excludes):
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


def main():
    print("Processing...")
    for x, idx in enumerate(list_all_file_paths):
        # Assigns cppheader file to current file in the list
        cppheader = CppHeaderParser.CppHeader(idx)
        # Clear list of classes to avoid re-iterating over previous file's classes
        list_all_classes = list()

        # Populate list of classes from our current cppheader file
        for i in cppheader.classes:
            # print("HERE WE ARE " + str(cppheader.variables))
            list_all_classes.append(i)

        mod_list = list()
        # Change extension to .cpp
        for j in list_all_files:
            new_ext = os.path.splitext(j)[0] + ".cpp"
            mod_list.append(new_ext)

        # Check if test_x.cpp exists, if it does not, this creates the file and adds includes.
        if not os.path.exists(my_file_path + '/test/test_' + mod_list[x]):
            # Adding the includes, handles both " " and < > cases
            with open(my_file_path + '/test/test_' + mod_list[x], 'a') as f:
                f.write("#define GTEST_COUT std::cerr << [          ] [ WARNING ]" "\n")
                # Puts includes at the top of the file
                for incl in cppheader.includes:
                    f.write("#include " + " %s" % incl + "\n")
                f.write("\n")

        with open(my_file_path + '/test/test_' + mod_list[x], 'a+') as f:
            for y in list_all_classes:
                # Creates a list of methods from current cppheader
                new_method_list = (CppHeaderParser.CppClass.get_all_method_names(cppheader.classes[y]))
                list_of_comp_method_strings = []
                create_curr_test_list()
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
    for x, idx in enumerate(list_all_test_file_paths):
        with open(list_all_test_file_paths[x], 'r') as r:
            for line in r:
                if line.startswith("TEST("):
                    s = line[5:]
                    # Appends the class string, removing )\n and whitespaces
                    old_methods.append(s.split(',')[1][1:-2])


# Compares list of function names in source file to current test file, returns missing functions to be added
def list_check(list_x, list_y):
    if not list(set(list_x) - set(list_y)):
        pass
    else:
        # print("This is list x: " + str(list_x))
        # print("This is list y: " + str(list_y))
        print("New methods added are -> " + str(list(set(list_x) - set(list_y))))
    return list(set(list_x) - set(list_y))


def get_inputs():
    excludes = list()
    print("================================================================================")
    print("This is the menu to select folders you want CppParser to ignore.")
    print("You will be asked to enter the name of each folder, one by one, that you wish to skip.")
    print("Folders must be below the current working directory (CWD) in hierarchy, and when specified, nothing within will be parsed.")
    print("================================================================================")
    print("Enter folder names exactly as seen in file explorer.")
    print("To finish, enter the word || done || or leave blank and press Enter.\n")
    while True:
        i = input("Enter name of folder(s) below CWD to exclude: ")
        if i.strip() == 'done' or 'none':
            return excludes
        else:
            excludes.append(i)


my_file_path = os.path.dirname(os.path.abspath(__file__))   # os.path.dirname(__file__)  # Directory containing main.py

list_all_test_files = []  # Initialise empty container list for test cpp files

list_all_files = []  # Initialise empty container list for header files below my_file_path in hierarchy

list_all_test_file_paths = []  # Initialise empty container list for full paths to test files

list_all_file_paths = []  # Initialise empty container list for full paths to header files

old_methods = []  # Initialise container for methods found when scanning directories

create_curr_test_list()

find_files(my_file_path, '.h', '.cpp', get_inputs())  # Populates list_all_files[] with .h files it has found

if __name__ == '__main__':
    main()

print("\n")
print("Program has finished!")









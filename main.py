import CppHeaderParser
import sys
import os


def find_files(path, extension):
    for root, dirs_list, files_list in os.walk(path):
        for file_name in files_list:
            if os.path.splitext(file_name)[-1] == extension:
                list_all_files.append(file_name)
                print(file_name)


def get_classes():
    for x in list_all_files:

        # Assigns cppheader file to current file in the list
        cppheader = CppHeaderParser.CppHeader(my_file_path + "/" + x)

        # Clear list of classes to avoid re-iterating over previous file's classes
        # Must do this to avoid KeyErrors
        list_all_classes.clear()

        # Populate list of classes from our current cppheader file
        for i in cppheader.classes:
            list_all_classes.append(i)

        for y in list_all_classes:

            # Creates a list of methods from current cppheader
            method_list = CppHeaderParser.CppClass.get_all_method_names(cppheader.classes[y])

            # Change extension to .cpp
            new_ext = os.path.splitext(x)[0]+".cpp"

            with open(my_file_path + '/test/' + new_ext, 'a') as f:
                for z in method_list:
                    t1 = str(z)
                    t2 = "WHAT_WE_ARE_TESTING"

                    # Template that matches GTest's basic TEST() function
                    template = "TEST(" + t1 + ", " + t2 + ")\n{\n // This is a placeholder test-function for " + t1 + "()\n}"
                    f.write(template + "\n\n")


my_file_path = os.path.dirname(__file__)  # Directory containing main.py

list_all_classes = []  # Initialise empty container list for classes within a file

list_all_files = []  # Initialise empty container list for header files below my_file_path in hierarchy

find_files(my_file_path, '.h')  # Populates list_all_files[] with .h files it has found

get_classes()

print("------------------------------------------------------------")









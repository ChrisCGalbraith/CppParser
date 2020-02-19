import CppHeaderParser
import sys
import os

try:
    cppHeader = CppHeaderParser.CppHeader("C:\Development\PycharmProjects\CppParser\instance_molten_core.h")
except CppHeaderParser.CppParseError as e:
    print(e)
    sys.exit(1)


def find_files(path, extension):
    for root, dirs_list, files_list in os.walk(path):
        for file_name in files_list:
            if os.path.splitext(file_name)[-1] == extension:
                list_all_files.append(file_name)
                # file_name_path = os.path.join(root, file_name)
                print(file_name)
                # print(file_name_path)   # This is the full path of the filter file


def create_gtest_templates():
    for y in list_all_classes:
        new_method_list = CppHeaderParser.CppClass.get_all_method_names(cppHeader.classes[y])
        with open(my_file_path + '/test/test_myproject.cpp', 'a') as f:
            for x in new_method_list:
                t1 = str(x)
                t2 = "WHAT_WE_ARE_TESTING"
                template = "TEST(" + t1 + ", " + t2 + ")\n{\n // This is a placeholder test-function for " + t1 + "()\n}"
                f.write(template + "\n\n")


def get_classes():
    for i in cppHeader.classes:
        list_all_classes.append(i)


list_all_classes = []  # Initialise empty container list for classes within a file

get_classes()  # Populate list_all_classes[]

my_file_path = os.path.dirname(__file__)  # Directory containing main.py

list_all_files = []  # Initialise empty container list for header files below my_file_path in hierarchy

find_files(my_file_path, '.h')  # Populates list_all_files[] with .h files it has found

create_gtest_templates()
print(list_all_classes)





import CppHeaderParser
import sys
import os

try:
    cppHeader = CppHeaderParser.CppHeader("C:\Development\PycharmProjects\CppParser\instance_molten_core.cpp")
except CppHeaderParser.CppParseError as e:
    print(e)
    sys.exit(1)

curr_class = cppHeader.classes["npc_firewalker"]


# This is the path where you want to search
my_path = r'C:\Development\PycharmProjects'
my_file_path = os.path.dirname(__file__)
# this is extension you want to detect
my_extension = '.cpp'   # this can be : .jpg  .png  .xls  .log .....


def find_files(path, extension):
    for root, dirs_list, files_list in os.walk(path):
        for file_name in files_list:
            if os.path.splitext(file_name)[-1] == extension:
                file_name_path = os.path.join(root, file_name)
                print(file_name)
                print(file_name_path)   # This is the full path of the filter file


def create_gtest_templates():
    list_method_names = CppHeaderParser.CppClass.get_all_method_names(curr_class)
    with open(my_file_path + '/test/test_myproject.cpp', 'w') as f:
        for x in list_method_names:
            t1 = str(x)
            t2 = "WHAT_WE_ARE_TESTING"
            template = "TEST(" + t1 + ", " + t2 + ")\n{\n // This is a placeholder test-function for " + t1 + "()\n}"
            f.write(template + "\n\n")


find_files('C:/Development/PycharmProjects', '.cpp')
create_gtest_templates()

print(my_file_path)
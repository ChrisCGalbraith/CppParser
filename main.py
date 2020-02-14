import CppHeaderParser
import sys

try:
    cppHeader = CppHeaderParser.CppHeader("C:/Users/Christopher/Desktop/VanillaRemix/src/server/scripts/EasternKingdoms/BlackrockMountain/MoltenCore/instance_molten_core.cpp")
except CppHeaderParser.CppParseError as e:
    print(e)
    sys.exit(1)

curr_class = cppHeader.classes["instance_molten_core"]



def create_test_file():
    with open('example.txt', 'w') as f:
        contents = CppHeaderParser.CppHeader('C:/Users/Christopher/Desktop/VanillaRemix/src/server/scripts/EasternKingdoms/BlackrockMountain/MoltenCore/instance_molten_core.cpp')
        f.write(str(contents))


def create_gtest_templates():
    list_method_names = CppHeaderParser.CppClass.get_all_method_names(curr_class)
    with open('test_myproject.cpp', 'a') as f:
        for x in list_method_names:
            t1 = str(x)
            t2 = "WHAT_WE_ARE_TESTING"
            template = "TEST(" + t1 + ", " + t2 + ")\n{\n // This is a placeholder test-function for " + t1 + "()\n}"
            f.write(template + "\n\n")


def generate_unit_tests(template):
    with open('test_myproject.cpp', 'a') as f:
        f.write("\n" + template)


create_gtest_templates()

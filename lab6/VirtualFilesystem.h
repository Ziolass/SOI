//
// Created by MZiol on 22/01/2018.
//

#ifndef LAB6_VIRTUALFILESYSTEM_H
#define LAB6_VIRTUALFILESYSTEM_H

#include <string>

void init_virtual_drive();
void delete_virtual_drive();
void copy_to_virtual_drive();
void copy_from_virtual_drive();
void delete_file();
void list_catalog();
void show_statistics();
void print_options();
void  handle_input(const std::string& user_input);
void list_catalog_with_name(const std::string& fsname);

#endif //LAB6_VIRTUALFILESYSTEM_H

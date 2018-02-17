//
// Created by MZiol on 22/01/2018.
//

#include "VirtualFilesystem.h"
#include <array>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <cmath>
#include <iomanip>

std::string fs_name;
unsigned long numeric_fs_size; // defined by user, in bytes
unsigned long num_of_dblocks;
unsigned int BLOCK_SIZE = 1024;
unsigned int NUM_OF_IBOCKS = 1;
unsigned int MAX_FILE_NAME_LENGTH = 16;

struct INode {
    bool is_valid;
    char file_name[16];

    unsigned int data_size; // size of data in bytes
    unsigned int data_begin; // first data block
    unsigned int data_end; // last data block
};

std::vector<INode> inodes_table;
std::set<unsigned int> free_inodes_table;
std::set<unsigned int> free_dblocks_table;


void get_virtualFS_name() {
    std::cout << "Podaj nazwę wirtualnego dysku: " << std::endl;
    getline(std::cin, fs_name);
}

unsigned long getFileSize(const std::string &file_name) {
    // open file,
    std::ifstream in(file_name, std::ios_base::ate | std::ios_base::binary);
    unsigned long result = in.tellg();
    in.close();
    return result;
}

void loadVirtualFS() {
    get_virtualFS_name();

    numeric_fs_size = getFileSize(fs_name);
    unsigned long total_num_of_blocks = numeric_fs_size / BLOCK_SIZE;
    num_of_dblocks = total_num_of_blocks - NUM_OF_IBOCKS;

    std::ifstream in_stream;
    in_stream.open(fs_name, std::ios_base::in | std::ios_base::binary);

    if(!in_stream)
    {
        std::cout << "Niepoprawna nazwa dysku " << std::endl;
        exit(1);
    }

    unsigned int inode_size = sizeof(INode);

    // Initialize data blocks
    for (unsigned int i = NUM_OF_IBOCKS; i < NUM_OF_IBOCKS + num_of_dblocks; i++) {
        free_dblocks_table.insert(i);
    }

    INode temp_inode;
    for (unsigned int i = 0; i < NUM_OF_IBOCKS; ++i) {
        for (unsigned int j = 0; j < BLOCK_SIZE / inode_size; ++j) {

            // Initialize inodes
            in_stream.read((char *) &temp_inode, inode_size);
            inodes_table.push_back(temp_inode);

            // Initialize free inodes and dblocks
            if (!temp_inode.is_valid) {
                free_inodes_table.insert(j);
            } else {

                // Delete taken dblocks
                for (unsigned int k = temp_inode.data_begin; k <= temp_inode.data_end; ++k)
                    free_dblocks_table.erase(k);
            }
        }
    }
    in_stream.close();
}

void closeVirtualFS() {
    std::ofstream dump_inodes_stream;
    dump_inodes_stream.open(fs_name, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    dump_inodes_stream.write((char *) inodes_table.data(), NUM_OF_IBOCKS * BLOCK_SIZE); // save ALL inodes to VirtualFS
    dump_inodes_stream.close();
}

std::pair<unsigned long, unsigned long> returnFirstLastBlock(unsigned long size) {
    unsigned long number_of_required_dblocks = (size + BLOCK_SIZE - 1) / BLOCK_SIZE; // ceil division

    unsigned long prev_dblock = 0;
    unsigned long continuous_space_size = 0;

    for (auto &current_dblock : free_dblocks_table) {
        if (current_dblock != prev_dblock + 1) {
            continuous_space_size = 0;
        }

        ++continuous_space_size;
        if (continuous_space_size == number_of_required_dblocks) {

            unsigned long first_block = current_dblock - number_of_required_dblocks + 1;
            return std::make_pair(first_block, current_dblock);
        }

        prev_dblock = current_dblock;
    }

    std::cout << "Not enough space, sorry" << std::endl;
    exit(1);

}

INode* getFileByName(const std::string &file_name) {

    for (auto &current_inode : inodes_table) {

        if (current_inode.is_valid) {
            if (current_inode.file_name == file_name || current_inode.file_name == ('!'+file_name)) {
                return &current_inode;
            }
        }
    }
    return nullptr;
};

void assertFileExists(const INode* ind, const std::string& destination_file_name){
    if (!ind){
        std::cout << "Plik " << destination_file_name << " nie istnieje" << std::endl;
        exit(1);
    }
};

void init_virtual_drive() {
    get_virtualFS_name();

    std::cout << "Podaj docelowy rozmiar wirtualnego dysku (w kilobajtach, min 2kB)." << std::endl;
    std::string fs_size_str;
    std::getline(std::cin, fs_size_str);
    numeric_fs_size = std::stol(fs_size_str) * 1024;

    unsigned long total_num_of_blocks = numeric_fs_size / BLOCK_SIZE;
    num_of_dblocks = total_num_of_blocks - NUM_OF_IBOCKS; // rounds up down, 1 block for inodes
    if (num_of_dblocks < 1) {
        std::cout << "Podany rozmiar jest zbyt mały!" << std::endl;
        std::cout << "Tworzenie wirtualnego dysku nie powiodło się." << std::endl;
        exit(1);
    }

    // Initialize inodes in the first block
    std::ofstream out_stream;
    out_stream.open(fs_name, std::ios::out | std::ios::binary);
    for (int i = 0; i < total_num_of_blocks * BLOCK_SIZE; i++) {
        out_stream << '\0';
    }
    out_stream.close();
}

void delete_virtual_drive() {
    get_virtualFS_name();
    if (std::remove(fs_name.c_str()) != 0)
        std::cout << "Usuwanie nie powiodło się. Dysk o podanej nazwie nie istnieje." << std::endl;
}

void copy_to_virtual_drive() {

    loadVirtualFS();
    list_catalog_with_name(fs_name);
    std::cout << "Podaj nazwę pliku, który chcesz przenieśc na dysk wirtualny:" << std::endl;
    std::string source_file_name;
    getline(std::cin, source_file_name);

    INode* ind = getFileByName(source_file_name);
    if (ind){
        std::cout << "Plik o nazwie " << source_file_name << " już istnieje na tym dysku." << std::endl;
        std::cout << "Plik nie został skopiowany." << std::endl;

        exit(1);
    }

    if (source_file_name.length() > MAX_FILE_NAME_LENGTH - 2) {
        std::cout << "Zbyt długa nazwa pliku" << std::endl;
        std::cout << "Nazwa pliku nie może przekraczac 14 znaków." << std::endl;

        exit(1);
    }

    unsigned long source_file_size = getFileSize(source_file_name);

    // Check if free space avaliable
    std::pair<unsigned long, unsigned long> first_last_dblock = returnFirstLastBlock(source_file_size);
    unsigned int inode_index = *free_inodes_table.begin();
    free_inodes_table.erase(inode_index);

    INode &free_inode = inodes_table[inode_index];
    free_inode.is_valid = true;

    strncpy(free_inode.file_name, source_file_name.c_str(), sizeof(free_inode.file_name));
    free_inode.data_begin = first_last_dblock.first;
    free_inode.data_end = first_last_dblock.second;
    free_inode.data_size = source_file_size;

    std::cout << "Czy ustawic atrybut [nieusuwalny]? [Y/N]" << std::endl;
    std::string if_deletable;
    getline(std::cin, if_deletable);

    if (if_deletable == "Y"){
        strncpy(free_inode.file_name, ("!" + source_file_name).c_str(), sizeof(free_inode.file_name)+1);
    }

    if (if_deletable == "Y"){
        std::cout << "Plik zapisano z atrybutem [nieusuwalny]" << std::endl;

    }


    std::ifstream in_stream;
    in_stream.open(source_file_name, std::ios::in | std::ios::binary);

    std::ofstream out_stream;
    out_stream.open(fs_name, std::ios::in | std::ios::out | std::ios::binary);
    out_stream.seekp(free_inode.data_begin * BLOCK_SIZE);
    out_stream << in_stream.rdbuf();
    in_stream.close();
    out_stream.close();

    closeVirtualFS();
}

void copy_from_virtual_drive() {

    loadVirtualFS();
    list_catalog_with_name(fs_name);
    std::cout << "Podaj nazwę pliku, który chcesz pobrac" << std::endl;
    std::string destination_file_name;
    getline(std::cin, destination_file_name);

    INode* ind = getFileByName(destination_file_name);
    assertFileExists(ind, destination_file_name);

    unsigned long ind_file_begin = ind->data_begin;
    unsigned long ind_file_size = ind->data_size;

    std::ifstream in_stream;
    in_stream.open(fs_name, std::ios::in | std::ios::binary);
    in_stream.seekg(ind_file_begin * BLOCK_SIZE); // shift to begin of data

    std::ofstream out_stream;
    out_stream.open(destination_file_name, std::ios::out | std::ios::binary);

    // Copy n bytes from input to output
    std::copy_n(std::istreambuf_iterator<char>(in_stream), ind_file_size,
                std::ostreambuf_iterator<char>(out_stream));

    in_stream.close();
    out_stream.close();

}

void delete_file() {
    loadVirtualFS();
    list_catalog_with_name(fs_name);
    std::cout << "Enter a file name you want to delete from VirtualFS " << std::endl;
    std::string destination_file_name;
    getline(std::cin, destination_file_name);

    INode* ind = getFileByName(destination_file_name);

    assertFileExists(ind, destination_file_name);

    if(ind->file_name[0] == '!'){
        std::cout << "Nie masz uprawnień do usunięcia pliku " << destination_file_name << std::endl;
        exit(1);

    }
    ind->is_valid = false;
    closeVirtualFS();
}

void list_catalog() {
    loadVirtualFS();
    std::cout << "Lista plików na dysku " << fs_name <<  std::endl;
    std::cout << std::setw(5) << "Nazwa" << std::setw(20) << "Rozmiar" << std::setw(20) << "Atrybut" << std::endl;
    for (auto &current_inode : inodes_table) {

        if (current_inode.is_valid) {

            if (current_inode.file_name[0] == '!'){
                std::cout << std::setw(5);
                for (int i=1; i < strlen(current_inode.file_name); i++){
                    std::cout<< current_inode.file_name[i];
                }
                std::cout << std::setw(15) << current_inode.data_size << "B" << std::setw(20) << "X" <<std::endl;
            }
            else {
                std::cout << std::setw(5) << current_inode.file_name << std::setw(15) << current_inode.data_size << "B" << std::setw(20) << "" <<std::endl;
            }
        }
    }
}
void list_catalog_with_name(const std::string& fsname){
    std::cout << "Lista plików na dysku " << fsname <<  std::endl;
    std::cout << std::setw(5) << "Nazwa" << std::setw(20) << "Rozmiar" << std::endl;
    for (auto &current_inode : inodes_table) {

        if (current_inode.is_valid) {

            if (current_inode.file_name[0] == '!'){
                std::cout << std::setw(5);
                for (int i=1; i < strlen(current_inode.file_name); i++){
                    std::cout<< current_inode.file_name[i];
                }
                std::cout << std::setw(15) << current_inode.data_size << "B" << std::setw(20) << "X" <<std::endl;
            }
            else {
                std::cout << std::setw(5) << current_inode.file_name << std::setw(15) << current_inode.data_size << "B" << std::setw(20) << "" <<std::endl;
            }
        }
    }
}

void show_statistics() {
    loadVirtualFS();
    std::cout << "Całkowita pamięc dysku: " << numeric_fs_size << " B" << std::endl;

    unsigned long inodes_space = NUM_OF_IBOCKS * BLOCK_SIZE;
    std::cout << "Pamięc zarezerwowana na INody: " << inodes_space << " B" << std::endl;

    unsigned long free_inodes = free_inodes_table.size();
    std::cout << "Wolne INody: " << free_inodes << std::endl;

    unsigned long taken_inodes = inodes_space / sizeof(INode) - free_inodes;
    std::cout << "Zajęte INody: " << taken_inodes << std::endl;

    unsigned long data_space = numeric_fs_size - inodes_space;
    std::cout << "Pamięc dostępna dla plików: " << data_space << " B" << std::endl;

    unsigned long free_data_space = free_dblocks_table.size() * 1024;
    std::cout << "Wolna pamięc plików: " << free_data_space << " B" << std::endl;

    unsigned long used_data_space = (num_of_dblocks - free_dblocks_table.size()) * 1024;
    std::cout << "Zajęta pamięc plików: " << used_data_space << " B" << std::endl;

}


void print_options() {
    std::cout << "" << std::endl;
    std::cout << "Dostępne funkcje:" << std::endl;
    std::cout << "A - stwórz nowy dysk" << std::endl;
    std::cout << "B - usuń istniejący dysk" << std::endl;
    std::cout << "C - dodaj plik do dysku" << std::endl;
    std::cout << "D - pobierz plik z dysku" << std::endl;
    std::cout << "E - usuń plik z dysku" << std::endl;
    std::cout << "F - pokaż katalog" << std::endl;
    std::cout << "G - pokaż statystyki dysku" << std::endl;
    std::cout << "H - pomoc" << std::endl;
    std::cout << "X - wyjście" << std::endl;
}

void handle_input(const std::string& user_input){

    if (user_input == "A") init_virtual_drive();
    else if (user_input == "B") {
        delete_virtual_drive();
    }
    else if (user_input == "C") {
        copy_to_virtual_drive();
    }
    else if (user_input == "D") {
        copy_from_virtual_drive();
    }
    else if (user_input == "E") {
        delete_file();
    }
    else if (user_input == "F") {
        list_catalog();
    }
    else if (user_input == "G") {
        show_statistics();
    }
    else if (user_input == "H") {
        print_options();
    }
    else if (user_input == "X") {
    }
    else {
        std::cout << "Nieobsługiwana opcja: " << user_input << std::endl;
        print_options();
    }
}

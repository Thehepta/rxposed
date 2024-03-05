//
// Created by chic on 2023/11/25.
//

#pragma once

#include <string>
#include <vector>

extern const char* const kZipFileSeparator;

void format_string(std::string* str, const std::vector<std::pair<std::string, std::string>>& params);

bool file_is_in_dir(const std::string& file, const std::string& dir);
bool file_is_under_dir(const std::string& file, const std::string& dir);
bool normalize_path(const char* path, std::string* normalized_path);
bool parse_zip_path(const char* input_path, std::string* zip_path, std::string* entry_path);

// For every path element this function checks of it exists, and is a directory,
// and normalizes it:
// 1. For regular path it converts it to realpath()
// 2. For path in a zip file it uses realpath on the zipfile
//    normalizes entry name by calling normalize_path function.
void resolve_paths(std::vector<std::string>& paths,
                   std::vector<std::string>* resolved_paths);
// Resolve a single path. Return empty string when the path is invalid or can't
// be resolved.
std::string resolve_path(const std::string& path);

void split_path(const char* path, const char* delimiters, std::vector<std::string>* paths);

std::string dirname(const char* path);

off64_t page_start(off64_t offset);
size_t page_offset(off64_t offset);
bool safe_add(off64_t* out, off64_t a, size_t b);
bool is_first_stage_init();
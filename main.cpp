#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

namespace fs = std::filesystem;

static const uint64_t address_not_found = 0;
static const uint64_t address_already_patched = 1;
static const uint64_t check_range_start = 0xB94000;
static const uint64_t check_range_end = 0xB98000;
static const uint64_t patch_offset_in_sequence = 1;
uint32_t patched_limit = 500;

static const std::vector<uint64_t> known_candidates{0xB95CF6, 0xB96366,
                                                    0xB967A6, 0xB95E56};
static const std::vector<uint8_t> unpatched_sequence{0xBA, 0xC0, 0x00, 0x00,
                                                     0x00, 0x48, 0x8D, 0x4B};
static const std::vector<uint8_t> patched_sequence{0xBA, 0xF4, 0x01, 0x00,
                                                   0x00, 0x48, 0x8D, 0x4B};

uint64_t find_patch_address(const std::vector<uint8_t> &search_buffer) {
  for (auto candidate : known_candidates) {
    size_t candidate_range_start =
        candidate - check_range_start - patch_offset_in_sequence;
    std::vector<uint8_t> checked_bytes(
        &search_buffer[candidate_range_start],
        &search_buffer[candidate_range_start + unpatched_sequence.size()]);

    if (checked_bytes == unpatched_sequence) {
      return candidate;
    } else if (checked_bytes == patched_sequence) {
      return address_already_patched;
    }
  }

  for (size_t i = 0; i < search_buffer.size() - unpatched_sequence.size();
       i++) {
    std::vector<uint8_t> checked_bytes(
        &search_buffer[i], &search_buffer[i + unpatched_sequence.size()]);

    if (checked_bytes == unpatched_sequence) {
      return i + check_range_start + patch_offset_in_sequence;
    }
  }

  for (size_t i = 0; i < search_buffer.size() - patched_sequence.size(); i++) {
    std::vector<uint8_t> checked_bytes(
        &search_buffer[i], &search_buffer[i + patched_sequence.size()]);

    if (checked_bytes == patched_sequence) {
      return address_already_patched;
    }
  }

  return address_not_found;
}

int main() {
  ifstream ifs("modlimitfix.conf");
  uint32_t mod_limit;
  bool using_conf = false;
  ifs >> mod_limit;
  if (mod_limit) {
    patched_limit = mod_limit;
    using_conf = true;
  }
  if (using_conf) {
    cout << "Using configuration for mod limit value..." << endl;
  } else {
    cout << "Using defaults for mod limit values..." << endl;
  }
  cout << "Patching with a limit of " << patched_limit << "..." << endl;
  auto path = fs::absolute(fs::current_path() / "witcher3.exe");

  if (!fs::exists(path)) {
    cout << "Could not find witcher3.exe file, make sure you are running this "
         << "in the bin/x64 directory." << endl;
    cout << "Please press ENTER or close the window to terminate the program"
         << endl;
    cin.get();
    return 0;
  }

  size_t read_size = check_range_end - check_range_start;
  std::vector<uint8_t> search_buffer(read_size);

  {
    std::ifstream file(path.c_str(), std::ios::binary);
    file.seekg(check_range_start);

    if (file.eof() ||
        !file.read(reinterpret_cast<char *>(search_buffer.data()), read_size)) {
      cout << "witcher3.exe is unexpectedly small, aborting patching." << endl;
      cout << "Please press ENTER or close the window to terminate the program"
           << endl;
      cin.get();
      return 0;
    }
  }

  uint64_t patch_address = find_patch_address(search_buffer);

  if (patch_address == address_not_found) {
    cout << "Did not find the address to patch, possibly an unexpected version "
            "of executable. Contact sedmelluq and give him your exe."
         << endl;
    cout << "Please press ENTER or close the window to terminate the program"
         << endl;
    cin.get();
    return 0;
  } else if (patch_address == address_already_patched) {
    cout << "It seems the executable is already patched" << endl;
    cout << "Please press ENTER or close the window to terminate the program"
         << endl;
    cin.get();
    return 0;
  }

  auto backup_path = fs::absolute(fs::current_path() / "witcher3.exe.orig");

  if (fs::exists(backup_path)) {
    cout << "Backup file witcher3.exe.orig already exists. Rename it or delete "
            "it (might be left over from previous patch attempt)."
         << endl;
    cout << "Please press ENTER or close the window to terminate the program"
         << endl;
    cin.get();
    return 0;
  } else if (!fs::copy_file(path, backup_path)) {
    cout << "Failed to create backup file, try running as administrator."
         << endl;
    cout << "Please press ENTER or close the window to terminate the program"
         << endl;
    cin.get();
    return 0;
  }

  {
    std::fstream file(path.c_str(), std::fstream::binary | std::fstream::in |
                                        std::fstream::out);
    file.seekp(patch_address);

    if (file.eof()) {
      cout << "Unexpectedly encountered file end when writing patch." << endl;
    } else if (!file.write((char *)&patched_limit, sizeof(patched_limit))) {
      cout << "Failed to write patch" << endl;
    } else {
      cout << "Successfully patched." << endl;
    }
  }
  cout << "Please press ENTER or close the window to terminate the program"
       << endl;
  cin.get();
  return 0;
}

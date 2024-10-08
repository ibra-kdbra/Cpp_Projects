# CSV Patch Set Converter

This Python script processes a CSV file that contains musical instrument patch data and converts it into a structured format for use in MIDI patch sets. The output organizes the patches into banks and sets, which are categorized by type and bank numbers, along with specific patch information.

## Features

### 1. CSV Parsing
- The script reads a CSV file provided as a command-line argument.
- The CSV contains data such as:
  - Patch type
  - Bank information (MSB, LSB)
  - Patch name and program change number (PC)
  - Patch category

### 2. Patch Organization
- The patches are grouped into sets based on their type and bank.
- Within each set, patches are further organized by MSB and LSB bank numbers.
- The result is a dictionary structure where patches are categorized for easy lookup.

### 3. Output Structure
- The script outputs:
  1. A list of patch sets, specifying the total number of sets.
  2. A bank list for each patch set, detailing the MSB/LSB bank numbers.
  3. A detailed patch list, which lists every patch and its corresponding program change (PC) number.

### Usage

1. Prepare a CSV file with the following columns:
   - Patch type
   - Bank number
   - Name of the patch
   - MSB
   - LSB
   - PC (Program Change)
   - Category

2. Run the script using Python, passing the CSV file as a parameter:
   ```bash
   python csv_patch_converter.py <patch_file.csv>
   ```

3. The script will output:
   - A list of patch sets
   - A detailed breakdown of banks and patches

### Example Output

1. **List of Patch Sets**
   ```bash
   [List of Patch Sets]
   NumSets=2
   Set1=General MIDI Level 2: Strings
   Set2=General MIDI Level 2: Brass
   ```

2. **Bank List**
   ```bash
   [General MIDI Level 2 Strings:Bank 0/1]
   Type=Banks
   Method=CC32
   Default=0/1
   Bank0/1=Strings Bank 0/1
   ```

3. **Patch List**
   ```bash
   [Strings: Bank 0/1]
   Type=Patches
   1=Violin
   2=Viola
   3=Cello
   ```

## Summary

This script provides an efficient way to convert CSV patch data into a structured format for use in MIDI devices or patch set configurations. By grouping patches into sets and banks, it simplifies the organization and retrieval of patch data for musical instruments.

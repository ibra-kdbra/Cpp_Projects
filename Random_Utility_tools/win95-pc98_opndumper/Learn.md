
# OPN Binary Patch Extractor

This Python script extracts OPN (Yamaha FM synthesizer chip) patch data from a binary file and converts it into other formats. The script provides tools to read, convert, and write OPN and OPM (another FM synthesizer chip) patch data.

## Features

### 1. OPN Patch Extraction
- The script reads binary OPN patch data from a file and extracts key properties such as:
  - Feedback
  - Algorithm
  - Operator parameters like AR (Attack Rate), D1R (Decay Rate 1), D2R (Decay Rate 2), RR (Release Rate), SL (Sustain Level), TL (Total Level), and more.

### 2. Patch Conversion
- Converts OPN patch data to OPM format using the `convOPNToOPM` function.
- The script translates OPN-specific parameters into their OPM equivalents, preserving the core attributes of each patch.

### 3. Output Formats
- Writes extracted patch data to two formats:
  - **VOPM (.OPM) format**: Used for a virtual software synthesizer, saving the patch data for further use.
  - **FMMIDI format**: Creates a bank of MIDI-compatible patches.

### 4. Binary File Processing
- The binary patch data is read and processed with Python's `struct` module, which interprets the binary format of the OPN patches.

## How It Works

### 1. **OPN Patch Extraction (`readOPNBin`)**
- This function reads the binary OPN data from the file and extracts patch information, which includes LFO parameters, operator parameters (like envelope rates and multipliers), and synthesis settings.

### 2. **OPN to OPM Conversion (`convOPNToOPM`)**
- Converts OPN patches to OPM patches, adjusting specific parameters like `DT1`, `DT2`, and operator enable flags to match the OPM chip specifications.

### 3. **Patch Writing (`writeOPMBank` and `writeFMMIDIBank`)**
- The script writes the patch data into a format compatible with the VOPM synthesizer or a general MIDI patch bank. The data is saved in `.txt` format for FMMIDI or `.OPM` format for VOPM.

## Usage

1. Prepare the OPN binary patch file you wish to extract data from.
2. Run the script using Python:
   ```bash
   python opn_patch_extractor.py <binary_file>
   ```
   This will extract the patches from the file and save the result in `programs.txt` (FMMIDI format).
   
3. The extracted patches can be further converted into OPM format and saved to `.OPM` files if required.

## Example

Given an OPN binary file, the script reads the file, prints each patch’s parameters, converts them to OPM patches, and saves them to the specified output format.

## Dependencies

- **Python 2.x**: The script is written in Python 2.x and relies on the `struct` module for binary data parsing.

## Summary

This OPN binary patch extractor simplifies the process of extracting and converting patch data from Yamaha OPN synthesizers into formats that can be used with other synthesizers or as part of MIDI programs. It provides a straightforward way to interact with FM synthesizer patches and export them for further use.

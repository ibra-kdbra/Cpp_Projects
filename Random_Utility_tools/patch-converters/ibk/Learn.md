# Creative IBK to MSOPL/JungleVision Instrument Bank Converter

This tool converts Creative IBK instrument banks to OPL3 instrument data that can be used with MidiPlay or the Windows OPL3 MIDI driver.

## Features

- Converts instruments from `.IBK` format into OPL3 registers.
- Provides an option to output instruments in a `patch.h` header format for the WinFM software OPL3 driver.

## Usage

```bash
./ibk_converter <IBK file> <OP3 file> [-header]
```

- `<IBK file>`: Path to the input IBK instrument bank file.
- `<OP3 file>`: Path to the output file where the OPL3 instrument data will be saved.
- `-header`: Optional flag. If specified, the output will be in a `patch.h` header file format instead of binary.

## Structure Overview

### Main Data Structures

- `oplinst_t`: Represents an OPL3 instrument, including settings like attack, decay, sustain, release, and waveform selection.
- `ibktimbre_t`: Represents an IBK instrument bank sound.
- `ibk_t`: Represents the full IBK instrument bank.

### Conversion Process

The conversion process reads instrument data from an IBK file, translates it into the appropriate OPL3 register values, and writes the data to an output file in binary or header format.

### Example

```bash
./ibk_converter my_ibk_file.ibk output_op3.op3 -header
```

This will convert the instruments in `my_ibk_file.ibk` to OPL3 format and save the output to `output_op3.op3` as a `patch.h` header file.

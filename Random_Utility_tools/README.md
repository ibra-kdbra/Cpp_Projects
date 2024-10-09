
# Random Utility Tools

This document provides an overview of several converters used for instrument conversion between different formats such as BNK, IBK, and OPN. Each converter serves a specific purpose in processing sound patches and instrument banks. Below are links to detailed readme files for each converter along with a brief description of their functionality.

## 1. OPN Dumper Converter

The `opndumper.py` script reads binary patch data and extracts OPN (Yamaha FM synth) patches for further conversion or analysis. It includes functionality to convert OPN data to OPM format and also to write FMMIDI banks. This tool is helpful for those working with vintage Yamaha FM synth data.

- [Read the full OPN Dumper Readme](./win95-pc98_opndumper/Learn.md)

## 2. BNK Patch Converters

### BNK to VGMCK and SiON
This converter reads `.BNK` instrument files and converts them into various formats such as VGMCK MML for YM3812 and YMF262 chips, and SiON MML for OPL/OPN/OPM/OPX/MA3 formats. The tool supports a range of formats, making it versatile for working with different FM synth systems.

### BNK to OPL3
This converter specifically targets OPL3 chip formats. It reads BNK instrument data and converts it to the necessary register information compatible with OPL3 systems, making it ideal for those working on projects related to OPL3-based FM synths.


### BNK to SBI
The `bnk_to_sbi` converter reads BNK files and converts the patch data into the SBI format, used for storing FM instrument patches. This tool is especially useful for exporting instrument patches that can be used with various SBI-compatible software.

- [Read the BNK to Patch Converters Readme](./patch-converters/bnk/Learn.md)

## 3. CSV Patch Set Converter

The CSV patch converter reads a CSV file and outputs organized patch sets for GM (General MIDI) Level 2. It groups the patch data into tone banks, MSB, LSB, and other categories, which is useful for creating structured patch sets for MIDI instruments.

- [Read the CSV Patch Set Converter Readme](./patch-converters/csv-winjammer-patch-ini/Learn.md)

## 4. IBK to OPL

The IBK to OPL converter is designed for converting Creative IBK instrument banks to MSOPL/JungleVision instrument formats. The converter supports the WinFM OPL3 register format and can also export patches as C header files for software drivers. This tool is useful for those who want to transfer instrument banks from older SoundBlaster systems to more modern applications.

- [Read the IBK to OPL Readme](./patch-converters/ibk/Learn.md)

---

These tools offer flexible and powerful options for converting FM synth data between different formats, making them ideal for anyone working on FM synthesis projects. Be sure to check out the detailed readme files for more information on each tool.

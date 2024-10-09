# BNK Patch Converters

This set of three C programs is designed to convert `.BNK` instrument patches into different formats, specifically for various sound chips and software synthesizers. The BNK format, often used in early AdLib or FM synthesizer-based software, contains instrument definitions that need to be converted for use in modern sound chip emulators or synthesizers.

## 1. BNK to VGMCK and SiON MML Converter

### Overview
- This program converts `.BNK` instrument patches into various MML (Music Macro Language) formats for different sound chips such as YM3812, YMF262, and others.
- The program takes a `.BNK` file and outputs MML code that can be used in VGMCK or SiON, depending on the specified conversion mode.

### Features
- Converts `.BNK` patches to:
  - VGMCK MML for YM3812
  - VGMCK MML for YMF262
  - SiON MML for OPL, OPN, OPM, OPX, and MA3
- The conversion mode is specified by a command-line argument (1-7).

### Usage
```bash
./bnk_to_mml <BNK file> <TXT file> <Conversion mode>
```

### Key Functions
- **seekToBNKData()**: Seeks to the data section of the `.BNK` file.
- **convertBNKToMML()**: Converts the `.BNK` patch to the selected MML format.
- **outVGMCKOPL2(), outSiONOPL()**: Outputs the converted patch to the specified format.

---

## 2. BNK to OPL3 Register Converter

### Overview
- This program converts `.BNK` patches into OPL3 register values, which are used by the OPL3 chip (commonly found in Sound Blaster cards) to define instrument sounds.
- It outputs the converted patch data into a binary `.OP3` file for use in emulators or hardware OPL3 chips.

### Features
- Converts `.BNK` patches into OPL3 register values.
- Generates a `.OP3` file with the instrument definitions.

### Usage
```bash
./bnk_to_opl <BNK file> <OP3 file>
```

### Key Functions
- **convertBNKToOPL()**: Converts the `.BNK` patch into OPL3 register values.
- **initBNKPatch(), initOPLPatch()**: Initializes the patch data structures for BNK and OPL.

---

## 3. BNK to SBI Patch Converter

### Overview
- This program converts `.BNK` patches into `.SBI` format patches, which are used in FM synthesizer software.
- The `.SBI` format defines instrument data for use with FM synthesizers such as the OPL series.

### Features
- Converts `.BNK` patches into `.SBI` patches.
- Outputs each instrument patch into its own `.SBI` file.

### Usage
```bash
./bnk_to_sbi <BNK file>
```

### Key Functions
- **convertBNKtoSBI()**: Converts the `.BNK` patch into `.SBI` format.
- **initBNKPatch(), initSBIPatch()**: Initializes the patch data structures for BNK and SBI.

## Summary

These three converters allow for the transformation of legacy `.BNK` instrument patches into modern formats for sound chip emulators and synthesizers. Whether you're working with VGMCK, OPL3 hardware, or FM synthesizers, these tools make it easy to convert and preserve instrument patches for a variety of use cases.

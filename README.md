# DES Encryption Suite

A full-featured Data Encryption Standard (DES) implementation in C, with a cross-platform Python GUI frontend. Supports multiple cipher modes, string and file encryption, DLL integration, and key generation.

---

## 🗂 Project Structure

.
├── .idea/                  # JetBrains IDE config
├── .vscode/                # VS Code settings
├── app/                    # Python GUI app
│   └── gui.py
├── scripts/                # Build & run scripts
│   ├── des.def
│   ├── libdes.a
│   ├── make.bat
│   ├── makeDump.bat
│   ├── make_dll.bat
│   └── run.bat
├── src/                    # C source code
│   ├── constants.h
│   ├── main.c
│   ├── des_block/
│   │   ├── f_function/
│   │   ├── main_encryption/
│   │   └── subkey_genaration/
│   ├── key_iv/
│   ├── modes/
│   └── pkcs7_padding/
├── test/                   # Sample files for testing
│   ├── plaintext.txt
│   ├── ciphertext.txt
│   └── decoded.txt
└── bin/                    # Compiled outputs (ignored by Git)
    ├── des.dll
    └── out.exe

---

## 🔐 Features

- Full DES block cipher (64-bit block, 16 rounds)
- Modes: ECB, CBC, PCBC, CFB, OFB, CTR
- File and string encryption/decryption
- Manual or random 64-bit key input
- DLL compiled for use with Python via ctypes
- Lightweight Python GUI with mode selector and key entry

---

## 🛠 Build Instructions

### Requirements

- MinGW-w64 (64-bit, POSIX, SEH recommended)

### Build DLL & Executable

cd scripts
make_dll.bat    # Creates bin/des.dll
make.bat        # Creates bin/out.exe (for testing)

Optional: run.bat to execute out.exe.

---

## ▶️ Run GUI

### Prerequisites

- Python 3.x
- Tkinter (usually preinstalled)

### Launch

cd app
python gui.py

---

## 🧪 Testing

Test the DES system with sample data:

- test/plaintext.txt → encrypted to → test/ciphertext.txt
- test/ciphertext.txt → decrypted to → test/decoded.txt

You can test this automatically via main.c or interactively through the GUI.

---

## 💡 Notes

- DES key must be 64 bits (entered as 16 hex characters)
- Random key generation uses C-side secure PRNG
- All cryptographic computation is in native C for performance
- GUI is a thin wrapper over the DLL for usability

---

## ✨ Credits

Developed by Aviv Esh

- Custom DES implementation with:
  - F-function, S-boxes, expansion, permutations
  - Key schedule (PC-1, shifts, PC-2)
  - Block cipher chaining modes
  - PKCS#7 padding
- Clean integration between C and Python via DLL

---

## 📜 License

MIT License 

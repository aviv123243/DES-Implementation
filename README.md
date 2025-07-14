

# DES Encryption Suite

A full-featured DES encryption system implemented in C with a cross-platform Python GUI frontend. Supports multiple cipher modes, string and file encryption, and DLL-based interoperability with Python.

---

## 🗂 Project Structure

```
.
├── .idea/                   # JetBrains IDE config
├── .vscode/                # VS Code settings
├── app/                    # Python GUI app
│   └── gui.py
├── bin/                    # Compiled binaries (DLL, EXE)
│   ├── des.dll
│   └── out.exe
├── build/                  # Build scripts and intermediate files
│   ├── make.bat
│   ├── make_dll.bat
│   ├── makeDump.bat
│   ├── run.bat
│   ├── des.def
│   └── libdes.a
├── src/                    # C source code
│   ├── main.c
│   ├── constants.h
│   ├── des_block/
│   │   ├── f_function/
│   │   ├── main_encryption/
│   │   └── subkey_genaration/
│   ├── modes/
│   ├── key_iv/
│   └── pkcs7_padding/
└── test/                   # Test input/output files
    ├── plaintext.txt
    ├── ciphertext.txt
    └── decoded.txt
```

---

## 🔐 Features

- Full DES block cipher (64-bit block, 16 rounds) in C
- Modes: `ECB`, `CBC`, `PCBC`, `CFB`, `OFB`, `CTR`
- File and string encryption/decryption
- Python GUI with mode selection and key management
- DLL built with MinGW for seamless Python integration via `ctypes`
- Manual or random 64-bit key input with C-side secure PRNG

---

## 🛠 Build Instructions

### Requirements

- [MinGW-w64](https://github.com/rcpacini/mingw-w64) (x64, POSIX/SEH recommended)

### Build DLL & Executable

```sh
cd build
make_dll.bat    # Creates bin/des.dll
make.bat        # Creates bin/out.exe 
```

Optional: Run `run.bat` to build execute `out.exe`. (for testing)

---

## ▶️ Run GUI

### Prerequisites

- Python 3.x
- `tkinter` (usually preinstalled with Python)

### Run

```sh
cd app
python gui.py
```

---

## 🧪 Testing

Use the files in `test/` to verify encryption/decryption:

- `plaintext.txt`: Input file
- `ciphertext.txt`: Output after encryption
- `decoded.txt`: Output after decryption

Test automatically via `main.c` or interactively through the GUI.

---

## 💡 Notes

- DES key must be 64-bit (16 hex characters)
- Random key generation available via GUI ("Random" button) or C-side PRNG
- All cryptographic computation performed in native C for performance
- GUI serves as a lightweight wrapper over the DLL for usability

---

## ✨ Credits

Created by **Aviv Esh**  
Features a custom DES implementation with:
- F-function, S-boxes, expansion, and permutations
- Key schedule (PC-1, shifts, PC-2)
- Block cipher chaining modes
- PKCS#7 padding
- Clean C and Python integration via DLL

---

## 📜 License

MIT License


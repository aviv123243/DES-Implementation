# DES Encryption Suite

A full-featured DES encryption system in C with a Python GUI frontend. Supports multiple modes, string and file encryption, and DLL-based interoperability.

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
│   ├── run.bat
│   ├── des.def
│   └── libdes.a
├── src/                    # C source code
│   ├── main.c
│   ├── des_block/
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

- Full DES encryption in C
- Modes: `ECB`, `CBC`, `PCBC`, `CFB`, `OFB`, `CTR`
- File & string encryption/decryption
- Python GUI with mode selection and key management
- DLL built with MinGW for seamless Python binding via `ctypes`
- Supports random key generation from C directly

---

## 🛠 Build Instructions

### Requirements

- [MinGW-w64](https://github.com/rcpacini/mingw-w64) (x64, POSIX/SEH build)
- `make_dll.bat` script (in `build/`) or compile manually:

```sh
cd build
make_dll.bat
```

DLL is created at `bin/des.dll`.

---

## ▶️ Run GUI

### Prerequisites

- Python 3.x
- `tkinter` (usually preinstalled)

### Run

```sh
cd app
python gui.py
```

---

## 🧪 Testing

Use the files in `test/` to test encryption/decryption:

- `plaintext.txt`: input file
- `ciphertext.txt`: output after encryption
- `decoded.txt`: output after decryption

You can also encrypt and decrypt any string via the GUI.

---

## 💡 Notes

- DES key must be 64-bit (16 hex characters)
- Random key generator available via GUI ("Random" button)
- Supports encryption/decryption for both files and strings
- All encryption logic runs in compiled C, GUI just interfaces with DLL

---

## ✨ Credits

Created by **Aviv Esh**  
Includes custom implementation of DES, key scheduling, padding, and all major modes.

---

## 📜 License

MIT License (or your preferred license)

import tkinter as tk
from tkinter import filedialog, messagebox, ttk
import ctypes
import os

# Load DLL
dll_path = os.path.join(os.path.dirname(__file__), "..", "bin", "des.dll")
des = ctypes.CDLL(dll_path)

# Set up random key function from DLL
des.generate_random_key.argtypes = []
des.generate_random_key.restype = ctypes.c_uint64

def generate_random_key_hex():
    return f"{des.generate_random_key():016X}"

# Modes and their file/string functions
class Mode:
    def __init__(self, name, file_enc, file_dec, str_enc, str_dec):
        self.name = name
        self.file_enc = file_enc
        self.file_dec = file_dec
        self.str_enc = str_enc
        self.str_dec = str_dec

modes = [
    Mode("ECB", des.des_ECB_encrypt_file, des.des_ECB_decrypt_file, des.des_ECB_encrypt_string, des.des_ECB_decrypt_string),
    Mode("CBC", des.des_CBC_encrypt_file, des.des_CBC_decrypt_file, des.des_CBC_encrypt_string, des.des_CBC_decrypt_string),
    Mode("PCBC", des.des_PCBC_encrypt_file, des.des_PCBC_decrypt_file, des.des_PCBC_encrypt_string, des.des_PCBC_decrypt_string),
    Mode("CFB", des.des_CFB_encrypt_file, des.des_CFB_decrypt_file, des.des_CFB_encrypt_string, des.des_CFB_decrypt_string),
    Mode("OFB", des.des_OFB_encrypt_file, des.des_OFB_decrypt_file, des.des_OFB_encrypt_string, des.des_OFB_decrypt_string),
    Mode("CTR", des.des_CTR_encrypt_file, des.des_CTR_decrypt_file, des.des_CRT_encrypt_string, des.des_CRT_decrypt_string),
]
mode_map = {m.name: m for m in modes}

# Set types for file functions
for m in modes:
    for func in [m.file_enc, m.file_dec]:
        func.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_uint64]
        func.restype = None

    for func in [m.str_enc, m.str_dec]:
        func.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int, ctypes.c_uint64]
        func.restype = ctypes.c_int

# GUI
root = tk.Tk()
root.title("DES Encryptor")
root.geometry("600x500")

notebook = ttk.Notebook(root)
notebook.pack(fill=tk.BOTH, expand=True)

# === File Tab ===
file_tab = ttk.Frame(notebook)
notebook.add(file_tab, text="File")

file_mode = tk.StringVar(value="ECB")
file_key = tk.StringVar(value="1122334455667788")
file_input = tk.StringVar()
file_output = tk.StringVar()

ttks = ttk

# File widgets
ttks.Label(file_tab, text="Mode:").grid(row=0, column=0, sticky=tk.W)
ttks.Combobox(file_tab, textvariable=file_mode, values=list(mode_map.keys()), state="readonly").grid(row=0, column=1, sticky=tk.EW)

ttks.Label(file_tab, text="Key (hex):").grid(row=1, column=0, sticky=tk.W)
ttks.Entry(file_tab, textvariable=file_key).grid(row=1, column=1, sticky=tk.EW)
ttks.Button(file_tab, text="Random", command=lambda: file_key.set(generate_random_key_hex())).grid(row=1, column=2)

ttks.Label(file_tab, text="Input File:").grid(row=2, column=0, sticky=tk.W)
ttks.Entry(file_tab, textvariable=file_input).grid(row=2, column=1, sticky=tk.EW)
ttks.Button(file_tab, text="Browse", command=lambda: file_input.set(filedialog.askopenfilename())).grid(row=2, column=2)

ttks.Label(file_tab, text="Output File:").grid(row=3, column=0, sticky=tk.W)
ttks.Entry(file_tab, textvariable=file_output).grid(row=3, column=1, sticky=tk.EW)
ttks.Button(file_tab, text="Browse", command=lambda: file_output.set(filedialog.asksaveasfilename())).grid(row=3, column=2)

def file_op(encrypt):
    mode = mode_map[file_mode.get()]
    key = int(file_key.get(), 16)
    try:
        func = mode.file_enc if encrypt else mode.file_dec
        func(file_input.get().encode(), file_output.get().encode(), key)
        messagebox.showinfo("Success", "Done")
    except Exception as e:
        messagebox.showerror("Error", str(e))

ttks.Button(file_tab, text="Encrypt", command=lambda: file_op(True)).grid(row=4, column=1, sticky=tk.E, pady=10)
ttks.Button(file_tab, text="Decrypt", command=lambda: file_op(False)).grid(row=4, column=2, sticky=tk.W)

for i in range(3): file_tab.columnconfigure(i, weight=1)

# === String Tab ===
string_tab = ttk.Frame(notebook)
notebook.add(string_tab, text="String")

string_mode = tk.StringVar(value="ECB")
string_key = tk.StringVar(value="133457799BBCDFF1")
plaintext = tk.Text(string_tab, height=5)
ciphertext = tk.Text(string_tab, height=5)

# String widgets
ttks.Label(string_tab, text="Mode:").grid(row=0, column=0, sticky=tk.W)
ttks.Combobox(string_tab, textvariable=string_mode, values=list(mode_map.keys()), state="readonly").grid(row=0, column=1, sticky=tk.EW)

ttks.Label(string_tab, text="Key (hex):").grid(row=1, column=0, sticky=tk.W)
ttks.Entry(string_tab, textvariable=string_key).grid(row=1, column=1, sticky=tk.EW)
ttks.Button(string_tab, text="Random", command=lambda: string_key.set(generate_random_key_hex())).grid(row=1, column=2)

ttks.Label(string_tab, text="Plaintext:").grid(row=2, column=0, sticky=tk.W)
plaintext.grid(row=3, column=0, columnspan=3, sticky=tk.EW)

ttks.Label(string_tab, text="Ciphertext:").grid(row=4, column=0, sticky=tk.W)
ciphertext.grid(row=5, column=0, columnspan=3, sticky=tk.EW)

def string_encrypt():
    mode = mode_map[string_mode.get()]
    key = int(string_key.get(), 16)
    src = plaintext.get("1.0", tk.END).strip().encode()
    buf = ctypes.create_string_buffer(1024)
    newlen = mode.str_enc(src, buf, len(src), key)
    ciphertext.delete("1.0", tk.END)
    ciphertext.insert("1.0", buf.raw[:newlen])

def string_decrypt():
    mode = mode_map[string_mode.get()]
    key = int(string_key.get(), 16)
    src = ciphertext.get("1.0", tk.END).strip().encode()
    buf = ctypes.create_string_buffer(1024)
    newlen = mode.str_dec(src, buf, len(src), key)
    plaintext.delete("1.0", tk.END)
    plaintext.insert("1.0", buf.raw[:newlen].decode(errors="ignore"))

ttks.Button(string_tab, text="Encrypt", command=string_encrypt).grid(row=6, column=1, sticky=tk.E, pady=10)
ttks.Button(string_tab, text="Decrypt", command=string_decrypt).grid(row=6, column=2, sticky=tk.W)

for i in range(3): string_tab.columnconfigure(i, weight=1)

root.mainloop()

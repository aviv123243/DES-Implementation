import tkinter as tk
from tkinter import ttk, filedialog, messagebox
import ctypes
from ctypes import c_char_p, c_uint64, c_int

class DESApp:
    def __init__(self, root):
        self.root = root
        self.root.title("DES Encryption")
        self.root.geometry("500x500")
        self.dll = ctypes.WinDLL("../bin/des.dll")
        self.setup_dll()
        self.create_widgets()

    def setup_dll(self):
        """Configure DLL function prototypes."""
        string_funcs = [
            "des_ECB_encrypt_string", "des_CBC_encrypt_string", "des_CFB_encrypt_string",
            "des_OFB_encrypt_string", "des_CRT_encrypt_string", "des_PCBC_encrypt_string",
            "des_ECB_decrypt_string", "des_CBC_decrypt_string", "des_CFB_decrypt_string",
            "des_OFB_decrypt_string", "des_CRT_decrypt_string", "des_PCBC_decrypt_string"
        ]
        for func in string_funcs:
            f = getattr(self.dll, func)
            f.argtypes = [c_char_p, c_char_p, c_int, c_uint64] if "decrypt" in func else [c_char_p, c_char_p, c_uint64]
            f.restype = None if "decrypt" in func else c_int

        file_funcs = [
            "des_ECB_encrypt_file", "des_CBC_encrypt_file", "des_CFB_encrypt_file",
            "des_OFB_encrypt_file", "des_CTR_encrypt_file", "des_PCBC_encrypt_file",
            "des_ECB_decrypt_file", "des_CBC_decrypt_file", "des_CFB_decrypt_file",
            "des_OFB_decrypt_file", "des_CTR_decrypt_file", "des_PCBC_decrypt_file"
        ]
        for func in file_funcs:
            getattr(self.dll, func).argtypes = [c_char_p, c_char_p, c_uint64]

        self.dll.generate_random_key.restype = c_uint64

    def create_widgets(self):
        """Create GUI with two tabs."""
        notebook = ttk.Notebook(self.root)
        notebook.pack(pady=10, fill="both", expand=True)

        # Text Tab
        text_tab = ttk.Frame(notebook)
        notebook.add(text_tab, text="Text Encryption")
        self.create_text_tab(text_tab)

        # File Tab
        file_tab = ttk.Frame(notebook)
        notebook.add(file_tab, text="File Encryption")
        self.create_file_tab(file_tab)

    def create_text_tab(self, parent):
        """Create text encryption tab."""
        tk.Label(parent, text="Mode:").pack()
        self.text_mode = tk.StringVar(value="ECB")
        ttk.Combobox(parent, textvariable=self.text_mode, values=["ECB", "CBC", "CFB", "OFB", "CTR", "PCBC"], state="readonly").pack()

        tk.Label(parent, text="Key (hex):").pack()
        self.text_key = tk.Entry(parent)
        self.text_key.pack()
        tk.Button(parent, text="Generate Key", command=lambda: self.text_key.insert(0, hex(self.dll.generate_random_key()))).pack(pady=5)

        self.text_op = tk.StringVar(value="Encrypt")
        tk.Radiobutton(parent, text="Encrypt", variable=self.text_op, value="Encrypt").pack()
        tk.Radiobutton(parent, text="Decrypt", variable=self.text_op, value="Decrypt").pack()

        tk.Label(parent, text="Input:").pack()
        self.text_input = tk.Text(parent, height=4, width=40)
        self.text_input.pack()
        tk.Button(parent, text="Execute", command=self.execute_text).pack(pady=5)

        tk.Label(parent, text="Output:").pack()
        self.text_output = tk.Text(parent, height=4, width=40, state="disabled")
        self.text_output.pack()

    def create_file_tab(self, parent):
        """Create file encryption tab."""
        tk.Label(parent, text="Mode:").pack()
        self.file_mode = tk.StringVar(value="ECB")
        ttk.Combobox(parent, textvariable=self.file_mode, values=["ECB", "CBC", "CFB", "OFB", "CTR", "PCBC"], state="readonly").pack()

        tk.Label(parent, text="Key (hex):").pack()
        self.file_key = tk.Entry(parent)
        self.file_key.pack()
        tk.Button(parent, text="Generate Key", command=lambda: self.file_key.insert(0, hex(self.dll.generate_random_key()))).pack(pady=5)

        self.file_op = tk.StringVar(value="Encrypt")
        tk.Radiobutton(parent, text="Encrypt", variable=self.file_op, value="Encrypt").pack()
        tk.Radiobutton(parent, text="Decrypt", variable=self.file_op, value="Decrypt").pack()

        tk.Label(parent, text="Input File:").pack()
        self.input_file = tk.StringVar()
        tk.Entry(parent, textvariable=self.input_file, width=40, state="readonly").pack()
        tk.Button(parent, text="Browse", command=lambda: self.input_file.set(filedialog.askopenfilename())).pack()

        tk.Label(parent, text="Output File:").pack()
        self.output_file = tk.StringVar()
        tk.Entry(parent, textvariable=self.output_file, width=40, state="readonly").pack()
        tk.Button(parent, text="Browse", command=lambda: self.output_file.set(filedialog.asksaveasfilename(defaultextension=".txt"))).pack()

        tk.Button(parent, text="Execute", command=self.execute_file).pack(pady=5)

    def execute_text(self):
        """Execute text encryption/decryption."""
        try:
            key = int(self.text_key.get(), 16)
            func_name = f"des_{self.text_mode.get()}_{'encrypt' if self.text_op.get() == 'Encrypt' else 'decrypt'}_string"
            func = getattr(self.dll, func_name)
            input_text = self.text_input.get("1.0", tk.END).strip().encode('latin1')
            output_buffer = ctypes.create_string_buffer(1024)
            if self.text_op.get() == "Encrypt":
                length = func(input_text, output_buffer, key)
                result = output_buffer.raw[:length].decode('latin1')
            else:
                func(input_text, output_buffer, len(input_text), key)
                result = output_buffer.value.decode('utf-8', errors='ignore')
            self.text_output.config(state="normal")
            self.text_output.delete("1.0", tk.END)
            self.text_output.insert("1.0", result)
            self.text_output.config(state="disabled")
        except Exception as e:
            messagebox.showerror("Error", f"Invalid input or key: {str(e)}")

    def execute_file(self):
        """Execute file encryption/decryption."""
        try:
            key = int(self.file_key.get(), 16)
            func_name = f"des_{self.file_mode.get()}_{'encrypt' if self.file_op.get() == 'Encrypt' else 'decrypt'}_file"
            func = getattr(self.dll, func_name)
            input_file = self.input_file.get()
            output_file = self.output_file.get()
            if not input_file or not output_file:
                messagebox.showerror("Error", "Select input and output files")
                return
            func(input_file.encode('utf-8'), output_file.encode('utf-8'), key)
            messagebox.showinfo("Success", f"Output saved to {output_file}")
        except Exception as e:
            messagebox.showerror("Error", f"Invalid input or key: {str(e)}")

def main():
    root = tk.Tk()
    app = DESApp(root)
    root.mainloop()

if __name__ == "__main__":
    main()

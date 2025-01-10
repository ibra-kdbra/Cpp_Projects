# FileLockSocket

Transfer files quickly and securely.

## How much secure? you may ask

- **Key Derivation:** Utilizes PBKDF2 to derive a cryptographic key from a password, adding a layer of security against brute-force attacks.
- **Encryption/Decryption:** Employs AES-256-GCM, a widely accepted symmetric encryption algorithm, ensuring that transferred files remain confidential.
- **Integrity Check:** Uses SHA3-256, a member of the Secure Hash Algorithm 3 family, to guarantee the integrity and authenticity of the file.

## How do I use it?

1) Navigate to the project directory in your terminal.
2) Execute the command ``make all`` or simply ``make``. This will compile and link your files, producing the two executable ``ufsend`` and ``ufreceive``.
3) If you want to clean up the build artifacts later, use ``make clean``.
4) Run following commands *first on Receiver* and then Sender.

### Receiver's Terminal

```shell
./ufrec <input file> [-d <Port>] [-l]
```

### Sender's Terminal

```shell
./ufsend <input file> [-d <IP:Port>] [-l]
```

## Few Solutions to a bad day

1) ``make`` tool with ``g++`` or ``openssl`` libraries might not be installed.
2) Executable permission might not be set for the binaries. Use ```chmod +ux <executable>``` to add executable permissions to owner.
3) Running sender before receiver.
4) Not having permissions to write in the parent directory. Again ``chmod`` will come to your rescue.
5) Network might not capable of reaching the receiver. Check with ``ping``.

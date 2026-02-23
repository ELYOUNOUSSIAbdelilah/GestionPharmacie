# Gestion Pharmacie (C)

## 📌 Description
A console application in C for managing a pharmacy:
- Client management
- Medicines list
- Sales (vente)
- Billing (factures)
- Suppliers (fournisseurs)
- Data saved in text/binary files

## 🛠️ Features
✅ Add / remove clients  
✅ View medicines  
✅ Record sales  
✅ Generate invoices  
✅ Store data in files

## 📂 Project Structure
- `main.c` — program entry point
- `Client.c` / `Client.h` — client functions
- `Medicaments.c` / `Medicaments.h` — medicines
- `Vente.c` / `Vente.h` — sales
- `Facture.c` / `Facture.h` — invoicing
- `Fournisseur.c` / `Fournisseur.h` — suppliers
- `Utils.c` / `Utils.h` — helper functions

## 💻 How to Compile

Using **GCC** (MinGW on Windows):

```bash
gcc *.c -o GestionPharmacie.exe

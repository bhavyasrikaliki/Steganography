# 🔐 Steganography using C

## 📌 Overview
This project implements image steganography in C, which allows users to securely hide and extract secret messages within BMP image files.
The secret data is embedded using the Least Significant Bit (LSB) technique without affecting image quality.

---

## ✨ Features
- Encode secret text into BMP images
- Decode hidden messages from stego images
- Uses LSB (Least Significant Bit) technique
- Command-line based application
- No visible distortion in the image

---

## 🛠️ Technologies Used
- C Programming
- File Handling
- Bitwise Operations
- Linux System Calls

---

## 📂 Project Structure
Steganography/
├── main.c
├── encode.c
├── decode.c
├── stego.h
├── README.md

---

## ▶️ How to Compile and Run

Compile:
gcc *.c -o steganography

Encode:
./steganography -e input.bmp secret.txt output.bmp

Decode:
./steganography -d output.bmp decoded.txt

---

## 📚 Learning Outcomes
- Understanding BMP image format
- Practical use of bitwise manipulation
- Experience with low-level file operations in C

---

## 👩‍💻 Author
Bhavya
